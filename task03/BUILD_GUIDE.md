# CMake 없이 빌드하기

CMake가 설치되어 있지 않은 경우, 기존 Makefile을 사용하여 빌드할 수 있습니다.

## CMake 설치 (선택사항)

### Windows에서 CMake 설치

```powershell
# winget 사용 (Windows 10/11)
winget install Kitware.CMake

# 설치 후 새 PowerShell 세션 시작
```

## 현재 사용 가능한 빌드 방법

### 1. Makefile 사용 (추천 - CMake 불필요)

```powershell
# 빌드
.\build.ps1

# 테스트 실행
.\build.ps1 test

# 실행
.\build.ps1 run
```

### 2. CMake 사용 (CMake 설치 후)

```powershell
# CMake 설치
winget install Kitware.CMake

# 새 PowerShell 세션 시작 후
.\cmake-build.ps1 configure
.\cmake-build.ps1 build
.\cmake-build.ps1 test
```

## 빌드 시스템 비교

| 특징 | Makefile | CMake |
|------|----------|-------|
| 설치 필요 | MinGW Make | CMake + 빌드 시스템 |
| 구성 | 간단 | 복잡하지만 강력 |
| 크로스 플랫폼 | 제한적 | 우수 |
| IDE 통합 | 제한적 | 우수 (VS Code, CLion 등) |
| 현재 상태 | ✅ 작동 중 | ⚠️ CMake 설치 필요 |

## 권장 사항

현재 환경에서는 **Makefile 방식**을 사용하는 것을 권장합니다:

```powershell
# 전체 빌드 및 테스트
.\build.ps1 rebuild
.\build.ps1 test
```

CMake는 더 고급 기능이 필요하거나 다른 플랫폼으로 이식할 때 유용합니다.

## CMake 설치 후 사용법

CMake를 설치한 후에는:

```powershell
# 1. 프로젝트 구성
.\cmake-build.ps1 configure

# 2. 빌드
.\cmake-build.ps1 build

# 3. 테스트
.\cmake-build.ps1 test

# 4. 실행
.\build\bin\library.exe
```

## 테스트 실행 확인

두 빌드 시스템 모두 동일한 테스트를 실행합니다:

```powershell
# Makefile 방식
.\build.ps1 test

# CMake 방식 (설치 후)
.\cmake-build.ps1 test
```

둘 다 27개의 Book 모듈 단위 테스트를 실행합니다.
