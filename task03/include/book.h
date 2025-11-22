#ifndef BOOK_H
#define BOOK_H

#include <sqlite3.h>

/**
 * @brief Structure representing a book in the library.
 */
typedef struct {
    int book_id;
    char title[100];
    char author[50];
    char isbn[20];
    char genre[30];
    char publisher[50];
    int publication_year;
    int quantity;
    int available;
} Book;

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
             int publication_year, const char *isbn, const char *genre, int quantity);

/**
 * @brief Search for books by keyword (title, author, or ISBN).
 * 
 * @param keyword The search keyword.
 * @return int Returns number of books found, -1 on failure.
 */
int search_book(const char *keyword);

/**
 * @brief Search for a book by its ID.
 * 
 * @param book_id The ID of the book.
 * @param book Pointer to Book structure to store the result.
 * @return int Returns 0 on success, -1 on failure.
 */
int get_book_by_id(int book_id, Book *book);

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
                const char *publisher, int publication_year, const char *genre);

/**
 * @brief Delete a book from the database.
 * 
 * @param book_id The ID of the book to delete.
 * @return int Returns 0 on success, -1 on failure.
 */
int delete_book(int book_id);

/**
 * @brief Display all books in the library.
 * 
 * @return int Returns number of books displayed, -1 on failure.
 */
int display_all_books(void);

/**
 * @brief Check if a book is available for loan.
 * 
 * @param book_id The ID of the book.
 * @return int Returns 1 if available, 0 if not available, -1 on failure.
 */
int check_book_availability(int book_id);

/**
 * @brief Update the available quantity of a book.
 * 
 * @param book_id The ID of the book.
 * @param change The change in available quantity (positive or negative).
 * @return int Returns 0 on success, -1 on failure.
 */
int update_book_availability(int book_id, int change);

/**
 * @brief Search books by genre.
 * 
 * @param genre The genre to search for.
 * @return int Returns number of books found, -1 on failure.
 */
int search_books_by_genre(const char *genre);

/**
 * @brief Search books by author.
 * 
 * @param author The author name to search for.
 * @return int Returns number of books found, -1 on failure.
 */
int search_books_by_author(const char *author);

#endif // BOOK_H