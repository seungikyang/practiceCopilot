# GitHub Copilot Instructions for C/C++ Development

This document provides guidelines and instructions for GitHub Copilot when working with C and C++ code in this project.

## Code Style and Conventions

### General Guidelines
- Follow ISO C99 standard for C code
- Follow C++14 or later standards for C++ code
- Use clear, descriptive variable and function names
- Prefer snake_case for C functions and variables
- Prefer camelCase for C++ methods and variables (or follow existing project conventions)
- Keep functions small and focused on a single responsibility
- Add comments for complex logic, but prefer self-documenting code

### Naming Conventions

#### C Code
```c
// Functions: snake_case with verb prefixes
int is_valid_url(const char* url);
char* format_url(const char* url);
void process_data(struct data* input);

// Constants: UPPER_SNAKE_CASE
#define MAX_BUFFER_SIZE 1024
#define DEFAULT_TIMEOUT 30

// Struct names: snake_case
struct url_info {
    char* protocol;
    char* domain;
    int port;
};

// Type definitions: snake_case_t
typedef struct url_info url_info_t;
```

#### C++ Code
```cpp
// Classes: PascalCase
class UrlParser {
public:
    // Methods: camelCase
    bool isValid() const;
    std::string formatUrl(const std::string& url);
    
private:
    // Member variables: camelCase with m_ prefix or trailing underscore
    std::string m_protocol;
    int port_;
};

// Constants: kPascalCase or UPPER_SNAKE_CASE
const int kMaxRetries = 3;
constexpr size_t MAX_BUFFER_SIZE = 1024;
```

## Memory Management

### C Code Best Practices
```c
// Always check malloc/calloc return values
char* buffer = (char*)malloc(size);
if (buffer == NULL) {
    // Handle allocation failure
    return NULL;
}

// Always free allocated memory
free(buffer);
buffer = NULL;  // Prevent dangling pointers

// Use strdup carefully and document ownership
char* copy = strdup(original);
if (copy == NULL) {
    return NULL;
}
// Caller is responsible for freeing 'copy'
```

### C++ Code Best Practices
```cpp
// Prefer RAII and smart pointers
#include <memory>

std::unique_ptr<Resource> resource = std::make_unique<Resource>();
std::shared_ptr<Data> data = std::make_shared<Data>();

// Use vectors instead of manual array management
std::vector<int> numbers;
numbers.reserve(100);  // Pre-allocate if size is known

// Avoid raw pointers for ownership
// Use references for non-owning access
void processData(const Data& data);  // Good
void processData(Data* data);        // Avoid unless nullptr is valid
```

## Error Handling

### C Error Handling
```c
// Return error codes
int process_url(const char* url, char** result) {
    if (url == NULL || result == NULL) {
        return -1;  // Invalid parameter
    }
    
    *result = format_url(url);
    if (*result == NULL) {
        return -2;  // Memory allocation failed
    }
    
    return 0;  // Success
}

// Document error codes in function comments
/**
 * Process the given URL.
 * 
 * @param url Input URL to process (must not be NULL)
 * @param result Output parameter for result (must not be NULL)
 * @return 0 on success, -1 for invalid parameters, -2 for allocation failure
 */
```

### C++ Error Handling
```cpp
// Use exceptions for exceptional conditions
#include <stdexcept>

void processUrl(const std::string& url) {
    if (url.empty()) {
        throw std::invalid_argument("URL cannot be empty");
    }
    
    // Process URL
}

// Use std::optional for optional return values
#include <optional>

std::optional<UrlInfo> parseUrl(const std::string& url) {
    if (!isValid(url)) {
        return std::nullopt;
    }
    return UrlInfo{/* ... */};
}

// Use std::expected (C++23) or Result types for error handling
```

## Testing Guidelines

