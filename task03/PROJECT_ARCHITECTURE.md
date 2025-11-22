# 작은 도서관 시스템 - 프로젝트 다이어그램

## 📊 1. 시스템 아키텍처

```mermaid
graph TB
    subgraph "User Interface"
        CLI[CLI Menu<br/>main.c]
    end
    
    subgraph "Business Logic"
        BOOK[Book Module<br/>book.c/h]
        MEMBER[Member Module<br/>member.c/h]
        LOAN[Loan Module<br/>loan.c/h]
    end
    
    subgraph "Data Access"
        DB[Database Manager<br/>database.c/h]
    end
    
    subgraph "Storage"
        SQLITE[(SQLite DB<br/>library.db)]
    end
    
    CLI --> BOOK
    CLI --> MEMBER
    CLI --> LOAN
    
    BOOK --> DB
    MEMBER --> DB
    LOAN --> DB
    
    DB --> SQLITE
```

## 🗄️ 2. 데이터베이스 ERD

```mermaid
erDiagram
    Books ||--o{ Loans : "대출됨"
    Members ||--o{ Loans : "대출함"
    Loans ||--o| Returns : "반납"
    
    Books {
        int book_id PK "도서ID"
        text title "제목"
        text author "저자"
        text publisher "출판사"
        int publication_year "출판년도"
        text isbn UK "ISBN"
        text genre "장르"
    }
    
    Members {
        int member_id PK "회원ID"
        text name "이름"
        text phone "전화번호"
        text address "주소"
        text registration_date "등록일"
        int penalty_days "페널티(일)"
    }
    
    Loans {
        int loan_id PK "대출ID"
        int book_id FK "도서ID"
        int member_id FK "회원ID"
        text loan_date "대출일"
        text due_date "반납예정일"
        int is_returned "반납여부"
    }
    
    Returns {
        int return_id PK "반납ID"
        int loan_id FK "대출ID"
        text return_date "반납일"
        int overdue_days "연체일수"
    }
```

## 🔄 3. 대출 프로세스 시퀀스

```mermaid
sequenceDiagram
    actor User as 사용자
    participant CLI
    participant Member as 회원모듈
    participant Book as 도서모듈
    participant Loan as 대출모듈
    participant DB as 데이터베이스
    
    User->>CLI: 대출 요청
    CLI->>Member: check_penalty_status()
    Member->>DB: SELECT penalty_days
    DB-->>Member: penalty_days
    
    alt 연체 중 (penalty_days > 0)
        Member-->>CLI: 대출 불가
        CLI-->>User: "대출 제한 중"
    else 정상 (penalty_days = 0)
        CLI->>Book: 도서 확인
        Book->>DB: SELECT book
        DB-->>Book: book info
        
        alt 도서 있음
            CLI->>Loan: process_loan()
            Loan->>DB: BEGIN TRANSACTION
            Loan->>DB: INSERT Loans
            Loan->>DB: UPDATE Books
            Loan->>DB: COMMIT
            DB-->>Loan: Success
            Loan-->>CLI: 대출 완료
            CLI-->>User: "대출되었습니다"
        else 도서 없음
            Book-->>CLI: 대출 불가
            CLI-->>User: "재고 없음"
        end
    end
```

## 📤 4. 반납 및 연체 처리

```mermaid
sequenceDiagram
    actor User as 사용자
    participant CLI
    participant Loan as 대출모듈
    participant Member as 회원모듈
    participant DB as 데이터베이스
    
    User->>CLI: 반납 요청
    CLI->>Loan: process_return()
    Loan->>DB: SELECT loan info
    DB-->>Loan: loan data
    
    Loan->>Loan: calculate_overdue_days()
    
    alt 연체 (overdue > 0)
        Loan->>DB: BEGIN TRANSACTION
        Loan->>DB: INSERT Returns
        Loan->>DB: UPDATE Loans
        Note over Loan: 페널티 계산<br/>연체일 × 2
        Loan->>Member: apply_penalty()
        Member->>DB: UPDATE Members<br/>(penalty_days)
        Loan->>DB: COMMIT
        Loan-->>CLI: 반납 완료(연체)
        CLI-->>User: "연체: X일<br/>대출제한: Y일"
    else 정상 반납
        Loan->>DB: BEGIN TRANSACTION
        Loan->>DB: INSERT Returns
        Loan->>DB: UPDATE Loans
        Loan->>DB: COMMIT
        Loan-->>CLI: 반납 완료
        CLI-->>User: "정상 반납"
    end
```

## 📁 5. 파일 구조

