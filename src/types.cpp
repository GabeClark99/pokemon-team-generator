#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <unordered_map>
#include "types.h"
#include "logger.h"

using std::string;
using std::vector;
using json = nlohmann::json;

// Helper: array of type names matching the Type enum order
static const char* TYPE_NAMES[] = {
    "Normal", "Fire", "Water", "Grass", "Electric", "Ice",
    "Fighting", "Poison", "Ground", "Flying", "Psychic",
    "Bug", "Rock", "Ghost", "Dragon", "Dark", "Steel", "Fairy"
};

vector<Type> TypeUtils::all() {
    vector<Type> result;
    for (size_t i = 0; i < static_cast<size_t>(Type::COUNT); ++i) {
        result.push_back(static_cast<Type>(i));
    }
    return result;
}

string typeToString(Type type) {
    size_t idx = static_cast<size_t>(type);
    if (idx < static_cast<size_t>(Type::COUNT)) {
        return TYPE_NAMES[idx];
    }
    return "???";
}

Type stringToType(const string& typeStr) {
    for (size_t i = 0; i < static_cast<size_t>(Type::COUNT); ++i) {
        if (typeStr == TYPE_NAMES[i]) {
            return static_cast<Type>(i);
        }
    }
    throw std::invalid_argument("Unknown type: " + typeStr);
}

TypeEffectiveness loadTypeEffectiveness(const string& path) {
    Logger::info("Loading type effectiveness chart from: " + path);
    TypeEffectiveness chart = {};

    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open type effectiveness file: " + path);
    }

    json j;
    file >> j;

    // Build a map from type name to Type enum for lookup
    std::unordered_map<string, Type> nameToType;
    for (size_t i = 0; i < static_cast<size_t>(Type::COUNT); ++i) {
        nameToType[TYPE_NAMES[i]] = static_cast<Type>(i);
    }

    // Expecting JSON structure: { "Fire": { "Grass": 2.0, ... }, ... }
    for (const auto& [attackerName, defenderMap] : j.items()) {
        auto attackerIt = nameToType.find(attackerName);
        if (attackerIt == nameToType.end()) continue;

        Type attacker = attackerIt->second;
        for (const auto& [defenderName, multiplier] : defenderMap.items()) {
            auto defenderIt = nameToType.find(defenderName);
            if (defenderIt == nameToType.end()) continue;

            Type defender = defenderIt->second;
            chart[static_cast<size_t>(attacker)][static_cast<size_t>(defender)] = multiplier.get<double>();
        }
    }

    return chart;
}

TypeAbilityComboList loadTypeAbilityCombos(const string& path) {
    Logger::info("Loading type-ability combos from: " + path);
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open type-ability combos file: " + path);
    }

    json j;
    file >> j;

    TypeAbilityComboList combos;
    for (const auto& entry : j) {
        TypeAbilityCombo combo;
        combo.primaryType = stringToType(entry["primaryType"]);
        if (entry.contains("secondaryType") && !entry["secondaryType"].is_null()) {
            combo.secondaryType = stringToType(entry["secondaryType"]);
        } else {
            combo.secondaryType = std::nullopt;
        }
        combo.abilities = entry["abilities"].get<vector<string>>();
        combos.push_back(std::move(combo));
    }

    return combos;
}

double getTypeEffectiveness(
    const TypeEffectiveness& chart, 
    const Type& attacker, 
    const Type& defender1,
    const vector<string>& defenderAbilities,
    const std::optional<Type>& defender2
) {
    // Logger::debug("Calculating type effectiveness: attacker=" + typeToString(attacker) + ", defender1=" + typeToString(defender1) + (defender2 ? ", defender2=" + typeToString(*defender2) : "") );
    double effectiveness = chart[static_cast<size_t>(attacker)][static_cast<size_t>(defender1)];
    if (defender2 && *defender2 != defender1) {
        effectiveness *= chart[static_cast<size_t>(attacker)][static_cast<size_t>(*defender2)];
    }

    // This abilities section assumes the defender will use an ability that reduces effectiveness the most

    // Evaluate potential immunities
    for (const auto& ability : defenderAbilities) {
        if (
            (ability == "Levitate" && attacker == Type::Ground) ||
            (ability == "Flash Fire" && attacker == Type::Fire) ||
            (ability == "Water Absorb" && attacker == Type::Water) ||
            ((
                ability == "Volt Absorb" || 
                ability == "Motor Drive" || 
                ability == "Lightning Rod"
            ) && attacker == Type::Electric) ||
            (ability == "Dry Skin" && attacker == Type::Water) ||
            (ability == "Wonder Guard" && effectiveness <= 1.0)
        ) return 0.0;
    }

    // Evaluate other modifiers
    for (const auto& ability : defenderAbilities) {
        // Resistances
        if (ability == "Thick Fat" && attacker == Type::Fire) return effectiveness * 0.5;
        if (ability == "Thick Fat" && attacker == Type::Ice) return effectiveness * 0.5;
        if (ability == "Heatproof" && attacker == Type::Fire) return effectiveness * 0.5;
        if ((
            ability == "Filter" ||
            ability == "Solid Rock"
        ) && effectiveness > 1.0) return effectiveness * 0.75;

        // The rest
        if (ability == "Dry Skin" && attacker == Type::Fire) return effectiveness * 1.25;
        // todo: scrappy
        // todo: mold breaker
    }

    return effectiveness;
}