### Unit Testing with Google Test
```cpp
#include <gtest/gtest.h>

// Test fixture for related tests
class UrlToolsTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code
    }
    
    void TearDown() override {
        // Cleanup code
    }
};

// Test naming: TestFixture.TestName
TEST_F(UrlToolsTest, FormatUrl_AddsHttpsPrefix) {
    char* result = format_url("example.com");
    ASSERT_NE(nullptr, result);
    EXPECT_STREQ("https://example.com", result);
    free(result);
}

// Test edge cases
TEST_F(UrlToolsTest, FormatUrl_HandlesNullInput) {
    EXPECT_EQ(nullptr, format_url(nullptr));
}
```

### Test Coverage Requirements
- Write tests for all public functions
- Test normal cases, edge cases, and error cases
- Test NULL/nullptr inputs for pointer parameters
- Test empty strings and boundary conditions
- Test memory allocation failures when applicable
- Use descriptive test names that explain what is being tested

## Header Files

### C Header Guards
```c
#ifndef URL_TOOLS_H
#define URL_TOOLS_H

#ifdef __cplusplus
extern "C" {
#endif

// Function declarations
int is_valid_url(const char* url);
char* format_url(const char* url);

#ifdef __cplusplus
}
#endif

#endif /* URL_TOOLS_H */
```

### C++ Header Guards
```cpp
#ifndef URL_PARSER_HPP
#define URL_PARSER_HPP

// Or use #pragma once (if supported)
#pragma once

#include <string>
#include <memory>

class UrlParser {
    // Class definition
};

#endif /* URL_PARSER_HPP */
```

## Documentation Standards

### Function Documentation
```c
/**
 * @brief Validates if the given string is a valid URL.
 * 
 * Checks if the URL starts with http:// or https:// and contains
 * at least one dot in the domain portion.
 * 
 * @param url The URL string to validate. Can be NULL.
 * @return 1 if valid, 0 if invalid or NULL.
 * 
 * @example
 *   int result = is_valid_url("https://example.com");
 *   // result is 1
 * 
 * @note This function does not perform comprehensive URL validation.
 * @see format_url() for URL formatting
 */
int is_valid_url(const char* url);
```

### Class Documentation (C++)
```cpp
/**
 * @class UrlParser
 * @brief Parses and validates URLs.
 * 
 * This class provides methods to parse URLs into their component parts
 * and validate URL structure according to RFC 3986.
 * 
 * @example
 *   UrlParser parser;
 *   auto info = parser.parse("https://example.com:8080/path");
 *   if (info) {
 *       std::cout << info->domain << std::endl;
 *   }
 */
class UrlParser {
    // ...
};
```

## Performance Considerations

### C Optimization Tips
```c
// Minimize dynamic allocations
// Pre-allocate buffers when size is known
char buffer[MAX_BUFFER_SIZE];

// Use const for immutable parameters
void process_data(const char* input);

// Use restrict keyword for non-aliased pointers (C99)
void copy_buffer(char* restrict dest, const char* restrict src, size_t n);

// Inline small, frequently-called functions
static inline int min(int a, int b) {
    return a < b ? a : b;
}
```

### C++ Optimization Tips
```cpp
// Pass by const reference for large objects
void processData(const std::vector<int>& data);

// Use move semantics for ownership transfer
std::string formatUrl(std::string url) {
    // Modify url
    return url;  // RVO or move
}

// Use reserve() for vectors with known size
std::vector<int> numbers;
numbers.reserve(1000);

// Prefer emplace over push for containers
vec.emplace_back(arg1, arg2);  // Construct in-place
```

## Build System Integration

### CMake Best Practices
```cmake
# Set C/C++ standards
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable warnings
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Separate interface and implementation
add_library(mylib 
    src/file1.c
    src/file2.c
)

target_include_directories(mylib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
```

## Security Considerations