```
task03/
│
├── 📂 .vscode/              VS Code 설정
│   ├── settings.json        Copilot 지침 설정
│   ├── tasks.json           빌드 태스크
│   ├── launch.json          디버깅 설정
│   └── c_cpp_properties.json C/C++ 설정
│
├── 📂 include/              헤더 파일
│   ├── database.h           DB 연결 인터페이스
│   ├── book.h               도서 관리 인터페이스
│   ├── member.h             회원 관리 인터페이스
│   └── loan.h               대출/반납 인터페이스
│
├── 📂 src/                  소스 파일
│   ├── main.c               메인 프로그램
│   ├── database.c           DB 초기화 및 연결
│   ├── book.c               도서 CRUD 구현
│   ├── member.c             회원 관리 구현
│   └── loan.c               대출/반납 로직
│
├── 📂 database/             데이터베이스
│   ├── library.db           SQLite DB 파일
│   └── init_db.sql          테이블 생성 스크립트
│
├── 📂 tests/                테스트
│   └── test_main.cpp        Google Test 테스트
│
├── 📂 bin/                  실행 파일
│   └── library.exe          컴파일된 실행 파일
│
├── 📂 docs/                 문서
│   ├── spec.md              프로젝트 명세
│   ├── C-style.md           코딩 스타일 가이드
│   ├── database-style.md    DB 설계 가이드
│   └── test-style.md        테스트 가이드
│
└── 📂 diagrams/             다이어그램
    └── architecture.md      아키텍처 다이어그램
```

## 🔗 6. 모듈 의존성

```mermaid
graph LR
    MAIN[main.c]
    
    subgraph "Core Modules"
        BOOK[book.c]
        MEMBER[member.c]
        LOAN[loan.c]
    end
    
    DB[database.c]
    SQLITE[libsqlite3]
    
    MAIN --> BOOK
    MAIN --> MEMBER
    MAIN --> LOAN
    MAIN --> DB
    
    BOOK --> DB
    MEMBER --> DB
    LOAN --> DB
    LOAN -.-> MEMBER
    LOAN -.-> BOOK
    
    DB --> SQLITE
    
    style MAIN fill:#4fc3f7
    style BOOK fill:#ffb74d
    style MEMBER fill:#ffb74d
    style LOAN fill:#ffb74d
    style DB fill:#ba68c8
    style SQLITE fill:#81c784
```

## 📊 7. 데이터 흐름

```mermaid
flowchart TD
    START([시작])
    INIT[데이터베이스 초기화]
    MENU{메뉴 선택}
    
    EXIT([종료])
    
    subgraph "도서 관리"
        B1[도서 등록]
        B2[도서 검색]
        B3[도서 수정]
        B4[도서 삭제]
    end
    
    subgraph "회원 관리"
        M1[회원 등록]
        M2[회원 수정]
        M3[연체 확인]
    end
    
    subgraph "대출/반납"
        L1[대출 처리]
        L2[반납 처리]
        L3[보고서]
    end
    
    DB[(Database)]
    
    START --> INIT
    INIT --> MENU
    
    MENU -->|1| B1
    MENU -->|2| B2
    MENU -->|3| B3
    MENU -->|4| B4
    
    MENU -->|5| M1
    MENU -->|6| M2
    MENU -->|7| M3
    
    MENU -->|8| L1
    MENU -->|9| L2
    MENU -->|10| L3
    
    MENU -->|0| EXIT
    
    B1 --> DB
    B2 --> DB
    B3 --> DB
    B4 --> DB
    M1 --> DB
    M2 --> DB
    M3 --> DB
    L1 --> DB
    L2 --> DB
    L3 --> DB
    
    DB --> MENU
    
    style START fill:#4caf50
    style EXIT fill:#f44336
    style DB fill:#2196f3
```

## ⚠️ 8. 연체 관리 로직

```mermaid
flowchart TD
    START([반납 요청])
    GET[대출 정보 조회]
    CALC[연체일 계산]
    CHECK{연체?}
    
    NORMAL[정상 반납]
    OVERDUE[연체 반납]
    
    PENALTY["페널티 계산<br/>연체일수 × 2 = 대출제한일"]
    UPDATE_M[회원 페널티 업데이트]
    UPDATE_L[대출 상태 업데이트]
    INSERT_R[반납 기록 저장]
    
    END([완료])
    
    START --> GET
    GET --> CALC
    CALC --> CHECK
    
    CHECK -->|연체일 = 0| NORMAL
    CHECK -->|연체일 > 0| OVERDUE
    
    NORMAL --> UPDATE_L
    
    OVERDUE --> PENALTY
    PENALTY --> UPDATE_M
    UPDATE_M --> UPDATE_L
    
    UPDATE_L --> INSERT_R
    INSERT_R --> END
    
    style START fill:#4caf50
    style END fill:#4caf50
    style CHECK fill:#ff9800
    style PENALTY fill:#e91e63
    style UPDATE_M fill:#e91e63
```

