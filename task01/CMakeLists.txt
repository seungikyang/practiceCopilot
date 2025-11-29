cmake_minimum_required(VERSION 3.14)
project(URLTools C CXX)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set C++ standard for tests
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable testing
enable_testing()

# Include FetchContent module for downloading GoogleTest
include(FetchContent)

# Fetch GoogleTest
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.12.1
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

# Source files
set(SOURCES
    src/url_tools.c
    src/url.c
)

# Create a library from the C source files
add_library(url_tools_lib ${SOURCES})

# Include directories
target_include_directories(url_tools_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Test executable for url_tools
add_executable(url_tools_test
    tests/test_url_tools_gtest.cpp
)

# Link test executable with library and GTest
target_link_libraries(url_tools_test
    url_tools_lib
    gtest
    gtest_main
)

# Include directories for tests
target_include_directories(url_tools_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Test executable for url management
add_executable(url_test
    tests/test_url_gtest.cpp
)

# Link url test executable with library and GTest
target_link_libraries(url_test
    url_tools_lib
    gtest
    gtest_main
)

# Include directories for url tests
target_include_directories(url_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Test executable for integration tests
add_executable(integration_test
    tests/test_integration_gtest.cpp
)

# Link integration test executable with library and GTest
target_link_libraries(integration_test
    url_tools_lib
    gtest
    gtest_main
)

# Include directories for integration tests
target_include_directories(integration_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Add tests to CTest
include(GoogleTest)
gtest_discover_tests(url_tools_test)
gtest_discover_tests(url_test)
gtest_discover_tests(integration_test)

# Add custom target to run tests
add_custom_target(check
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS url_tools_test
)
