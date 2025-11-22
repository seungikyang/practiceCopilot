#include "book.h"
#include "database.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Add a new book to the library database.
 * 
 * @param title The title of the book.
 * @param author The author of the book.
 * @param publisher The publisher of the book.
 * @param publication_year The publication year.
 * @param isbn The ISBN of the book.
 * @param genre The genre of the book.
 * @param quantity The total quantity of the book.
 * @return int Returns 0 on success, -1 on failure.
 */
int add_book(const char *title, const char *author, const char *publisher, 
             int publication_year, const char *isbn, const char *genre, int quantity) {
    sqlite3 *db = get_db_connection();
    if (db == NULL) {
        fprintf(stderr, "Database not initialized\n");
        return -1;
    }
    
    if (title == NULL || isbn == NULL) {
        fprintf(stderr, "Title and ISBN are required\n");
        return -1;
    }
    
    const char *sql = "INSERT INTO Books (title, author, publisher, publication_year, isbn, genre, quantity, available) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?, ?);";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    /* Bind parameters */
    sqlite3_bind_text(stmt, 1, title, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, author ? author : "", -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, publisher ? publisher : "", -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, publication_year);
    sqlite3_bind_text(stmt, 5, isbn, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, genre ? genre : "", -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 7, quantity);
    sqlite3_bind_int(stmt, 8, quantity);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to insert book: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    printf("Book added successfully (ID: %lld)\n", sqlite3_last_insert_rowid(db));
    return 0;
}

/**
 * @brief Search for books by keyword (title, author, or ISBN).
 * 
 * @param keyword The search keyword.
 * @return int Returns number of books found, -1 on failure.
 */
int search_book(const char *keyword) {
    sqlite3 *db = get_db_connection();
    if (db == NULL) {
        fprintf(stderr, "Database not initialized\n");
        return -1;
    }
    
    if (keyword == NULL) {
        fprintf(stderr, "Search keyword is required\n");
        return -1;
    }
    
    const char *sql = "SELECT book_id, title, author, publisher, publication_year, isbn, genre, quantity, available "
                      "FROM Books "
                      "WHERE title LIKE ? OR author LIKE ? OR isbn LIKE ?;";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    /* Create search pattern */
    char pattern[256];
    snprintf(pattern, sizeof(pattern), "%%%s%%", keyword);
    
    sqlite3_bind_text(stmt, 1, pattern, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, pattern, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, pattern, -1, SQLITE_TRANSIENT);
    
    printf("\n=== Search Results ===\n");
    printf("%-5s %-30s %-20s %-20s %-6s %-15s %-15s %-8s %-8s\n",
           "ID", "Title", "Author", "Publisher", "Year", "ISBN", "Genre", "Quantity", "Available");
    printf("---------------------------------------------------------------------------------------------------------------------------\n");
    
    int count = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int book_id = sqlite3_column_int(stmt, 0);
        const unsigned char *title = sqlite3_column_text(stmt, 1);
        const unsigned char *author = sqlite3_column_text(stmt, 2);
        const unsigned char *publisher = sqlite3_column_text(stmt, 3);
        int year = sqlite3_column_int(stmt, 4);
        const unsigned char *isbn = sqlite3_column_text(stmt, 5);
        const unsigned char *genre = sqlite3_column_text(stmt, 6);
        int quantity = sqlite3_column_int(stmt, 7);
        int available = sqlite3_column_int(stmt, 8);
        
        printf("%-5d %-30s %-20s %-20s %-6d %-15s %-15s %-8d %-8d\n",
               book_id, title, author, publisher, year, isbn, genre, quantity, available);
        count++;
    }
    
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Error during query execution: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    printf("\nTotal books found: %d\n", count);
    return count;
}

/**
 * @brief Search for a book by its ID.
 * 
 * @param book_id The ID of the book.
 * @param book Pointer to Book structure to store the result.
 * @return int Returns 0 on success, -1 on failure.
 */
