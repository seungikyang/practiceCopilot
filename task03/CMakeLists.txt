cmake_minimum_required(VERSION 3.15)
project(LibraryManagementSystem C CXX)

# Set C and C++ standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add compile options
add_compile_options(-Wall -Wextra)

# Find SQLite3
find_package(SQLite3 REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${SQLite3_INCLUDE_DIRS})

# Source files for the main library (excluding main.c)
set(LIB_SOURCES
    src/database.c
    src/book.c
    src/member.c
    src/loan.c
)

# Create a library from common sources
add_library(library_core STATIC ${LIB_SOURCES})
target_link_libraries(library_core ${SQLite3_LIBRARIES})

# Main executable
add_executable(library src/main.c)
target_link_libraries(library library_core ${SQLite3_LIBRARIES})

# Set output directory
set_target_properties(library PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Enable testing
enable_testing()

# Download Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Add subdirectory for tests
add_subdirectory(tests)

# Custom target to run all tests
add_custom_target(check
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS test_book test_book_gtest
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running all tests..."
)

# Print configuration summary
message(STATUS "")
message(STATUS "===========================================")
message(STATUS "  Library Management System Configuration")
message(STATUS "===========================================")
message(STATUS "  C Compiler:    ${CMAKE_C_COMPILER}")
message(STATUS "  Build Type:    ${CMAKE_BUILD_TYPE}")
message(STATUS "  SQLite3:       ${SQLite3_LIBRARIES}")
message(STATUS "  Install Path:  ${CMAKE_INSTALL_PREFIX}")
message(STATUS "===========================================")
message(STATUS "")
