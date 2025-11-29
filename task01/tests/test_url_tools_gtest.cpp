#include <gtest/gtest.h>
#include <cstring>

extern "C" {
    #include "url_tools.h"
}

// Test fixture for URL Tools
class URLToolsTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }

    void TearDown() override {
        // Cleanup code if needed
    }
};

// ========================================
// Tests for is_valid_url
// ========================================

TEST_F(URLToolsTest, IsValidUrl_WithHttp) {
    EXPECT_EQ(1, is_valid_url("http://example.com"));
}

TEST_F(URLToolsTest, IsValidUrl_WithHttps) {
    EXPECT_EQ(1, is_valid_url("https://example.com"));
}

TEST_F(URLToolsTest, IsValidUrl_WithSubdomain) {
    EXPECT_EQ(1, is_valid_url("https://www.example.com"));
}

TEST_F(URLToolsTest, IsValidUrl_WithPath) {
    EXPECT_EQ(1, is_valid_url("https://example.com/path/to/page"));
}

TEST_F(URLToolsTest, IsValidUrl_WithPort) {
    EXPECT_EQ(1, is_valid_url("https://example.com:8080"));
}

TEST_F(URLToolsTest, IsValidUrl_WithQueryParams) {
    EXPECT_EQ(1, is_valid_url("https://example.com?param=value&another=test"));
}

TEST_F(URLToolsTest, IsValidUrl_WithFragment) {
    EXPECT_EQ(1, is_valid_url("https://example.com/page#section"));
}

TEST_F(URLToolsTest, IsValidUrl_ComplexUrl) {
    EXPECT_EQ(1, is_valid_url("https://subdomain.example.com:8080/path?query=value#fragment"));
}

TEST_F(URLToolsTest, IsValidUrl_WithoutProtocol_ShouldFail) {
    EXPECT_EQ(0, is_valid_url("example.com"));
}

TEST_F(URLToolsTest, IsValidUrl_NullInput) {
    EXPECT_EQ(0, is_valid_url(NULL));
}

TEST_F(URLToolsTest, IsValidUrl_EmptyAfterProtocol) {
    EXPECT_EQ(0, is_valid_url("http://"));
    EXPECT_EQ(0, is_valid_url("https://"));
}

TEST_F(URLToolsTest, IsValidUrl_NoDot_ShouldFail) {
    EXPECT_EQ(0, is_valid_url("http://localhost"));
}

TEST_F(URLToolsTest, IsValidUrl_InvalidProtocol_ShouldFail) {
    EXPECT_EQ(0, is_valid_url("ftp://example.com"));
    EXPECT_EQ(0, is_valid_url("file://example.com"));
}

TEST_F(URLToolsTest, IsValidUrl_EmptyString) {
    EXPECT_EQ(0, is_valid_url(""));
}

// ========================================
// Tests for format_url
// ========================================

TEST_F(URLToolsTest, FormatUrl_WithoutProtocol) {
    char* result = format_url("example.com");
    ASSERT_NE(nullptr, result);
    EXPECT_STREQ("https://example.com", result);
    free(result);
}

TEST_F(URLToolsTest, FormatUrl_WithHttp_ShouldKeep) {
    char* result = format_url("http://example.com");
    ASSERT_NE(nullptr, result);
    EXPECT_STREQ("http://example.com", result);
    free(result);
}

TEST_F(URLToolsTest, FormatUrl_WithHttps_ShouldKeep) {
    char* result = format_url("https://example.com");
    ASSERT_NE(nullptr, result);
    EXPECT_STREQ("https://example.com", result);
    free(result);
}

TEST_F(URLToolsTest, FormatUrl_WithPath) {
    char* result = format_url("example.com/path/to/page");
    ASSERT_NE(nullptr, result);
    EXPECT_STREQ("https://example.com/path/to/page", result);
    free(result);
}

TEST_F(URLToolsTest, FormatUrl_WithQueryParams) {
    char* result = format_url("example.com?query=value");
    ASSERT_NE(nullptr, result);
    EXPECT_STREQ("https://example.com?query=value", result);
    free(result);
}

TEST_F(URLToolsTest, FormatUrl_WithSubdomain) {
    char* result = format_url("www.example.com");
    ASSERT_NE(nullptr, result);
    EXPECT_STREQ("https://www.example.com", result);
    free(result);
}

TEST_F(URLToolsTest, FormatUrl_NullInput) {
    char* result = format_url(NULL);
    EXPECT_EQ(nullptr, result);
}

TEST_F(URLToolsTest, FormatUrl_EmptyString) {
    char* result = format_url("");
    ASSERT_NE(nullptr, result);
    EXPECT_STREQ("https://", result);
    free(result);
}

TEST_F(URLToolsTest, FormatUrl_ComplexUrl) {
    char* result = format_url("subdomain.example.com:8080/path?query=value#fragment");
    ASSERT_NE(nullptr, result);
    EXPECT_STREQ("https://subdomain.example.com:8080/path?query=value#fragment", result);
    free(result);
}

// ========================================
// Tests for shorten_url
// ========================================

TEST_F(URLToolsTest, ShortenUrl_LongerThanLimit) {
    char* result = shorten_url("https://example.com/very/long/path", 20);
    ASSERT_NE(nullptr, result);
    EXPECT_STREQ("https://example.com/...", result);
    EXPECT_EQ(23, strlen(result)); // 20 chars + "..."
    free(result);
}

TEST_F(URLToolsTest, ShortenUrl_ShorterThanLimit) {
    char* result = shorten_url("https://ex.com", 20);
    ASSERT_NE(nullptr, result);
    EXPECT_STREQ("https://ex.com", result);
    free(result);
}

