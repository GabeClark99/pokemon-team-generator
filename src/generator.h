#pragma once

#include <vector>
#include <cstddef>
#include "pokemon.h"
#include "team.h"

class TeamGenerator {
public:
    TeamGenerator(const PokemonList& potentialMembers, const TeamEvaluator& evaluator)
        : potentialMembers_(potentialMembers), evaluator_(evaluator) {}

    std::vector<ScoredTeam> generateTopTeams(
        size_t teamSize, 
        size_t topN = 10,
        const PokemonList& pinnedMembers = {}
    );
    std::vector<ScoredTeam> scoreAndFilterTeams(const std::vector<Team>& teams, const TypeAbilityComboList& targets);
    static void reportProgress(size_t completed, size_t total);

private:
    const PokemonList& potentialMembers_;
    const TeamEvaluator& evaluator_;
};
