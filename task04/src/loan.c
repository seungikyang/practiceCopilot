#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "loan.h"
#include "book.h"
#include "member.h"
#include "date_utils.h"

int loan_book(sqlite3 *db, int book_id, int member_id, int loan_days) {
    // 1. 도서 대출 가능 여부 확인
    if (is_book_available(db, book_id) != 1) {
        fprintf(stderr, "Book (ID: %d) is not available for loan\n", book_id);
        return -1;
    }
    
    // 2. 회원 대출 가능 여부 확인
    if (is_member_eligible_to_loan(db, member_id) != 1) {
        fprintf(stderr, "Member (ID: %d) is not eligible to loan books\n", member_id);
        return -1;
    }
    
    // 3. 대출 날짜와 반납 예정일 계산
    char loan_date[20];
    char due_date[20];
    get_date_string(loan_date, sizeof(loan_date), 0);
    get_date_string(due_date, sizeof(due_date), loan_days);
    
    // 4. 트랜잭션 시작
    char *err_msg = NULL;
    if (sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }
    
    // 5. Loans 테이블에 대출 정보 삽입
    const char *loan_sql = "INSERT INTO Loans (book_id, member_id, loan_date, due_date, is_returned) "
                           "VALUES (?, ?, ?, ?, 0)";
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, loan_sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_exec(db, "ROLLBACK", NULL, NULL, NULL);
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, book_id);
    sqlite3_bind_int(stmt, 2, member_id);
    sqlite3_bind_text(stmt, 3, loan_date, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, due_date, -1, SQLITE_STATIC);
    
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_exec(db, "ROLLBACK", NULL, NULL, NULL);
        return -1;
    }
    
    sqlite3_finalize(stmt);
    
    // 6. Books 테이블의 available 필드 업데이트
    const char *book_sql = "UPDATE Books SET available = available - 1 WHERE book_id = ?";
    
    if (sqlite3_prepare_v2(db, book_sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_exec(db, "ROLLBACK", NULL, NULL, NULL);
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, book_id);
    
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_exec(db, "ROLLBACK", NULL, NULL, NULL);
        return -1;
    }
    
    sqlite3_finalize(stmt);
    
    // 7. 트랜잭션 커밋
    if (sqlite3_exec(db, "COMMIT", NULL, NULL, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_exec(db, "ROLLBACK", NULL, NULL, NULL);
        return -1;
    }
    
    return 0;
}

int return_book(sqlite3 *db, int loan_id) {
    // 1. 대출 정보 조회
    Loan loan;
    if (get_loan_by_id(db, loan_id, &loan) != 0) {
        fprintf(stderr, "Loan (ID: %d) not found\n", loan_id);
        return -1;
    }
    
    // 이미 반납된 도서인지 확인
    if (loan.is_returned) {
        fprintf(stderr, "Book already returned for loan ID: %d\n", loan_id);
        return -1;
    }
    
    // 2. 현재 날짜 구하기
    char return_date[20];
    get_date_string(return_date, sizeof(return_date), 0);
    
    // 3. 트랜잭션 시작
    char *err_msg = NULL;
    if (sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }
    
    // 4. 연체 일수 계산
    int days_overdue = calculate_overdue_days(loan.due_date, return_date);
    
    // 5. Loans 테이블 업데이트 (반납 처리)
    const char *loan_sql = "UPDATE Loans SET is_returned = 1 WHERE loan_id = ?";
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, loan_sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_exec(db, "ROLLBACK", NULL, NULL, NULL);
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, loan_id);
    
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_exec(db, "ROLLBACK", NULL, NULL, NULL);
        return -1;
    }
    
    sqlite3_finalize(stmt);
    
    // 6. Returns 테이블에 반납 정보 삽입
    const char *return_sql = "INSERT INTO Returns (loan_id, return_date, days_overdue) "
                             "VALUES (?, ?, ?)";
    
    if (sqlite3_prepare_v2(db, return_sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_exec(db, "ROLLBACK", NULL, NULL, NULL);
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, loan_id);
    sqlite3_bind_text(stmt, 2, return_date, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, days_overdue);
    
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_exec(db, "ROLLBACK", NULL, NULL, NULL);
        return -1;
    }
    
    sqlite3_finalize(stmt);
    
    // 7. Books 테이블의 available 필드 업데이트
    const char *book_sql = "UPDATE Books SET available = available + 1 WHERE book_id = ?";
    
    if (sqlite3_prepare_v2(db, book_sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_exec(db, "ROLLBACK", NULL, NULL, NULL);
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, loan.book_id);
    
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_exec(db, "ROLLBACK", NULL, NULL, NULL);
        return -1;
    }
    
    sqlite3_finalize(stmt);
    
    // 8. 연체 시 패널티 처리
    if (days_overdue > 0) {
        // 1일 연체 당 2일의 패널티
        int penalty_days = days_overdue * 2;
        if (record_penalty(db, loan.member_id, penalty_days) != 0) {
            fprintf(stderr, "Failed to record penalty for member ID: %d\n", loan.member_id);
            sqlite3_exec(db, "ROLLBACK", NULL, NULL, NULL);
            return -1;
        }
    }
    
    // 9. 트랜잭션 커밋
    if (sqlite3_exec(db, "COMMIT", NULL, NULL, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_exec(db, "ROLLBACK", NULL, NULL, NULL);
        return -1;
    }
    
    return 0;
}

