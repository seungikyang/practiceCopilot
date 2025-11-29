#include <gtest/gtest.h>
#include <cstring>

extern "C" {
    #include "url.h"
    #include "url_tools.h"
}

// Test fixture for URL management
class URLManagementTest : public ::testing::Test {
protected:
    char** urls;
    char** results;
    int urlCount;

    void SetUp() override {
        urls = nullptr;
        results = nullptr;
        urlCount = 0;
    }

    void TearDown() override {
        // Clean up results if allocated
        if (results != nullptr) {
            for (int i = 0; i < urlCount; i++) {
                if (results[i] != nullptr) {
                    free(results[i]);
                }
            }
            delete[] results;
        }
        
        // Clean up urls if dynamically allocated
        if (urls != nullptr) {
            delete[] urls;
        }
    }

    // Helper function to allocate result array
    void allocateResults(int count) {
        urlCount = count;
        results = new char*[count];
        for (int i = 0; i < count; i++) {
            results[i] = nullptr;
        }
    }
};

// ========================================
// Tests for manageUrls with NULL inputs
// ========================================

TEST_F(URLManagementTest, ManageUrls_NullUrlsArray) {
    allocateResults(1);
    int result = manageUrls(nullptr, 1, "checkValid", results);
    EXPECT_EQ(-1, result);
}

TEST_F(URLManagementTest, ManageUrls_NullAction) {
    const char* url = "http://example.com";
    urls = new char*[1];
    urls[0] = const_cast<char*>(url);
    allocateResults(1);
    
    int result = manageUrls(urls, 1, nullptr, results);
    EXPECT_EQ(-1, result);
}

TEST_F(URLManagementTest, ManageUrls_NullResults) {
    const char* url = "http://example.com";
    urls = new char*[1];
    urls[0] = const_cast<char*>(url);
    
    int result = manageUrls(urls, 1, "checkValid", nullptr);
    EXPECT_EQ(-1, result);
}

TEST_F(URLManagementTest, ManageUrls_ZeroUrlCount) {
    urls = new char*[0];
    allocateResults(0);
    
    int result = manageUrls(urls, 0, "checkValid", results);
    EXPECT_EQ(-1, result);
}

TEST_F(URLManagementTest, ManageUrls_NegativeUrlCount) {
    urls = new char*[1];
    allocateResults(1);
    
    int result = manageUrls(urls, -1, "checkValid", results);
    EXPECT_EQ(-1, result);
}

TEST_F(URLManagementTest, ManageUrls_InvalidAction) {
    const char* url = "http://example.com";
    urls = new char*[1];
    urls[0] = const_cast<char*>(url);
    allocateResults(1);
    
    int result = manageUrls(urls, 1, "invalidAction", results);
    EXPECT_EQ(-1, result);
}

// ========================================
// Tests for checkValid action
// ========================================

TEST_F(URLManagementTest, ManageUrls_CheckValid_SingleValidUrl) {
    const char* url = "https://example.com";
    urls = new char*[1];
    urls[0] = const_cast<char*>(url);
    allocateResults(1);
    
    int result = manageUrls(urls, 1, "checkValid", results);
    EXPECT_EQ(0, result);
    ASSERT_NE(nullptr, results[0]);
    EXPECT_STREQ("1", results[0]);
}

TEST_F(URLManagementTest, ManageUrls_CheckValid_SingleInvalidUrl) {
    const char* url = "example.com";
    urls = new char*[1];
    urls[0] = const_cast<char*>(url);
    allocateResults(1);
    
    int result = manageUrls(urls, 1, "checkValid", results);
    EXPECT_EQ(0, result);
    ASSERT_NE(nullptr, results[0]);
    EXPECT_STREQ("0", results[0]);
}

TEST_F(URLManagementTest, ManageUrls_CheckValid_MultipleUrls) {
    urls = new char*[4];
    urls[0] = const_cast<char*>("https://example.com");
    urls[1] = const_cast<char*>("http://test.org");
    urls[2] = const_cast<char*>("invalid-url");
    urls[3] = const_cast<char*>("ftp://ftp.example.com");
    allocateResults(4);
    
    int result = manageUrls(urls, 4, "checkValid", results);
    EXPECT_EQ(0, result);
    
    ASSERT_NE(nullptr, results[0]);
    EXPECT_STREQ("1", results[0]);
    
    ASSERT_NE(nullptr, results[1]);
    EXPECT_STREQ("1", results[1]);
    
    ASSERT_NE(nullptr, results[2]);
    EXPECT_STREQ("0", results[2]);
    
    ASSERT_NE(nullptr, results[3]);
    EXPECT_STREQ("0", results[3]);
}

