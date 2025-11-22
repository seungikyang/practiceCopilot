#include "loan.h"
#include "book.h"
#include "member.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

#define DEFAULT_LOAN_PERIOD 14  // 기본 대출 기간 (일)
#define SUSPENSION_MULTIPLIER 2  // 연체일수 * 2 = 대출 정지 일수

/**
 * @brief Get current date in YYYY-MM-DD format.
 * 
 * @param date_str Buffer to store the date string.
 * @param size Size of the buffer.
 */
static void get_current_date(char *date_str, size_t size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(date_str, size, "%Y-%m-%d", t);
}

/**
 * @brief Add days to a date string.
 * 
 * @param date_str Input date in YYYY-MM-DD format.
 * @param days Number of days to add.
 * @param result_str Buffer to store result date.
 * @param size Size of the buffer.
 */
static void add_days_to_date(const char *date_str, int days, char *result_str, size_t size) {
    struct tm t = {0};
    if (sscanf(date_str, "%d-%d-%d", &t.tm_year, &t.tm_mon, &t.tm_mday) != 3) {
        fprintf(stderr, "Invalid date format: %s\n", date_str);
        strncpy(result_str, date_str, size - 1);
        result_str[size - 1] = '\0';
        return;
    }
    t.tm_year -= 1900;
    t.tm_mon -= 1;
    
    time_t timestamp = mktime(&t);
    timestamp += days * 24 * 60 * 60;
    struct tm *new_t = localtime(&timestamp);
    strftime(result_str, size, "%Y-%m-%d", new_t);
}

/**
 * @brief Calculate days between two dates.
 * 
 * @param date1 First date in YYYY-MM-DD format.
 * @param date2 Second date in YYYY-MM-DD format.
 * @return int Returns difference in days (date2 - date1).
 */
static int calculate_date_diff(const char *date1, const char *date2) {
    struct tm t1 = {0}, t2 = {0};
    
    if (sscanf(date1, "%d-%d-%d", &t1.tm_year, &t1.tm_mon, &t1.tm_mday) != 3) {
        fprintf(stderr, "Invalid date1 format: %s\n", date1);
        return 0;
    }
    t1.tm_year -= 1900;
    t1.tm_mon -= 1;
    
    if (sscanf(date2, "%d-%d-%d", &t2.tm_year, &t2.tm_mon, &t2.tm_mday) != 3) {
        fprintf(stderr, "Invalid date2 format: %s\n", date2);
        return 0;
    }
    t2.tm_year -= 1900;
    t2.tm_mon -= 1;
    
    time_t time1 = mktime(&t1);
    time_t time2 = mktime(&t2);
    
    return (int)((time2 - time1) / (24 * 60 * 60));
}

