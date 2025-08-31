#include <nlohmann/json.hpp>
#include <fstream>
#include <stdexcept>
#include "pokemon.h"
#include "logger.h"

namespace { // file-local aliases
    using std::runtime_error;
    using json = nlohmann::json;
}

/*
 * Loads Pokemon data from a JSON file.
 * Expected JSON structure:
 * [
 *   {
 *     "name": "Bulbasaur",
 *     "primaryType": "Grass",
 *     "secondaryType": "Poison", // optional
 *     "abilities": ["Overgrow", "Thick Fat"]
 *   },
 *   ...
 * ]
 */
PokemonList loadPokemon(const string& path) {
    Logger::info("Loading Pokemon data from: " + path);
    PokemonList pokemonList;

    std::ifstream file(path);
    if (!file.is_open()) {
        // Logger::error("Could not open Pokemon data file: " + path);
        throw runtime_error("Could not open Pokemon data file: " + path);
    }

    json j;
    try {
        file >> j;
    } catch (const json::parse_error& e) {
        // Logger::error("JSON parse error: " + string(e.what()));
        throw runtime_error("JSON parse error: " + string(e.what()));
    }

    for (const auto& item : j) {
        // Validate required fields
        if (!item.contains("name") || !item.contains("primaryType") || !item.contains("abilities")) {
            // Logger::error("Invalid Pokemon data format");
            throw runtime_error("Invalid Pokemon data format");
        }

        Pokemon pokemon;
        pokemon.name = item.at("name").get<string>();
        pokemon.primaryType = stringToType(item.at("primaryType").get<string>());

        // optional secondary type
        if (item.contains("secondaryType") && !item.at("secondaryType").is_null()) {
            pokemon.secondaryType = stringToType(item.at("secondaryType").get<string>());
        }
        else {
            pokemon.secondaryType = std::nullopt; // single-typed
        }

        pokemon.abilities = item.at("abilities").get<vector<string>>();

        pokemonList.push_back(pokemon);
    }

    Logger::info("Loaded " + std::to_string(pokemonList.size()) + " Pokemon from file.");
    return pokemonList;
}
