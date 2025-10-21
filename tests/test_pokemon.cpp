#include <catch2/catch_test_macros.hpp>
#include <fstream>
#include "pokemon.h"

using std::string;

TEST_CASE("loadPokemon") {
    SECTION("valid data") {
        const string fileName = "test_pokemon_valid.json";
        const string testJson = R"([
            {
                "name": "Bulbasaur",
                "primaryType": "Grass",
                "secondaryType": "Poison",
                "abilities": ["Overgrow", "Chlorophyll"]
            },
            {
                "name": "Charmander",
                "primaryType": "Fire",
                "abilities": ["Blaze", "Solar Power"]
            },
            {
                "name": "Mamoswine",
                "primaryType": "Ground",
                "secondaryType": "Ice",
                "abilities": ["Snow Cloak", "Thick Fat"]
            },
            {
                "name": "Mismagius",
                "primaryType": "Ghost",
                "secondaryType": "Fairy",
                "abilities": ["Levitate"]
            }
        ])";
        {
            std::ofstream outFile(fileName);
            outFile << testJson;
        }

        PokemonList list = loadPokemon(fileName);
        REQUIRE(list.size() == 4);

        REQUIRE(list[0].name == "Bulbasaur");
        REQUIRE(list[0].primaryType == Type::Grass);
        REQUIRE(list[0].secondaryType.has_value());
        REQUIRE(list[0].secondaryType.value() == Type::Poison);
        REQUIRE(list[0].abilities.size() == 2);
        REQUIRE(list[0].abilities[0] == "Overgrow");
        REQUIRE(list[0].abilities[1] == "Chlorophyll");

        REQUIRE(list[1].name == "Charmander");
        REQUIRE(list[1].primaryType == Type::Fire);
        REQUIRE(!list[1].secondaryType.has_value());
        REQUIRE(list[1].abilities.size() == 2);
        REQUIRE(list[1].abilities[0] == "Blaze");
        REQUIRE(list[1].abilities[1] == "Solar Power");

        REQUIRE(list[2].name == "Mamoswine");
        REQUIRE(list[2].primaryType == Type::Ground);
        REQUIRE(list[2].secondaryType.has_value());
        REQUIRE(list[2].secondaryType.value() == Type::Ice);
        REQUIRE(list[2].abilities.size() == 2);
        REQUIRE(list[2].abilities[0] == "Snow Cloak");
        REQUIRE(list[2].abilities[1] == "Thick Fat");

        REQUIRE(list[3].name == "Mismagius");
        REQUIRE(list[3].primaryType == Type::Ghost);
        REQUIRE(list[3].secondaryType.has_value());
        REQUIRE(list[3].secondaryType.value() == Type::Fairy);
        REQUIRE(list[3].abilities.size() == 1);
        REQUIRE(list[3].abilities[0] == "Levitate");
    }
    SECTION("missing field") {
        const string fileName = "data/test_pokemon_invalid.json";
        const string testJson = R"([
            {
                "name": "Bulbasaur",
                "primaryType": "Grass",
                // missing abilities field
            }
        ])";
        {
            std::ofstream outFile(fileName);
            outFile << testJson;
        }

        REQUIRE_THROWS_AS(loadPokemon(fileName), std::runtime_error);
    }
    SECTION("malformed json") {
        const string fileName = "test_pokemon_malformed.json";
        const string testJson = R"([
            {
                "name": "Bulbasaur",
                "primaryType": "Grass",
                "abilities": ["Overgrow"]
            }
        )"; // missing closing bracket
        {
            std::ofstream outFile(fileName);
            outFile << testJson;
        }

        REQUIRE_THROWS_AS(loadPokemon(fileName), std::runtime_error);
    }
}
