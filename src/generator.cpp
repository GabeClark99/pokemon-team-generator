#include <algorithm>
#include <cmath>
#include <fstream>
#include <queue>
#include <set>
#include <string>
#include "generator.h"
#include "logger.h"
#include "types.h"

namespace { // file-local helpers, constants, and aliases
    using std::priority_queue;
    using std::to_string;
    using std::vector;

    static constexpr size_t kProgressReportInterval = 100000;

    // Min-heap comparator: returns true when 'a' is better than 'b' 
    // (priority_queue with this comparator places the worst team at top)
    struct ScoredTeamMinComparator {
        bool operator()(const ScoredTeam& a, const ScoredTeam& b) const {
            if (a.offensiveScore != b.offensiveScore) return a.offensiveScore > b.offensiveScore;
            if (a.defensiveScore != b.defensiveScore) return a.defensiveScore > b.defensiveScore;
            // deterministic tie-breaker by concatenated member names
            std::string sa, sb;
            for (const auto &m : a.team) { if (!sa.empty()) sa.push_back('|'); sa += m.name; }
            for (const auto &m : b.team) { if (!sb.empty()) sb.push_back('|'); sb += m.name; }
            return sa > sb;
        }
    };

    using MinHeap = std::priority_queue<ScoredTeam, std::vector<ScoredTeam>, ScoredTeamMinComparator>;

    // Returns true if any two Pokemon in the team share a type
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

    // Push a scored team into the min-heap while keeping only topN teams
    static void pushIfTop(
        MinHeap& heap,
        const ScoredTeam& sTeam,
        size_t topN
    ) {
        if (heap.size() < topN) {
            heap.push(sTeam);
        } else if (heap.top() < sTeam) {
            heap.pop();
            heap.push(sTeam);
        }
    }

    // Collect heap contents into a sorted vector (descending), cap to topN
    static vector<ScoredTeam> collectResultsFromHeap(
        MinHeap& heap,
        size_t topN
    ) {
        vector<ScoredTeam> allResults;
        while (!heap.empty()) {
            allResults.push_back(heap.top());
            heap.pop();
        }
        std::sort(allResults.begin(), allResults.end());
        std::reverse(allResults.begin(), allResults.end());
        if (allResults.size() > topN) allResults.resize(topN);
        return allResults;
    }

    // Generate, score, and filter teams on-the-fly
    void processCombinationsAndUpdateHeap(
        const PokemonList& sortedMembers,
        size_t slotsToFill,
        size_t topN,
        const TeamEvaluator& evaluator,
        const TypeAbilityComboList& targets,
        MinHeap& heap,
        size_t& completedTeams,
        size_t totalTeams,
        const Team& pinnedMembers
    ) {
        vector<bool> selectMask(sortedMembers.size(), false);
        std::fill(selectMask.end() - slotsToFill, selectMask.end(), true);
        do {
            Team currentTeam = pinnedMembers;
            for (size_t i = 0; i < sortedMembers.size(); ++i) {
                if (selectMask[i]) {
                    currentTeam.push_back(sortedMembers[i]);
                }
            }
            if (currentTeam.size() != pinnedMembers.size() + slotsToFill) continue;
            
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
                pushIfTop(heap, sTeam, topN);
            }
            ++completedTeams;
            TeamGenerator::reportProgress(completedTeams, totalTeams);
        } while (std::next_permutation(selectMask.begin(), selectMask.end()));
    }

    vector<ScoredTeam> getTopNTeams(const vector<ScoredTeam>& scoredTeams, size_t topN) {
        MinHeap heap;
        for (const auto& sTeam : scoredTeams) {
            pushIfTop(heap, sTeam, topN);
        }
        auto allResults = collectResultsFromHeap(heap, topN);
        Logger::info("Team generation complete. Results: " + to_string(allResults.size()));
        return allResults;
    }

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
} // namespace

vector<ScoredTeam> TeamGenerator::generateTopTeams(size_t teamSize, size_t topN, const PokemonList& pinnedMembers) {
    Logger::info("Starting team generation");
    if (teamSize < pinnedMembers.size()) {
        Logger::error("Number of pinned members exceeds team size.");
        return {};
    }
    if (teamSize > potentialMembers_.size()) {
        Logger::error("Requested team size exceeds available members.");
        return {};
    }

    // Remove pinned members from pool to avoid duplicates
    PokemonList availableMembers;
    for (const auto& p : potentialMembers_) {
        bool isPinned = false;
        for (const auto& pin : pinnedMembers) {
            if (p.name == pin.name) {
                isPinned = true;
                break;
            }
        }
        if (!isPinned) availableMembers.push_back(p);
    }

    // Copy and sort to ensure deterministic order
    PokemonList sortedMembers = availableMembers;
    std::sort(sortedMembers.begin(), sortedMembers.end(), [](const Pokemon& a, const Pokemon& b) {
        return a.name < b.name;
    });

    size_t slotsToFill = teamSize - pinnedMembers.size();
    size_t totalTeams = binomial_coefficient(sortedMembers.size(), slotsToFill);
    size_t completedTeams = 0;

    TypeAbilityComboList targets = loadTypeAbilityCombos("type_ability_combos.json");

    MinHeap heap;
    processCombinationsAndUpdateHeap(
        sortedMembers, 
        slotsToFill, 
        topN, 
        evaluator_, 
        targets, 
        heap, 
        completedTeams, 
        totalTeams,
        pinnedMembers
    );

    auto allResults = collectResultsFromHeap(heap, topN);
    Logger::info("Team generation complete. Results: " + to_string(allResults.size()));
    return allResults;
}

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

void TeamGenerator::reportProgress(size_t completed, size_t total) {
    if (completed % kProgressReportInterval == 0 || completed == total) {
        double percent = (double)completed / total * 100.0;
        Logger::info("Progress: " + to_string(completed) + " / " + to_string(total) +
            " teams (" + to_string(percent) + "%)");
    }
}