TEST_F(URLManagementTest, ManageUrls_CheckValid_WithNullUrl) {
    urls = new char*[3];
    urls[0] = const_cast<char*>("https://example.com");
    urls[1] = nullptr;
    urls[2] = const_cast<char*>("http://test.org");
    allocateResults(3);
    
    int result = manageUrls(urls, 3, "checkValid", results);
    EXPECT_EQ(0, result);
    
    ASSERT_NE(nullptr, results[0]);
    EXPECT_STREQ("1", results[0]);
    
    EXPECT_EQ(nullptr, results[1]);
    
    ASSERT_NE(nullptr, results[2]);
    EXPECT_STREQ("1", results[2]);
}

TEST_F(URLManagementTest, ManageUrls_CheckValid_ComplexUrls) {
    urls = new char*[5];
    urls[0] = const_cast<char*>("https://www.example.com/path/to/page");
    urls[1] = const_cast<char*>("http://example.com:8080");
    urls[2] = const_cast<char*>("https://example.com?query=value&foo=bar");
    urls[3] = const_cast<char*>("http://localhost");
    urls[4] = const_cast<char*>("https://sub.domain.example.com/path#fragment");
    allocateResults(5);
    
    int result = manageUrls(urls, 5, "checkValid", results);
    EXPECT_EQ(0, result);
    
    EXPECT_STREQ("1", results[0]);
    EXPECT_STREQ("1", results[1]);
    EXPECT_STREQ("1", results[2]);
    EXPECT_STREQ("0", results[3]); // localhost has no dot
    EXPECT_STREQ("1", results[4]);
}

// ========================================
// Tests for format action
// ========================================

TEST_F(URLManagementTest, ManageUrls_Format_SingleUrl) {
    const char* url = "example.com";
    urls = new char*[1];
    urls[0] = const_cast<char*>(url);
    allocateResults(1);
    
    int result = manageUrls(urls, 1, "format", results);
    EXPECT_EQ(0, result);
    ASSERT_NE(nullptr, results[0]);
    EXPECT_STREQ("https://example.com", results[0]);
}

TEST_F(URLManagementTest, ManageUrls_Format_UrlWithProtocol) {
    const char* url = "http://example.com";
    urls = new char*[1];
    urls[0] = const_cast<char*>(url);
    allocateResults(1);
    
    int result = manageUrls(urls, 1, "format", results);
    EXPECT_EQ(0, result);
    ASSERT_NE(nullptr, results[0]);
    EXPECT_STREQ("http://example.com", results[0]);
}

TEST_F(URLManagementTest, ManageUrls_Format_MultipleUrls) {
    urls = new char*[3];
    urls[0] = const_cast<char*>("example.com");
    urls[1] = const_cast<char*>("http://test.org");
    urls[2] = const_cast<char*>("subdomain.example.com/path");
    allocateResults(3);
    
    int result = manageUrls(urls, 3, "format", results);
    EXPECT_EQ(0, result);
    
    ASSERT_NE(nullptr, results[0]);
    EXPECT_STREQ("https://example.com", results[0]);
    
    ASSERT_NE(nullptr, results[1]);
    EXPECT_STREQ("http://test.org", results[1]);
    
    ASSERT_NE(nullptr, results[2]);
    EXPECT_STREQ("https://subdomain.example.com/path", results[2]);
}

TEST_F(URLManagementTest, ManageUrls_Format_WithNullUrl) {
    urls = new char*[2];
    urls[0] = const_cast<char*>("example.com");
    urls[1] = nullptr;
    allocateResults(2);
    
    int result = manageUrls(urls, 2, "format", results);
    EXPECT_EQ(0, result);
    
    ASSERT_NE(nullptr, results[0]);
    EXPECT_STREQ("https://example.com", results[0]);
    
    EXPECT_EQ(nullptr, results[1]);
}

TEST_F(URLManagementTest, ManageUrls_Format_ComplexUrls) {
    urls = new char*[4];
    urls[0] = const_cast<char*>("www.example.com/path?query=value");
    urls[1] = const_cast<char*>("https://already-formatted.com");
    urls[2] = const_cast<char*>("sub.domain.com:8080");
    urls[3] = const_cast<char*>("example.com/very/long/path#fragment");
    allocateResults(4);
    
    int result = manageUrls(urls, 4, "format", results);
    EXPECT_EQ(0, result);
    
    EXPECT_STREQ("https://www.example.com/path?query=value", results[0]);
    EXPECT_STREQ("https://already-formatted.com", results[1]);
    EXPECT_STREQ("https://sub.domain.com:8080", results[2]);
    EXPECT_STREQ("https://example.com/very/long/path#fragment", results[3]);
}

