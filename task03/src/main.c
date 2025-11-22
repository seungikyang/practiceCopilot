#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h"
#include "book.h"
#include "member.h"
#include "loan.h"

#define MAX_INPUT 256

/**
 * @brief Clear input buffer.
 */
void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/**
 * @brief Display main menu.
 */
void display_main_menu(void) {
    printf("\n");
    printf("=====================================\n");
    printf("    작은 도서관 관리 시스템\n");
    printf("=====================================\n");
    printf("1. 도서 관리\n");
    printf("2. 회원 관리\n");
    printf("3. 대출/반납 관리\n");
    printf("4. 보고서\n");
    printf("0. 종료\n");
    printf("=====================================\n");
    printf("선택: ");
}

/**
 * @brief Display book management menu.
 */
void display_book_menu(void) {
    printf("\n");
    printf("===== 도서 관리 =====\n");
    printf("1. 도서 등록\n");
    printf("2. 도서 검색\n");
    printf("3. 도서 수정\n");
    printf("4. 도서 삭제\n");
    printf("5. 전체 도서 목록\n");
    printf("6. 장르별 검색\n");
    printf("7. 저자별 검색\n");
    printf("0. 메인 메뉴로\n");
    printf("=====================\n");
    printf("선택: ");
}

/**
 * @brief Display member management menu.
 */
void display_member_menu(void) {
    printf("\n");
    printf("===== 회원 관리 =====\n");
    printf("1. 회원 등록\n");
    printf("2. 회원 검색 (이름)\n");
    printf("3. 회원 정보 조회 (ID)\n");
    printf("4. 회원 정보 수정\n");
    printf("5. 회원 삭제\n");
    printf("6. 전체 회원 목록\n");
    printf("7. 회원 연체 상태 확인\n");
    printf("0. 메인 메뉴로\n");
    printf("=====================\n");
    printf("선택: ");
}

/**
 * @brief Display loan management menu.
 */
void display_loan_menu(void) {
    printf("\n");
    printf("===== 대출/반납 관리 =====\n");
    printf("1. 도서 대출\n");
    printf("2. 도서 반납\n");
    printf("3. 회원별 대출 목록\n");
    printf("4. 활성 대출 목록\n");
    printf("5. 연체 도서 목록\n");
    printf("6. 대출 이력 조회 (회원)\n");
    printf("7. 대출 이력 조회 (도서)\n");
    printf("0. 메인 메뉴로\n");
    printf("=========================\n");
    printf("선택: ");
}

/**
 * @brief Display report menu.
 */
void display_report_menu(void) {
    printf("\n");
    printf("===== 보고서 =====\n");
    printf("1. 인기 도서 Top 10\n");
    printf("2. 연체 현황 보고서\n");
    printf("3. 도서 재고 현황\n");
    printf("4. 회원 통계\n");
    printf("0. 메인 메뉴로\n");
    printf("==================\n");
    printf("선택: ");
}

/**
 * @brief Handle book management operations.
 */