int get_book_by_id(int book_id, Book *book) {
    sqlite3 *db = get_db_connection();
    if (db == NULL) {
        fprintf(stderr, "Database not initialized\n");
        return -1;
    }
    
    if (book == NULL) {
        fprintf(stderr, "Book pointer is NULL\n");
        return -1;
    }
    
    const char *sql = "SELECT book_id, title, author, publisher, publication_year, isbn, genre, quantity, available "
                      "FROM Books WHERE book_id = ?;";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, book_id);
    
    rc = sqlite3_step(stmt);
    
    if (rc == SQLITE_ROW) {
        book->book_id = sqlite3_column_int(stmt, 0);
        strncpy(book->title, (const char*)sqlite3_column_text(stmt, 1), sizeof(book->title) - 1);
        book->title[sizeof(book->title) - 1] = '\0';
        strncpy(book->author, (const char*)sqlite3_column_text(stmt, 2), sizeof(book->author) - 1);
        book->author[sizeof(book->author) - 1] = '\0';
        strncpy(book->publisher, (const char*)sqlite3_column_text(stmt, 3), sizeof(book->publisher) - 1);
        book->publisher[sizeof(book->publisher) - 1] = '\0';
        book->publication_year = sqlite3_column_int(stmt, 4);
        strncpy(book->isbn, (const char*)sqlite3_column_text(stmt, 5), sizeof(book->isbn) - 1);
        book->isbn[sizeof(book->isbn) - 1] = '\0';
        strncpy(book->genre, (const char*)sqlite3_column_text(stmt, 6), sizeof(book->genre) - 1);
        book->genre[sizeof(book->genre) - 1] = '\0';
        book->quantity = sqlite3_column_int(stmt, 7);
        book->available = sqlite3_column_int(stmt, 8);
        
        sqlite3_finalize(stmt);
        return 0;
    } else if (rc == SQLITE_DONE) {
        fprintf(stderr, "Book not found (ID: %d)\n", book_id);
        sqlite3_finalize(stmt);
        return -1;
    } else {
        fprintf(stderr, "Error during query execution: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }
}

/**
 * @brief Update book information.
 * 
 * @param book_id The ID of the book to update.
 * @param title The new title (NULL to keep unchanged).
 * @param author The new author (NULL to keep unchanged).
 * @param publisher The new publisher (NULL to keep unchanged).
 * @param publication_year The new publication year (0 to keep unchanged).
 * @param genre The new genre (NULL to keep unchanged).
 * @return int Returns 0 on success, -1 on failure.
 */
int update_book(int book_id, const char *title, const char *author, 
                const char *publisher, int publication_year, const char *genre) {
    sqlite3 *db = get_db_connection();
    if (db == NULL) {
        fprintf(stderr, "Database not initialized\n");
        return -1;
    }
    
    char sql[1024] = "UPDATE Books SET ";
    char updates[512] = "";
    int first = 1;
    
    if (title != NULL) {
        strcat(updates, first ? "title = ?" : ", title = ?");
        first = 0;
    }
    if (author != NULL) {
        strcat(updates, first ? "author = ?" : ", author = ?");
        first = 0;
    }
    if (publisher != NULL) {
        strcat(updates, first ? "publisher = ?" : ", publisher = ?");
        first = 0;
    }
    if (publication_year > 0) {
        strcat(updates, first ? "publication_year = ?" : ", publication_year = ?");
        first = 0;
    }
    if (genre != NULL) {
        strcat(updates, first ? "genre = ?" : ", genre = ?");
        first = 0;
    }
    
    if (first) {
        fprintf(stderr, "No fields to update\n");
        return -1;
    }
    
    strcat(sql, updates);
    strcat(sql, " WHERE book_id = ?;");
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    int param_index = 1;
    if (title != NULL) sqlite3_bind_text(stmt, param_index++, title, -1, SQLITE_STATIC);
    if (author != NULL) sqlite3_bind_text(stmt, param_index++, author, -1, SQLITE_STATIC);
    if (publisher != NULL) sqlite3_bind_text(stmt, param_index++, publisher, -1, SQLITE_STATIC);
    if (publication_year > 0) sqlite3_bind_int(stmt, param_index++, publication_year);
    if (genre != NULL) sqlite3_bind_text(stmt, param_index++, genre, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, param_index, book_id);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to update book: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    if (sqlite3_changes(db) == 0) {
        fprintf(stderr, "Book not found (ID: %d)\n", book_id);
        return -1;
    }
    
    printf("Book updated successfully\n");
    return 0;
}

/**
 * @brief Delete a book from the database.
 * 
 * @param book_id The ID of the book to delete.
 * @return int Returns 0 on success, -1 on failure.
 */
int delete_book(int book_id) {
    sqlite3 *db = get_db_connection();
    if (db == NULL) {
        fprintf(stderr, "Database not initialized\n");
        return -1;
    }
    
    const char *sql = "DELETE FROM Books WHERE book_id = ?;";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, book_id);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to delete book: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    if (sqlite3_changes(db) == 0) {
        fprintf(stderr, "Book not found (ID: %d)\n", book_id);
        return -1;
    }
    
    printf("Book deleted successfully\n");
    return 0;
}