int init_loan_tables(sqlite3 *db) {
    if (db == NULL) {
        fprintf(stderr, "Database connection is NULL\n");
        return -1;
    }

    char *err_msg = NULL;
    
    // Create Loans table
    const char *loans_sql = 
        "CREATE TABLE IF NOT EXISTS Loans ("
        "loan_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "book_id INTEGER NOT NULL,"
        "member_id INTEGER NOT NULL,"
        "loan_date TEXT NOT NULL,"
        "due_date TEXT NOT NULL,"
        "is_returned INTEGER DEFAULT 0,"
        "FOREIGN KEY (book_id) REFERENCES Books(book_id),"
        "FOREIGN KEY (member_id) REFERENCES Members(member_id)"
        ");";
    
    if (sqlite3_exec(db, loans_sql, 0, 0, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "Failed to create Loans table: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }
    
    // Create Returns table
    const char *returns_sql = 
        "CREATE TABLE IF NOT EXISTS Returns ("
        "return_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "loan_id INTEGER NOT NULL,"
        "return_date TEXT NOT NULL,"
        "overdue_days INTEGER DEFAULT 0,"
        "FOREIGN KEY (loan_id) REFERENCES Loans(loan_id)"
        ");";
    
    if (sqlite3_exec(db, returns_sql, 0, 0, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "Failed to create Returns table: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }
    
    // Create indexes
    const char *idx_loans_book = "CREATE INDEX IF NOT EXISTS idx_loans_book_id ON Loans(book_id);";
    const char *idx_loans_member = "CREATE INDEX IF NOT EXISTS idx_loans_member_id ON Loans(member_id);";
    const char *idx_loans_returned = "CREATE INDEX IF NOT EXISTS idx_loans_returned ON Loans(is_returned);";
    
    sqlite3_exec(db, idx_loans_book, 0, 0, NULL);
    sqlite3_exec(db, idx_loans_member, 0, 0, NULL);
    sqlite3_exec(db, idx_loans_returned, 0, 0, NULL);
    
    return 0;
}

int process_loan(sqlite3 *db, int book_id, int member_id, int loan_period) {
    if (db == NULL) {
        fprintf(stderr, "Database connection is NULL\n");
        return -1;
    }
    
    if (loan_period <= 0) {
        loan_period = DEFAULT_LOAN_PERIOD;
    }
    
    // Check if member can borrow (not suspended)
    if (!can_member_borrow(db, member_id)) {
        fprintf(stderr, "Member %d is suspended due to overdue books\n", member_id);
        return -1;
    }
    
    // Check book availability
    if (!check_book_availability(book_id)) {
        fprintf(stderr, "Book %d is not available for loan\n", book_id);
        return -1;
    }
    
    // Get current date and calculate due date
    char loan_date[MAX_DATE_LEN];
    char due_date[MAX_DATE_LEN];
    get_current_date(loan_date, sizeof(loan_date));
    add_days_to_date(loan_date, loan_period, due_date, sizeof(due_date));
    
    // Begin transaction
    sqlite3_exec(db, "BEGIN TRANSACTION;", 0, 0, NULL);
    
    // Insert loan record
    const char *sql = "INSERT INTO Loans (book_id, member_id, loan_date, due_date, is_returned) "
                      "VALUES (?, ?, ?, ?, 0);";
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_exec(db, "ROLLBACK;", 0, 0, NULL);
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, book_id);
    sqlite3_bind_int(stmt, 2, member_id);
    sqlite3_bind_text(stmt, 3, loan_date, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, due_date, -1, SQLITE_STATIC);
    
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "Failed to insert loan record: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_exec(db, "ROLLBACK;", 0, 0, NULL);
        return -1;
    }
    
    int loan_id = (int)sqlite3_last_insert_rowid(db);
    sqlite3_finalize(stmt);
    
    // Update book availability
    if (update_book_availability(book_id, -1) != 0) {
        fprintf(stderr, "Failed to update book availability\n");
        sqlite3_exec(db, "ROLLBACK;", 0, 0, NULL);
        return -1;
    }
    
    // Commit transaction
    sqlite3_exec(db, "COMMIT;", 0, 0, NULL);
    
    printf("Loan processed successfully (Loan ID: %d)\n", loan_id);
    printf("Loan Date: %s, Due Date: %s\n", loan_date, due_date);
    
    return loan_id;
}

int process_return(sqlite3 *db, int loan_id) {
    if (db == NULL) {
        fprintf(stderr, "Database connection is NULL\n");
        return -1;
    }
    
    // Get loan information
    Loan loan;
    if (get_loan_by_id(db, loan_id, &loan) != 0) {
        fprintf(stderr, "Loan %d not found\n", loan_id);
        return -1;
    }
    
    if (loan.is_returned) {
        fprintf(stderr, "Loan %d has already been returned\n", loan_id);
        return -1;
    }
    
    // Get current date
    char return_date[MAX_DATE_LEN];
    get_current_date(return_date, sizeof(return_date));
    
    // Calculate overdue days
    int overdue_days = calculate_date_diff(loan.due_date, return_date);
    if (overdue_days < 0) {
        overdue_days = 0;
    }
    
    // Begin transaction
    sqlite3_exec(db, "BEGIN TRANSACTION;", 0, 0, NULL);
    
    // Insert return record
    const char *insert_sql = "INSERT INTO Returns (loan_id, return_date, overdue_days) "
                             "VALUES (?, ?, ?);";
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, insert_sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_exec(db, "ROLLBACK;", 0, 0, NULL);
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, loan_id);
    sqlite3_bind_text(stmt, 2, return_date, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, overdue_days);
    
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "Failed to insert return record: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_exec(db, "ROLLBACK;", 0, 0, NULL);
        return -1;
    }
    
    int return_id = (int)sqlite3_last_insert_rowid(db);
    sqlite3_finalize(stmt);
    
    // Update loan record
    const char *update_sql = "UPDATE Loans SET is_returned = 1 WHERE loan_id = ?;";
    if (sqlite3_prepare_v2(db, update_sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare update statement: %s\n", sqlite3_errmsg(db));
        sqlite3_exec(db, "ROLLBACK;", 0, 0, NULL);
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, loan_id);
    
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "Failed to update loan record: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_exec(db, "ROLLBACK;", 0, 0, NULL);
        return -1;
    }
    
    sqlite3_finalize(stmt);
    
    // Update book availability
    if (update_book_availability(loan.book_id, 1) != 0) {
        fprintf(stderr, "Failed to update book availability\n");
        sqlite3_exec(db, "ROLLBACK;", 0, 0, NULL);
        return -1;
    }
    
    // Commit transaction
    sqlite3_exec(db, "COMMIT;", 0, 0, NULL);
    
    printf("Return processed successfully (Return ID: %d)\n", return_id);
    printf("Return Date: %s\n", return_date);
    
    if (overdue_days > 0) {
        int suspension_days = calculate_suspension_days(overdue_days);
        printf("WARNING: Overdue by %d days. Suspension period: %d days\n", 
               overdue_days, suspension_days);
    }
    
    return return_id;
}

