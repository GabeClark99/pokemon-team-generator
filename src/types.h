#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <string>

// Enum for all Pokemon types
enum class Type : uint8_t {
    Normal, Fire, Water, Grass, Electric, Ice, Fighting, Poison,
    Ground, Flying, Psychic, Bug, Rock, Ghost, Dragon, Dark,
    Steel, Fairy, COUNT
};

struct TypeUtils {
    static std::vector<Type> all();
};

struct TypeAbilityCombo {
    Type primaryType;
    std::optional<Type> secondaryType;
    std::vector<std::string> abilities;
};
using TypeAbilityComboList = std::vector<TypeAbilityCombo>;

// Converts a Type enum to its string representation
std::string typeToString(Type type);

// Converts a string to its respective Type enum
Type stringToType(const std::string& typeStr);

// Type effectiveness matrix
constexpr size_t NUM_TYPES = static_cast<size_t>(Type::COUNT);
using TypeEffectiveness = std::array<std::array<double, NUM_TYPES>, NUM_TYPES>;

// Loads type effectiveness data from a JSON file
TypeEffectiveness loadTypeEffectiveness(const std::string& path);

TypeAbilityComboList loadTypeAbilityCombos(const std::string& path);

double getTypeEffectiveness(
    const TypeEffectiveness& chart, 
    const Type& attacker, 
    const Type& defender1,
    const std::vector<std::string>& defenderAbilities,
    const std::optional<Type>& defender2 = std::nullopt
);
