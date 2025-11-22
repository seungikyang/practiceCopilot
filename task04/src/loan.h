#ifndef LOAN_H
#define LOAN_H

#include <sqlite3.h>
#include <time.h>
#include "book.h"
#include "member.h"

/**
 * @brief 대출 정보를 담는 구조체
 */
typedef struct {
    int loan_id;
    int book_id;
    int member_id;
    char loan_date[20];
    char due_date[20];
    int is_returned;
} Loan;

/**
 * @brief 반납 정보를 담는 구조체
 */
typedef struct {
    int return_id;
    int loan_id;
    char return_date[20];
    int days_overdue; // 연체 일수 (0이면 연체 없음)
} Return;

/**
 * @brief 도서를 대출 처리한다.
 * 
 * @param db 데이터베이스 연결
 * @param book_id 대출할 도서 ID
 * @param member_id 대출하는 회원 ID
 * @param loan_days 대출 기간 (일)
 * @return int 성공 시 0, 실패 시 -1 반환
 */
int loan_book(sqlite3 *db, int book_id, int member_id, int loan_days);

/**
 * @brief 도서를 반납 처리한다.
 * 
 * @param db 데이터베이스 연결
 * @param loan_id 대출 ID
 * @return int 성공 시 0, 실패 시 -1 반환 (연체의 경우에도 성공으로 처리하고 연체 패널티는 별도 기록)
 */
int return_book(sqlite3 *db, int loan_id);

/**
 * @brief 특정 회원의 현재 대출 목록을 조회한다.
 * 
 * @param db 데이터베이스 연결
 * @param member_id 회원 ID
 * @param loans 대출 정보를 저장할 배열
 * @param max_loans 배열의 최대 크기
 * @return int 조회된 대출 수, 실패 시 -1 반환
 */
int get_member_loans(sqlite3 *db, int member_id, Loan *loans, int max_loans);

/**
 * @brief 특정 도서의 대출 이력을 조회한다.
 * 
 * @param db 데이터베이스 연결
 * @param book_id 도서 ID
 * @param loans 대출 정보를 저장할 배열
 * @param max_loans 배열의 최대 크기
 * @return int 조회된 대출 수, 실패 시 -1 반환
 */
int get_book_loan_history(sqlite3 *db, int book_id, Loan *loans, int max_loans);

/**
 * @brief 대출 ID로 대출 정보를 조회한다.
 * 
 * @param db 데이터베이스 연결
 * @param loan_id 대출 ID
 * @param loan 조회 결과를 저장할 구조체 포인터
 * @return int 성공 시 0, 실패 시 -1 반환
 */
int get_loan_by_id(sqlite3 *db, int loan_id, Loan *loan);

/**
 * @brief 도서의 대출 가능 여부를 확인한다.
 * 
 * @param db 데이터베이스 연결
 * @param book_id 도서 ID
 * @return int 대출 가능하면 1, 불가능하면 0, 에러 시 -1 반환
 */
int is_book_available(sqlite3 *db, int book_id);

/**
 * @brief 회원의 대출 가능 여부를 확인한다 (연체 패널티 등 고려).
 * 
 * @param db 데이터베이스 연결
 * @param member_id 회원 ID
 * @return int 대출 가능하면 1, 불가능하면 0, 에러 시 -1 반환
 */
int is_member_eligible_to_loan(sqlite3 *db, int member_id);

/**
 * @brief 연체 중인 모든 대출 목록을 조회한다.
 * 
 * @param db 데이터베이스 연결
 * @param loans 대출 정보를 저장할 배열
 * @param max_loans 배열의 최대 크기
 * @return int 조회된 대출 수, 실패 시 -1 반환
 */
int get_overdue_loans(sqlite3 *db, Loan *loans, int max_loans);

/**
 * @brief 대출 연체 일수를 계산한다.
 * 
 * @param due_date 반납 예정일 (YYYY-MM-DD 형식)
 * @param return_date 실제 반납일 (YYYY-MM-DD 형식) - NULL인 경우 현재 날짜 사용
 * @return int 연체 일수 (연체가 없으면 0)
 */
int calculate_overdue_days(const char *due_date, const char *return_date);

/**
 * @brief 인기 도서 목록을 조회한다 (대출 횟수 기준).
 * 
 * @param db 데이터베이스 연결
 * @param book_ids 인기 도서 ID를 저장할 배열
 * @param loan_counts 각 도서의 대출 횟수를 저장할 배열
 * @param max_books 배열의 최대 크기
 * @return int 조회된 도서 수, 실패 시 -1 반환
 */
int get_popular_books(sqlite3 *db, int *book_ids, int *loan_counts, int max_books);

#endif /* LOAN_H */