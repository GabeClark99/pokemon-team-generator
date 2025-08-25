#include <catch2/catch_test_macros.hpp>
#include "team.h"
#include "types.h"
#include "pokemon.h"

TEST_CASE("evaluateOffense") {
    const TypeEffectiveness typeChart = loadTypeEffectiveness("typeChart.json");
    const TeamEvaluator evaluator(typeChart);

    SECTION("Single member, single target, super effective") {
        Team team { PokemonList{ Pokemon{
            "Charizard", 
            Type::Fire, 
            Type::Dragon,
            {"Levitate", "Blaze"}
        } } };
        TypeAbilityComboList targets { TypeAbilityCombo{ 
            Type::Grass, 
            Type::Poison,
            {}
        } };

        double score = evaluator.evaluateOffense(team, targets);
        REQUIRE(score == 1.0);
    }
    SECTION("Single member, single target, not super effective") {
        Team team { PokemonList{ Pokemon{
            "Charizard", 
            Type::Fire, 
            Type::Dragon,
            {"Levitate", "Blaze"}
        } } };
        TypeAbilityComboList targets { TypeAbilityCombo{ 
            Type::Water, 
            std::nullopt,
            {}
        } };

        double score = evaluator.evaluateOffense(team, targets);
        REQUIRE(score == 0.0);
    }
    SECTION("Single member, two targets, super effective on one") {
        Team team { PokemonList{ Pokemon{
            "Charizard", 
            Type::Fire, 
            Type::Dragon,
            {"Levitate", "Blaze"}
        } } };
        TypeAbilityComboList targets { 
            TypeAbilityCombo{ 
                Type::Grass, 
                Type::Poison,
                {"Overgrow"}
            }, TypeAbilityCombo{ 
                Type::Water, 
                std::nullopt,
                {}
            }
        };

        double score = evaluator.evaluateOffense(team, targets);
        REQUIRE(score == 1.0);
    }
    SECTION("Single member, two targets, super effective on both") {
        Team team { PokemonList{ Pokemon{
            "Charizard", 
            Type::Fire, 
            Type::Dragon,
            {"Levitate", "Blaze"}
        } } };
        TypeAbilityComboList targets { 
            TypeAbilityCombo{ 
                Type::Grass, 
                Type::Poison,
                {"Overgrow", "Chlorophyll"}
            }, TypeAbilityCombo{
                Type::Grass, 
                Type::Poison,
                {"Overgrow", "Chlorophyll"}
            }
        };

        double score = evaluator.evaluateOffense(team, targets);
        REQUIRE(score == 2.0);
    }
    SECTION("Single member, two targets, super effective on neither") {
        Team team { PokemonList{ Pokemon{
            "Charizard", 
            Type::Fire, 
            Type::Dragon,
            {"Levitate", "Blaze"}
        } } };
        TypeAbilityComboList targets { 
            TypeAbilityCombo{ 
                Type::Water, 
                std::nullopt,
                {}
            }, TypeAbilityCombo{ 
                Type::Water, 
                std::nullopt,
                {}
            }
        };

        double score = evaluator.evaluateOffense(team, targets);
        REQUIRE(score == 0.0);
    }
    
    SECTION("Two members, one target, neither super effective") {
        Team team{ 
            PokemonList{ 
                Pokemon{
                    "Charizard", 
                    Type::Fire, 
                    Type::Dragon,
                    {"Levitate", "Blaze"}
                }, Pokemon{
                    "Charizard", 
                    Type::Fire, 
                    Type::Dragon,
                    {"Levitate", "Blaze"}
                }
            } 
        };

        TypeAbilityComboList targets{ 
            TypeAbilityCombo{ 
                Type::Water, 
                std::nullopt,
                {}
            }
        };

        double score = evaluator.evaluateOffense(team, targets);
        REQUIRE(score == 0.0);
    }
    SECTION("Two members, one target, one super effective") {
        Team team{ 
            PokemonList{ 
                Pokemon{
                    "Charizard", 
                    Type::Fire, 
                    Type::Dragon,
                    {"Levitate", "Blaze"}
                }, Pokemon {
                    "Squirtle", 
                    Type::Water,
                    std::nullopt,
                    {"Torrent", "Rain Dish"}
                } 
            } 
        };

        TypeAbilityComboList targets{ 
            TypeAbilityCombo{ 
                Type::Fire, 
                Type::Dragon,
                {"Levitate"}
            } 
        };

        double score = evaluator.evaluateOffense(team, targets);
        REQUIRE(score == 1.0);
    }
    SECTION("Two members, one target, both super effective") {
        Team team{ 
            PokemonList{ 
                Pokemon {
                    "Squirtle", 
                    Type::Water,
                    std::nullopt,
                    {"Torrent", "Rain Dish"}
                }, Pokemon {
                    "Squirtle", 
                    Type::Water,
                    std::nullopt,
                    {"Torrent", "Rain Dish"}
                } 
            } 
        };

        TypeAbilityComboList targets{ 
            TypeAbilityCombo{ 
                Type::Fire, 
                Type::Flying,
                {"Levitate"}
            } 
        };

        double score = evaluator.evaluateOffense(team, targets);
        REQUIRE(score == 1.0);
    }

    SECTION("Two members, two targets, both SE on both") {
        Team team{ 
            PokemonList{ 
                Pokemon {
                    "blah1", 
                    Type::Water,
                    std::nullopt,
                    {}
                }, Pokemon {
                    "blah2", 
                    Type::Water,
                    std::nullopt,
                    {}
                } 
            } 
        };

        TypeAbilityComboList targets{ 
            TypeAbilityCombo{ 
                Type::Fire, 
                Type::Flying,
                {}
            }, TypeAbilityCombo{ 
                Type::Fire, 
                Type::Flying,
                {}
            }
        };

        double score = evaluator.evaluateOffense(team, targets);
        REQUIRE(score == 2.0);
    }
    SECTION("Two members, two targets, both SE on one, both neutral on other") {
        Team team{ 
            PokemonList{ 
                Pokemon {
                    "blah1", 
                    Type::Water,
                    std::nullopt,
                    {}
                }, Pokemon {
                    "blah2", 
                    Type::Water,
                    std::nullopt,
                    {}
                } 
            } 
        };

        TypeAbilityComboList targets{ 
            TypeAbilityCombo{ 
                Type::Fire, 
                Type::Flying,
                {}
            }, TypeAbilityCombo{ 
                Type::Normal, 
                std::nullopt,
                {}
            }
        };

        double score = evaluator.evaluateOffense(team, targets);
        REQUIRE(score == 1.0);
    }
    SECTION("Two members, two targets, one SE on one, other SE on other") {
        Team team{ 
            PokemonList{ 
                Pokemon {
                    "blah1", 
                    Type::Water,
                    std::nullopt,
                    {}
                }, Pokemon {
                    "blah2", 
                    Type::Fire,
                    std::nullopt,
                    {}
                } 
            } 
        };

        TypeAbilityComboList targets{ 
            TypeAbilityCombo{ 
                Type::Fire, 
                std::nullopt,
                {}
            }, TypeAbilityCombo{ 
                Type::Grass, 
                std::nullopt,
                {}
            }
        };

        double score = evaluator.evaluateOffense(team, targets);
        REQUIRE(score == 2.0);
    }
}