int get_loan_by_id(sqlite3 *db, int loan_id, Loan *loan) {
    if (db == NULL || loan == NULL) {
        fprintf(stderr, "Invalid parameters\n");
        return -1;
    }
    
    const char *sql = "SELECT loan_id, book_id, member_id, loan_date, due_date, is_returned "
                      "FROM Loans WHERE loan_id = ?;";
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, loan_id);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        loan->loan_id = sqlite3_column_int(stmt, 0);
        loan->book_id = sqlite3_column_int(stmt, 1);
        loan->member_id = sqlite3_column_int(stmt, 2);
        strncpy(loan->loan_date, (const char *)sqlite3_column_text(stmt, 3), MAX_DATE_LEN - 1);
        loan->loan_date[MAX_DATE_LEN - 1] = '\0';
        strncpy(loan->due_date, (const char *)sqlite3_column_text(stmt, 4), MAX_DATE_LEN - 1);
        loan->due_date[MAX_DATE_LEN - 1] = '\0';
        loan->is_returned = sqlite3_column_int(stmt, 5);
        
        sqlite3_finalize(stmt);
        return 0;
    }
    
    sqlite3_finalize(stmt);
    return -1;
}

int get_active_loans_by_member(sqlite3 *db, int member_id, Loan *loans, int max_count) {
    if (db == NULL || loans == NULL) {
        fprintf(stderr, "Invalid parameters\n");
        return -1;
    }
    
    const char *sql = "SELECT loan_id, book_id, member_id, loan_date, due_date, is_returned "
                      "FROM Loans WHERE member_id = ? AND is_returned = 0 "
                      "ORDER BY loan_date DESC LIMIT ?;";
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, member_id);
    sqlite3_bind_int(stmt, 2, max_count);
    
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && count < max_count) {
        loans[count].loan_id = sqlite3_column_int(stmt, 0);
        loans[count].book_id = sqlite3_column_int(stmt, 1);
        loans[count].member_id = sqlite3_column_int(stmt, 2);
        strncpy(loans[count].loan_date, (const char *)sqlite3_column_text(stmt, 3), MAX_DATE_LEN - 1);
        loans[count].loan_date[MAX_DATE_LEN - 1] = '\0';
        strncpy(loans[count].due_date, (const char *)sqlite3_column_text(stmt, 4), MAX_DATE_LEN - 1);
        loans[count].due_date[MAX_DATE_LEN - 1] = '\0';
        loans[count].is_returned = sqlite3_column_int(stmt, 5);
        count++;
    }
    
    sqlite3_finalize(stmt);
    return count;
}

