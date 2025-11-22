/**
 * @file test_book_gtest.cpp
 * @brief Google Test based unit tests for Book module
 * 
 * This file contains comprehensive unit tests for the Book module
 * using Google Test framework and SQLite in-memory database.
 */

#include <gtest/gtest.h>
#include <sqlite3.h>
#include <cstring>

extern "C" {
    #include "../include/database.h"
    #include "../include/book.h"
}

// We need to directly manipulate the internal database connection
// Since database.c uses a static variable, we'll use file operations
extern "C" {
    extern sqlite3* get_db_connection();
    extern void set_db_connection(sqlite3* new_db);
}

// Test fixture class for Book module tests
class BookTest : public ::testing::Test {
protected:
    sqlite3* test_db;
    sqlite3* saved_db;
    
    // Setup: Create in-memory database before each test
    void SetUp() override {
        test_db = nullptr;
        
        // Save current database connection
        saved_db = get_db_connection();
        
        // Open in-memory database
        int rc = sqlite3_open(":memory:", &test_db);
        ASSERT_EQ(rc, SQLITE_OK) << "Failed to open in-memory database";
        
        // Set the global database connection to our test database
        set_db_connection(test_db);
        
        // Enable foreign keys
        const char* fk_sql = "PRAGMA foreign_keys = ON;";
        sqlite3_exec(test_db, fk_sql, nullptr, nullptr, nullptr);
        
        // Create tables manually for testing
        const char* create_books_sql = 
            "CREATE TABLE IF NOT EXISTS Books ("
            "book_id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "title TEXT NOT NULL,"
            "author TEXT,"
            "isbn TEXT UNIQUE,"
            "genre TEXT,"
            "publisher TEXT,"
            "publication_year INTEGER,"
            "quantity INTEGER DEFAULT 0,"
            "available INTEGER DEFAULT 0"
            ");";
        
        rc = sqlite3_exec(test_db, create_books_sql, nullptr, nullptr, nullptr);
        ASSERT_EQ(rc, SQLITE_OK) << "Failed to create Books table";
    }

    // Teardown: Close database after each test
    void TearDown() override {
        // Restore original database connection
        set_db_connection(saved_db);
        
        if (test_db) {
            sqlite3_close(test_db);
            test_db = nullptr;
        }
    }

    // Helper function to count books in database
    int count_books() {
        const char* sql = "SELECT COUNT(*) FROM Books;";
        sqlite3_stmt* stmt;
        int count = 0;

        if (sqlite3_prepare_v2(test_db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                count = sqlite3_column_int(stmt, 0);
            }
            sqlite3_finalize(stmt);
        }
        return count;
    }

    // Helper function to check if book exists
    bool book_exists(int book_id) {
        Book book;
        return get_book_by_id(book_id, &book) == 0;
    }

    // Helper function to get book ID by ISBN
    int get_book_id_by_isbn(const char* isbn) {
        const char* sql = "SELECT book_id FROM Books WHERE isbn = ?;";
        sqlite3_stmt* stmt;
        int book_id = -1;

        if (sqlite3_prepare_v2(test_db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, isbn, -1, SQLITE_STATIC);
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                book_id = sqlite3_column_int(stmt, 0);
            }
            sqlite3_finalize(stmt);
        }
        return book_id;
    }
};

// ============================================================================
// Test Suite 1: add_book() tests
// ============================================================================

TEST_F(BookTest, AddBookSuccess) {
    int result = add_book("Test Book", "Test Author", "Test Publisher", 
                          2024, "1234567890", "Fiction", 5);
    
    EXPECT_EQ(result, 0) << "add_book should return 0 on success";
    EXPECT_EQ(count_books(), 1) << "Database should contain 1 book";
    
    int book_id = get_book_id_by_isbn("1234567890");
    EXPECT_GT(book_id, 0) << "Book should have valid ID";
}

TEST_F(BookTest, AddBookNullTitle) {
    int result = add_book(nullptr, "Author", "Publisher", 
                          2024, "1234567890", "Fiction", 5);
    
    EXPECT_EQ(result, -1) << "Should fail when title is NULL";
    EXPECT_EQ(count_books(), 0) << "No book should be added";
}

