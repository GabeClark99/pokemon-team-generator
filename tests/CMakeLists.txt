set (TEST_SOURCES
    test_types.cpp
    test_pokemon.cpp
    test_team.cpp
)

add_executable(team_tests ${TEST_SOURCES})

target_link_libraries(team_tests
    PRIVATE
        team_core
        Catch2::Catch2WithMain
)

target_include_directories(team_tests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

# Copy in required data files after build
add_custom_command(
    TARGET team_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/data/typeChart.json
        $<TARGET_FILE_DIR:team_tests>/typeChart.json
)
add_custom_command(
    TARGET team_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/data/coolPokemon.json
        $<TARGET_FILE_DIR:team_tests>/coolPokemon.json
)
add_custom_command(
    TARGET team_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/data/type_ability_combos.json
        $<TARGET_FILE_DIR:team_tests>/type_ability_combos.json
)

include(CTest)
include(Catch)
catch_discover_tests(team_tests)
