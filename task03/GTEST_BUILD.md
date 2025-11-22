# Google Test 기반 빌드 가이드

## 개요
이 문서는 Google Test를 사용한 Book 모듈 단위 테스트 빌드 및 실행 방법을 설명합니다.

## 사전 요구사항

### 1. CMake 설치
```powershell
winget install Kitware.CMake
```

설치 후 새 PowerShell 터미널을 열어주세요.

### 2. 필수 도구
- **MinGW-w64 GCC**: C/C++ 컴파일러
- **SQLite3**: 데이터베이스 라이브러리
- **Git**: Google Test 다운로드에 필요

## 빌드 방법

### 방법 1: cmake-build.ps1 스크립트 사용 (권장)

#### 초기 구성
```powershell
.\cmake-build.ps1 configure
```

#### 빌드
```powershell
.\cmake-build.ps1 build
```

#### 테스트 실행
```powershell
.\cmake-build.ps1 test
```

#### 전체 과정 (구성 + 빌드 + 테스트)
```powershell
.\cmake-build.ps1 rebuild
```

### 방법 2: 수동 CMake 명령

#### 1. 빌드 디렉토리 생성 및 구성
```powershell
mkdir build -Force
cd build
cmake .. -G "MinGW Makefiles"
```

#### 2. 빌드
```powershell
cmake --build .
```

#### 3. 테스트 실행
```powershell
# CTest 사용
ctest --output-on-failure

# 또는 직접 실행
.\bin\tests\test_book_gtest.exe
```

## Google Test 특징

### 자동 다운로드
CMake가 자동으로 Google Test v1.14.0을 다운로드하고 빌드합니다:
```cmake
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0
)
```

### 테스트 구조
- **Test Fixture**: `BookTest` 클래스로 테스트 환경 자동 관리
- **Setup/TearDown**: 각 테스트 전후로 in-memory DB 생성/삭제
- **Helper Functions**: `count_books()`, `book_exists()` 등 유틸리티 제공

### 테스트 케이스
총 **27개** 테스트:
1. **add_book() 테스트 (4개)**
   - AddBookSuccess
   - AddBookNullTitle
   - AddBookNullDatabase
   - AddBookDuplicateISBN

2. **search_books() 테스트 (4개)**
   - SearchBooksSuccess
   - SearchBooksNoResults
   - SearchBooksNullKeyword
   - SearchBooksNullBooksPointer

3. **get_book_by_id() 테스트 (3개)**
   - GetBookByIdSuccess
   - GetBookByIdNotFound
   - GetBookByIdNullBook

4. **update_book() 테스트 (4개)**
   - UpdateBookSuccess
   - UpdateBookNotFound
   - UpdateBookNullBook
   - UpdateBookDuplicateISBN

5. **delete_book() 테스트 (2개)**
   - DeleteBookSuccess
   - DeleteBookNotFound

6. **Book availability 테스트 (5개)**
   - CheckBookAvailabilityAvailable
   - CheckBookAvailabilityNotAvailable
   - CheckBookAvailabilityNotFound
   - UpdateBookAvailabilityIncrease
   - UpdateBookAvailabilityDecrease

7. **검색 테스트 (5개)**
   - SearchBooksByGenreSuccess
   - SearchBooksByGenreNoResults
   - SearchBooksByAuthorSuccess
   - SearchBooksByAuthorNoResults
   - SearchBooksByAuthorNullAuthor

## 출력 예시

### 성공적인 테스트 실행
```
[==========] Running 27 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 27 tests from BookTest
[ RUN      ] BookTest.AddBookSuccess
[       OK ] BookTest.AddBookSuccess (5 ms)
[ RUN      ] BookTest.AddBookNullTitle
[       OK ] BookTest.AddBookNullTitle (1 ms)
...
[----------] 27 tests from BookTest (150 ms total)

[----------] Global test environment tear-down
[==========] 27 tests from 1 test suite ran. (152 ms total)
[  PASSED  ] 27 tests.
```

### CTest 출력
```
Test project C:/___myProject/projects/task03/build
    Start 1: BookModuleTests
1/2 Test #1: BookModuleTests ..................   Passed    0.25 sec
    Start 2: BookModuleGoogleTests
2/2 Test #2: BookModuleGoogleTests ............   Passed    0.15 sec

100% tests passed, 0 tests failed out of 2
```

## Google Test vs Custom Test 비교

| 항목 | Google Test | Custom Test (test_book.c) |
|------|-------------|---------------------------|
| **프레임워크** | Google Test 1.14.0 | 직접 구현한 매크로 |
| **언어** | C++ | C |
| **설치** | CMake 자동 다운로드 | 불필요 |
| **테스트 케이스** | 27개 | 27개 |
| **출력 형식** | 상세한 Google Test 포맷 | 간단한 PASS/FAIL |
| **필터링** | `--gtest_filter` 지원 | 미지원 |
| **XML 리포트** | 지원 (`--gtest_output=xml`) | 미지원 |
| **픽스처** | Test Fixture 클래스 | 수동 setup/cleanup |
| **성숙도** | 업계 표준 | 프로젝트용 |

## 고급 사용법

### 특정 테스트만 실행
```powershell
.\bin\tests\test_book_gtest.exe --gtest_filter="BookTest.AddBook*"
```

### XML 리포트 생성
```powershell
.\bin\tests\test_book_gtest.exe --gtest_output=xml:test_results.xml
```

### 상세 출력
```powershell
.\bin\tests\test_book_gtest.exe --gtest_verbose
```

### 테스트 목록 확인
```powershell
.\bin\tests\test_book_gtest.exe --gtest_list_tests
```

## 문제 해결

### CMake 찾을 수 없음
```
Error: CMake is not installed or not in PATH
```
**해결**: PowerShell을 재시작하거나 수동으로 PATH 추가
```powershell
$env:Path += ";C:\Program Files\CMake\bin"
```

### Google Test 다운로드 실패
**해결**: 인터넷 연결 확인 또는 프록시 설정
```powershell
# 프록시 설정 (필요시)
$env:HTTP_PROXY = "http://proxy.example.com:8080"
$env:HTTPS_PROXY = "http://proxy.example.com:8080"
```

### 컴파일 오류
**해결**: MinGW 경로 확인
```powershell
g++ --version
cmake --version
```

## 참고 자료
- [Google Test 공식 문서](https://google.github.io/googletest/)
- [CMake FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html)
- [CTest 문서](https://cmake.org/cmake/help/latest/manual/ctest.1.html)

## 다음 단계
- [ ] Member 모듈 Google Test 추가
- [ ] Loan 모듈 Google Test 추가
- [ ] CI/CD 통합 (GitHub Actions)
- [ ] Code Coverage 설정 (gcov/lcov)