int get_member_loans(sqlite3 *db, int member_id, Loan *loans, int max_loans) {
    const char *sql = "SELECT loan_id, book_id, member_id, loan_date, due_date, is_returned "
                      "FROM Loans WHERE member_id = ? AND is_returned = 0 "
                      "ORDER BY due_date ASC";
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, member_id);
    
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && count < max_loans) {
        loans[count].loan_id = sqlite3_column_int(stmt, 0);
        loans[count].book_id = sqlite3_column_int(stmt, 1);
        loans[count].member_id = sqlite3_column_int(stmt, 2);
        
        const char *loan_date = (const char *)sqlite3_column_text(stmt, 3);
        const char *due_date = (const char *)sqlite3_column_text(stmt, 4);
        
        strncpy(loans[count].loan_date, loan_date, sizeof(loans[count].loan_date) - 1);
        loans[count].loan_date[sizeof(loans[count].loan_date) - 1] = '\0';
        
        strncpy(loans[count].due_date, due_date, sizeof(loans[count].due_date) - 1);
        loans[count].due_date[sizeof(loans[count].due_date) - 1] = '\0';
        
        loans[count].is_returned = sqlite3_column_int(stmt, 5);
        
        count++;
    }
    
    sqlite3_finalize(stmt);
    return count;
}

int get_book_loan_history(sqlite3 *db, int book_id, Loan *loans, int max_loans) {
    const char *sql = "SELECT loan_id, book_id, member_id, loan_date, due_date, is_returned "
                      "FROM Loans WHERE book_id = ? "
                      "ORDER BY loan_date DESC";
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, book_id);
    
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && count < max_loans) {
        loans[count].loan_id = sqlite3_column_int(stmt, 0);
        loans[count].book_id = sqlite3_column_int(stmt, 1);
        loans[count].member_id = sqlite3_column_int(stmt, 2);
        
        const char *loan_date = (const char *)sqlite3_column_text(stmt, 3);
        const char *due_date = (const char *)sqlite3_column_text(stmt, 4);
        
        strncpy(loans[count].loan_date, loan_date, sizeof(loans[count].loan_date) - 1);
        loans[count].loan_date[sizeof(loans[count].loan_date) - 1] = '\0';
        
        strncpy(loans[count].due_date, due_date, sizeof(loans[count].due_date) - 1);
        loans[count].due_date[sizeof(loans[count].due_date) - 1] = '\0';
        
        loans[count].is_returned = sqlite3_column_int(stmt, 5);
        
        count++;
    }
    
    sqlite3_finalize(stmt);
    return count;
}

