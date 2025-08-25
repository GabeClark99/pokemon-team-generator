#pragma once

#include <optional>
#include <string>
#include <vector>
#include "types.h"

using std::string;
using std::vector;

struct Pokemon {
    string name;
    Type primaryType;
    std::optional<Type> secondaryType;
    vector<string> abilities;
};

using PokemonList = std::vector<Pokemon>;

// Loads pokemon data from a JSON file
PokemonList loadPokemon(const string& path);