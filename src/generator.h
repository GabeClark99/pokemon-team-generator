#pragma once

#include <vector>
#include <cstddef>
#include <queue>
#include <mutex>
#include "pokemon.h"
#include "team.h"

class TeamGenerator {
public:
    TeamGenerator(const PokemonList& potentialMembers, const TeamEvaluator& evaluator)
        : potentialMembers_(potentialMembers), evaluator_(evaluator) {}

    std::vector<ScoredTeam> generateTopTeams(size_t teamSize, size_t topN = 10);
    std::vector<ScoredTeam> scoreAndFilterTeams(const std::vector<Team>& teams, const TypeAbilityComboList& targets);
    static void reportProgress(size_t completed, size_t total);

private:
    const PokemonList& potentialMembers_;
    const TeamEvaluator& evaluator_;
};

// Free helper functions
std::vector<Team> generateAllCombinations(const PokemonList& sortedMembers, size_t teamSize);
std::vector<ScoredTeam> getTopNTeams(const std::vector<ScoredTeam>& scoredTeams, size_t topN);
size_t binomial_coefficient(size_t n, size_t k);
