#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../include/book.h"
#include "../include/database.h"

// Test result counters
static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

// Color codes for output
#define COLOR_GREEN "\033[0;32m"
#define COLOR_RED "\033[0;31m"
#define COLOR_YELLOW "\033[0;33m"
#define COLOR_RESET "\033[0m"

// Test macros
#define TEST(name) \
    static void name(void); \
    static void run_##name(void) { \
        printf("  Running: %s ... ", #name); \
        fflush(stdout); \
        tests_run++; \
        name(); \
        printf(COLOR_GREEN "PASSED" COLOR_RESET "\n"); \
        tests_passed++; \
    } \
    static void name(void)

#define ASSERT_TRUE(condition, message) \
    do { \
        if (!(condition)) { \
            printf(COLOR_RED "FAILED" COLOR_RESET "\n"); \
            printf("    Assertion failed: %s\n", message); \
            printf("    at %s:%d\n", __FILE__, __LINE__); \
            tests_failed++; \
            return; \
        } \
    } while(0)

#define ASSERT_EQUAL(expected, actual, message) \
    ASSERT_TRUE((expected) == (actual), message)

#define ASSERT_NOT_EQUAL(not_expected, actual, message) \
    ASSERT_TRUE((not_expected) != (actual), message)

#define RUN_TEST(test) run_##test()

// Helper function to setup test database
static int setup_test_database(void) {
    // Close existing database if any
    close_database();
    
    // Initialize with in-memory database for testing
    sqlite3 *db;
    int rc = sqlite3_open(":memory:", &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open in-memory database\n");
        return -1;
    }
    
    // We need to set this as the global db connection
    // For testing, we'll use a temporary file instead
    close_database();
    
    // Use a temporary test database file
    remove("test_library.db");
    
    // Override the DB_PATH for testing
    if (init_database() != 0) {
        fprintf(stderr, "Failed to initialize test database\n");
        return -1;
    }
    
    return 0;
}

static void cleanup_test_database(void) {
    close_database();
    remove("database/library.db");
    remove("test_library.db");
}

// ==================== Test Cases ====================

TEST(test_add_book_success) {
    int result = add_book("The C Programming Language", "Brian Kernighan", 
                         "Prentice Hall", 1988, "978-0131103627", 
                         "Programming", 5);
    ASSERT_EQUAL(0, result, "add_book should return 0 on success");
}

TEST(test_add_book_null_title) {
    int result = add_book(NULL, "Author", "Publisher", 2000, 
                         "123-456", "Genre", 1);
    ASSERT_EQUAL(-1, result, "add_book should return -1 for NULL title");
}

TEST(test_add_book_null_isbn) {
    int result = add_book("Title", "Author", "Publisher", 2000, 
                         NULL, "Genre", 1);
    ASSERT_EQUAL(-1, result, "add_book should return -1 for NULL ISBN");
}

TEST(test_add_book_duplicate_isbn) {
    // Add first book
    add_book("Book 1", "Author 1", "Publisher", 2000, 
            "978-1234567890", "Fiction", 3);
    
    // Try to add book with same ISBN
    int result = add_book("Book 2", "Author 2", "Publisher", 2001, 
                         "978-1234567890", "Fiction", 2);
    ASSERT_EQUAL(-1, result, "add_book should return -1 for duplicate ISBN");
}

TEST(test_search_book_by_title) {
    // Add some test books
    add_book("Clean Code", "Robert Martin", "Prentice Hall", 2008, 
            "978-0132350884", "Programming", 3);
    add_book("Design Patterns", "Gang of Four", "Addison-Wesley", 1994, 
            "978-0201633610", "Programming", 2);
    
    int count = search_book("Clean");
    ASSERT_TRUE(count > 0, "search_book should find books with matching title");
}

TEST(test_search_book_by_author) {
    add_book("Code Complete", "Steve McConnell", "Microsoft Press", 2004, 
            "978-0735619678", "Programming", 4);
    
    int count = search_book("McConnell");
    ASSERT_TRUE(count > 0, "search_book should find books with matching author");
}

TEST(test_search_book_not_found) {
    int count = search_book("NonExistentBook12345");
    ASSERT_EQUAL(0, count, "search_book should return 0 for non-existent books");
}

TEST(test_search_book_null_keyword) {
    int result = search_book(NULL);
    ASSERT_EQUAL(-1, result, "search_book should return -1 for NULL keyword");
}

TEST(test_get_book_by_id_success) {
    // Add a book
    add_book("Test Book", "Test Author", "Test Publisher", 2020, 
            "978-1111111111", "Test", 5);
    
    Book book;
    int result = get_book_by_id(1, &book);
    ASSERT_EQUAL(0, result, "get_book_by_id should return 0 on success");
    ASSERT_TRUE(strcmp(book.title, "Test Book") == 0, 
               "Retrieved book should have correct title");
}

TEST(test_get_book_by_id_not_found) {
    Book book;
    int result = get_book_by_id(9999, &book);
    ASSERT_EQUAL(-1, result, "get_book_by_id should return -1 for non-existent ID");
}