TEST_F(URLToolsTest, ShortenUrl_EqualToLimit) {
    char* result = shorten_url("12345678901234567890", 20);
    ASSERT_NE(nullptr, result);
    EXPECT_STREQ("12345678901234567890", result);
    free(result);
}

TEST_F(URLToolsTest, ShortenUrl_VeryLongUrl) {
    const char* long_url = "https://example.com/very/very/very/very/very/very/very/long/path/to/resource";
    char* result = shorten_url(long_url, 10);
    ASSERT_NE(nullptr, result);
    EXPECT_STREQ("https://ex...", result);
    EXPECT_EQ(13, strlen(result)); // 10 chars + "..."
    free(result);
}

TEST_F(URLToolsTest, ShortenUrl_ZeroLength) {
    char* result = shorten_url("https://example.com", 0);
    ASSERT_NE(nullptr, result);
    EXPECT_STREQ("...", result);
    free(result);
}

TEST_F(URLToolsTest, ShortenUrl_SmallLength) {
    char* result = shorten_url("https://example.com", 5);
    ASSERT_NE(nullptr, result);
    EXPECT_STREQ("https...", result);
    free(result);
}

TEST_F(URLToolsTest, ShortenUrl_NullInput) {
    char* result = shorten_url(NULL, 20);
    EXPECT_EQ(nullptr, result);
}

TEST_F(URLToolsTest, ShortenUrl_EmptyString) {
    char* result = shorten_url("", 10);
    ASSERT_NE(nullptr, result);
    EXPECT_STREQ("", result);
    free(result);
}

TEST_F(URLToolsTest, ShortenUrl_ExactlyOneCharOverLimit) {
    char* result = shorten_url("123456789012345678901", 20); // 21 chars
    ASSERT_NE(nullptr, result);
    EXPECT_STREQ("12345678901234567890...", result);
    free(result);
}

// ========================================
// Integration Tests
// ========================================

TEST_F(URLToolsTest, Integration_FormatThenShorten) {
    char* formatted = format_url("example.com/long/path/to/resource");
    ASSERT_NE(nullptr, formatted);
    
    char* shortened = shorten_url(formatted, 15);
    ASSERT_NE(nullptr, shortened);
    EXPECT_STREQ("https://example...", shortened);
    
    free(shortened);
    free(formatted);
}

TEST_F(URLToolsTest, Integration_ValidateAndFormat) {
    const char* url = "example.com";
    
    // URL without protocol is invalid
    EXPECT_EQ(0, is_valid_url(url));
    
    // Format adds protocol
    char* formatted = format_url(url);
    ASSERT_NE(nullptr, formatted);
    
    // Formatted URL becomes valid
    EXPECT_EQ(1, is_valid_url(formatted));
    
    free(formatted);
}

TEST_F(URLToolsTest, Integration_FormatValidateAndShorten) {
    const char* url = "subdomain.example.com/very/long/path/to/resource";
    
    // Format the URL
    char* formatted = format_url(url);
    ASSERT_NE(nullptr, formatted);
    EXPECT_STREQ("https://subdomain.example.com/very/long/path/to/resource", formatted);
    
    // Validate the formatted URL
    EXPECT_EQ(1, is_valid_url(formatted));
    
    // Shorten the URL
    char* shortened = shorten_url(formatted, 25);
    ASSERT_NE(nullptr, shortened);
    EXPECT_STREQ("https://subdomain.example...", shortened);
    
    free(shortened);
    free(formatted);
}

TEST_F(URLToolsTest, Integration_MultipleOperations) {
    // Test multiple operations in sequence
    char* url1 = format_url("example1.com");
    char* url2 = format_url("example2.com");
    char* url3 = format_url("example3.com");
    
    ASSERT_NE(nullptr, url1);
    ASSERT_NE(nullptr, url2);
    ASSERT_NE(nullptr, url3);
    
    EXPECT_EQ(1, is_valid_url(url1));
    EXPECT_EQ(1, is_valid_url(url2));
    EXPECT_EQ(1, is_valid_url(url3));
    
    char* short1 = shorten_url(url1, 10);
    char* short2 = shorten_url(url2, 10);
    char* short3 = shorten_url(url3, 10);
    
    ASSERT_NE(nullptr, short1);
    ASSERT_NE(nullptr, short2);
    ASSERT_NE(nullptr, short3);
    
    free(short1);
    free(short2);
    free(short3);
    free(url1);
    free(url2);
    free(url3);
}

// ========================================
// Edge Case Tests
// ========================================

TEST_F(URLToolsTest, EdgeCase_MemoryLeakTest) {
    // Test that repeated allocations don't cause issues
    for (int i = 0; i < 100; i++) {
        char* result = format_url("example.com");
        ASSERT_NE(nullptr, result);
        free(result);
    }
}

TEST_F(URLToolsTest, EdgeCase_VeryShortUrl) {
    EXPECT_EQ(1, is_valid_url("http://a.b"));
    
    char* formatted = format_url("a.b");
    ASSERT_NE(nullptr, formatted);
    EXPECT_STREQ("https://a.b", formatted);
    free(formatted);
}

TEST_F(URLToolsTest, EdgeCase_SpecialCharactersInUrl) {
    const char* special_url = "https://example.com/path?query=hello%20world&foo=bar#section";
    EXPECT_EQ(1, is_valid_url(special_url));
    
    char* shortened = shorten_url(special_url, 30);
    ASSERT_NE(nullptr, shortened);
    free(shortened);
}

// Test summary statistics
TEST_F(URLToolsTest, TestSuiteSummary) {
    // This test always passes, just to verify test suite runs
    SUCCEED() << "URL Tools test suite completed successfully";
}