// ========================================
// Tests for shorten action
// ========================================

TEST_F(URLManagementTest, ManageUrls_Shorten_SingleUrl) {
    const char* url = "https://example.com/very/long/path/to/resource/file";
    urls = new char*[1];
    urls[0] = const_cast<char*>(url);
    allocateResults(1);
    
    int result = manageUrls(urls, 1, "shorten", results);
    EXPECT_EQ(0, result);
    ASSERT_NE(nullptr, results[0]);
    EXPECT_STREQ("https://example.com/very/long/...", results[0]);
}

TEST_F(URLManagementTest, ManageUrls_Shorten_ShortUrl) {
    const char* url = "https://ex.com";
    urls = new char*[1];
    urls[0] = const_cast<char*>(url);
    allocateResults(1);
    
    int result = manageUrls(urls, 1, "shorten", results);
    EXPECT_EQ(0, result);
    ASSERT_NE(nullptr, results[0]);
    EXPECT_STREQ("https://ex.com", results[0]);
}

TEST_F(URLManagementTest, ManageUrls_Shorten_MultipleUrls) {
    urls = new char*[3];
    urls[0] = const_cast<char*>("https://example.com/very/very/very/long/path");
    urls[1] = const_cast<char*>("http://short.url");
    urls[2] = const_cast<char*>("https://medium.example.com/path/to/page");
    allocateResults(3);
    
    int result = manageUrls(urls, 3, "shorten", results);
    EXPECT_EQ(0, result);
    
    ASSERT_NE(nullptr, results[0]);
    EXPECT_EQ(33, strlen(results[0])); // 30 + "..."
    
    ASSERT_NE(nullptr, results[1]);
    EXPECT_STREQ("http://short.url", results[1]);
    
    ASSERT_NE(nullptr, results[2]);
    EXPECT_EQ(33, strlen(results[2])); // 30 + "..."
}

TEST_F(URLManagementTest, ManageUrls_Shorten_WithNullUrl) {
    urls = new char*[2];
    urls[0] = const_cast<char*>("https://example.com/long/path");
    urls[1] = nullptr;
    allocateResults(2);
    
    int result = manageUrls(urls, 2, "shorten", results);
    EXPECT_EQ(0, result);
    
    ASSERT_NE(nullptr, results[0]);
    EXPECT_EQ(nullptr, results[1]);
}

TEST_F(URLManagementTest, ManageUrls_Shorten_ExactlyAtLimit) {
    // Create a URL that's exactly 30 characters
    const char* url = "https://example.com/123456789A";
    ASSERT_EQ(30, strlen(url));
    
    urls = new char*[1];
    urls[0] = const_cast<char*>(url);
    allocateResults(1);
    
    int result = manageUrls(urls, 1, "shorten", results);
    EXPECT_EQ(0, result);
    ASSERT_NE(nullptr, results[0]);
    EXPECT_STREQ(url, results[0]);
}

// ========================================
// Integration Tests
// ========================================

TEST_F(URLManagementTest, Integration_CheckValidThenFormat) {
    urls = new char*[2];
    urls[0] = const_cast<char*>("example.com");
    urls[1] = const_cast<char*>("test.org");
    allocateResults(2);
    
    // First check validity
    int result = manageUrls(urls, 2, "checkValid", results);
    EXPECT_EQ(0, result);
    EXPECT_STREQ("0", results[0]); // Invalid without protocol
    EXPECT_STREQ("0", results[1]); // Invalid without protocol
    
    // Clean up results
    for (int i = 0; i < 2; i++) {
        free(results[i]);
        results[i] = nullptr;
    }
    
    // Now format them
    result = manageUrls(urls, 2, "format", results);
    EXPECT_EQ(0, result);
    EXPECT_STREQ("https://example.com", results[0]);
    EXPECT_STREQ("https://test.org", results[1]);
}

TEST_F(URLManagementTest, Integration_FormatThenShorten) {
    urls = new char*[1];
    urls[0] = const_cast<char*>("example.com/very/long/path/to/resource");
    allocateResults(1);
    
    // First format
    int result = manageUrls(urls, 1, "format", results);
    EXPECT_EQ(0, result);
    ASSERT_NE(nullptr, results[0]);
    
    // Save formatted result
    char* formatted = strdup(results[0]);
    free(results[0]);
    results[0] = nullptr;
    
    // Update urls to use formatted version
    delete[] urls;
    urls = new char*[1];
    urls[0] = formatted;
    
    // Then shorten
    result = manageUrls(urls, 1, "shorten", results);
    EXPECT_EQ(0, result);
    ASSERT_NE(nullptr, results[0]);
    
    // Should be shortened
    EXPECT_LT(strlen(results[0]), strlen(formatted));
    
    free(formatted);
}