/**
 * @brief Display all books in the library.
 * 
 * @return int Returns number of books displayed, -1 on failure.
 */
int display_all_books(void) {
    sqlite3 *db = get_db_connection();
    if (db == NULL) {
        fprintf(stderr, "Database not initialized\n");
        return -1;
    }
    
    const char *sql = "SELECT book_id, title, author, publisher, publication_year, isbn, genre, quantity, available "
                      "FROM Books ORDER BY book_id;";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    printf("\n=== All Books ===\n");
    printf("%-5s %-30s %-20s %-20s %-6s %-15s %-15s %-8s %-8s\n",
           "ID", "Title", "Author", "Publisher", "Year", "ISBN", "Genre", "Quantity", "Available");
    printf("---------------------------------------------------------------------------------------------------------------------------\n");
    
    int count = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int book_id = sqlite3_column_int(stmt, 0);
        const unsigned char *title = sqlite3_column_text(stmt, 1);
        const unsigned char *author = sqlite3_column_text(stmt, 2);
        const unsigned char *publisher = sqlite3_column_text(stmt, 3);
        int year = sqlite3_column_int(stmt, 4);
        const unsigned char *isbn = sqlite3_column_text(stmt, 5);
        const unsigned char *genre = sqlite3_column_text(stmt, 6);
        int quantity = sqlite3_column_int(stmt, 7);
        int available = sqlite3_column_int(stmt, 8);
        
        printf("%-5d %-30s %-20s %-20s %-6d %-15s %-15s %-8d %-8d\n",
               book_id, title, author, publisher, year, isbn, genre, quantity, available);
        count++;
    }
    
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Error during query execution: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    printf("\nTotal books: %d\n", count);
    return count;
}

/**
 * @brief Check if a book is available for loan.
 * 
 * @param book_id The ID of the book.
 * @return int Returns 1 if available, 0 if not available, -1 on failure.
 */
int check_book_availability(int book_id) {
    sqlite3 *db = get_db_connection();
    if (db == NULL) {
        fprintf(stderr, "Database not initialized\n");
        return -1;
    }
    
    const char *sql = "SELECT available FROM Books WHERE book_id = ?;";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, book_id);
    
    rc = sqlite3_step(stmt);
    
    if (rc == SQLITE_ROW) {
        int available = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        return (available > 0) ? 1 : 0;
    } else {
        fprintf(stderr, "Book not found (ID: %d)\n", book_id);
        sqlite3_finalize(stmt);
        return -1;
    }
}

/**
 * @brief Update the available quantity of a book.
 * 
 * @param book_id The ID of the book.
 * @param change The change in available quantity (positive or negative).
 * @return int Returns 0 on success, -1 on failure.
 */
int update_book_availability(int book_id, int change) {
    sqlite3 *db = get_db_connection();
    if (db == NULL) {
        fprintf(stderr, "Database not initialized\n");
        return -1;
    }
    
    const char *sql = "UPDATE Books SET available = available + ? WHERE book_id = ?;";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, change);
    sqlite3_bind_int(stmt, 2, book_id);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to update book availability: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    if (sqlite3_changes(db) == 0) {
        fprintf(stderr, "Book not found (ID: %d)\n", book_id);
        return -1;
    }
    
    return 0;
}