void handle_book_menu(void) {
    int choice;
    char title[100], author[50], publisher[50], isbn[20], genre[30];
    int year, quantity, book_id;
    
    while (1) {
        display_book_menu();
        
        if (scanf("%d", &choice) != 1) {
            clear_input_buffer();
            printf("잘못된 입력입니다.\n");
            continue;
        }
        clear_input_buffer();
        
        switch (choice) {
            case 1: /* 도서 등록 */
                printf("\n=== 도서 등록 ===\n");
                printf("제목: ");
                fgets(title, sizeof(title), stdin);
                title[strcspn(title, "\n")] = 0;
                
                printf("저자: ");
                fgets(author, sizeof(author), stdin);
                author[strcspn(author, "\n")] = 0;
                
                printf("출판사: ");
                fgets(publisher, sizeof(publisher), stdin);
                publisher[strcspn(publisher, "\n")] = 0;
                
                printf("출판년도: ");
                scanf("%d", &year);
                clear_input_buffer();
                
                printf("ISBN: ");
                fgets(isbn, sizeof(isbn), stdin);
                isbn[strcspn(isbn, "\n")] = 0;
                
                printf("장르: ");
                fgets(genre, sizeof(genre), stdin);
                genre[strcspn(genre, "\n")] = 0;
                
                printf("수량: ");
                scanf("%d", &quantity);
                clear_input_buffer();
                
                add_book(title, author, publisher, year, isbn, genre, quantity);
                break;
                
            case 2: /* 도서 검색 */
                printf("\n=== 도서 검색 ===\n");
                printf("검색어 (제목/저자/ISBN): ");
                fgets(title, sizeof(title), stdin);
                title[strcspn(title, "\n")] = 0;
                search_book(title);
                break;
                
            case 3: /* 도서 수정 */
                printf("\n=== 도서 수정 ===\n");
                printf("도서 ID: ");
                scanf("%d", &book_id);
                clear_input_buffer();
                
                printf("새 제목 (변경 없으면 Enter): ");
                fgets(title, sizeof(title), stdin);
                title[strcspn(title, "\n")] = 0;
                
                printf("새 저자 (변경 없으면 Enter): ");
                fgets(author, sizeof(author), stdin);
                author[strcspn(author, "\n")] = 0;
                
                printf("새 출판사 (변경 없으면 Enter): ");
                fgets(publisher, sizeof(publisher), stdin);
                publisher[strcspn(publisher, "\n")] = 0;
                
                printf("새 출판년도 (변경 없으면 0): ");
                scanf("%d", &year);
                clear_input_buffer();
                
                printf("새 장르 (변경 없으면 Enter): ");
                fgets(genre, sizeof(genre), stdin);
                genre[strcspn(genre, "\n")] = 0;
                
                update_book(book_id, 
                           strlen(title) > 0 ? title : NULL,
                           strlen(author) > 0 ? author : NULL,
                           strlen(publisher) > 0 ? publisher : NULL,
                           year,
                           strlen(genre) > 0 ? genre : NULL);
                break;
                
            case 4: /* 도서 삭제 */
                printf("\n=== 도서 삭제 ===\n");
                printf("도서 ID: ");
                scanf("%d", &book_id);
                clear_input_buffer();
                
                printf("정말 삭제하시겠습니까? (y/n): ");
                char confirm;
                scanf("%c", &confirm);
                clear_input_buffer();
                
                if (confirm == 'y' || confirm == 'Y') {
                    delete_book(book_id);
                } else {
                    printf("삭제가 취소되었습니다.\n");
                }
                break;
                
            case 5: /* 전체 도서 목록 */
                display_all_books();
                break;
                
            case 6: /* 장르별 검색 */
                printf("\n=== 장르별 검색 ===\n");
                printf("장르: ");
                fgets(genre, sizeof(genre), stdin);
                genre[strcspn(genre, "\n")] = 0;
                search_books_by_genre(genre);
                break;
                
            case 7: /* 저자별 검색 */
                printf("\n=== 저자별 검색 ===\n");
                printf("저자: ");
                fgets(author, sizeof(author), stdin);
                author[strcspn(author, "\n")] = 0;
                search_books_by_author(author);
                break;
                
            case 0:
                return;
                
            default:
                printf("잘못된 선택입니다.\n");
        }
    }
}

/**
 * @brief Handle member management operations.
 */