### Buffer Safety
```c
// Always check buffer sizes
if (strlen(input) >= buffer_size) {
    return -1;  // Input too large
}

// Use safe string functions
strncpy(dest, src, dest_size - 1);
dest[dest_size - 1] = '\0';  // Ensure null termination

// Or use snprintf
snprintf(buffer, sizeof(buffer), "%s", input);
```

### Input Validation
```c
// Always validate inputs
int process_url(const char* url, int url_count, char** results) {
    // Validate pointers
    if (url == NULL || results == NULL) {
        return -1;
    }
    
    // Validate ranges
    if (url_count <= 0 || url_count > MAX_URL_COUNT) {
        return -1;
    }
    
    // Validate string content
    if (strlen(url) == 0 || strlen(url) > MAX_URL_LENGTH) {
        return -1;
    }
    
    // Process...
}
```

## Common Patterns

### Resource Management (RAII in C++)
```cpp
class FileHandle {
public:
    explicit FileHandle(const std::string& filename) 
        : file_(fopen(filename.c_str(), "r")) {
        if (!file_) {
            throw std::runtime_error("Failed to open file");
        }
    }
    
    ~FileHandle() {
        if (file_) {
            fclose(file_);
        }
    }
    
    // Delete copy operations
    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;
    
    // Allow move operations
    FileHandle(FileHandle&& other) noexcept 
        : file_(other.file_) {
        other.file_ = nullptr;
    }
    
private:
    FILE* file_;
};
```

### Iterator Pattern (C++)
```cpp
// Use standard algorithms
#include <algorithm>
#include <iterator>

std::vector<int> numbers = {1, 2, 3, 4, 5};

// Find element
auto it = std::find(numbers.begin(), numbers.end(), 3);

// Transform elements
std::transform(numbers.begin(), numbers.end(), 
               numbers.begin(), [](int n) { return n * 2; });

// Range-based for loops (C++11)
for (const auto& num : numbers) {
    std::cout << num << std::endl;
}
```

## Project-Specific Guidelines

### URL Processing Module
- All URL manipulation functions should handle NULL inputs gracefully
- Functions that allocate memory must document caller's responsibility to free
- URL validation should check for protocol (http/https) and domain structure
- Format functions should add "https://" prefix if no protocol is present
- Shorten functions should preserve URL validity when possible

### Integration Between Modules
- The `url` module uses functions from `url_tools` module
- Always verify that `url_tools` functions work correctly in isolation before testing `url` module
- Integration tests should verify data flow between modules
- Error codes should propagate correctly through module boundaries

## Additional Resources

- [C Coding Standards](https://www.kernel.org/doc/html/latest/process/coding-style.html)
- [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
- [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)
- [Modern CMake](https://cliutils.gitlab.io/modern-cmake/)
- [Google Test Documentation](https://google.github.io/googletest/)

## Copilot-Specific Instructions

When suggesting code:
1. **Always check for NULL/nullptr** before dereferencing pointers
2. **Document memory ownership** clearly in comments
3. **Include error handling** for all external inputs
4. **Write corresponding tests** for new functions
5. **Follow existing code style** in the file being edited
6. **Use const correctness** - mark parameters as const when they won't be modified
7. **Avoid magic numbers** - use named constants or enums
8. **Consider thread safety** if the code might be used in multi-threaded contexts
9. **Validate assumptions** with assertions in debug builds
10. **Keep platform compatibility** in mind (Windows, Linux, macOS)

When generating tests:
1. Use descriptive test names that explain the scenario
2. Test both success and failure paths
3. Always clean up allocated resources in tests
4. Use ASSERT_* for prerequisites and EXPECT_* for actual checks
5. Group related tests using test fixtures
6. Test edge cases: NULL, empty, maximum values, minimum values
7. Include integration tests when multiple modules interact

When refactoring:
1. Preserve existing behavior unless explicitly asked to change it
2. Maintain backward compatibility of public APIs
3. Update documentation when changing function signatures
4. Add tests before refactoring to ensure behavior is preserved
5. Refactor incrementally with tests passing at each step
