#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/url_tools.h"

// Test counter
static int tests_passed = 0;
static int tests_failed = 0;

// Color codes for output
#define COLOR_GREEN "\033[0;32m"
#define COLOR_RED "\033[0;31m"
#define COLOR_RESET "\033[0m"

// Test assertion macros
#define ASSERT_TRUE(condition, test_name) \
    do { \
        if (condition) { \
            printf(COLOR_GREEN "[PASS]" COLOR_RESET " %s\n", test_name); \
            tests_passed++; \
        } else { \
            printf(COLOR_RED "[FAIL]" COLOR_RESET " %s\n", test_name); \
            tests_failed++; \
        } \
    } while(0)

#define ASSERT_EQUAL(expected, actual, test_name) \
    ASSERT_TRUE((expected) == (actual), test_name)

#define ASSERT_STR_EQUAL(expected, actual, test_name) \
    ASSERT_TRUE(strcmp((expected), (actual)) == 0, test_name)

#define ASSERT_NULL(ptr, test_name) \
    ASSERT_TRUE((ptr) == NULL, test_name)

#define ASSERT_NOT_NULL(ptr, test_name) \
    ASSERT_TRUE((ptr) != NULL, test_name)

// Test functions for is_valid_url
void test_is_valid_url_with_http() {
    int result = is_valid_url("http://example.com");
    ASSERT_EQUAL(1, result, "is_valid_url with http://");
}

void test_is_valid_url_with_https() {
    int result = is_valid_url("https://example.com");
    ASSERT_EQUAL(1, result, "is_valid_url with https://");
}

void test_is_valid_url_with_subdomain() {
    int result = is_valid_url("https://www.example.com");
    ASSERT_EQUAL(1, result, "is_valid_url with subdomain");
}

void test_is_valid_url_with_path() {
    int result = is_valid_url("https://example.com/path/to/page");
    ASSERT_EQUAL(1, result, "is_valid_url with path");
}

void test_is_valid_url_without_protocol() {
    int result = is_valid_url("example.com");
    ASSERT_EQUAL(0, result, "is_valid_url without protocol (should fail)");
}

void test_is_valid_url_null() {
    int result = is_valid_url(NULL);
    ASSERT_EQUAL(0, result, "is_valid_url with NULL");
}

void test_is_valid_url_empty_after_protocol() {
    int result = is_valid_url("http://");
    ASSERT_EQUAL(0, result, "is_valid_url with empty domain");
}

void test_is_valid_url_no_dot() {
    int result = is_valid_url("http://localhost");
    ASSERT_EQUAL(0, result, "is_valid_url without dot (should fail)");
}

void test_is_valid_url_with_port() {
    int result = is_valid_url("https://example.com:8080");
    ASSERT_EQUAL(1, result, "is_valid_url with port number");
}

void test_is_valid_url_with_query_params() {
    int result = is_valid_url("https://example.com?param=value");
    ASSERT_EQUAL(1, result, "is_valid_url with query parameters");
}

// Test functions for format_url
void test_format_url_without_protocol() {
    char* result = format_url("example.com");
    ASSERT_NOT_NULL(result, "format_url returns non-NULL for valid input");
    if (result != NULL) {
        ASSERT_STR_EQUAL("https://example.com", result, "format_url adds https://");
        free(result);
    }
}

void test_format_url_with_http() {
    char* result = format_url("http://example.com");
    ASSERT_NOT_NULL(result, "format_url with http returns non-NULL");
    if (result != NULL) {
        ASSERT_STR_EQUAL("http://example.com", result, "format_url keeps http://");
        free(result);
    }
}

void test_format_url_with_https() {
    char* result = format_url("https://example.com");
    ASSERT_NOT_NULL(result, "format_url with https returns non-NULL");
    if (result != NULL) {
        ASSERT_STR_EQUAL("https://example.com", result, "format_url keeps https://");
        free(result);
    }
}

void test_format_url_null() {
    char* result = format_url(NULL);
    ASSERT_NULL(result, "format_url with NULL returns NULL");
}

void test_format_url_with_path() {
    char* result = format_url("example.com/path");
    ASSERT_NOT_NULL(result, "format_url with path returns non-NULL");
    if (result != NULL) {
        ASSERT_STR_EQUAL("https://example.com/path", result, "format_url adds https:// to URL with path");
        free(result);
    }
}

// Test functions for shorten_url
void test_shorten_url_longer_than_limit() {
    char* result = shorten_url("https://example.com/very/long/path", 20);
    ASSERT_NOT_NULL(result, "shorten_url returns non-NULL");
    if (result != NULL) {
        ASSERT_STR_EQUAL("https://example.com/...", result, "shorten_url truncates and adds ...");
        free(result);
    }
}