## 🔧 9. 함수 호출 관계

```mermaid
graph TD
    MAIN[main]
    
    subgraph "database.c"
        INIT_DB[init_database]
        CLOSE_DB[close_database]
        GET_CONN[get_db_connection]
    end
    
    subgraph "book.c"
        ADD_B[add_book]
        SEARCH_B[search_book]
        UPDATE_B[update_book]
        DELETE_B[delete_book]
    end
    
    subgraph "member.c"
        ADD_M[add_member]
        UPDATE_M[update_member]
        CHECK_P[check_penalty_status]
        UPDATE_P[update_penalty]
    end
    
    subgraph "loan.c"
        PROC_L[process_loan]
        PROC_R[process_return]
        CALC_O[calculate_overdue_days]
        APPLY_P[apply_penalty]
        REPORT_POP[generate_popular_books_report]
        REPORT_OVR[generate_overdue_report]
    end
    
    MAIN --> INIT_DB
    MAIN --> ADD_B
    MAIN --> ADD_M
    MAIN --> PROC_L
    MAIN --> PROC_R
    MAIN --> REPORT_POP
    MAIN --> REPORT_OVR
    MAIN --> CLOSE_DB
    
    ADD_B --> GET_CONN
    SEARCH_B --> GET_CONN
    UPDATE_B --> GET_CONN
    DELETE_B --> GET_CONN
    
    ADD_M --> GET_CONN
    UPDATE_M --> GET_CONN
    CHECK_P --> GET_CONN
    UPDATE_P --> GET_CONN
    
    PROC_L --> CHECK_P
    PROC_L --> GET_CONN
    
    PROC_R --> CALC_O
    PROC_R --> APPLY_P
    PROC_R --> GET_CONN
    
    APPLY_P --> UPDATE_P
    
    REPORT_POP --> GET_CONN
    REPORT_OVR --> GET_CONN
    
    style MAIN fill:#4fc3f7,stroke:#01579b,stroke-width:3px
    style INIT_DB fill:#ba68c8
    style CLOSE_DB fill:#ba68c8
    style GET_CONN fill:#ba68c8
    style PROC_L fill:#ffb74d
    style PROC_R fill:#ffb74d
    style APPLY_P fill:#e57373
```

## 🎯 10. 빌드 프로세스

```mermaid
flowchart LR
    subgraph "Source Files"
        MAIN_C[main.c]
        DB_C[database.c]
        BOOK_C[book.c]
        MEMBER_C[member.c]
        LOAN_C[loan.c]
    end
    
    subgraph "Header Files"
        DB_H[database.h]
        BOOK_H[book.h]
        MEMBER_H[member.h]
        LOAN_H[loan.h]
    end
    
    subgraph "Compiler"
        GCC[GCC Compiler]
    end
    
    subgraph "Libraries"
        SQLITE[libsqlite3]
    end
    
    subgraph "Output"
        EXE[library.exe]
    end
    
    MAIN_C --> GCC
    DB_C --> GCC
    BOOK_C --> GCC
    MEMBER_C --> GCC
    LOAN_C --> GCC
    
    DB_H -.-> GCC
    BOOK_H -.-> GCC
    MEMBER_H -.-> GCC
    LOAN_H -.-> GCC
    
    SQLITE --> GCC
    
    GCC --> EXE
    
    style GCC fill:#ff9800
    style EXE fill:#4caf50
    style SQLITE fill:#2196f3
```

---

## 📝 빌드 명령어

```bash
# 전체 프로젝트 빌드
gcc -o bin/library.exe \
    src/main.c \
    src/database.c \
    src/book.c \
    src/member.c \
    src/loan.c \
    -Iinclude \
    -lsqlite3 \
    -g -Wall

# 실행
./bin/library.exe
```

## 🧪 테스트 빌드

```bash
# Google Test 빌드
g++ -o bin/test_library.exe \
    tests/test_main.cpp \
    src/database.c \
    src/book.c \
    src/member.c \
    src/loan.c \
    -Iinclude \
    -lsqlite3 -lgtest -lgtest_main \
    -g

# 테스트 실행
./bin/test_library.exe
```