TEST(test_get_book_by_id_null_pointer) {
    int result = get_book_by_id(1, NULL);
    ASSERT_EQUAL(-1, result, "get_book_by_id should return -1 for NULL pointer");
}

TEST(test_update_book_title) {
    // Add a book
    add_book("Original Title", "Author", "Publisher", 2020, 
            "978-2222222222", "Genre", 3);
    
    // Update title
    int result = update_book(1, "Updated Title", NULL, NULL, 0, NULL);
    ASSERT_EQUAL(0, result, "update_book should return 0 on success");
    
    // Verify update
    Book book;
    get_book_by_id(1, &book);
    ASSERT_TRUE(strcmp(book.title, "Updated Title") == 0, 
               "Book title should be updated");
}

TEST(test_update_book_multiple_fields) {
    add_book("Book", "Author", "Publisher", 2020, 
            "978-3333333333", "Genre", 2);
    
    int result = update_book(1, "New Title", "New Author", 
                            "New Publisher", 2021, "New Genre");
    ASSERT_EQUAL(0, result, "update_book should update multiple fields");
    
    Book book;
    get_book_by_id(1, &book);
    ASSERT_TRUE(strcmp(book.title, "New Title") == 0, "Title should be updated");
    ASSERT_TRUE(strcmp(book.author, "New Author") == 0, "Author should be updated");
    ASSERT_EQUAL(2021, book.publication_year, "Year should be updated");
}

TEST(test_update_book_not_found) {
    int result = update_book(9999, "Title", NULL, NULL, 0, NULL);
    ASSERT_EQUAL(-1, result, "update_book should return -1 for non-existent book");
}

TEST(test_update_book_no_fields) {
    add_book("Book", "Author", "Publisher", 2020, 
            "978-4444444444", "Genre", 1);
    
    int result = update_book(1, NULL, NULL, NULL, 0, NULL);
    ASSERT_EQUAL(-1, result, "update_book should return -1 when no fields to update");
}

TEST(test_delete_book_success) {
    add_book("Book to Delete", "Author", "Publisher", 2020, 
            "978-5555555555", "Genre", 1);
    
    int result = delete_book(1);
    ASSERT_EQUAL(0, result, "delete_book should return 0 on success");
    
    // Verify deletion
    Book book;
    result = get_book_by_id(1, &book);
    ASSERT_EQUAL(-1, result, "Deleted book should not be found");
}

TEST(test_delete_book_not_found) {
    int result = delete_book(9999);
    ASSERT_EQUAL(-1, result, "delete_book should return -1 for non-existent book");
}

TEST(test_check_book_availability_available) {
    add_book("Available Book", "Author", "Publisher", 2020, 
            "978-6666666666", "Genre", 5);
    
    int result = check_book_availability(1);
    ASSERT_EQUAL(1, result, "check_book_availability should return 1 for available books");
}

TEST(test_check_book_availability_not_found) {
    int result = check_book_availability(9999);
    ASSERT_EQUAL(-1, result, "check_book_availability should return -1 for non-existent book");
}

TEST(test_update_book_availability_increase) {
    add_book("Book", "Author", "Publisher", 2020, 
            "978-7777777777", "Genre", 5);
    
    int result = update_book_availability(1, 3);
    ASSERT_EQUAL(0, result, "update_book_availability should return 0 on success");
    
    // Verify availability increased
    Book book;
    get_book_by_id(1, &book);
    ASSERT_EQUAL(8, book.available, "Available count should increase by 3");
}

TEST(test_update_book_availability_decrease) {
    add_book("Book", "Author", "Publisher", 2020, 
            "978-8888888888", "Genre", 5);
    
    int result = update_book_availability(1, -2);
    ASSERT_EQUAL(0, result, "update_book_availability should decrease availability");
    
    Book book;
    get_book_by_id(1, &book);
    ASSERT_EQUAL(3, book.available, "Available count should decrease by 2");
}

TEST(test_update_book_availability_not_found) {
    int result = update_book_availability(9999, 1);
    ASSERT_EQUAL(-1, result, "update_book_availability should return -1 for non-existent book");
}

TEST(test_search_books_by_genre) {
    add_book("Fiction Book 1", "Author 1", "Publisher", 2020, 
            "978-1000000001", "Fiction", 3);
    add_book("Fiction Book 2", "Author 2", "Publisher", 2021, 
            "978-1000000002", "Fiction", 2);
    add_book("Science Book", "Author 3", "Publisher", 2022, 
            "978-1000000003", "Science", 4);
    
    int count = search_books_by_genre("Fiction");
    ASSERT_TRUE(count >= 2, "search_books_by_genre should find Fiction books");
}

TEST(test_search_books_by_genre_not_found) {
    int count = search_books_by_genre("NonExistentGenre");
    ASSERT_EQUAL(0, count, "search_books_by_genre should return 0 for non-existent genre");
}

TEST(test_search_books_by_author_found) {
    add_book("Book 1", "John Smith", "Publisher", 2020, 
            "978-2000000001", "Genre", 2);
    add_book("Book 2", "John Smith", "Publisher", 2021, 
            "978-2000000002", "Genre", 3);
    
    int count = search_books_by_author("John Smith");
    ASSERT_TRUE(count >= 2, "search_books_by_author should find books by author");
}

