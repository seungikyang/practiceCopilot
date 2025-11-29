cmake_minimum_required(VERSION 3.14)
project(RockScissorsPaper C CXX)

# Set C and C++ standards
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable testing
enable_testing()

# Add source files
set(GAME_SOURCES
    src/game.c
)

set(GAME_HEADERS
    src/game.h
)

# Create game library
add_library(game_lib ${GAME_SOURCES} ${GAME_HEADERS})
target_include_directories(game_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Create main executable
add_executable(rock_scissors_paper src/main.c)
target_link_libraries(rock_scissors_paper game_lib)
set_target_properties(rock_scissors_paper PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
)

# Google Test setup
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Add test executable
add_executable(game_tests
    tests/test_game_gtest.cpp
)

target_link_libraries(game_tests
    game_lib
    gtest
    gtest_main
)

# Discover tests
include(GoogleTest)
gtest_discover_tests(game_tests)

# Add custom target to run tests
add_custom_target(check
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS game_tests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