/**
 * @brief Search books by genre.
 * 
 * @param genre The genre to search for.
 * @return int Returns number of books found, -1 on failure.
 */
int search_books_by_genre(const char *genre) {
    sqlite3 *db = get_db_connection();
    if (db == NULL) {
        fprintf(stderr, "Database not initialized\n");
        return -1;
    }
    
    if (genre == NULL) {
        fprintf(stderr, "Genre is required\n");
        return -1;
    }
    
    const char *sql = "SELECT book_id, title, author, publisher, publication_year, isbn, genre, quantity, available "
                      "FROM Books WHERE genre LIKE ?;";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    char pattern[128];
    snprintf(pattern, sizeof(pattern), "%%%s%%", genre);
    sqlite3_bind_text(stmt, 1, pattern, -1, SQLITE_TRANSIENT);
    
    printf("\n=== Books by Genre: %s ===\n", genre);
    printf("%-5s %-30s %-20s %-20s %-6s %-15s %-8s %-8s\n",
           "ID", "Title", "Author", "Publisher", "Year", "ISBN", "Quantity", "Available");
    printf("---------------------------------------------------------------------------------------------------------------\n");
    
    int count = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int book_id = sqlite3_column_int(stmt, 0);
        const unsigned char *title = sqlite3_column_text(stmt, 1);
        const unsigned char *author = sqlite3_column_text(stmt, 2);
        const unsigned char *publisher = sqlite3_column_text(stmt, 3);
        int year = sqlite3_column_int(stmt, 4);
        const unsigned char *isbn = sqlite3_column_text(stmt, 5);
        int quantity = sqlite3_column_int(stmt, 7);
        int available = sqlite3_column_int(stmt, 8);
        
        printf("%-5d %-30s %-20s %-20s %-6d %-15s %-8d %-8d\n",
               book_id, title, author, publisher, year, isbn, quantity, available);
        count++;
    }
    
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Error during query execution: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    printf("\nTotal books found: %d\n", count);
    return count;
}

/**
 * @brief Search books by author.
 * 
 * @param author The author name to search for.
 * @return int Returns number of books found, -1 on failure.
 */
int search_books_by_author(const char *author) {
    sqlite3 *db = get_db_connection();
    if (db == NULL) {
        fprintf(stderr, "Database not initialized\n");
        return -1;
    }
    
    if (author == NULL) {
        fprintf(stderr, "Author name is required\n");
        return -1;
    }
    
    const char *sql = "SELECT book_id, title, author, publisher, publication_year, isbn, genre, quantity, available "
                      "FROM Books WHERE author LIKE ?;";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    char pattern[128];
    snprintf(pattern, sizeof(pattern), "%%%s%%", author);
    sqlite3_bind_text(stmt, 1, pattern, -1, SQLITE_TRANSIENT);
    
    printf("\n=== Books by Author: %s ===\n", author);
    printf("%-5s %-30s %-20s %-6s %-15s %-15s %-8s %-8s\n",
           "ID", "Title", "Publisher", "Year", "ISBN", "Genre", "Quantity", "Available");
    printf("---------------------------------------------------------------------------------------------------------------\n");
    
    int count = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int book_id = sqlite3_column_int(stmt, 0);
        const unsigned char *title = sqlite3_column_text(stmt, 1);
        const unsigned char *publisher = sqlite3_column_text(stmt, 3);
        int year = sqlite3_column_int(stmt, 4);
        const unsigned char *isbn = sqlite3_column_text(stmt, 5);
        const unsigned char *genre = sqlite3_column_text(stmt, 6);
        int quantity = sqlite3_column_int(stmt, 7);
        int available = sqlite3_column_int(stmt, 8);
        
        printf("%-5d %-30s %-20s %-6d %-15s %-15s %-8d %-8d\n",
               book_id, title, publisher, year, isbn, genre, quantity, available);
        count++;
    }
    
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Error during query execution: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    printf("\nTotal books found: %d\n", count);
    return count;
}