int get_active_loans_by_book(sqlite3 *db, int book_id, Loan *loans, int max_count) {
    if (db == NULL || loans == NULL) {
        fprintf(stderr, "Invalid parameters\n");
        return -1;
    }
    
    const char *sql = "SELECT loan_id, book_id, member_id, loan_date, due_date, is_returned "
                      "FROM Loans WHERE book_id = ? AND is_returned = 0 "
                      "ORDER BY loan_date DESC LIMIT ?;";
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, book_id);
    sqlite3_bind_int(stmt, 2, max_count);
    
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && count < max_count) {
        loans[count].loan_id = sqlite3_column_int(stmt, 0);
        loans[count].book_id = sqlite3_column_int(stmt, 1);
        loans[count].member_id = sqlite3_column_int(stmt, 2);
        strncpy(loans[count].loan_date, (const char *)sqlite3_column_text(stmt, 3), MAX_DATE_LEN - 1);
        loans[count].loan_date[MAX_DATE_LEN - 1] = '\0';
        strncpy(loans[count].due_date, (const char *)sqlite3_column_text(stmt, 4), MAX_DATE_LEN - 1);
        loans[count].due_date[MAX_DATE_LEN - 1] = '\0';
        loans[count].is_returned = sqlite3_column_int(stmt, 5);
        count++;
    }
    
    sqlite3_finalize(stmt);
    return count;
}

int check_loan_overdue(sqlite3 *db, int loan_id, int *overdue_days) {
    if (db == NULL || overdue_days == NULL) {
        fprintf(stderr, "Invalid parameters\n");
        return -1;
    }
    
    Loan loan;
    if (get_loan_by_id(db, loan_id, &loan) != 0) {
        return -1;
    }
    
    if (loan.is_returned) {
        *overdue_days = 0;
        return 0;
    }
    
    char current_date[MAX_DATE_LEN];
    get_current_date(current_date, sizeof(current_date));
    
    int days = calculate_date_diff(loan.due_date, current_date);
    
    if (days > 0) {
        *overdue_days = days;
        return 1;
    }
    
    *overdue_days = 0;
    return 0;
}

int get_overdue_loans(sqlite3 *db, Loan *loans, int max_count) {
    if (db == NULL || loans == NULL) {
        fprintf(stderr, "Invalid parameters\n");
        return -1;
    }
    
    char current_date[MAX_DATE_LEN];
    get_current_date(current_date, sizeof(current_date));
    
    const char *sql = "SELECT loan_id, book_id, member_id, loan_date, due_date, is_returned "
                      "FROM Loans WHERE is_returned = 0 AND due_date < ? "
                      "ORDER BY due_date ASC LIMIT ?;";
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_text(stmt, 1, current_date, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, max_count);
    
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && count < max_count) {
        loans[count].loan_id = sqlite3_column_int(stmt, 0);
        loans[count].book_id = sqlite3_column_int(stmt, 1);
        loans[count].member_id = sqlite3_column_int(stmt, 2);
        strncpy(loans[count].loan_date, (const char *)sqlite3_column_text(stmt, 3), MAX_DATE_LEN - 1);
        loans[count].loan_date[MAX_DATE_LEN - 1] = '\0';
        strncpy(loans[count].due_date, (const char *)sqlite3_column_text(stmt, 4), MAX_DATE_LEN - 1);
        loans[count].due_date[MAX_DATE_LEN - 1] = '\0';
        loans[count].is_returned = sqlite3_column_int(stmt, 5);
        count++;
    }
    
    sqlite3_finalize(stmt);
    return count;
}

