#include <catch2/catch_test_macros.hpp>
#include <fstream>
#include <string>
#include "types.h"

using std::string;
using std::vector;

TEST_CASE("typeToString") {
    SECTION("Valid Types") {
        for (size_t i = 0; i < static_cast<size_t>(Type::COUNT); ++i) {
            Type t = static_cast<Type>(i);
            string s = typeToString(t);
            REQUIRE_FALSE(s.empty());
            // // Check that stringToType can convert back
            // REQUIRE(stringToType(s) == t);
        }
    }
    SECTION("Invalid Type") {
        REQUIRE(typeToString(static_cast<Type>(255)) == "???");
    }
}

TEST_CASE("stringToType") {
    SECTION("Valid Strings") {
        REQUIRE(stringToType("Normal") == Type::Normal);
        REQUIRE(stringToType("Fire") == Type::Fire);
        REQUIRE(stringToType("Water") == Type::Water);
        REQUIRE(stringToType("Grass") == Type::Grass);
        REQUIRE(stringToType("Electric") == Type::Electric);
        REQUIRE(stringToType("Ice") == Type::Ice);
        REQUIRE(stringToType("Fighting") == Type::Fighting);
        REQUIRE(stringToType("Poison") == Type::Poison);
        REQUIRE(stringToType("Ground") == Type::Ground);
        REQUIRE(stringToType("Flying") == Type::Flying);
        REQUIRE(stringToType("Psychic") == Type::Psychic);
        REQUIRE(stringToType("Bug") == Type::Bug);
        REQUIRE(stringToType("Rock") == Type::Rock);
        REQUIRE(stringToType("Ghost") == Type::Ghost);
        REQUIRE(stringToType("Dragon") == Type::Dragon);
        REQUIRE(stringToType("Dark") == Type::Dark);
        REQUIRE(stringToType("Steel") == Type::Steel);
        REQUIRE(stringToType("Fairy") == Type::Fairy);
    }
    SECTION("Invalid String") {
        REQUIRE_THROWS_AS(stringToType("UnknownType"), std::invalid_argument);
    }
}