int get_loan_by_id(sqlite3 *db, int loan_id, Loan *loan) {
    const char *sql = "SELECT loan_id, book_id, member_id, loan_date, due_date, is_returned "
                      "FROM Loans WHERE loan_id = ?";
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, loan_id);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        loan->loan_id = sqlite3_column_int(stmt, 0);
        loan->book_id = sqlite3_column_int(stmt, 1);
        loan->member_id = sqlite3_column_int(stmt, 2);
        
        const char *loan_date = (const char *)sqlite3_column_text(stmt, 3);
        const char *due_date = (const char *)sqlite3_column_text(stmt, 4);
        
        strncpy(loan->loan_date, loan_date, sizeof(loan->loan_date) - 1);
        loan->loan_date[sizeof(loan->loan_date) - 1] = '\0';
        
        strncpy(loan->due_date, due_date, sizeof(loan->due_date) - 1);
        loan->due_date[sizeof(loan->due_date) - 1] = '\0';
        
        loan->is_returned = sqlite3_column_int(stmt, 5);
        
        sqlite3_finalize(stmt);
        return 0;
    }
    
    sqlite3_finalize(stmt);
    return -1;
}

int is_book_available(sqlite3 *db, int book_id) {
    const char *sql = "SELECT available FROM Books WHERE book_id = ?";
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, book_id);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int available = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        return (available > 0) ? 1 : 0;
    }
    
    sqlite3_finalize(stmt);
    return -1; // 도서가 존재하지 않음
}

int is_member_eligible_to_loan(sqlite3 *db, int member_id) {
    // 회원 존재 확인 및 패널티 확인
    int penalty_days = 0;
    if (check_penalty_status(db, member_id, &penalty_days) != 0) {
        return -1; // 회원이 존재하지 않거나 오류
    }
    
    return (penalty_days <= 0) ? 1 : 0;
}

int get_overdue_loans(sqlite3 *db, Loan *loans, int max_loans) {
    char current_date[20];
    get_date_string(current_date, sizeof(current_date), 0);
    
    const char *sql = "SELECT loan_id, book_id, member_id, loan_date, due_date, is_returned "
                      "FROM Loans WHERE is_returned = 0 AND due_date < ? "
                      "ORDER BY due_date ASC";
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_text(stmt, 1, current_date, -1, SQLITE_STATIC);
    
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && count < max_loans) {
        loans[count].loan_id = sqlite3_column_int(stmt, 0);
        loans[count].book_id = sqlite3_column_int(stmt, 1);
        loans[count].member_id = sqlite3_column_int(stmt, 2);
        
        const char *loan_date = (const char *)sqlite3_column_text(stmt, 3);
        const char *due_date = (const char *)sqlite3_column_text(stmt, 4);
        
        strncpy(loans[count].loan_date, loan_date, sizeof(loans[count].loan_date) - 1);
        loans[count].loan_date[sizeof(loans[count].loan_date) - 1] = '\0';
        
        strncpy(loans[count].due_date, due_date, sizeof(loans[count].due_date) - 1);
        loans[count].due_date[sizeof(loans[count].due_date) - 1] = '\0';
        
        loans[count].is_returned = sqlite3_column_int(stmt, 5);
        
        count++;
    }
    
    sqlite3_finalize(stmt);
    return count;
}

int calculate_overdue_days(const char *due_date, const char *return_date) {
    char actual_return_date[20];
    
    // return_date가 NULL이면 현재 날짜 사용
    if (return_date == NULL) {
        get_date_string(actual_return_date, sizeof(actual_return_date), 0);
        return_date = actual_return_date;
    }
    
    int days = date_difference(due_date, return_date);
    
    // 음수이면 연체가 없음
    return (days > 0) ? days : 0;
}

int get_popular_books(sqlite3 *db, int *book_ids, int *loan_counts, int max_books) {
    const char *sql = "SELECT book_id, COUNT(*) as loan_count "
                      "FROM Loans "
                      "GROUP BY book_id "
                      "ORDER BY loan_count DESC "
                      "LIMIT ?";
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    
    sqlite3_bind_int(stmt, 1, max_books);
    
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && count < max_books) {
        book_ids[count] = sqlite3_column_int(stmt, 0);
        loan_counts[count] = sqlite3_column_int(stmt, 1);
        count++;
    }
    
    sqlite3_finalize(stmt);
    return count;
}