int get_loan_history_by_member(sqlite3 *db, int member_id, Loan *loans, int max_count) {
    if (db == NULL || loans == NULL) {
        fprintf(stderr, "Invalid parameters\n");
        return -1;
    }
    
    const char *sql = "SELECT loan_id, book_id, member_id, loan_date, due_date, is_returned "
                      "FROM Loans WHERE member_id = ? "
                      "ORDER BY loan_date DESC LIMIT ?;";
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, member_id);
    sqlite3_bind_int(stmt, 2, max_count);
    
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && count < max_count) {
        loans[count].loan_id = sqlite3_column_int(stmt, 0);
        loans[count].book_id = sqlite3_column_int(stmt, 1);
        loans[count].member_id = sqlite3_column_int(stmt, 2);
        strncpy(loans[count].loan_date, (const char *)sqlite3_column_text(stmt, 3), MAX_DATE_LEN - 1);
        loans[count].loan_date[MAX_DATE_LEN - 1] = '\0';
        strncpy(loans[count].due_date, (const char *)sqlite3_column_text(stmt, 4), MAX_DATE_LEN - 1);
        loans[count].due_date[MAX_DATE_LEN - 1] = '\0';
        loans[count].is_returned = sqlite3_column_int(stmt, 5);
        count++;
    }
    
    sqlite3_finalize(stmt);
    return count;
}

int get_loan_history_by_book(sqlite3 *db, int book_id, Loan *loans, int max_count) {
    if (db == NULL || loans == NULL) {
        fprintf(stderr, "Invalid parameters\n");
        return -1;
    }
    
    const char *sql = "SELECT loan_id, book_id, member_id, loan_date, due_date, is_returned "
                      "FROM Loans WHERE book_id = ? "
                      "ORDER BY loan_date DESC LIMIT ?;";
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, book_id);
    sqlite3_bind_int(stmt, 2, max_count);
    
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && count < max_count) {
        loans[count].loan_id = sqlite3_column_int(stmt, 0);
        loans[count].book_id = sqlite3_column_int(stmt, 1);
        loans[count].member_id = sqlite3_column_int(stmt, 2);
        strncpy(loans[count].loan_date, (const char *)sqlite3_column_text(stmt, 3), MAX_DATE_LEN - 1);
        loans[count].loan_date[MAX_DATE_LEN - 1] = '\0';
        strncpy(loans[count].due_date, (const char *)sqlite3_column_text(stmt, 4), MAX_DATE_LEN - 1);
        loans[count].due_date[MAX_DATE_LEN - 1] = '\0';
        loans[count].is_returned = sqlite3_column_int(stmt, 5);
        count++;
    }
    
    sqlite3_finalize(stmt);
    return count;
}

int calculate_suspension_days(int overdue_days) {
    return overdue_days * SUSPENSION_MULTIPLIER;
}

int display_active_loans(sqlite3 *db) {
    if (db == NULL) {
        fprintf(stderr, "Database connection is NULL\n");
        return -1;
    }
    
    const char *sql = "SELECT l.loan_id, b.title, m.name, l.loan_date, l.due_date "
                      "FROM Loans l "
                      "JOIN Books b ON l.book_id = b.book_id "
                      "JOIN Members m ON l.member_id = m.member_id "
                      "WHERE l.is_returned = 0 "
                      "ORDER BY l.loan_date DESC;";
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    printf("\n========== Active Loans ==========\n");
    printf("%-8s %-30s %-20s %-12s %-12s\n", 
           "Loan ID", "Book Title", "Member", "Loan Date", "Due Date");
    printf("----------------------------------------------------------\n");
    
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int loan_id = sqlite3_column_int(stmt, 0);
        const char *title = (const char *)sqlite3_column_text(stmt, 1);
        const char *name = (const char *)sqlite3_column_text(stmt, 2);
        const char *loan_date = (const char *)sqlite3_column_text(stmt, 3);
        const char *due_date = (const char *)sqlite3_column_text(stmt, 4);
        
        printf("%-8d %-30s %-20s %-12s %-12s\n", 
               loan_id, title, name, loan_date, due_date);
        count++;
    }
    
    printf("----------------------------------------------------------\n");
    printf("Total: %d active loans\n\n", count);
    
    sqlite3_finalize(stmt);
    return count;
}