void test_shorten_url_shorter_than_limit() {
    char* result = shorten_url("https://ex.com", 20);
    ASSERT_NOT_NULL(result, "shorten_url returns non-NULL for short URL");
    if (result != NULL) {
        ASSERT_STR_EQUAL("https://ex.com", result, "shorten_url keeps short URL unchanged");
        free(result);
    }
}

void test_shorten_url_equal_to_limit() {
    char* result = shorten_url("12345678901234567890", 20);
    ASSERT_NOT_NULL(result, "shorten_url returns non-NULL for URL equal to limit");
    if (result != NULL) {
        ASSERT_STR_EQUAL("12345678901234567890", result, "shorten_url keeps URL equal to limit");
        free(result);
    }
}

void test_shorten_url_null() {
    char* result = shorten_url(NULL, 20);
    ASSERT_NULL(result, "shorten_url with NULL returns NULL");
}

void test_shorten_url_zero_length() {
    char* result = shorten_url("https://example.com", 0);
    ASSERT_NOT_NULL(result, "shorten_url with zero length returns non-NULL");
    if (result != NULL) {
        ASSERT_STR_EQUAL("...", result, "shorten_url with zero length returns only ...");
        free(result);
    }
}

void test_shorten_url_very_long() {
    char* long_url = "https://example.com/very/very/very/very/very/very/very/long/path/to/resource";
    char* result = shorten_url(long_url, 10);
    ASSERT_NOT_NULL(result, "shorten_url handles very long URL");
    if (result != NULL) {
        ASSERT_STR_EQUAL("https://ex...", result, "shorten_url truncates very long URL correctly");
        free(result);
    }
}

// Integration tests
void test_integration_format_and_shorten() {
    char* formatted = format_url("example.com/long/path");
    ASSERT_NOT_NULL(formatted, "Integration: format_url succeeds");
    if (formatted != NULL) {
        char* shortened = shorten_url(formatted, 15);
        ASSERT_NOT_NULL(shortened, "Integration: shorten_url succeeds");
        if (shortened != NULL) {
            ASSERT_STR_EQUAL("https://example...", shortened, "Integration: format then shorten works correctly");
            free(shortened);
        }
        free(formatted);
    }
}

void test_integration_validate_and_format() {
    const char* url = "example.com";
    int is_valid = is_valid_url(url);
    ASSERT_EQUAL(0, is_valid, "Integration: URL without protocol is invalid");
    
    char* formatted = format_url(url);
    ASSERT_NOT_NULL(formatted, "Integration: format_url adds protocol");
    if (formatted != NULL) {
        is_valid = is_valid_url(formatted);
        ASSERT_EQUAL(1, is_valid, "Integration: formatted URL becomes valid");
        free(formatted);
    }
}

// Test runner
void run_all_tests() {
    printf("\n========================================\n");
    printf("Running URL Tools Unit Tests\n");
    printf("========================================\n\n");

    printf("--- Testing is_valid_url ---\n");
    test_is_valid_url_with_http();
    test_is_valid_url_with_https();
    test_is_valid_url_with_subdomain();
    test_is_valid_url_with_path();
    test_is_valid_url_without_protocol();
    test_is_valid_url_null();
    test_is_valid_url_empty_after_protocol();
    test_is_valid_url_no_dot();
    test_is_valid_url_with_port();
    test_is_valid_url_with_query_params();

    printf("\n--- Testing format_url ---\n");
    test_format_url_without_protocol();
    test_format_url_with_http();
    test_format_url_with_https();
    test_format_url_null();
    test_format_url_with_path();

    printf("\n--- Testing shorten_url ---\n");
    test_shorten_url_longer_than_limit();
    test_shorten_url_shorter_than_limit();
    test_shorten_url_equal_to_limit();
    test_shorten_url_null();
    test_shorten_url_zero_length();
    test_shorten_url_very_long();

    printf("\n--- Integration Tests ---\n");
    test_integration_format_and_shorten();
    test_integration_validate_and_format();

    printf("\n========================================\n");
    printf("Test Results\n");
    printf("========================================\n");
    printf(COLOR_GREEN "Passed: %d\n" COLOR_RESET, tests_passed);
    printf(COLOR_RED "Failed: %d\n" COLOR_RESET, tests_failed);
    printf("Total:  %d\n", tests_passed + tests_failed);
    printf("========================================\n\n");
}

int main() {
    run_all_tests();
    return (tests_failed == 0) ? 0 : 1;
}
