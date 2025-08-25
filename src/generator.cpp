#include <algorithm>
#include <cmath>
#include <fstream>
#include <set>
#include "generator.h"
#include "logger.h"
#include "types.h"

using std::priority_queue;
using std::to_string;
using std::vector;

// Helper: returns true if any two Pokemon in the team share a type
bool hasOverlappingTypes(const Team& team) {
    std::set<Type> seenTypes;
    for (const auto& member : team) {
        if (seenTypes.count(member.primaryType)) return true;
        seenTypes.insert(member.primaryType);
        if (member.secondaryType) {
            if (seenTypes.count(*member.secondaryType)) return true;
            seenTypes.insert(*member.secondaryType);
        }
    }
    return false;
}

// Streaming approach: generate, score, and filter teams on-the-fly
void processCombinationsAndUpdateHeap(
    const PokemonList& sortedMembers,
    size_t teamSize,
    size_t topN,
    const TeamEvaluator& evaluator,
    const TypeAbilityComboList& targets,
    std::priority_queue<ScoredTeam>& heap,
    size_t& completedTeams,
    size_t totalTeams
) {
    vector<bool> selectMask(sortedMembers.size(), false);
    std::fill(selectMask.end() - teamSize, selectMask.end(), true);
    do {
        Team currentTeam;
        for (size_t i = 0; i < sortedMembers.size(); ++i) {
            if (selectMask[i]) {
                currentTeam.push_back(sortedMembers[i]);
            }
        }
        if (currentTeam.size() != teamSize) continue;
        
        // Skip teams with overlapping types
        if (hasOverlappingTypes(currentTeam)) {
            ++completedTeams;
            TeamGenerator::reportProgress(completedTeams, totalTeams);
            continue;
        }

        double offenseScore = evaluator.evaluateOffense(currentTeam, targets);
        double defenseScore = evaluator.evaluateDefense(currentTeam, TypeUtils::all());
        if (defenseScore >= 0.0) {
            ScoredTeam sTeam{currentTeam, offenseScore, defenseScore};
            if (heap.size() < topN) {
                heap.push(sTeam);
            } else if (heap.top() < sTeam) {
                heap.pop();
                heap.push(sTeam);
            }
        }
        ++completedTeams;
        TeamGenerator::reportProgress(completedTeams, totalTeams);
    } while (std::next_permutation(selectMask.begin(), selectMask.end()));
}

// Helper to score and filter teams
vector<ScoredTeam> TeamGenerator::scoreAndFilterTeams(const vector<Team>& teams, const TypeAbilityComboList& targets) {
    vector<ScoredTeam> scored;
    for (const auto& team : teams) {
        double offenseScore = evaluator_.evaluateOffense(team, targets);
        double defenseScore = evaluator_.evaluateDefense(team, TypeUtils::all());
        if (defenseScore >= 0.0) {
            scored.push_back(ScoredTeam{team, offenseScore, defenseScore});
        }
    }
    return scored;
}

// Helper to keep only the top N teams using a heap
vector<ScoredTeam> getTopNTeams(const vector<ScoredTeam>& scoredTeams, size_t topN) {
    std::priority_queue<ScoredTeam> heap;
    for (const auto& sTeam : scoredTeams) {
        if (heap.size() < topN) {
            heap.push(sTeam);
        } else if (heap.top() < sTeam) {
            heap.pop();
            heap.push(sTeam);
        }
    }
    vector<ScoredTeam> allResults;
    while (!heap.empty()) {
        allResults.push_back(heap.top());
        heap.pop();
    }
    std::sort(allResults.begin(), allResults.end());
    std::reverse(allResults.begin(), allResults.end());
    if (allResults.size() > topN) allResults.resize(topN);
    Logger::info("Team generation complete. Results: " + to_string(allResults.size()));
    return allResults;
}

// Helper to report progress (static member)
void TeamGenerator::reportProgress(size_t completed, size_t total) {
    if (completed % 100000 == 0 || completed == total) {
        double percent = (double)completed / total * 100.0;
        Logger::info("Progress: " + to_string(completed) + " / " + to_string(total) +
            " teams (" + to_string(percent) + "%)");
    }
}

vector<ScoredTeam> TeamGenerator::generateTopTeams(size_t teamSize, size_t topN) {
    Logger::info("Starting team generation");
    if (teamSize > potentialMembers_.size()) {
        Logger::error("Requested team size exceeds available members.");
        return {};
    }

    // Copy and sort to ensure deterministic order
    PokemonList sortedMembers = potentialMembers_;
    std::sort(sortedMembers.begin(), sortedMembers.end(), [](const Pokemon& a, const Pokemon& b) {
        return a.name < b.name;
    });

    size_t totalTeams = binomial_coefficient(sortedMembers.size(), teamSize);
    size_t completedTeams = 0;

    // Load all targets once
    TypeAbilityComboList targets = loadTypeAbilityCombos("type_ability_combos.json");

    priority_queue<ScoredTeam> heap;
    processCombinationsAndUpdateHeap(
        sortedMembers, 
        teamSize, 
        topN, 
        evaluator_, 
        targets, 
        heap, 
        completedTeams, 
        totalTeams
    );

    vector<ScoredTeam> allResults;
    while (!heap.empty()) {
        allResults.push_back(heap.top());
        heap.pop();
    }
    std::sort(allResults.begin(), allResults.end());
    std::reverse(allResults.begin(), allResults.end());
    if (allResults.size() > topN) allResults.resize(topN);
    Logger::info("Team generation complete. Results: " + to_string(allResults.size()));
    return allResults;
}

// Helper
size_t binomial_coefficient(size_t n, size_t k) {
    if (k > n) return 0;
    if (k == 0 || k == n) return 1;
    if (k > n - k) k = n - k;
    size_t result = 1;
    for (size_t i = 1; i <= k; ++i) {
        result = result * (n - k + i) / i;
    }
    return result;
}