TEST_F(URLManagementTest, Integration_LargeUrlBatch) {
    const int batchSize = 10;
    urls = new char*[batchSize];
    urls[0] = const_cast<char*>("https://example1.com");
    urls[1] = const_cast<char*>("example2.com");
    urls[2] = const_cast<char*>("http://example3.com/path");
    urls[3] = const_cast<char*>("invalid");
    urls[4] = const_cast<char*>("https://example5.com?query=value");
    urls[5] = const_cast<char*>("example6.com:8080");
    urls[6] = const_cast<char*>("https://example7.com#fragment");
    urls[7] = const_cast<char*>("http://example8.com/very/long/path");
    urls[8] = nullptr;
    urls[9] = const_cast<char*>("https://example10.com");
    allocateResults(batchSize);
    
    int result = manageUrls(urls, batchSize, "checkValid", results);
    EXPECT_EQ(0, result);
    
    EXPECT_STREQ("1", results[0]);
    EXPECT_STREQ("0", results[1]);
    EXPECT_STREQ("1", results[2]);
    EXPECT_STREQ("0", results[3]);
    EXPECT_STREQ("1", results[4]);
    EXPECT_STREQ("0", results[5]);
    EXPECT_STREQ("1", results[6]);
    EXPECT_STREQ("1", results[7]);
    EXPECT_EQ(nullptr, results[8]);
    EXPECT_STREQ("1", results[9]);
}

// ========================================
// Edge Cases and Stress Tests
// ========================================

TEST_F(URLManagementTest, EdgeCase_EmptyAction) {
    const char* url = "https://example.com";
    urls = new char*[1];
    urls[0] = const_cast<char*>(url);
    allocateResults(1);
    
    int result = manageUrls(urls, 1, "", results);
    EXPECT_EQ(-1, result);
}

TEST_F(URLManagementTest, EdgeCase_CaseSensitiveAction) {
    const char* url = "https://example.com";
    urls = new char*[1];
    urls[0] = const_cast<char*>(url);
    allocateResults(1);
    
    // Test with different cases
    int result = manageUrls(urls, 1, "CheckValid", results);
    EXPECT_EQ(-1, result); // Should fail as action is case-sensitive
}

TEST_F(URLManagementTest, EdgeCase_AllNullUrls) {
    urls = new char*[3];
    urls[0] = nullptr;
    urls[1] = nullptr;
    urls[2] = nullptr;
    allocateResults(3);
    
    int result = manageUrls(urls, 3, "checkValid", results);
    EXPECT_EQ(0, result);
    
    EXPECT_EQ(nullptr, results[0]);
    EXPECT_EQ(nullptr, results[1]);
    EXPECT_EQ(nullptr, results[2]);
}

TEST_F(URLManagementTest, EdgeCase_VeryLongUrlArray) {
    const int largeSize = 100;
    urls = new char*[largeSize];
    for (int i = 0; i < largeSize; i++) {
        urls[i] = const_cast<char*>("https://example.com");
    }
    allocateResults(largeSize);
    
    int result = manageUrls(urls, largeSize, "checkValid", results);
    EXPECT_EQ(0, result);
    
    for (int i = 0; i < largeSize; i++) {
        ASSERT_NE(nullptr, results[i]);
        EXPECT_STREQ("1", results[i]);
    }
}

TEST_F(URLManagementTest, EdgeCase_SpecialCharactersInUrls) {
    urls = new char*[3];
    urls[0] = const_cast<char*>("https://example.com/path?name=John%20Doe");
    urls[1] = const_cast<char*>("http://example.com/search?q=C++&lang=en");
    urls[2] = const_cast<char*>("https://example.com/page#section-1.2.3");
    allocateResults(3);
    
    int result = manageUrls(urls, 3, "checkValid", results);
    EXPECT_EQ(0, result);
    
    EXPECT_STREQ("1", results[0]);
    EXPECT_STREQ("1", results[1]);
    EXPECT_STREQ("1", results[2]);
}

// Test summary
TEST_F(URLManagementTest, TestSuiteSummary) {
    SUCCEED() << "URL Management test suite completed successfully";
}
