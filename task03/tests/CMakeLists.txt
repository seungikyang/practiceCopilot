# Test executable for book module
add_executable(test_book test_book.c)

# Link against the library core and SQLite3
target_link_libraries(test_book library_core ${SQLite3_LIBRARIES})

# Set output directory for test executable
set_target_properties(test_book PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests
)

# Add test to CTest
add_test(NAME BookModuleTests
    COMMAND test_book
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Set test properties
set_tests_properties(BookModuleTests PROPERTIES
    TIMEOUT 30
    LABELS "unit;book"
)

# ============================================================================
# Google Test based test executable for book module
# ============================================================================

add_executable(test_book_gtest test_book_gtest.cpp)

# Link against the library core, SQLite3, and Google Test
target_link_libraries(test_book_gtest
    library_core
    ${SQLite3_LIBRARIES}
    gtest
    gtest_main
)

# Set output directory for Google Test executable
set_target_properties(test_book_gtest PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests
)

# Add Google Test to CTest
add_test(NAME BookModuleGoogleTests
    COMMAND test_book_gtest
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Set test properties
set_tests_properties(BookModuleGoogleTests PROPERTIES
    TIMEOUT 30
    LABELS "unit;book;gtest"
)

# Enable Google Test discovery (optional but recommended)
include(GoogleTest)
gtest_discover_tests(test_book_gtest)

# Print test configuration
message(STATUS "  Test: Book Module Unit Tests - ENABLED")
message(STATUS "  Test: Book Module Google Tests - ENABLED")