TEST_F(BookTest, AddBookNullDatabase) {
    // Save current connection and set to NULL
    set_db_connection(nullptr);
    
    int result = add_book("Title", "Author", "Publisher", 
                          2024, "1234567890", "Fiction", 5);
    
    EXPECT_EQ(result, -1) << "Should fail when database is NULL";
    
    // Restore connection
    set_db_connection(test_db);
}

TEST_F(BookTest, AddBookDuplicateISBN) {
    // Add first book
    int result1 = add_book("Book 1", "Author 1", "Publisher 1", 
                           2024, "1234567890", "Fiction", 5);
    ASSERT_EQ(result1, 0);
    
    // Try to add book with same ISBN
    int result2 = add_book("Book 2", "Author 2", "Publisher 2", 
                           2024, "1234567890", "Science", 3);
    
    EXPECT_EQ(result2, -1) << "Should fail due to duplicate ISBN";
    EXPECT_EQ(count_books(), 1) << "Only first book should exist";
}

// ============================================================================
// Test Suite 2: search_book() tests
// ============================================================================

TEST_F(BookTest, SearchBooksSuccess) {
    // Add test books
    add_book("C Programming", "Dennis Ritchie", "Prentice Hall", 
             1988, "1111111111", "Programming", 3);
    add_book("Python Programming", "Guido van Rossum", "O'Reilly", 
             1995, "2222222222", "Programming", 5);
    
    int count = search_book("Programming");
    
    EXPECT_EQ(count, 2) << "Should find 2 books";
}

TEST_F(BookTest, SearchBooksNoResults) {
    add_book("Test Book", "Test Author", "Publisher", 
             2024, "1234567890", "Fiction", 5);
    
    int count = search_book("Nonexistent");
    
    EXPECT_EQ(count, 0) << "Should find 0 books";
}

TEST_F(BookTest, SearchBooksNullKeyword) {
    int count = search_book(nullptr);
    
    EXPECT_EQ(count, -1) << "Should return error for NULL keyword";
}

TEST_F(BookTest, SearchBooksPartialMatch) {
    add_book("The Great Gatsby", "F. Scott Fitzgerald", "Scribner", 
             1925, "1111111111", "Fiction", 2);
    
    int count = search_book("Great");
    
    EXPECT_EQ(count, 1) << "Should find book with partial title match";
}

// ============================================================================
// Test Suite 3: get_book_by_id() tests
// ============================================================================

TEST_F(BookTest, GetBookByIdSuccess) {
    add_book("Test Book", "Test Author", "Test Publisher", 
             2024, "1234567890", "Fiction", 5);
    
    int book_id = get_book_id_by_isbn("1234567890");
    ASSERT_GT(book_id, 0);
    
    Book book;
    int result = get_book_by_id(book_id, &book);
    
    ASSERT_EQ(result, 0) << "Should successfully retrieve book";
    EXPECT_EQ(book.book_id, book_id);
    EXPECT_STREQ(book.title, "Test Book");
    EXPECT_STREQ(book.author, "Test Author");
    EXPECT_STREQ(book.isbn, "1234567890");
    EXPECT_EQ(book.quantity, 5);
    EXPECT_EQ(book.available, 5);
}

TEST_F(BookTest, GetBookByIdNotFound) {
    Book book;
    int result = get_book_by_id(99999, &book);
    
    EXPECT_EQ(result, -1) << "Should return error for non-existent book";
}

TEST_F(BookTest, GetBookByIdNullBook) {
    add_book("Test Book", "Test Author", "Publisher", 
             2024, "1234567890", "Fiction", 5);
    
    int book_id = get_book_id_by_isbn("1234567890");
    ASSERT_GT(book_id, 0);
    
    int result = get_book_by_id(book_id, nullptr);
    
    EXPECT_EQ(result, -1) << "Should return error for NULL book pointer";
}

// ============================================================================
// Test Suite 4: update_book() tests
// ============================================================================

