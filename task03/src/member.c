#include "member.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

int init_member_table(sqlite3 *db) {
    const char *sql = "CREATE TABLE IF NOT EXISTS Members ("
                      "member_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "name TEXT NOT NULL,"
                      "phone TEXT,"
                      "address TEXT,"
                      "registration_date TEXT NOT NULL);";
    
    char *err_msg = NULL;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to create Members table: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }
    
    return 0;
}

int add_member(sqlite3 *db, const char *name, const char *phone, const char *address) {
    if (!name) {
        fprintf(stderr, "Member name is required\n");
        return -1;
    }
    
    // 현재 날짜 가져오기
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char date[12];  // Increased size to 12 to avoid truncation warning
    snprintf(date, sizeof(date), "%04d-%02d-%02d", 
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
    
    const char *sql = "INSERT INTO Members (name, phone, address, registration_date) "
                      "VALUES (?, ?, ?, ?);";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, phone ? phone : "", -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, address ? address : "", -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, date, -1, SQLITE_STATIC);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to insert member: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    return (int)sqlite3_last_insert_rowid(db);
}

int search_member_by_id(sqlite3 *db, int member_id, Member *member) {
    if (!member) {
        return -1;
    }
    
    const char *sql = "SELECT member_id, name, phone, address, registration_date "
                      "FROM Members WHERE member_id = ?;";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, member_id);
    
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        member->member_id = sqlite3_column_int(stmt, 0);
        strncpy(member->name, (const char *)sqlite3_column_text(stmt, 1), MAX_NAME_LEN - 1);
        member->name[MAX_NAME_LEN - 1] = '\0';
        strncpy(member->phone, (const char *)sqlite3_column_text(stmt, 2), MAX_PHONE_LEN - 1);
        member->phone[MAX_PHONE_LEN - 1] = '\0';
        strncpy(member->address, (const char *)sqlite3_column_text(stmt, 3), MAX_ADDRESS_LEN - 1);
        member->address[MAX_ADDRESS_LEN - 1] = '\0';
        strncpy(member->registration_date, (const char *)sqlite3_column_text(stmt, 4), 10);
        member->registration_date[10] = '\0';
        
        sqlite3_finalize(stmt);
        
        // 연체 상태 확인
        check_member_overdue(db, member_id, &member->overdue_days);
        member->suspension_days = member->overdue_days * 2;
        
        return 0;
    }
    
    sqlite3_finalize(stmt);
    return -1;
}

int search_member_by_name(sqlite3 *db, const char *name, Member *members, int max_count) {
    if (!name || !members) {
        return -1;
    }
    
    const char *sql = "SELECT member_id, name, phone, address, registration_date "
                      "FROM Members WHERE name LIKE ? LIMIT ?;";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    char search_pattern[MAX_NAME_LEN + 2];
    snprintf(search_pattern, sizeof(search_pattern), "%%%s%%", name);
    
    sqlite3_bind_text(stmt, 1, search_pattern, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, max_count);
    
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && count < max_count) {
        members[count].member_id = sqlite3_column_int(stmt, 0);
        strncpy(members[count].name, (const char *)sqlite3_column_text(stmt, 1), MAX_NAME_LEN - 1);
        strncpy(members[count].phone, (const char *)sqlite3_column_text(stmt, 2), MAX_PHONE_LEN - 1);
        strncpy(members[count].address, (const char *)sqlite3_column_text(stmt, 3), MAX_ADDRESS_LEN - 1);
        strncpy(members[count].registration_date, (const char *)sqlite3_column_text(stmt, 4), 10);
        
        check_member_overdue(db, members[count].member_id, &members[count].overdue_days);
        members[count].suspension_days = members[count].overdue_days * 2;
        
        count++;
    }
    
    sqlite3_finalize(stmt);
    return count;
}