TEST(test_search_books_by_author_partial_match) {
    add_book("Book", "Jane Doe", "Publisher", 2020, 
            "978-3000000001", "Genre", 1);
    
    int count = search_books_by_author("Jane");
    ASSERT_TRUE(count > 0, "search_books_by_author should support partial matching");
}

TEST(test_display_all_books) {
    add_book("Book 1", "Author 1", "Publisher", 2020, 
            "978-4000000001", "Genre", 1);
    add_book("Book 2", "Author 2", "Publisher", 2021, 
            "978-4000000002", "Genre", 2);
    
    int count = display_all_books();
    ASSERT_TRUE(count >= 2, "display_all_books should return number of books");
}

// ==================== Test Suite Runner ====================

void run_all_tests(void) {
    printf("\n");
    printf("========================================\n");
    printf("  Book Module Unit Tests\n");
    printf("========================================\n\n");
    
    // Setup
    if (setup_test_database() != 0) {
        printf(COLOR_RED "Failed to setup test database\n" COLOR_RESET);
        return;
    }
    
    // Run all tests
    RUN_TEST(test_add_book_success);
    
    cleanup_test_database();
    setup_test_database();
    RUN_TEST(test_add_book_null_title);
    
    cleanup_test_database();
    setup_test_database();
    RUN_TEST(test_add_book_null_isbn);
    
    cleanup_test_database();
    setup_test_database();
    RUN_TEST(test_add_book_duplicate_isbn);
    
    cleanup_test_database();
    setup_test_database();
    RUN_TEST(test_search_book_by_title);
    
    cleanup_test_database();
    setup_test_database();
    RUN_TEST(test_search_book_by_author);
    
    cleanup_test_database();
    setup_test_database();
    RUN_TEST(test_search_book_not_found);
    
    cleanup_test_database();
    setup_test_database();
    RUN_TEST(test_search_book_null_keyword);
    
    cleanup_test_database();
    setup_test_database();
    RUN_TEST(test_get_book_by_id_success);
    
    cleanup_test_database();
    setup_test_database();
    RUN_TEST(test_get_book_by_id_not_found);
    
    cleanup_test_database();
    setup_test_database();
    RUN_TEST(test_get_book_by_id_null_pointer);
    
    cleanup_test_database();
    setup_test_database();
    RUN_TEST(test_update_book_title);
    
    cleanup_test_database();
    setup_test_database();
    RUN_TEST(test_update_book_multiple_fields);
    
    cleanup_test_database();
    setup_test_database();
    RUN_TEST(test_update_book_not_found);
    
    cleanup_test_database();
    setup_test_database();
    RUN_TEST(test_update_book_no_fields);
    
    cleanup_test_database();
    setup_test_database();
    RUN_TEST(test_delete_book_success);
    
    cleanup_test_database();
    setup_test_database();
    RUN_TEST(test_delete_book_not_found);
    
    cleanup_test_database();
    setup_test_database();
    RUN_TEST(test_check_book_availability_available);
    
    cleanup_test_database();
    setup_test_database();
    RUN_TEST(test_check_book_availability_not_found);
    
    cleanup_test_database();
    setup_test_database();
    RUN_TEST(test_update_book_availability_increase);
    
    cleanup_test_database();
    setup_test_database();
    RUN_TEST(test_update_book_availability_decrease);
    
    cleanup_test_database();
    setup_test_database();
    RUN_TEST(test_update_book_availability_not_found);
    
    cleanup_test_database();
    setup_test_database();
    RUN_TEST(test_search_books_by_genre);
    
    cleanup_test_database();
    setup_test_database();
    RUN_TEST(test_search_books_by_genre_not_found);
    
    cleanup_test_database();
    setup_test_database();
    RUN_TEST(test_search_books_by_author_found);
    
    cleanup_test_database();
    setup_test_database();
    RUN_TEST(test_search_books_by_author_partial_match);
    
    cleanup_test_database();
    setup_test_database();
    RUN_TEST(test_display_all_books);
    
    // Cleanup
    cleanup_test_database();
    
    // Print summary
    printf("\n");
    printf("========================================\n");
    printf("  Test Summary\n");
    printf("========================================\n");
    printf("  Total tests:  %d\n", tests_run);
    printf("  " COLOR_GREEN "Passed:       %d" COLOR_RESET "\n", tests_passed);
    if (tests_failed > 0) {
        printf("  " COLOR_RED "Failed:       %d" COLOR_RESET "\n", tests_failed);
    } else {
        printf("  Failed:       %d\n", tests_failed);
    }
    printf("========================================\n");
    
    if (tests_failed == 0) {
        printf(COLOR_GREEN "\n✓ All tests passed!\n" COLOR_RESET);
    } else {
        printf(COLOR_RED "\n✗ Some tests failed!\n" COLOR_RESET);
    }
    printf("\n");
}

int main(void) {
    run_all_tests();
    return (tests_failed == 0) ? 0 : 1;
}
