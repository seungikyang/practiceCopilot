#ifndef MEMBER_H
#define MEMBER_H

#include <sqlite3.h>

#define MAX_NAME_LEN 50
#define MAX_PHONE_LEN 20
#define MAX_ADDRESS_LEN 100

typedef struct {
    int member_id;
    char name[MAX_NAME_LEN];
    char phone[MAX_PHONE_LEN];
    char address[MAX_ADDRESS_LEN];
    char registration_date[11]; // YYYY-MM-DD
    int overdue_days;           // 연체 일수
    int suspension_days;        // 대출 정지 일수 (연체일수 * 2)
} Member;

/**
 * @brief Initialize the Members table in the database.
 * 
 * @param db SQLite database connection.
 * @return int Returns 0 on success, -1 on failure.
 */
int init_member_table(sqlite3 *db);

/**
 * @brief Add a new member to the database.
 * 
 * @param db SQLite database connection.
 * @param name Member's name.
 * @param phone Member's phone number.
 * @param address Member's address.
 * @return int Returns member_id on success, -1 on failure.
 */
int add_member(sqlite3 *db, const char *name, const char *phone, const char *address);

/**
 * @brief Search for a member by ID.
 * 
 * @param db SQLite database connection.
 * @param member_id Member ID to search.
 * @param member Pointer to Member structure to store result.
 * @return int Returns 0 on success, -1 on failure.
 */
int search_member_by_id(sqlite3 *db, int member_id, Member *member);

/**
 * @brief Search for members by name.
 * 
 * @param db SQLite database connection.
 * @param name Member's name to search.
 * @param members Array to store found members.
 * @param max_count Maximum number of members to return.
 * @return int Returns number of members found, -1 on failure.
 */
int search_member_by_name(sqlite3 *db, const char *name, Member *members, int max_count);

/**
 * @brief Update member information.
 * 
 * @param db SQLite database connection.
 * @param member_id Member ID to update.
 * @param name New name (NULL to keep unchanged).
 * @param phone New phone (NULL to keep unchanged).
 * @param address New address (NULL to keep unchanged).
 * @return int Returns 0 on success, -1 on failure.
 */
int update_member(sqlite3 *db, int member_id, const char *name, const char *phone, const char *address);

/**
 * @brief Delete a member from the database.
 * 
 * @param db SQLite database connection.
 * @param member_id Member ID to delete.
 * @return int Returns 0 on success, -1 on failure.
 */
int delete_member(sqlite3 *db, int member_id);

/**
 * @brief Check if a member has overdue loans.
 * 
 * @param db SQLite database connection.
 * @param member_id Member ID to check.
 * @param overdue_days Pointer to store overdue days.
 * @return int Returns 1 if overdue exists, 0 if not, -1 on failure.
 */
int check_member_overdue(sqlite3 *db, int member_id, int *overdue_days);

/**
 * @brief Check if a member can borrow books (not suspended).
 * 
 * @param db SQLite database connection.
 * @param member_id Member ID to check.
 * @return int Returns 1 if can borrow, 0 if suspended, -1 on failure.
 */
int can_member_borrow(sqlite3 *db, int member_id);

/**
 * @brief List all members.
 * 
 * @param db SQLite database connection.
 * @param members Array to store members.
 * @param max_count Maximum number of members to return.
 * @return int Returns number of members found, -1 on failure.
 */
int list_all_members(sqlite3 *db, Member *members, int max_count);

/**
 * @brief Get total number of members.
 * 
 * @param db SQLite database connection.
 * @return int Returns total count, -1 on failure.
 */
int get_member_count(sqlite3 *db);

#endif // MEMBER_H