TEST_F(BookTest, UpdateBookSuccess) {
    add_book("Original Title", "Original Author", "Original Publisher", 
             2020, "1111111111", "Fiction", 3);
    
    int book_id = get_book_id_by_isbn("1111111111");
    ASSERT_GT(book_id, 0);
    
    int result = update_book(book_id, "Updated Title", "Updated Author", 
                            "Updated Publisher", 2024, "Science");
    
    ASSERT_EQ(result, 0) << "Update should succeed";
    
    // Verify update
    Book book;
    get_book_by_id(book_id, &book);
    EXPECT_STREQ(book.title, "Updated Title");
    EXPECT_STREQ(book.author, "Updated Author");
    EXPECT_EQ(book.publication_year, 2024);
}

TEST_F(BookTest, UpdateBookNotFound) {
    int result = update_book(99999, "Title", "Author", "Publisher", 2024, "Fiction");
    
    EXPECT_EQ(result, -1) << "Should fail for non-existent book";
}

TEST_F(BookTest, UpdateBookPartialUpdate) {
    add_book("Original Title", "Original Author", "Original Publisher", 
             2020, "1111111111", "Fiction", 3);
    
    int book_id = get_book_id_by_isbn("1111111111");
    ASSERT_GT(book_id, 0);
    
    // Update only title (pass NULL for others)
    int result = update_book(book_id, "New Title", nullptr, nullptr, 0, nullptr);
    
    EXPECT_EQ(result, 0) << "Partial update should succeed";
    
    Book book;
    get_book_by_id(book_id, &book);
    EXPECT_STREQ(book.title, "New Title");
    EXPECT_STREQ(book.author, "Original Author"); // Should remain unchanged
}

TEST_F(BookTest, UpdateBookNullDatabase) {
    // Save current connection and set to NULL
    set_db_connection(nullptr);
    
    int result = update_book(1, "Title", "Author", "Publisher", 2024, "Fiction");
    
    EXPECT_EQ(result, -1) << "Should fail when database is NULL";
    
    // Restore connection
    set_db_connection(test_db);
}

// ============================================================================
// Test Suite 5: delete_book() tests
// ============================================================================

TEST_F(BookTest, DeleteBookSuccess) {
    add_book("To Delete", "Author", "Publisher", 
             2024, "1234567890", "Fiction", 5);
    
    int book_id = get_book_id_by_isbn("1234567890");
    ASSERT_GT(book_id, 0);
    ASSERT_EQ(count_books(), 1);
    
    int result = delete_book(book_id);
    
    EXPECT_EQ(result, 0) << "Delete should succeed";
    EXPECT_EQ(count_books(), 0) << "Book should be removed from database";
    EXPECT_FALSE(book_exists(book_id)) << "Deleted book should not exist";
}

TEST_F(BookTest, DeleteBookNotFound) {
    int result = delete_book(99999);
    
    EXPECT_EQ(result, -1) << "Should fail for non-existent book";
}

// ============================================================================
// Test Suite 6: Book availability tests
// ============================================================================

TEST_F(BookTest, CheckBookAvailabilityAvailable) {
    add_book("Available Book", "Author", "Publisher", 
             2024, "1234567890", "Fiction", 5);
    
    int book_id = get_book_id_by_isbn("1234567890");
    ASSERT_GT(book_id, 0);
    
    int available = check_book_availability(book_id);
    
    EXPECT_EQ(available, 1) << "Book should be available";
}

TEST_F(BookTest, CheckBookAvailabilityNotAvailable) {
    add_book("Unavailable Book", "Author", "Publisher", 
             2024, "1234567890", "Fiction", 0);
    
    int book_id = get_book_id_by_isbn("1234567890");
    ASSERT_GT(book_id, 0);
    
    // Set available count to 0
    const char* sql = "UPDATE Books SET available = 0 WHERE book_id = ?;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(test_db, sql, -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, book_id);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    int available = check_book_availability(book_id);
    
    EXPECT_EQ(available, 0) << "Book should not be available";
}