int update_member(sqlite3 *db, int member_id, const char *name, const char *phone, const char *address) {
    // 동적으로 UPDATE 쿼리 생성
    char sql[512] = "UPDATE Members SET ";
    int first = 1;
    
    if (name) {
        strcat(sql, "name = ?");
        first = 0;
    }
    if (phone) {
        if (!first) strcat(sql, ", ");
        strcat(sql, "phone = ?");
        first = 0;
    }
    if (address) {
        if (!first) strcat(sql, ", ");
        strcat(sql, "address = ?");
    }
    
    strcat(sql, " WHERE member_id = ?;");
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    int param = 1;
    if (name) sqlite3_bind_text(stmt, param++, name, -1, SQLITE_STATIC);
    if (phone) sqlite3_bind_text(stmt, param++, phone, -1, SQLITE_STATIC);
    if (address) sqlite3_bind_text(stmt, param++, address, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, param, member_id);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to update member: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    return 0;
}

int delete_member(sqlite3 *db, int member_id) {
    // 먼저 대출 이력이 있는지 확인
    const char *check_sql = "SELECT COUNT(*) FROM Loans WHERE member_id = ?;";
    sqlite3_stmt *stmt;
    
    int rc = sqlite3_prepare_v2(db, check_sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, member_id);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int loan_count = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        
        if (loan_count > 0) {
            fprintf(stderr, "Cannot delete member with loan history\n");
            return -1;
        }
    } else {
        sqlite3_finalize(stmt);
        return -1;
    }
    
    // 회원 삭제
    const char *sql = "DELETE FROM Members WHERE member_id = ?;";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, member_id);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to delete member: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    return 0;
}

int check_member_overdue(sqlite3 *db, int member_id, int *overdue_days) {
    const char *sql = "SELECT MAX(julianday('now') - julianday(due_date)) as overdue "
                      "FROM Loans "
                      "WHERE member_id = ? AND loan_id NOT IN (SELECT loan_id FROM Returns);";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, member_id);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        if (sqlite3_column_type(stmt, 0) != SQLITE_NULL) {
            double days = sqlite3_column_double(stmt, 0);
            *overdue_days = days > 0 ? (int)days : 0;
            sqlite3_finalize(stmt);
            return *overdue_days > 0 ? 1 : 0;
        }
    }
    
    sqlite3_finalize(stmt);
    *overdue_days = 0;
    return 0;
}

int can_member_borrow(sqlite3 *db, int member_id) {
    int overdue_days = 0;
    int has_overdue = check_member_overdue(db, member_id, &overdue_days);
    
    if (has_overdue < 0) {
        return -1;
    }
    
    // 연체가 있으면 대출 불가
    if (overdue_days > 0) {
        return 0;
    }
    
    return 1;
}

int list_all_members(sqlite3 *db, Member *members, int max_count) {
    if (!members) {
        return -1;
    }
    
    const char *sql = "SELECT member_id, name, phone, address, registration_date "
                      "FROM Members ORDER BY member_id LIMIT ?;";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, max_count);
    
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && count < max_count) {
        members[count].member_id = sqlite3_column_int(stmt, 0);
        strncpy(members[count].name, (const char *)sqlite3_column_text(stmt, 1), MAX_NAME_LEN - 1);
        members[count].name[MAX_NAME_LEN - 1] = '\0';
        strncpy(members[count].phone, (const char *)sqlite3_column_text(stmt, 2), MAX_PHONE_LEN - 1);
        members[count].phone[MAX_PHONE_LEN - 1] = '\0';
        strncpy(members[count].address, (const char *)sqlite3_column_text(stmt, 3), MAX_ADDRESS_LEN - 1);
        members[count].address[MAX_ADDRESS_LEN - 1] = '\0';
        strncpy(members[count].registration_date, (const char *)sqlite3_column_text(stmt, 4), 10);
        members[count].registration_date[10] = '\0';
        
        check_member_overdue(db, members[count].member_id, &members[count].overdue_days);
        members[count].suspension_days = members[count].overdue_days * 2;
        
        count++;
    }
    
    sqlite3_finalize(stmt);
    return count;
}

int get_member_count(sqlite3 *db) {
    const char *sql = "SELECT COUNT(*) FROM Members;";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    int count = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    
    sqlite3_finalize(stmt);
    return count;
}
