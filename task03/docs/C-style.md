# C 기반 도서 대출 관리 프로그램 코딩 스타일 가이드

1. **명명 규칙**:

    *   **함수 이름**: 동사로 시작하고 소문자로 시작하는 snake_case를 사용합니다 (예: `add_book`, `search_member`, `process_loan`).
    *   **변수 이름**: 소문자로 시작하는 snake_case를 사용합니다 (예: `book_id`, `member_name`, `loan_date`).
    *   **상수 이름**: 모두 대문자로 하고 단어 사이에 밑줄을 사용합니다 (예: `MAX_BOOKS`, `DEFAULT_LOAN_PERIOD`).
    *   **구조체 이름**: 대문자로 시작하는 CamelCase를 사용합니다 (예: `Book`, `Member`, `Loan`).
    *   **파일 이름**: 소문자로 시작하는 snake_case를 사용하며, `.h`와 `.c` 확장자를 사용합니다 (예: `database.h`, `library.c`).

2. **코드 구조**:

    *   **헤더 파일**:
        - 구조체 정의, 상수, 함수 프로토타입 선언은 헤더 파일 (`.h`)에 작성합니다.
        - 예: `book.h`, `member.h`, `loan.h`.
    *   **소스 파일**:
        - 함수 구현은 소스 파일 (`.c`)에 작성합니다.
        - 예: `book.c`, `member.c`, `loan.c`.
    *   **모듈화**:
        - 각 주요 기능(도서 관리, 회원 관리, 대출/반납 관리)은 별도의 모듈로 분리합니다.
    *   **메인 파일**:
        - `main.c` 파일에서 프로그램의 진입점을 정의하고, 각 모듈을 호출합니다.

3. **주석**:

    *   **함수 주석**:
        - 함수의 역할, 파라미터, 반환 값을 설명합니다.
        - 예:
        ```c
        /**
         * @brief Add a new book to the library database.
         * 
         * @param title The title of the book.
         * @param author The author of the book.
         * @param isbn The ISBN of the book.
         * @return int Returns 0 on success, -1 on failure.
         */
        int add_book(const char *title, const char *author, const char *isbn);
        ```
    *   **코드 블록 주석**:
        - 복잡한 로직이나 중요한 결정에 대한 이유를 설명합니다.

4. **에러 처리**:

    *   함수는 성공 시 `0`, 실패 시 `-1` 또는 적절한 에러 코드를 반환합니다.
    *   데이터베이스 작업 실패 시 에러 메시지를 출력하고, 적절한 복구 작업을 수행합니다.

5. **메모리 관리**:

    *   동적 메모리 할당 시 `malloc`과 `free`를 사용하며, 메모리 누수를 방지하기 위해 모든 할당된 메모리를 해제합니다.
    *   데이터베이스 연결 및 결과 집합은 사용 후 반드시 닫습니다.

6. **SQLite 연동**:

    *   SQLite C API를 사용하여 데이터베이스 작업을 수행합니다.
    *   SQL 쿼리는 문자열로 작성하며, 파라미터 바인딩을 사용하여 SQL Injection을 방지합니다.
    *   데이터베이스 연결은 프로그램 시작 시 열고, 종료 시 닫습니다.

7. **기타**:

    *   `const` 키워드를 사용하여 변경되지 않는 데이터를 보호합니다.
    *   `#define`을 사용하여 상수를 정의합니다.
    *   코드 포맷터 (예: `clang-format`)를 사용하여 코드 스타일을 일관되게 유지합니다.
  
# 예제 코드 스타일

 **구조체 정의**:
```c
// filepath: include/book.h
#ifndef BOOK_H
#define BOOK_H

typedef struct {
    int id;
    char title[100];
    char author[50];
    char isbn[20];
    char genre[30];
    char publisher[50];
    int publication_year;
    int quantity;
    int available;
} Book;

#endif // BOOK_H