void handle_member_menu(void) {
    int choice;
    char name[MAX_NAME_LEN], phone[MAX_PHONE_LEN], address[MAX_ADDRESS_LEN];
    int member_id;
    sqlite3 *db = get_db_connection();
    
    while (1) {
        display_member_menu();
        
        if (scanf("%d", &choice) != 1) {
            clear_input_buffer();
            printf("잘못된 입력입니다.\n");
            continue;
        }
        clear_input_buffer();
        
        switch (choice) {
            case 1: /* 회원 등록 */
                printf("\n=== 회원 등록 ===\n");
                printf("이름: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;
                
                printf("전화번호: ");
                fgets(phone, sizeof(phone), stdin);
                phone[strcspn(phone, "\n")] = 0;
                
                printf("주소: ");
                fgets(address, sizeof(address), stdin);
                address[strcspn(address, "\n")] = 0;
                
                member_id = add_member(db, name, phone, address);
                if (member_id > 0) {
                    printf("회원 등록 성공 (회원 ID: %d)\n", member_id);
                }
                break;
                
            case 2: /* 회원 검색 (이름) */
                printf("\n=== 회원 검색 ===\n");
                printf("이름: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;
                
                Member members[10];
                int count = search_member_by_name(db, name, members, 10);
                
                if (count > 0) {
                    printf("\n%-8s %-20s %-15s %-30s %-12s\n",
                           "회원 ID", "이름", "전화번호", "주소", "등록일");
                    printf("-----------------------------------------------------------------------\n");
                    for (int i = 0; i < count; i++) {
                        printf("%-8d %-20s %-15s %-30s %-12s\n",
                               members[i].member_id, members[i].name, 
                               members[i].phone, members[i].address,
                               members[i].registration_date);
                    }
                } else if (count == 0) {
                    printf("검색 결과가 없습니다.\n");
                }
                break;
                
            case 3: /* 회원 정보 조회 (ID) */
                printf("\n=== 회원 정보 조회 ===\n");
                printf("회원 ID: ");
                scanf("%d", &member_id);
                clear_input_buffer();
                
                Member member;
                if (search_member_by_id(db, member_id, &member) == 0) {
                    printf("\n회원 ID: %d\n", member.member_id);
                    printf("이름: %s\n", member.name);
                    printf("전화번호: %s\n", member.phone);
                    printf("주소: %s\n", member.address);
                    printf("등록일: %s\n", member.registration_date);
                    printf("연체 일수: %d일\n", member.overdue_days);
                    printf("대출 정지 일수: %d일\n", member.suspension_days);
                    
                    if (member.overdue_days > 0) {
                        printf("⚠️  연체 중입니다!\n");
                    } else {
                        printf("✅ 정상 회원입니다.\n");
                    }
                } else {
                    printf("회원을 찾을 수 없습니다.\n");
                }
                break;
                
            case 4: /* 회원 정보 수정 */
                printf("\n=== 회원 정보 수정 ===\n");
                printf("회원 ID: ");
                scanf("%d", &member_id);
                clear_input_buffer();
                
                printf("새 이름 (변경 없으면 Enter): ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;
                
                printf("새 전화번호 (변경 없으면 Enter): ");
                fgets(phone, sizeof(phone), stdin);
                phone[strcspn(phone, "\n")] = 0;
                
                printf("새 주소 (변경 없으면 Enter): ");
                fgets(address, sizeof(address), stdin);
                address[strcspn(address, "\n")] = 0;
                
                update_member(db, member_id,
                             strlen(name) > 0 ? name : NULL,
                             strlen(phone) > 0 ? phone : NULL,
                             strlen(address) > 0 ? address : NULL);
                break;
                
            case 5: /* 회원 삭제 */
                printf("\n=== 회원 삭제 ===\n");
                printf("회원 ID: ");
                scanf("%d", &member_id);
                clear_input_buffer();
                
                printf("정말 삭제하시겠습니까? (y/n): ");
                char confirm;
                scanf("%c", &confirm);
                clear_input_buffer();
                
                if (confirm == 'y' || confirm == 'Y') {
                    delete_member(db, member_id);
                } else {
                    printf("삭제가 취소되었습니다.\n");
                }
                break;
                
            case 6: /* 전체 회원 목록 */
                {
                    Member all_members[100];
                    int total = list_all_members(db, all_members, 100);
                    
                    if (total > 0) {
                        printf("\n%-8s %-20s %-15s %-30s %-12s %-8s\n",
                               "회원 ID", "이름", "전화번호", "주소", "등록일", "연체일");
                        printf("---------------------------------------------------------------------------------\n");
                        for (int i = 0; i < total; i++) {
                            printf("%-8d %-20s %-15s %-30s %-12s %-8d\n",
                                   all_members[i].member_id, all_members[i].name,
                                   all_members[i].phone, all_members[i].address,
                                   all_members[i].registration_date, all_members[i].overdue_days);
                        }
                        printf("\n총 회원 수: %d\n", total);
                    }
                }
                break;
                
            case 7: /* 회원 연체 상태 확인 */
                printf("\n=== 회원 연체 상태 확인 ===\n");
                printf("회원 ID: ");
                scanf("%d", &member_id);
                clear_input_buffer();
                
                int overdue_days;
                int has_overdue = check_member_overdue(db, member_id, &overdue_days);
                
                if (has_overdue > 0) {
                    printf("⚠️  연체 중입니다!\n");
                    printf("연체 일수: %d일\n", overdue_days);
                    printf("대출 정지 기간: %d일\n", calculate_suspension_days(overdue_days));
                } else if (has_overdue == 0) {
                    printf("✅ 연체 없음 (정상)\n");
                }
                break;
                
            case 0:
                return;
                
            default:
                printf("잘못된 선택입니다.\n");
        }
    }
}

/**
 * @brief Handle loan management operations.
 */
void handle_loan_menu(void) {
    int choice;
    int book_id, member_id, loan_id, loan_period;
    sqlite3 *db = get_db_connection();
    
    while (1) {
        display_loan_menu();
        
        if (scanf("%d", &choice) != 1) {
            clear_input_buffer();
            printf("잘못된 입력입니다.\n");
            continue;
        }
        clear_input_buffer();
        
        switch (choice) {
            case 1: /* 도서 대출 */
                printf("\n=== 도서 대출 ===\n");
                printf("회원 ID: ");
                scanf("%d", &member_id);
                clear_input_buffer();
                
                printf("도서 ID: ");
                scanf("%d", &book_id);
                clear_input_buffer();
                
                printf("대출 기간(일) [기본 14일]: ");
                if (scanf("%d", &loan_period) != 1) {
                    loan_period = 14;
                }
                clear_input_buffer();
                
                process_loan(db, book_id, member_id, loan_period);
                break;
                
            case 2: /* 도서 반납 */
                printf("\n=== 도서 반납 ===\n");
                printf("대출 ID: ");
                scanf("%d", &loan_id);
                clear_input_buffer();
                
                process_return(db, loan_id);
                break;
                
            case 3: /* 회원별 대출 목록 */
                printf("\n=== 회원별 대출 목록 ===\n");
                printf("회원 ID: ");
                scanf("%d", &member_id);
                clear_input_buffer();
                
                Loan loans[50];
                int count = get_active_loans_by_member(db, member_id, loans, 50);
                
                if (count > 0) {
                    printf("\n%-8s %-8s %-12s %-12s\n",
                           "대출 ID", "도서 ID", "대출일", "반납 예정일");
                    printf("-----------------------------------------------\n");
                    for (int i = 0; i < count; i++) {
                        printf("%-8d %-8d %-12s %-12s\n",
                               loans[i].loan_id, loans[i].book_id,
                               loans[i].loan_date, loans[i].due_date);
                    }
                    printf("\n총 %d건의 대출\n", count);
                } else if (count == 0) {
                    printf("대출 중인 도서가 없습니다.\n");
                }
                break;
                
            case 4: /* 활성 대출 목록 */
                display_active_loans(db);
                break;
                
            case 5: /* 연체 도서 목록 */
                display_overdue_report(db);
                break;
                
            case 6: /* 대출 이력 조회 (회원) */
                printf("\n=== 대출 이력 조회 (회원) ===\n");
                printf("회원 ID: ");
                scanf("%d", &member_id);
                clear_input_buffer();
                
                {
                    Loan history[100];
                    int count = get_loan_history_by_member(db, member_id, history, 100);
                    
                    if (count > 0) {
                        printf("\n%-8s %-8s %-12s %-12s %-8s\n",
                               "대출 ID", "도서 ID", "대출일", "반납 예정일", "반납 상태");
                        printf("---------------------------------------------------------\n");
                        for (int i = 0; i < count; i++) {
                            printf("%-8d %-8d %-12s %-12s %-8s\n",
                                   history[i].loan_id, history[i].book_id,
                                   history[i].loan_date, history[i].due_date,
                                   history[i].is_returned ? "반납완료" : "대출중");
                        }
                        printf("\n총 %d건\n", count);
                    }
                }
                break;
                
            case 7: /* 대출 이력 조회 (도서) */
                printf("\n=== 대출 이력 조회 (도서) ===\n");
                printf("도서 ID: ");
                scanf("%d", &book_id);
                clear_input_buffer();
                
                {
                    Loan history[100];
                    int count = get_loan_history_by_book(db, book_id, history, 100);
                    
                    if (count > 0) {
                        printf("\n%-8s %-8s %-12s %-12s %-8s\n",
                               "대출 ID", "회원 ID", "대출일", "반납 예정일", "반납 상태");
                        printf("---------------------------------------------------------\n");
                        for (int i = 0; i < count; i++) {
                            printf("%-8d %-8d %-12s %-12s %-8s\n",
                                   history[i].loan_id, history[i].member_id,
                                   history[i].loan_date, history[i].due_date,
                                   history[i].is_returned ? "반납완료" : "대출중");
                        }
                        printf("\n총 %d건\n", count);
                    }
                }
                break;
                
            case 0:
                return;
                
            default:
                printf("잘못된 선택입니다.\n");
        }
    }
}

/**
 * @brief Handle report operations.
 */
void handle_report_menu(void) {
    int choice;
    sqlite3 *db = get_db_connection();
    
    while (1) {
        display_report_menu();
        
        if (scanf("%d", &choice) != 1) {
            clear_input_buffer();
            printf("잘못된 입력입니다.\n");
            continue;
        }
        clear_input_buffer();
        
        switch (choice) {
            case 1: /* 인기 도서 Top 10 */
                get_popular_books(db, 10);
                break;
                
            case 2: /* 연체 현황 보고서 */
                display_overdue_report(db);
                break;
                
            case 3: /* 도서 재고 현황 */
                printf("\n=== 도서 재고 현황 ===\n");
                display_all_books();
                break;
                
            case 4: /* 회원 통계 */
                {
                    int total = get_member_count(db);
                    printf("\n=== 회원 통계 ===\n");
                    printf("총 회원 수: %d명\n", total);
                    
                    /* 연체 회원 수 조회 */
                    const char *sql = "SELECT COUNT(DISTINCT member_id) FROM Loans "
                                     "WHERE is_returned = 0 AND date('now') > date(due_date);";
                    
                    sqlite3_stmt *stmt;
                    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
                        if (sqlite3_step(stmt) == SQLITE_ROW) {
                            int overdue_count = sqlite3_column_int(stmt, 0);
                            printf("연체 회원 수: %d명\n", overdue_count);
                        }
                        sqlite3_finalize(stmt);
                    }
                }
                break;
                
            case 0:
                return;
                
            default:
                printf("잘못된 선택입니다.\n");
        }
    }
}

/**
 * @brief Main function - entry point of the program.
 */
int main(void) {
    printf("\n");
    printf("########################################\n");
    printf("#                                      #\n");
    printf("#     작은 도서관 관리 시스템 v1.0     #\n");
    printf("#                                      #\n");
    printf("########################################\n");
    printf("\n");
    
    /* Initialize database */
    if (init_database() != 0) {
        fprintf(stderr, "데이터베이스 초기화 실패\n");
        return EXIT_FAILURE;
    }
    
    /* Initialize loan tables */
    sqlite3 *db = get_db_connection();
    if (init_loan_tables(db) != 0) {
        fprintf(stderr, "대출 테이블 초기화 실패\n");
        close_database();
        return EXIT_FAILURE;
    }
    
    /* Initialize member table */
    if (init_member_table(db) != 0) {
        fprintf(stderr, "회원 테이블 초기화 실패\n");
        close_database();
        return EXIT_FAILURE;
    }
    
    int choice;
    
    /* Main loop */
    while (1) {
        display_main_menu();
        
        if (scanf("%d", &choice) != 1) {
            clear_input_buffer();
            printf("잘못된 입력입니다. 숫자를 입력해주세요.\n");
            continue;
        }
        clear_input_buffer();
        
        switch (choice) {
            case 1:
                handle_book_menu();
                break;
                
            case 2:
                handle_member_menu();
                break;
                
            case 3:
                handle_loan_menu();
                break;
                
            case 4:
                handle_report_menu();
                break;
                
            case 0:
                printf("\n프로그램을 종료합니다.\n");
                close_database();
                return EXIT_SUCCESS;
                
            default:
                printf("잘못된 선택입니다. 다시 선택해주세요.\n");
        }
    }
    
    return EXIT_SUCCESS;
}