int display_overdue_report(sqlite3 *db) {
    if (db == NULL) {
        fprintf(stderr, "Database connection is NULL\n");
        return -1;
    }
    
    char current_date[MAX_DATE_LEN];
    get_current_date(current_date, sizeof(current_date));
    
    const char *sql = "SELECT l.loan_id, b.title, m.name, l.loan_date, l.due_date, "
                      "julianday(?) - julianday(l.due_date) AS overdue_days "
                      "FROM Loans l "
                      "JOIN Books b ON l.book_id = b.book_id "
                      "JOIN Members m ON l.member_id = m.member_id "
                      "WHERE l.is_returned = 0 AND l.due_date < ? "
                      "ORDER BY overdue_days DESC;";
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_text(stmt, 1, current_date, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, current_date, -1, SQLITE_STATIC);
    
    printf("\n========== Overdue Loans Report ==========\n");
    printf("%-8s %-30s %-20s %-12s %-12s %s\n", 
           "Loan ID", "Book Title", "Member", "Loan Date", "Due Date", "Overdue(Days)");
    printf("--------------------------------------------------------------------------------\n");
    
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int loan_id = sqlite3_column_int(stmt, 0);
        const char *title = (const char *)sqlite3_column_text(stmt, 1);
        const char *name = (const char *)sqlite3_column_text(stmt, 2);
        const char *loan_date = (const char *)sqlite3_column_text(stmt, 3);
        const char *due_date = (const char *)sqlite3_column_text(stmt, 4);
        int overdue_days = sqlite3_column_int(stmt, 5);
        
        printf("%-8d %-30s %-20s %-12s %-12s %d (Susp: %d days)\n", 
               loan_id, title, name, loan_date, due_date, 
               overdue_days, calculate_suspension_days(overdue_days));
        count++;
    }
    
    printf("--------------------------------------------------------------------------------\n");
    printf("Total: %d overdue loans\n\n", count);
    
    sqlite3_finalize(stmt);
    return count;
}

int get_popular_books(sqlite3 *db, int limit) {
    if (db == NULL) {
        fprintf(stderr, "Database connection is NULL\n");
        return -1;
    }
    
    const char *sql = "SELECT b.book_id, b.title, b.author, COUNT(l.loan_id) AS loan_count "
                      "FROM Books b "
                      "LEFT JOIN Loans l ON b.book_id = l.book_id "
                      "GROUP BY b.book_id "
                      "ORDER BY loan_count DESC "
                      "LIMIT ?;";
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, limit);
    
    printf("\n========== Popular Books Report ==========\n");
    printf("%-8s %-40s %-25s %s\n", 
           "Book ID", "Title", "Author", "Loan Count");
    printf("-----------------------------------------------------------------------------------\n");
    
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int book_id = sqlite3_column_int(stmt, 0);
        const char *title = (const char *)sqlite3_column_text(stmt, 1);
        const char *author = (const char *)sqlite3_column_text(stmt, 2);
        int loan_count = sqlite3_column_int(stmt, 3);
        
        printf("%-8d %-40s %-25s %d\n", book_id, title, author, loan_count);
        count++;
    }
    
    printf("-----------------------------------------------------------------------------------\n");
    printf("Total: %d books\n\n", count);
    
    sqlite3_finalize(stmt);
    return count;
}
