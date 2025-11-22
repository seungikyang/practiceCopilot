# Library Management System - CMake Build

이 프로젝트는 CMake를 사용하여 빌드할 수 있습니다.

## 필수 요구사항

- CMake 3.15 이상
- C 컴파일러 (GCC, Clang, MSVC)
- SQLite3 라이브러리

## CMake 설치

### Windows
```powershell
# winget 사용
winget install Kitware.CMake

# 또는 Chocolatey 사용
choco install cmake

# 또는 수동 다운로드
# https://cmake.org/download/
```

### Linux (Ubuntu/Debian)
```bash
sudo apt-get install cmake
```

### macOS
```bash
brew install cmake
```

## 빌드 방법

### Windows (PowerShell)

```powershell
# 프로젝트 구성
.\cmake-build.ps1 configure

# 빌드
.\cmake-build.ps1 build

# 테스트 실행
.\cmake-build.ps1 test

# 클린 빌드
.\cmake-build.ps1 rebuild

# Release 빌드
.\cmake-build.ps1 build -BuildType Release
```

### Linux/macOS

```bash
# 프로젝트 구성
mkdir build && cd build
cmake ..

# 빌드
cmake --build .

# 테스트 실행
cmake --build . --target check

# 또는 CTest 직접 사용
ctest --output-on-failure
```

## 빌드 타입

- **Debug**: 디버그 정보 포함 (기본값)
- **Release**: 최적화된 릴리스 빌드
- **RelWithDebInfo**: 디버그 정보가 포함된 릴리스 빌드
- **MinSizeRel**: 크기 최적화 빌드

## 출력 파일

빌드 후 생성되는 파일:

```
build/
├── bin/
│   ├── library.exe          # 메인 실행 파일
│   └── tests/
│       └── test_book.exe    # 테스트 실행 파일
└── lib/
    └── liblibrary_core.a    # 정적 라이브러리
```

## 테스트 실행

```powershell
# PowerShell 스크립트 사용
.\cmake-build.ps1 test

# 또는 직접 실행
.\build\bin\tests\test_book.exe

# CTest 사용 (build 디렉토리 내에서)
cd build
ctest --output-on-failure
ctest -V  # Verbose 출력
```

## CMake 옵션

```powershell
# Ninja 빌드 시스템 사용
.\cmake-build.ps1 configure -Generator Ninja

# Visual Studio 사용
.\cmake-build.ps1 configure -Generator "Visual Studio 17 2022"
```

## 프로젝트 구조

```
project/
├── CMakeLists.txt           # 메인 CMake 설정
├── cmake-build.ps1          # Windows 빌드 스크립트
├── include/                 # 헤더 파일
├── src/                     # 소스 파일
├── tests/                   # 테스트 파일
│   ├── CMakeLists.txt      # 테스트 CMake 설정
│   └── test_book.c         # Book 모듈 테스트
└── build/                   # 빌드 출력 (생성됨)
```

## 주요 CMake 타겟

- **library**: 메인 실행 파일
- **library_core**: 공용 라이브러리
- **test_book**: Book 모듈 테스트
- **check**: 모든 테스트 실행
- **all**: 모든 타겟 빌드

## 문제 해결

### SQLite3를 찾을 수 없는 경우

```powershell
# MSYS2 환경
pacman -S mingw-w64-ucrt-x86_64-sqlite3

# SQLite3 경로 수동 지정
.\cmake-build.ps1 configure -DSQLITE3_INCLUDE_DIR="C:/path/to/sqlite3/include" -DSQLITE3_LIBRARY="C:/path/to/sqlite3/lib/libsqlite3.a"
```

### CMake가 컴파일러를 찾을 수 없는 경우

```powershell
# GCC 경로 확인
where.exe gcc

# 컴파일러 수동 지정
$env:CC="C:/msys64/ucrt64/bin/gcc.exe"
.\cmake-build.ps1 configure
```

## 기존 Makefile과의 비교

| 기능 | Makefile | CMake |
|------|----------|-------|
| 크로스 플랫폼 | 제한적 | 우수 |
| IDE 통합 | 제한적 | 우수 |
| 의존성 관리 | 수동 | 자동 |
| 테스트 프레임워크 | 수동 | CTest 통합 |
| 빌드 타입 관리 | 수동 | 자동 |

## 참고 자료

- [CMake 공식 문서](https://cmake.org/documentation/)
- [CTest 문서](https://cmake.org/cmake/help/latest/manual/ctest.1.html)
- [Modern CMake](https://cliutils.gitlab.io/modern-cmake/)
