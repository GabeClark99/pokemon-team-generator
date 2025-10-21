cmake_minimum_required(VERSION 3.14)
project(team_builder)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

find_package(nlohmann_json REQUIRED)
find_package(Catch2 REQUIRED)

add_subdirectory(src)
add_subdirectory(tests)
add_executable(team_builder src/main.cpp)
target_link_libraries(team_builder PRIVATE team_core)

add_custom_command(TARGET team_builder POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/data
    $<TARGET_FILE_DIR:team_builder>/data
)
