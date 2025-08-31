#include <iostream>
#include "generator.h"
#include "types.h"
#include "pokemon.h"
#include "logger.h"

using std::cout;

int main() {
    Logger::setLogLevel(LogLevel::Info);

    TypeEffectiveness typeChart = loadTypeEffectiveness("typeChart.json");
    PokemonList coolPokemon = loadPokemon("coolPokemon.json");
    TeamEvaluator evaluator(typeChart);
    TeamGenerator generator(coolPokemon, evaluator);

    vector<ScoredTeam> topTeams = generator.generateTopTeams(
        6, 
        10,
        {
            // {"Greninja", Type::Water, Type::Dark, {"Protean"}},
            // {"Galvantula", Type::Electric, Type::Bug, {"Compound Eyes"}},
            // {"Togekiss", Type::Fairy, Type::Flying, {"Serene Grace"}}
        }
    );

    // Display the top teams
    for (size_t i = 0; i < topTeams.size(); ++i) {
        const auto& scoredTeam = topTeams[i];
        cout << "Team #" << (i + 1) << ":\n";
        for (const auto& member : scoredTeam.team) {
            cout << "  - " << member.name;
            cout << " (" << typeToString(member.primaryType);
            if (member.secondaryType.has_value()) {
                cout << "/" << typeToString(member.secondaryType.value());
            }
            cout << ") Abilities: ";
            for (size_t j = 0; j < member.abilities.size(); ++j) {
                cout << member.abilities[j];
                if (j + 1 < member.abilities.size()) cout << ", ";
            }
            cout << "\n";
        }
        cout << "  Offensive Score: " << scoredTeam.offensiveScore << ", Defensive Score: " << scoredTeam.defensiveScore << "\n\n";
    }

    return 0;
}