TEST_CASE("getTypeEffectiveness") {
    // Initialize to all neutral
    TypeEffectiveness chart = {};
    for (size_t i = 0; i < NUM_TYPES; ++i)
        for (size_t j = 0; j < NUM_TYPES; ++j)
            chart[i][j] = 1.0;

    chart[static_cast<size_t>(Type::Fire)][static_cast<size_t>(Type::Grass)] = 2.0;
    chart[static_cast<size_t>(Type::Fire)][static_cast<size_t>(Type::Water)] = 0.5;
    chart[static_cast<size_t>(Type::Fire)][static_cast<size_t>(Type::Bug)] = 2.0;
    chart[static_cast<size_t>(Type::Fire)][static_cast<size_t>(Type::Fire)] = 0.5;
    chart[static_cast<size_t>(Type::Ground)][static_cast<size_t>(Type::Fire)] = 2.0;
    chart[static_cast<size_t>(Type::Ground)][static_cast<size_t>(Type::Bug)] = 0.5;
    chart[static_cast<size_t>(Type::Water)][static_cast<size_t>(Type::Fire)] = 2.0;
    chart[static_cast<size_t>(Type::Water)][static_cast<size_t>(Type::Grass)] = 0.5;
    chart[static_cast<size_t>(Type::Fire)][static_cast<size_t>(Type::Ice)] = 2.0;
    chart[static_cast<size_t>(Type::Ice)][static_cast<size_t>(Type::Grass)] = 2.0;

    SECTION("Single-type defender, no ability") {
        REQUIRE(getTypeEffectiveness(chart, Type::Fire, Type::Grass, {}) == 2.0);
        REQUIRE(getTypeEffectiveness(chart, Type::Fire, Type::Water, {}) == 0.5);
        REQUIRE(getTypeEffectiveness(chart, Type::Fire, Type::Bug, {}) == 2.0);
        REQUIRE(getTypeEffectiveness(chart, Type::Fire, Type::Fire, {}) == 0.5);
        REQUIRE(getTypeEffectiveness(chart, Type::Fire, Type::Grass, {}, std::nullopt) == 2.0);
    }

    SECTION("Dual-type defender, no ability") {
        REQUIRE(getTypeEffectiveness(chart, Type::Fire, Type::Grass, {}, Type::Bug) == 4.0);
        REQUIRE(getTypeEffectiveness(chart, Type::Fire, Type::Grass, {}, Type::Water) == 1.0);
        REQUIRE(getTypeEffectiveness(chart, Type::Fire, Type::Water, {}, Type::Bug) == 1.0);
    }

    SECTION("Single-type defender with immunity ability") {
        vector<string> abilities = {"Flash Fire"};
        REQUIRE(getTypeEffectiveness(chart, Type::Fire, Type::Grass, abilities) == 0.0);
        REQUIRE(getTypeEffectiveness(chart, Type::Fire, Type::Water, abilities) == 0.0);
        REQUIRE(getTypeEffectiveness(chart, Type::Fire, Type::Normal, abilities) == 0.0);

        abilities = {"Levitate"};
        REQUIRE(getTypeEffectiveness(chart, Type::Ground, Type::Fire, abilities) == 0.0);
        REQUIRE(getTypeEffectiveness(chart, Type::Ground, Type::Bug, abilities) == 0.0);
        REQUIRE(getTypeEffectiveness(chart, Type::Ground, Type::Normal, abilities) == 0.0);
        REQUIRE(getTypeEffectiveness(chart, Type::Ground, Type::Flying, abilities) == 0.0);

        abilities = {"Water Absorb"};
        REQUIRE(getTypeEffectiveness(chart, Type::Water, Type::Fire, abilities) == 0.0);
        REQUIRE(getTypeEffectiveness(chart, Type::Water, Type::Grass, abilities) == 0.0);
        REQUIRE(getTypeEffectiveness(chart, Type::Water, Type::Normal, abilities) == 0.0);
    }

    SECTION("Single-type defender with resistance ability") {
        vector<string> abilities = {"Thick Fat"};
        REQUIRE(getTypeEffectiveness(chart, Type::Fire, Type::Ice, abilities) == 1.0);
        REQUIRE(getTypeEffectiveness(chart, Type::Ice, Type::Grass, abilities) == 1.0);
        REQUIRE(getTypeEffectiveness(chart, Type::Fire, Type::Normal, abilities) == 0.5);
        REQUIRE(getTypeEffectiveness(chart, Type::Ice, Type::Normal, abilities) == 0.5);

        abilities = {"Filter"};
        REQUIRE(getTypeEffectiveness(chart, Type::Fire, Type::Grass, abilities) == 1.5);
        REQUIRE(getTypeEffectiveness(chart, Type::Fire, Type::Ice, abilities) == 1.5);
        REQUIRE(getTypeEffectiveness(chart, Type::Fire, Type::Normal, abilities) == 1.0);
    }

    SECTION("Single-type defender with misc ability") {
        vector<string> abilities = {"Dry Skin"};
        REQUIRE(getTypeEffectiveness(chart, Type::Fire, Type::Normal, abilities) == 1.25);
        REQUIRE(getTypeEffectiveness(chart, Type::Fire, Type::Grass, abilities) == 2.5);
    }
}

TEST_CASE("loadTypeEffectiveness") {
    const string testJson = R"({
        "Fire": {
            "Grass": 2.0,
            "Water": 0.5
        },
        "Water": {
            "Fire": 2.0,
            "Grass": 0.5
        },
        "Grass": {
            "Fire": 0.5,
            "Water": 2.0
        }
    })";

    const string fileName = "test_type_chart.json";
    {
        std::ofstream out(fileName);
        out << testJson;
    }

    TypeEffectiveness chart = loadTypeEffectiveness(fileName);

    REQUIRE(getTypeEffectiveness(chart, Type::Fire, Type::Grass, {}) == 2.0);
    REQUIRE(getTypeEffectiveness(chart, Type::Fire, Type::Water, {}) == 0.5);
    REQUIRE(getTypeEffectiveness(chart, Type::Water, Type::Fire, {}) == 2.0);
    REQUIRE(getTypeEffectiveness(chart, Type::Water, Type::Grass, {}) == 0.5);
    REQUIRE(getTypeEffectiveness(chart, Type::Grass, Type::Water, {}) == 2.0);
    REQUIRE(getTypeEffectiveness(chart, Type::Grass, Type::Fire, {}) == 0.5);
}