TEST_F(BookTest, CheckBookAvailabilityNotFound) {
    int available = check_book_availability(99999);
    
    EXPECT_EQ(available, -1) << "Should return error for non-existent book";
}

TEST_F(BookTest, UpdateBookAvailabilityIncrease) {
    add_book("Test Book", "Author", "Publisher", 
             2024, "1234567890", "Fiction", 5);
    
    int book_id = get_book_id_by_isbn("1234567890");
    ASSERT_GT(book_id, 0);
    
    int result = update_book_availability(book_id, 1);
    
    EXPECT_EQ(result, 0) << "Update should succeed";
    
    Book book;
    get_book_by_id(book_id, &book);
    EXPECT_EQ(book.available, 6) << "Available count should increase by 1";
}

TEST_F(BookTest, UpdateBookAvailabilityDecrease) {
    add_book("Test Book", "Author", "Publisher", 
             2024, "1234567890", "Fiction", 5);
    
    int book_id = get_book_id_by_isbn("1234567890");
    ASSERT_GT(book_id, 0);
    
    int result = update_book_availability(book_id, -1);
    
    EXPECT_EQ(result, 0) << "Update should succeed";
    
    Book book;
    get_book_by_id(book_id, &book);
    EXPECT_EQ(book.available, 4) << "Available count should decrease by 1";
}

// ============================================================================
// Test Suite 7: Search by genre and author tests
// ============================================================================

TEST_F(BookTest, SearchBooksByGenreSuccess) {
    add_book("Fiction Book 1", "Author 1", "Publisher 1", 
             2024, "1111111111", "Fiction", 3);
    add_book("Fiction Book 2", "Author 2", "Publisher 2", 
             2024, "2222222222", "Fiction", 5);
    add_book("Science Book", "Author 3", "Publisher 3", 
             2024, "3333333333", "Science", 2);
    
    int count = search_books_by_genre("Fiction");
    
    EXPECT_EQ(count, 2) << "Should find 2 fiction books";
}

TEST_F(BookTest, SearchBooksByGenreNoResults) {
    add_book("Fiction Book", "Author", "Publisher", 
             2024, "1234567890", "Fiction", 5);
    
    int count = search_books_by_genre("Horror");
    
    EXPECT_EQ(count, 0) << "Should find 0 books";
}

TEST_F(BookTest, SearchBooksByAuthorSuccess) {
    add_book("Book 1", "John Doe", "Publisher 1", 
             2024, "1111111111", "Fiction", 3);
    add_book("Book 2", "John Doe", "Publisher 2", 
             2024, "2222222222", "Science", 5);
    add_book("Book 3", "Jane Smith", "Publisher 3", 
             2024, "3333333333", "Fiction", 2);
    
    int count = search_books_by_author("John Doe");
    
    EXPECT_EQ(count, 2) << "Should find 2 books by John Doe";
}

TEST_F(BookTest, SearchBooksByAuthorNoResults) {
    add_book("Book", "John Doe", "Publisher", 
             2024, "1234567890", "Fiction", 5);
    
    int count = search_books_by_author("Unknown Author");
    
    EXPECT_EQ(count, 0) << "Should find 0 books";
}

TEST_F(BookTest, SearchBooksByAuthorNullAuthor) {
    int count = search_books_by_author(nullptr);
    
    EXPECT_EQ(count, -1) << "Should return error for NULL author";
}

// ============================================================================
// Test Suite 8: Display all books test
// ============================================================================

TEST_F(BookTest, DisplayAllBooks) {
    add_book("Book 1", "Author 1", "Publisher 1", 2024, "1111111111", "Fiction", 3);
    add_book("Book 2", "Author 2", "Publisher 2", 2023, "2222222222", "Science", 5);
    
    int count = display_all_books();
    
    EXPECT_EQ(count, 2) << "Should display 2 books";
}

TEST_F(BookTest, DisplayAllBooksEmpty) {
    int count = display_all_books();
    
    EXPECT_EQ(count, 0) << "Should display 0 books when database is empty";
}

// ============================================================================
// Main function
// ============================================================================

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
