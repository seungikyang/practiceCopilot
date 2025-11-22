#ifndef LOAN_H
#define LOAN_H

#include <sqlite3.h>

#define MAX_DATE_LEN 11  // YYYY-MM-DD format

/**
 * @brief Structure representing a loan record.
 */
typedef struct {
    int loan_id;
    int book_id;
    int member_id;
    char loan_date[MAX_DATE_LEN];
    char due_date[MAX_DATE_LEN];
    int is_returned;  // 0: not returned, 1: returned
} Loan;

/**
 * @brief Structure representing a return record.
 */
typedef struct {
    int return_id;
    int loan_id;
    char return_date[MAX_DATE_LEN];
    int overdue_days;  // 연체 일수
} Return;

/**
 * @brief Initialize the Loans and Returns tables in the database.
 * 
 * @param db SQLite database connection.
 * @return int Returns 0 on success, -1 on failure.
 */
int init_loan_tables(sqlite3 *db);

/**
 * @brief Process a book loan.
 * 
 * @param db SQLite database connection.
 * @param book_id The ID of the book to loan.
 * @param member_id The ID of the member borrowing the book.
 * @param loan_period The loan period in days (default 14 days).
 * @return int Returns loan_id on success, -1 on failure.
 */
int process_loan(sqlite3 *db, int book_id, int member_id, int loan_period);

/**
 * @brief Process a book return.
 * 
 * @param db SQLite database connection.
 * @param loan_id The ID of the loan to return.
 * @return int Returns return_id on success, -1 on failure.
 */
int process_return(sqlite3 *db, int loan_id);

/**
 * @brief Get loan information by loan ID.
 * 
 * @param db SQLite database connection.
 * @param loan_id The loan ID to search.
 * @param loan Pointer to Loan structure to store result.
 * @return int Returns 0 on success, -1 on failure.
 */
int get_loan_by_id(sqlite3 *db, int loan_id, Loan *loan);

/**
 * @brief Get all active loans for a member.
 * 
 * @param db SQLite database connection.
 * @param member_id The member ID.
 * @param loans Array to store loan records.
 * @param max_count Maximum number of loans to return.
 * @return int Returns number of loans found, -1 on failure.
 */
int get_active_loans_by_member(sqlite3 *db, int member_id, Loan *loans, int max_count);

/**
 * @brief Get all active loans for a book.
 * 
 * @param db SQLite database connection.
 * @param book_id The book ID.
 * @param loans Array to store loan records.
 * @param max_count Maximum number of loans to return.
 * @return int Returns number of loans found, -1 on failure.
 */
int get_active_loans_by_book(sqlite3 *db, int book_id, Loan *loans, int max_count);

/**
 * @brief Check if a loan is overdue.
 * 
 * @param db SQLite database connection.
 * @param loan_id The loan ID to check.
 * @param overdue_days Pointer to store overdue days (0 if not overdue).
 * @return int Returns 1 if overdue, 0 if not overdue, -1 on failure.
 */
int check_loan_overdue(sqlite3 *db, int loan_id, int *overdue_days);

/**
 * @brief Get all overdue loans.
 * 
 * @param db SQLite database connection.
 * @param loans Array to store overdue loan records.
 * @param max_count Maximum number of loans to return.
 * @return int Returns number of overdue loans found, -1 on failure.
 */
int get_overdue_loans(sqlite3 *db, Loan *loans, int max_count);

/**
 * @brief Get loan history for a member.
 * 
 * @param db SQLite database connection.
 * @param member_id The member ID.
 * @param loans Array to store loan records.
 * @param max_count Maximum number of loans to return.
 * @return int Returns number of loans found, -1 on failure.
 */
int get_loan_history_by_member(sqlite3 *db, int member_id, Loan *loans, int max_count);

/**
 * @brief Get loan history for a book.
 * 
 * @param db SQLite database connection.
 * @param book_id The book ID.
 * @param loans Array to store loan records.
 * @param max_count Maximum number of loans to return.
 * @return int Returns number of loans found, -1 on failure.
 */
int get_loan_history_by_book(sqlite3 *db, int book_id, Loan *loans, int max_count);

/**
 * @brief Calculate suspension days for overdue (연체일수 * 2).
 * 
 * @param overdue_days Number of overdue days.
 * @return int Returns suspension days.
 */
int calculate_suspension_days(int overdue_days);

/**
 * @brief Display all active loans.
 * 
 * @param db SQLite database connection.
 * @return int Returns number of loans displayed, -1 on failure.
 */
int display_active_loans(sqlite3 *db);

/**
 * @brief Display overdue loans report.
 * 
 * @param db SQLite database connection.
 * @return int Returns number of overdue loans, -1 on failure.
 */
int display_overdue_report(sqlite3 *db);

/**
 * @brief Get most popular books (by loan count).
 * 
 * @param db SQLite database connection.
 * @param limit Number of books to return.
 * @return int Returns number of books displayed, -1 on failure.
 */
int get_popular_books(sqlite3 *db, int limit);

#endif // LOAN_H
