* **함수 이름**: 동사로 시작하고 소문자로 시작하는 snake_case를 사용합니다 (예: `add_book`, `search_member`, `process_loan`).
* **변수 이름**: 소문자로 시작하는 snake_case를 사용합니다 (예: `book_id`, `member_name`, `loan_date`).
* **상수 이름**: 모두 대문자로 하고 단어 사이에 밑줄을 사용합니다 (예: `MAX_BOOKS`, `DEFAULT_LOAN_PERIOD`).
* **구조체 이름**: 대문자로 시작하는 CamelCase를 사용합니다 (예: `Book`, `Member`, `Loan`).
*   **파일 이름**: 소문자로 시작하는 snake_case를 사용하며, `.h`와 `.c` 확장자를 사용합니다 (예: `database.h`, `library.c`).

* **헤더 파일**:
    - 구조체 정의, 상수, 함수 프로토타입 선언은 헤더 파일 (`.h`)에 작성합니다.
    - 예: `book.h`, `member.h`, `loan.h`.
* **소스 파일**:
    - 함수 구현은 소스 파일 (`.c`)에 작성합니다.
    - 예: `book.c`, `member.c`, `loan.c`.
* **모듈화**:
    - 각 주요 기능(도서 관리, 회원 관리, 대출/반납 관리)은 별도의 모듈로 분리합니다.
* **메인 파일**:
    - `main.c` 파일에서 프로그램의 진입점을 정의하고, 각 모듈을 호출합니다.