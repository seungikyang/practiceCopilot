#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>

#define DB_PATH "database/library.db"
#define MAX_QUERY_LENGTH 1024

/**
 * @brief Initialize the database connection and create tables if they don't exist.
 * 
 * @return int Returns 0 on success, -1 on failure.
 */
int init_database(void);

/**
 * @brief Close the database connection.
 * 
 * @return int Returns 0 on success, -1 on failure.
 */
int close_database(void);

/**
 * @brief Get the database connection pointer.
 * 
 * @return sqlite3* Returns the database connection pointer, or NULL if not initialized.
 */
sqlite3* get_db_connection(void);

/**
 * @brief Set the database connection pointer (for testing purposes only).
 * 
 * @param new_db The new database connection pointer.
 */
void set_db_connection(sqlite3* new_db);

/**
 * @brief Execute a SQL query without returning results.
 * 
 * @param sql The SQL query string to execute.
 * @return int Returns 0 on success, -1 on failure.
 */
int execute_query(const char *sql);

/**
 * @brief Begin a database transaction.
 * 
 * @return int Returns 0 on success, -1 on failure.
 */
int begin_transaction(void);

/**
 * @brief Commit a database transaction.
 * 
 * @return int Returns 0 on success, -1 on failure.
 */
int commit_transaction(void);

/**
 * @brief Rollback a database transaction.
 * 
 * @return int Returns 0 on success, -1 on failure.
 */
int rollback_transaction(void);

/**
 * @brief Create all necessary database tables.
 * 
 * @return int Returns 0 on success, -1 on failure.
 */
int create_tables(void);

/**
 * @brief Create indexes on database tables for better performance.
 * 
 * @return int Returns 0 on success, -1 on failure.
 */
int create_indexes(void);

/**
 * @brief Enable foreign key constraints.
 * 
 * @return int Returns 0 on success, -1 on failure.
 */
int enable_foreign_keys(void);

#endif // DATABASE_H