cmake_minimum_required(VERSION 3.14)
project(Task04Tests C CXX)

# Set C and C++ standards
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable testing
enable_testing()

# Add include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Create libraries for testable modules
add_library(discount_lib src/getDiscount.c)
target_include_directories(discount_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(date_utils_lib src/date_utils.c src/date_utils.h)
target_include_directories(date_utils_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Google Test setup
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Test: getDiscount module
add_executable(test_discount
    tests/test_discount_gtest.cpp
)
target_link_libraries(test_discount
    discount_lib
    gtest
    gtest_main
)

# Test: date_utils module
add_executable(test_date_utils
    tests/test_date_utils_gtest.cpp
)
target_link_libraries(test_date_utils
    date_utils_lib
    gtest
    gtest_main
)

# Discover tests
include(GoogleTest)
gtest_discover_tests(test_discount)
gtest_discover_tests(test_date_utils)

# Add custom target to run all tests
add_custom_target(check
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS test_discount test_date_utils
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
