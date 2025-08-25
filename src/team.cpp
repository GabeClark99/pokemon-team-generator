#include "team.h"
#include "logger.h"

using std::vector;

// Evaluates the offensive coverage of a team against a list of target Pokemon.
// The score increases by 1 for each unique Pokemon in the given list that any team member can hit super effectively (effectiveness > 1.0).
double TeamEvaluator::evaluateOffense(const Team& team, const TypeAbilityComboList& targets) const {
    // Logger::debug("Evaluating offense for team of size " + std::to_string(team.size()) + " against " + std::to_string(targets.size()) + " targets");
    size_t score = 0;
    for (const auto& target : targets) {
        bool canHitSE = false;
        for (const auto& member : team) {
            // For each attacking type the member has...
            vector<Type> attackerTypes = { member.primaryType };
            if (member.secondaryType && member.secondaryType.value() != member.primaryType) {
                attackerTypes.push_back(member.secondaryType.value());
            }
            for (const auto& atkType : attackerTypes) {
                double eff = getTypeEffectiveness(
                    typeChart_,
                    atkType,
                    target.primaryType,
                    target.abilities,
                    target.secondaryType
                );
                if (eff > 1.0) {
                    canHitSE = true;
                    break;
                }
            }
            if (canHitSE) break;
        }
        if (canHitSE) ++score;
    }
    Logger::debug("Offensive score: " + std::to_string(score));
    return static_cast<double>(score);
}

// Defensive value calculation
// For each attacking type: if the result is a 0.5 resistance, the defensive value will be increased by one point.
// If it's a 0.25 resistance, it'll be increased by two.
// An immunity (0.0) will add two points.
// For each 2.0 weakness (does not have to be unique), subtract a point.
// For each 4.0 weakness, subtract 2.
double TeamEvaluator::evaluateDefense(const Team& team, const vector<Type>& attackingTypes) const {
    // Logger::debug("Evaluating defense for team of size " + std::to_string(team.size()) + " against " + std::to_string(attackingTypes.size()) + " attacking types");
    double score = 0.0;
    for (const auto& attacker : attackingTypes) {
        double bestResist = 10.0; // higher than any possible effectiveness
        // Track best resistance/immunity for this attacking type
        for (const auto& member : team) {
            double eff = getTypeEffectiveness(
                typeChart_,
                attacker,
                member.primaryType,
                member.abilities,
                member.secondaryType
            );
            if (eff < bestResist) bestResist = eff;
            // Penalize all weaknesses proportionally to account for ability modifiers
            if (eff > 1.0) {
                score -= (eff - 1.0);
            }
        }
        // Only add the best resistance/immunity for this attacking type
        if (bestResist == 0.0 || bestResist == 0.25) {
            score += 2.0;
        } else if (bestResist == 0.5) {
            score += 1.0;
        }
        // Neutral (1.0) or other values: no change
    }
    Logger::debug("Defensive score: " + std::to_string(score));
    return score;
}
