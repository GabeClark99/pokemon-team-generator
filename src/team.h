#pragma once

#include <vector>
#include <string>
#include <optional>
#include "pokemon.h"
#include "types.h"

using Team = std::vector<Pokemon>;

struct ScoredTeam {
    Team team;
    double offensiveScore;
    double defensiveScore;
    // For sorting. higher offense, then higher defense
    bool operator<(const ScoredTeam& other) const {
        return (offensiveScore + 4*defensiveScore) < (other.offensiveScore + 4*other.defensiveScore);
    }
};

class TeamEvaluator {
public:
    TeamEvaluator(const TypeEffectiveness& typeChart)
        : typeChart_(typeChart) {}

    double evaluateOffense(const Team& team, const TypeAbilityComboList& targets) const;
    double evaluateDefense(const Team& team, const vector<Type>& attackingTypes) const;

private:
    const TypeEffectiveness& typeChart_;
};