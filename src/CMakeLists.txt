
set(SRC_FILES
    pokemon.cpp
    types.cpp
    team.cpp
    generator.cpp
)

add_library(team_core STATIC ${SRC_FILES})

target_include_directories(team_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(team_core PUBLIC nlohmann_json::nlohmann_json)