TEST_CASE("evaluateDefense") {
    const TypeEffectiveness typeChart = loadTypeEffectiveness("typeChart.json");
    const TeamEvaluator evaluator(typeChart);

    SECTION("One attacker, one defender, neutral") {
        Team team { PokemonList{ Pokemon{
            "Charizard", 
            Type::Fire, 
            Type::Dragon,
            {"Levitate", "Blaze"}
        } } };

        std::vector<Type> attackingTypes{Type::Normal};
        double score = evaluator.evaluateDefense(team, attackingTypes);
        REQUIRE(score == 0.0);
    }
    SECTION("One attacker, one defender, super effective") {
        Team team { PokemonList{ Pokemon{
            "Charizard", 
            Type::Fire, 
            Type::Dragon,
            {"Levitate", "Blaze"}
        } } };

        std::vector<Type> attackingTypes{Type::Rock};
        double score = evaluator.evaluateDefense(team, attackingTypes);
        REQUIRE(score == -1.0);
    }
    SECTION("One attacker, one defender, resist") {
        Team team { PokemonList{ Pokemon{
            "Charizard", 
            Type::Fire, 
            Type::Dragon,
            {"Levitate", "Blaze"}
        } } };

        std::vector<Type> attackingTypes{Type::Bug};
        double score = evaluator.evaluateDefense(team, attackingTypes);
        REQUIRE(score == 1.0);
    }
    SECTION("One attacker, one defender, double resist") {
        Team team { PokemonList{ Pokemon{
            "Charizard", 
            Type::Fire, 
            Type::Dragon,
            {"Levitate", "Blaze"}
        } } };

        std::vector<Type> attackingTypes{Type::Grass};
        double score = evaluator.evaluateDefense(team, attackingTypes);
        REQUIRE(score == 2.0);
    }
    SECTION("One attacker, one defender, immune") {
        Team team { PokemonList{ Pokemon{
            "Charizard", 
            Type::Fire, 
            Type::Dragon,
            {"Levitate", "Blaze"}
        } } };

        std::vector<Type> attackingTypes{Type::Ground};
        double score = evaluator.evaluateDefense(team, attackingTypes);
        REQUIRE(score == 2.0);
    }

    SECTION("Two attackers, one defender, both neutral") {
        Team team { PokemonList{ 
            Pokemon{
                "Charizard", 
                Type::Fire, 
                Type::Dragon,
                {"Levitate", "Blaze"}
            }
        } };

        std::vector<Type> attackingTypes{Type::Normal, Type::Ghost};
        double score = evaluator.evaluateDefense(team, attackingTypes);
        REQUIRE(score == 0.0);
    }
    SECTION("Two attackers, one defender, one SE one neutral") {
        Team team { PokemonList{ 
            Pokemon{
                "Charizard", 
                Type::Fire, 
                Type::Dragon,
                {"Levitate", "Blaze"}
            }
        } };

        std::vector<Type> attackingTypes{Type::Rock, Type::Ghost};
        double score = evaluator.evaluateDefense(team, attackingTypes);
        REQUIRE(score == -1.0);
    }
    SECTION("Two attackers, one defender, one resist one neutral") {
        Team team { PokemonList{ 
            Pokemon{
                "Charizard", 
                Type::Fire, 
                Type::Dragon,
                {"Levitate", "Blaze"}
            }
        } };

        std::vector<Type> attackingTypes{Type::Steel, Type::Ghost};
        double score = evaluator.evaluateDefense(team, attackingTypes);
        REQUIRE(score == 1.0);
    }
    SECTION("Two attackers, one defender, one immune one neutral") {
        Team team { PokemonList{ 
            Pokemon{
                "Charizard", 
                Type::Fire, 
                Type::Dragon,
                {"Levitate", "Blaze"}
            }
        } };

        std::vector<Type> attackingTypes{Type::Ground, Type::Ghost};
        double score = evaluator.evaluateDefense(team, attackingTypes);
        REQUIRE(score == 2.0);
    }
    SECTION("Two attackers, one defender, both SE") {
        Team team { PokemonList{ 
            Pokemon{
                "Charizard", 
                Type::Fire, 
                Type::Dragon,
                {"Levitate", "Blaze"}
            }
        } };

        std::vector<Type> attackingTypes{Type::Rock, Type::Dragon};
        double score = evaluator.evaluateDefense(team, attackingTypes);
        REQUIRE(score == -2.0);
    }
    SECTION("Two attackers, one defender, one SE one resist") {
        Team team { PokemonList{ 
            Pokemon{
                "Charizard", 
                Type::Fire, 
                Type::Dragon,
                {"Levitate", "Blaze"}
            }
        } };

        std::vector<Type> attackingTypes{Type::Rock, Type::Steel};
        double score = evaluator.evaluateDefense(team, attackingTypes);
        REQUIRE(score == 0.0);
    }
    SECTION("Two attackers, one defender, one SE one immune") {
        Team team { PokemonList{ 
            Pokemon{
                "Charizard", 
                Type::Fire, 
                Type::Dragon,
                {"Levitate", "Blaze"}
            }
        } };

        std::vector<Type> attackingTypes{Type::Rock, Type::Ground};
        double score = evaluator.evaluateDefense(team, attackingTypes);
        REQUIRE(score == 1.0);
    }

    SECTION("One attacker, two defenders, both neutral") {
        Team team { PokemonList{ 
            Pokemon{
                "Charizard", 
                Type::Fire, 
                Type::Dragon,
                {"Levitate", "Blaze"}
            },
            Pokemon{
                "Charizard", 
                Type::Fire, 
                Type::Dragon,
                {"Levitate", "Blaze"}
            }
        } };

        std::vector<Type> attackingTypes{Type::Normal};
        double score = evaluator.evaluateDefense(team, attackingTypes);
        REQUIRE(score == 0.0);
    }
    SECTION("One attacker, two defenders, both SE") {
        Team team { PokemonList{ 
            Pokemon{
                "Charizard", 
                Type::Fire, 
                Type::Dragon,
                {"Levitate", "Blaze"}
            },
            Pokemon{
                "Charizard", 
                Type::Fire, 
                Type::Dragon,
                {"Levitate", "Blaze"}
            }
        } };

        std::vector<Type> attackingTypes{Type::Rock};
        double score = evaluator.evaluateDefense(team, attackingTypes);
        REQUIRE(score == -2.0);
    }
    SECTION("One attacker, two defenders, both resist") {
        Team team { PokemonList{ 
            Pokemon{
                "Charizard", 
                Type::Fire, 
                Type::Dragon,
                {"Levitate", "Blaze"}
            },
            Pokemon{
                "Charizard", 
                Type::Fire, 
                Type::Dragon,
                {"Levitate", "Blaze"}
            }
        } };

        std::vector<Type> attackingTypes{Type::Steel};
        double score = evaluator.evaluateDefense(team, attackingTypes);
        REQUIRE(score == 1.0);
    }
    SECTION("One attacker, two defenders, both immune") {
        Team team { PokemonList{ 
            Pokemon{
                "Charizard", 
                Type::Fire, 
                Type::Dragon,
                {"Levitate"}
            },
            Pokemon{
                "Misdreavus", 
                Type::Ghost, 
                Type::Fairy,
                {"Levitate"}
            }
        } };

        std::vector<Type> attackingTypes{Type::Ground};
        double score = evaluator.evaluateDefense(team, attackingTypes);
        REQUIRE(score == 2.0);
    }

    SECTION("Two attackers, two defenders, both SE on both") {
        Team team { PokemonList{ 
            Pokemon{
                "Charizard", 
                Type::Fire, 
                Type::Dragon,
                {"Levitate"}
            },
            Pokemon{
                "Charizard", 
                Type::Fire, 
                Type::Dragon,
                {"Levitate"}
            }
        } };

        std::vector<Type> attackingTypes{Type::Rock, Type::Dragon};
        double score = evaluator.evaluateDefense(team, attackingTypes);
        REQUIRE(score == -4.0);
    }
    SECTION("Two attackers, two defenders, both SE on one, both neutral on other") {
        Team team { PokemonList{ 
            Pokemon{
                "Charizard", 
                Type::Fire, 
                std::nullopt,
                {}
            },
            Pokemon{
                "Misdreavus", 
                Type::Ghost, 
                Type::Fairy,
                {"Levitate"}
            }
        } };

        std::vector<Type> attackingTypes{Type::Water, Type::Rock};
        double score = evaluator.evaluateDefense(team, attackingTypes);
        REQUIRE(score == -2.0);
    }
    SECTION("Two attackers, two defenders, both SE on one, one SE on other") {
        Team team { PokemonList{ 
            Pokemon{
                "Charizard", 
                Type::Fire, 
                std::nullopt,
                {}
            },
            Pokemon{
                "Pidgeot", 
                Type::Normal, 
                Type::Flying,
                {}
            }
        } };

        std::vector<Type> attackingTypes{Type::Water, Type::Rock};
        double score = evaluator.evaluateDefense(team, attackingTypes);
        REQUIRE(score == -3.0);
    }
    SECTION("Two attackers, two defenders, both SE on one, both NVE on other") {
        Team team { PokemonList{ 
            Pokemon{
                "Charizard", 
                Type::Fire, 
                std::nullopt,
                {}
            },
            Pokemon{
                "blah", 
                Type::Grass, 
                Type::Fighting,
                {}
            }
        } };

        std::vector<Type> attackingTypes{Type::Water, Type::Rock};
        double score = evaluator.evaluateDefense(team, attackingTypes);
        REQUIRE(score == 0.0);
    }
    SECTION("Two attackers, two defenders, both SE on one, both immune on other") {
        Team team { PokemonList{ 
            Pokemon{
                "blah1", 
                Type::Fire, 
                Type::Ghost,
                {}
            },
            Pokemon{
                "blah2", 
                Type::Flying, 
                Type::Normal,
                {}
            }
        } };

        std::vector<Type> attackingTypes{Type::Ground, Type::Ghost};
        double score = evaluator.evaluateDefense(team, attackingTypes);
        REQUIRE(score == 2.0);
    }
}
