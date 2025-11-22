#include "database.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static sqlite3 *db = NULL;

/**
 * @brief Initialize the database connection and create tables if they don't exist.
 * 
 * @return int Returns 0 on success, -1 on failure.
 */
int init_database(void) {
    int rc = sqlite3_open(DB_PATH, &db);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        db = NULL;
        return -1;
    }
    
    printf("Database connection established successfully\n");
    
    /* Enable foreign key constraints */
    if (enable_foreign_keys() != 0) {
        fprintf(stderr, "Failed to enable foreign keys\n");
        sqlite3_close(db);
        db = NULL;
        return -1;
    }
    
    /* Create tables */
    if (create_tables() != 0) {
        fprintf(stderr, "Failed to create tables\n");
        sqlite3_close(db);
        db = NULL;
        return -1;
    }
    
    /* Create indexes */
    if (create_indexes() != 0) {
        fprintf(stderr, "Failed to create indexes\n");
        sqlite3_close(db);
        db = NULL;
        return -1;
    }
    
    printf("Database initialized successfully\n");
    return 0;
}

/**
 * @brief Close the database connection.
 * 
 * @return int Returns 0 on success, -1 on failure.
 */
int close_database(void) {
    if (db == NULL) {
        return 0;
    }
    
    int rc = sqlite3_close(db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to close database: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    db = NULL;
    printf("Database connection closed successfully\n");
    return 0;
}

/**
 * @brief Get the database connection pointer.
 * 
 * @return sqlite3* Returns the database connection pointer, or NULL if not initialized.
 */
sqlite3* get_db_connection(void) {
    return db;
}

/**
 * @brief Set the database connection pointer (for testing purposes only).
 * 
 * @param new_db The new database connection pointer.
 */
void set_db_connection(sqlite3* new_db) {
    db = new_db;
}

/**
 * @brief Execute a SQL query without returning results.
 * 
 * @param sql The SQL query string to execute.
 * @return int Returns 0 on success, -1 on failure.
 */
int execute_query(const char *sql) {
    if (db == NULL) {
        fprintf(stderr, "Database not initialized\n");
        return -1;
    }
    
    if (sql == NULL) {
        fprintf(stderr, "SQL query is NULL\n");
        return -1;
    }
    
    char *err_msg = NULL;
    int rc = sqlite3_exec(db, sql, NULL, NULL, &err_msg);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }
    
    return 0;
}

/**
 * @brief Begin a database transaction.
 * 
 * @return int Returns 0 on success, -1 on failure.
 */
int begin_transaction(void) {
    return execute_query("BEGIN TRANSACTION;");
}

/**
 * @brief Commit a database transaction.
 * 
 * @return int Returns 0 on success, -1 on failure.
 */
int commit_transaction(void) {
    return execute_query("COMMIT;");
}

/**
 * @brief Rollback a database transaction.
 * 
 * @return int Returns 0 on success, -1 on failure.
 */
int rollback_transaction(void) {
    return execute_query("ROLLBACK;");
}

/**
 * @brief Create all necessary database tables.
 * 
 * @return int Returns 0 on success, -1 on failure.
 */
int create_tables(void) {
    /* Books table */
    const char *sql_books = 
        "CREATE TABLE IF NOT EXISTS Books ("
        "book_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "title TEXT NOT NULL,"
        "author TEXT,"
        "publisher TEXT,"
        "publication_year INTEGER,"
        "isbn TEXT UNIQUE,"
        "genre TEXT,"
        "quantity INTEGER DEFAULT 1,"
        "available INTEGER DEFAULT 1"
        ");";
    
    /* Members table */
    const char *sql_members = 
        "CREATE TABLE IF NOT EXISTS Members ("
        "member_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "phone TEXT,"
        "address TEXT,"
        "registration_date TEXT,"
        "penalty_days INTEGER DEFAULT 0"
        ");";
    
    /* Loans table */
    const char *sql_loans = 
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
    
    /* Returns table */
    const char *sql_returns = 
        "CREATE TABLE IF NOT EXISTS Returns ("
        "return_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "loan_id INTEGER NOT NULL,"
        "return_date TEXT NOT NULL,"
        "overdue_days INTEGER DEFAULT 0,"
        "FOREIGN KEY (loan_id) REFERENCES Loans(loan_id)"
        ");";
    
    /* Execute table creation queries */
    if (execute_query(sql_books) != 0) {
        return -1;
    }
    
    if (execute_query(sql_members) != 0) {
        return -1;
    }
    
    if (execute_query(sql_loans) != 0) {
        return -1;
    }
    
    if (execute_query(sql_returns) != 0) {
        return -1;
    }
    
    printf("All tables created successfully\n");
    return 0;
}

/**
 * @brief Create indexes on database tables for better performance.
 * 
 * @return int Returns 0 on success, -1 on failure.
 */
int create_indexes(void) {
    const char *indexes[] = {
        "CREATE INDEX IF NOT EXISTS idx_books_title ON Books(title);",
        "CREATE INDEX IF NOT EXISTS idx_books_author ON Books(author);",
        "CREATE INDEX IF NOT EXISTS idx_books_isbn ON Books(isbn);",
        "CREATE INDEX IF NOT EXISTS idx_members_name ON Members(name);",
        "CREATE INDEX IF NOT EXISTS idx_loans_book_id ON Loans(book_id);",
        "CREATE INDEX IF NOT EXISTS idx_loans_member_id ON Loans(member_id);",
        "CREATE INDEX IF NOT EXISTS idx_loans_is_returned ON Loans(is_returned);",
        "CREATE INDEX IF NOT EXISTS idx_returns_loan_id ON Returns(loan_id);"
    };
    
    int num_indexes = sizeof(indexes) / sizeof(indexes[0]);
    
    for (int i = 0; i < num_indexes; i++) {
        if (execute_query(indexes[i]) != 0) {
            return -1;
        }
    }
    
    printf("All indexes created successfully\n");
    return 0;
}

/**
 * @brief Enable foreign key constraints.
 * 
 * @return int Returns 0 on success, -1 on failure.
 */
int enable_foreign_keys(void) {
    return execute_query("PRAGMA foreign_keys = ON;");
}