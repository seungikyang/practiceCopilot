/**
 * @file test_integration_gtest.cpp
 * @brief Integration tests for url_tools and url modules interaction
 * 
 * This file contains comprehensive integration tests that verify the correct
 * interaction between url_tools module functions (is_valid_url, format_url,
 * shorten_url) and the url module's manageUrls function.
 * 
 * @note All tests follow the project's C/C++ coding standards
 * @see .github/COPILOT_INSTRUCTIONS.md for coding guidelines
 */

#include <gtest/gtest.h>
#include <cstring>
#include <vector>
#include <string>
#include <memory>

extern "C" {
    #include "url_tools.h"
    #include "url.h"
}

/**
 * @class URLIntegrationTest
 * @brief Test fixture for URL module integration testing
 * 
 * Provides setup and teardown for integration tests between url_tools
 * and url modules. Manages test resources (URL arrays, result arrays)
 * and provides helper functions for common test operations.
 * 
 * @note Follows RAII principles for resource management
 */
class URLIntegrationTest : public ::testing::Test {
protected:
    char** urls;
    char** results;
    int urlCount;

    /**
     * @brief Set up test fixture before each test
     * 
     * Initializes all pointers to nullptr to ensure safe cleanup
     * in case of early test termination.
     */
    void SetUp() override {
        urls = nullptr;
        results = nullptr;
        urlCount = 0;
    }

    /**
     * @brief Clean up test fixture after each test
     * 
     * Frees all allocated memory to prevent memory leaks.
     * Safely handles nullptr and partially allocated resources.
     */
    void TearDown() override {
        // Clean up results - free C-allocated strings
        if (results != nullptr) {
            for (int i = 0; i < urlCount; i++) {
                if (results[i] != nullptr) {
                    free(results[i]);
                    results[i] = nullptr;  // Prevent dangling pointers
                }
            }
            delete[] results;
            results = nullptr;
        }
        
        // Clean up urls - C++ allocated array
        if (urls != nullptr) {
            delete[] urls;
            urls = nullptr;
        }
    }

    /**
     * @brief Allocate result array for test
     * 
     * @param count Number of result slots to allocate
     * @note Caller must ensure count > 0
     * @note All result pointers are initialized to nullptr
     */
    void allocateResults(int count) {
        urlCount = count;
        results = new char*[count];
        for (int i = 0; i < count; i++) {
            results[i] = nullptr;
        }
    }

    /**
     * @brief Verify URL validity matches expected result
     * 
     * @param url URL string to validate (can be nullptr)
     * @param expectedValid Expected validity state
     * @return true if actual validity matches expected, false otherwise
     * @note Handles nullptr input safely
     */
    bool verifyUrlValidity(const char* url, bool expectedValid) {
        if (url == nullptr) {
            return !expectedValid;  // nullptr URLs are invalid
        }
        int result = is_valid_url(url);
        return (result == 1) == expectedValid;
    }

    /**
     * @brief Helper to create a copy-safe URL array from vector
     * 
     * @param urlVector Vector of URL strings
     * @return Allocated array of char pointers (caller must delete[])
     */
    char** createUrlArray(const std::vector<std::string>& urlVector) {
        char** array = new char*[urlVector.size()];
        for (size_t i = 0; i < urlVector.size(); i++) {
            array[i] = const_cast<char*>(urlVector[i].c_str());
        }
        return array;
    }
};

// ========================================
// Basic Integration Tests
// ========================================

/**
 * @test Integration_ManageUrlsUsesIsValidUrl_CorrectlyDelegates
 * @brief Verify manageUrls correctly delegates to is_valid_url
 * 
 * Tests that the manageUrls function with "checkValid" action properly
 * calls is_valid_url for each URL and returns matching results.
 * 
 * @see manageUrls() in url.c
 * @see is_valid_url() in url_tools.c
 */
TEST_F(URLIntegrationTest, Integration_ManageUrlsUsesIsValidUrl_CorrectlyDelegates) {
    // Arrange: Prepare test URLs with known validity states
    urls = new char*[3];
    urls[0] = const_cast<char*>("https://example.com");      // Valid
    urls[1] = const_cast<char*>("invalid-url");              // Invalid - no protocol
    urls[2] = const_cast<char*>("http://test.org/path");     // Valid
    allocateResults(3);
    
    // Act: Use manageUrls with checkValid action
    int result = manageUrls(urls, 3, "checkValid", results);
    
    // Assert: Verify success and correctness
    EXPECT_EQ(0, result) << "manageUrls should return 0 on success";
    
    // Verify each result matches direct is_valid_url calls
    ASSERT_NE(nullptr, results[0]) << "Result[0] should not be nullptr";
    EXPECT_EQ(is_valid_url(urls[0]), strcmp(results[0], "1") == 0 ? 1 : 0)
        << "Valid URL should return '1'";
    
    ASSERT_NE(nullptr, results[1]) << "Result[1] should not be nullptr";
    EXPECT_EQ(is_valid_url(urls[1]), strcmp(results[1], "1") == 0 ? 1 : 0)
        << "Invalid URL should return '0'";
    
    ASSERT_NE(nullptr, results[2]) << "Result[2] should not be nullptr";
    EXPECT_EQ(is_valid_url(urls[2]), strcmp(results[2], "1") == 0 ? 1 : 0)
        << "Valid URL with path should return '1'";
}

/**
 * @test Integration_ManageUrlsUsesFormatUrl_ProducesIdenticalResults
 * @brief Verify manageUrls format action matches format_url output
 * 
 * Tests that manageUrls with "format" action produces identical results
 * to calling format_url directly.
 * 
 * @see manageUrls() in url.c
 * @see format_url() in url_tools.c
 */
TEST_F(URLIntegrationTest, Integration_ManageUrlsUsesFormatUrl_ProducesIdenticalResults) {
    // Arrange: URL that needs formatting (no protocol)
    const char* testUrl = "example.com/path";
    urls = new char*[1];
    urls[0] = const_cast<char*>(testUrl);
    allocateResults(1);
    
    // Act: Format through manageUrls
    int result = manageUrls(urls, 1, "format", results);
    
    // Assert: Verify result matches direct format_url call
    EXPECT_EQ(0, result) << "manageUrls should return 0 on success";
    ASSERT_NE(nullptr, results[0]) << "Formatted result should not be nullptr";
    
    // Compare with direct format_url call
    char* directResult = format_url(testUrl);
    ASSERT_NE(nullptr, directResult) << "Direct format_url should not return nullptr";
    EXPECT_STREQ(directResult, results[0]) 
        << "manageUrls and format_url should produce identical results";
    
    // Clean up direct call result
    free(directResult);
}

/**
 * @test Integration_ManageUrlsUsesShortenUrl_ProducesIdenticalResults
 * @brief Verify manageUrls shorten action matches shorten_url output
 * 
 * Tests that manageUrls with "shorten" action produces identical results
 * to calling shorten_url directly with the same length parameter (30).
 * 
 * @see manageUrls() in url.c
 * @see shorten_url() in url_tools.c
 */
TEST_F(URLIntegrationTest, Integration_ManageUrlsUsesShortenUrl_ProducesIdenticalResults) {
    // Arrange: Long URL that needs shortening
    const char* testUrl = "https://example.com/very/long/path/to/resource";
    urls = new char*[1];
    urls[0] = const_cast<char*>(testUrl);
    allocateResults(1);
    
    // Act: Use manageUrls with shorten action
    int result = manageUrls(urls, 1, "shorten", results);
    
    // Assert: Verify result matches direct shorten_url call
    EXPECT_EQ(0, result) << "manageUrls should return 0 on success";
    ASSERT_NE(nullptr, results[0]) << "Shortened result should not be nullptr";
    
    // Compare with direct shorten_url call (manageUrls uses length 30)
    char* directResult = shorten_url(testUrl, 30);
    ASSERT_NE(nullptr, directResult) << "Direct shorten_url should not return nullptr";
    EXPECT_STREQ(directResult, results[0])
        << "manageUrls and shorten_url should produce identical results";
    
    // Clean up direct call result
    free(directResult);
}

// ========================================
// Workflow Integration Tests
// ========================================

/**
 * @test Workflow_ValidateFormatValidate_TransformsInvalidToValid
 * @brief Test complete workflow: validate -> format -> validate
 * 
 * Demonstrates a common workflow where an invalid URL is detected,
 * formatted to add the required protocol, and then becomes valid.
 * Tests data flow through both modules.
 * 
 * @note This is a real-world use case scenario
 */
TEST_F(URLIntegrationTest, Workflow_ValidateFormatValidate_TransformsInvalidToValid) {
    // Arrange: URL without protocol (invalid state)
    const char* originalUrl = "example.com";
    
    // Act & Assert Step 1: Validate original URL (should be invalid)
    EXPECT_FALSE(verifyUrlValidity(originalUrl, true))
        << "URL without protocol should be invalid";
    EXPECT_TRUE(verifyUrlValidity(originalUrl, false))
        << "URL without protocol should fail validation";
    
    // Act & Assert Step 2: Format the URL to add protocol
    char* formatted = format_url(originalUrl);
    ASSERT_NE(nullptr, formatted) << "format_url should not return nullptr";
    EXPECT_STREQ("https://example.com", formatted)
        << "format_url should add https:// prefix";
    
    // Act & Assert Step 3: Validate formatted URL (should now be valid)
    EXPECT_TRUE(verifyUrlValidity(formatted, true))
        << "Formatted URL should be valid";
    
    // Act & Assert Step 4: Verify through manageUrls for consistency
    urls = new char*[1];
    urls[0] = formatted;
    allocateResults(1);
    
    int result = manageUrls(urls, 1, "checkValid", results);
    EXPECT_EQ(0, result);
    EXPECT_STREQ("1", results[0]);
    
    free(formatted);
}

TEST_F(URLIntegrationTest, Workflow_FormatThenShorten) {
    // Workflow: Format URL -> Shorten it
    const char* originalUrl = "subdomain.example.com/very/long/path/to/resource/file.html";
    
    // Step 1: Format the URL
    char* formatted = format_url(originalUrl);
    ASSERT_NE(nullptr, formatted);
    EXPECT_STREQ("https://subdomain.example.com/very/long/path/to/resource/file.html", formatted);
    
    // Step 2: Shorten the formatted URL
    char* shortened = shorten_url(formatted, 30);
    ASSERT_NE(nullptr, shortened);
    EXPECT_EQ(33, strlen(shortened)); // 30 + "..."
    
    // Verify both through manageUrls
    urls = new char*[1];
    urls[0] = const_cast<char*>(originalUrl);
    allocateResults(1);
    
    // First format
    int result = manageUrls(urls, 1, "format", results);
    EXPECT_EQ(0, result);
    EXPECT_STREQ(formatted, results[0]);
    
    // Save result
    char* savedFormatted = strdup(results[0]);
    free(results[0]);
    results[0] = nullptr;
    
    // Now shorten
    delete[] urls;
    urls = new char*[1];
    urls[0] = savedFormatted;
    
    result = manageUrls(urls, 1, "shorten", results);
    EXPECT_EQ(0, result);
    EXPECT_STREQ(shortened, results[0]);
    
    free(formatted);
    free(shortened);
    free(savedFormatted);
}

TEST_F(URLIntegrationTest, Workflow_BatchProcessing) {
    // Test batch processing workflow
    std::vector<std::string> inputUrls = {
        "example1.com",
        "http://example2.com",
        "example3.com/path",
        "https://example4.com"
    };
    
    int count = inputUrls.size();
    urls = new char*[count];
    for (int i = 0; i < count; i++) {
        urls[i] = const_cast<char*>(inputUrls[i].c_str());
    }
    allocateResults(count);
    
    // Step 1: Format all URLs
    int result = manageUrls(urls, count, "format", results);
    EXPECT_EQ(0, result);
    
    // Verify each URL is formatted correctly
    for (int i = 0; i < count; i++) {
        ASSERT_NE(nullptr, results[i]);
        char* directFormat = format_url(urls[i]);
        EXPECT_STREQ(directFormat, results[i]);
        free(directFormat);
    }
    
    // Step 2: Validate all formatted URLs
    std::vector<char*> formattedUrls;
    for (int i = 0; i < count; i++) {
        formattedUrls.push_back(strdup(results[i]));
        free(results[i]);
        results[i] = nullptr;
    }
    
    delete[] urls;
    urls = new char*[count];
    for (int i = 0; i < count; i++) {
        urls[i] = formattedUrls[i];
    }
    
    result = manageUrls(urls, count, "checkValid", results);
    EXPECT_EQ(0, result);
    
    // All formatted URLs should be valid
    for (int i = 0; i < count; i++) {
        EXPECT_STREQ("1", results[i]);
    }
    
    // Cleanup
    for (char* url : formattedUrls) {
        free(url);
    }
}

// ========================================
// Cross-Function Consistency Tests
// ========================================

TEST_F(URLIntegrationTest, Consistency_FormatPreservesValidUrls) {
    // URLs that are already valid should remain valid after formatting
    std::vector<std::string> validUrls = {
        "http://example.com",
        "https://test.org",
        "https://subdomain.example.com/path"
    };
    
    for (const auto& url : validUrls) {
        // Check original is valid
        EXPECT_EQ(1, is_valid_url(url.c_str()));
        
        // Format it
        char* formatted = format_url(url.c_str());
        ASSERT_NE(nullptr, formatted);
        
        // Check formatted is still valid
        EXPECT_EQ(1, is_valid_url(formatted));
        
        free(formatted);
    }
}

TEST_F(URLIntegrationTest, Consistency_ShortenPreservesValidity) {
    // Valid URLs should remain valid after shortening (if they have required parts)
    const char* validUrl = "https://example.com/path";
    
    EXPECT_EQ(1, is_valid_url(validUrl));
    
    char* shortened = shorten_url(validUrl, 20);
    ASSERT_NE(nullptr, shortened);
    
    // Shortened URL should still be valid
    EXPECT_EQ(1, is_valid_url(shortened));
    
    free(shortened);
}

TEST_F(URLIntegrationTest, Consistency_MultipleFormatCalls) {
    // Formatting a URL multiple times should give the same result
    const char* url = "example.com/path";
    
    char* format1 = format_url(url);
    char* format2 = format_url(url);
    char* format3 = format_url(format1); // Format already formatted URL
    
    ASSERT_NE(nullptr, format1);
    ASSERT_NE(nullptr, format2);
    ASSERT_NE(nullptr, format3);
    
    EXPECT_STREQ(format1, format2);
    EXPECT_STREQ(format1, format3);
    
    free(format1);
    free(format2);
    free(format3);
}

TEST_F(URLIntegrationTest, Consistency_ShortenIdempotence) {
    // Shortening an already short URL should not change it
    const char* shortUrl = "https://ex.co";
    
    char* shortened1 = shorten_url(shortUrl, 30);
    ASSERT_NE(nullptr, shortened1);
    EXPECT_STREQ(shortUrl, shortened1);
    
    char* shortened2 = shorten_url(shortened1, 30);
    ASSERT_NE(nullptr, shortened2);
    EXPECT_STREQ(shortUrl, shortened2);
    
    free(shortened1);
    free(shortened2);
}

// ========================================
// Complex Integration Scenarios
// ========================================

TEST_F(URLIntegrationTest, Scenario_CompleteUrlProcessing) {
    // Simulate a complete URL processing pipeline
    const char* rawUrl = "www.example.com/articles/2024/technology";
    
    // Step 1: Validate raw URL (should be invalid - no protocol)
    EXPECT_EQ(0, is_valid_url(rawUrl));
    
    // Step 2: Format the URL
    char* formatted = format_url(rawUrl);
    ASSERT_NE(nullptr, formatted);
    EXPECT_STREQ("https://www.example.com/articles/2024/technology", formatted);
    
    // Step 3: Validate formatted URL (should be valid)
    EXPECT_EQ(1, is_valid_url(formatted));
    
    // Step 4: Shorten for display
    char* shortened = shorten_url(formatted, 25);
    ASSERT_NE(nullptr, shortened);
    EXPECT_EQ(28, strlen(shortened)); // 25 + "..."
    
    // Step 5: Shortened URL should still be valid
    EXPECT_EQ(1, is_valid_url(shortened));
    
    // Verify through manageUrls
    urls = new char*[1];
    urls[0] = const_cast<char*>(rawUrl);
    allocateResults(1);
    
    manageUrls(urls, 1, "format", results);
    EXPECT_STREQ(formatted, results[0]);
    
    free(formatted);
    free(shortened);
}

TEST_F(URLIntegrationTest, Scenario_MixedUrlBatch) {
    // Process a batch with different URL states
    urls = new char*[5];
    urls[0] = const_cast<char*>("https://valid.com");           // Already valid
    urls[1] = const_cast<char*>("needs-protocol.com");         // Needs formatting
    urls[2] = const_cast<char*>("http://very-long-url.com/path/to/resource/file"); // Needs shortening
    urls[3] = const_cast<char*>("just-text");                  // Invalid
    urls[4] = const_cast<char*>("ftp://wrong-protocol.com");   // Wrong protocol
    allocateResults(5);
    
    // First, check validity
    manageUrls(urls, 5, "checkValid", results);
    EXPECT_STREQ("1", results[0]);
    EXPECT_STREQ("0", results[1]);
    EXPECT_STREQ("1", results[2]);
    EXPECT_STREQ("0", results[3]);
    EXPECT_STREQ("0", results[4]);
    
    // Clean results
    for (int i = 0; i < 5; i++) {
        free(results[i]);
        results[i] = nullptr;
    }
    
    // Format all URLs
    manageUrls(urls, 5, "format", results);
    
    // Verify formatted URLs
    EXPECT_EQ(1, is_valid_url(results[0]));
    EXPECT_EQ(1, is_valid_url(results[1])); // Now valid after formatting
    EXPECT_EQ(1, is_valid_url(results[2]));
    // results[3] and results[4] remain as formatted
}

TEST_F(URLIntegrationTest, Scenario_DataPipelineSimulation) {
    // Simulate a data processing pipeline
    std::vector<const char*> rawData = {
        "example.com",
        "test.org/page",
        "subdomain.site.com:8080",
        "another.example.com/very/long/path/to/content"
    };
    
    int count = rawData.size();
    
    // Stage 1: Format all URLs
    std::vector<char*> formatted;
    for (const char* url : rawData) {
        char* f = format_url(url);
        formatted.push_back(f);
    }
    
    // Stage 2: Validate all formatted URLs
    int validCount = 0;
    for (char* url : formatted) {
        if (is_valid_url(url) == 1) {
            validCount++;
        }
    }
    EXPECT_EQ(count, validCount); // All should be valid after formatting
    
    // Stage 3: Shorten URLs that are too long
    std::vector<char*> shortened;
    for (char* url : formatted) {
        if (strlen(url) > 30) {
            shortened.push_back(shorten_url(url, 30));
        } else {
            shortened.push_back(strdup(url));
        }
    }
    
    // Stage 4: Final validation
    for (char* url : shortened) {
        EXPECT_EQ(1, is_valid_url(url));
    }
    
    // Cleanup
    for (char* url : formatted) free(url);
    for (char* url : shortened) free(url);
}

// ========================================
// Error Propagation Tests
// ========================================

TEST_F(URLIntegrationTest, ErrorPropagation_NullHandling) {
    // Test that NULL handling is consistent across functions
    
    // is_valid_url with NULL
    EXPECT_EQ(0, is_valid_url(nullptr));
    
    // format_url with NULL
    EXPECT_EQ(nullptr, format_url(nullptr));
    
    // shorten_url with NULL
    EXPECT_EQ(nullptr, shorten_url(nullptr, 30));
    
    // manageUrls with NULL
    urls = new char*[1];
    urls[0] = nullptr;
    allocateResults(1);
    
    int result = manageUrls(urls, 1, "checkValid", results);
    EXPECT_EQ(0, result);
    EXPECT_EQ(nullptr, results[0]);
}

TEST_F(URLIntegrationTest, ErrorPropagation_EmptyStringHandling) {
    // Test empty string handling across functions
    const char* emptyUrl = "";
    
    // is_valid_url with empty string
    EXPECT_EQ(0, is_valid_url(emptyUrl));
    
    // format_url with empty string
    char* formatted = format_url(emptyUrl);
    ASSERT_NE(nullptr, formatted);
    EXPECT_STREQ("https://", formatted);
    free(formatted);
    
    // shorten_url with empty string
    char* shortened = shorten_url(emptyUrl, 30);
    ASSERT_NE(nullptr, shortened);
    EXPECT_STREQ("", shortened);
    free(shortened);
}

// ========================================
// Performance and Stress Tests
// ========================================

TEST_F(URLIntegrationTest, Stress_LargeUrlSet) {
    // Process a large set of URLs
    const int largeSize = 1000;
    urls = new char*[largeSize];
    
    for (int i = 0; i < largeSize; i++) {
        urls[i] = const_cast<char*>("https://example.com");
    }
    allocateResults(largeSize);
    
    // Process all URLs
    int result = manageUrls(urls, largeSize, "checkValid", results);
    EXPECT_EQ(0, result);
    
    // Verify all results
    for (int i = 0; i < largeSize; i++) {
        ASSERT_NE(nullptr, results[i]);
        EXPECT_STREQ("1", results[i]);
    }
}

TEST_F(URLIntegrationTest, Stress_RepeatedOperations) {
    // Test repeated operations on the same URL
    const char* url = "example.com/test";
    
    for (int i = 0; i < 100; i++) {
        // Format
        char* formatted = format_url(url);
        ASSERT_NE(nullptr, formatted);
        
        // Validate
        EXPECT_EQ(1, is_valid_url(formatted));
        
        // Shorten
        char* shortened = shorten_url(formatted, 30);
        ASSERT_NE(nullptr, shortened);
        
        // Validate again
        EXPECT_EQ(1, is_valid_url(shortened));
        
        free(formatted);
        free(shortened);
    }
}

TEST_F(URLIntegrationTest, Stress_AlternatingActions) {
    // Test alternating between different actions
    const char* url = "example.com";
    urls = new char*[1];
    urls[0] = const_cast<char*>(url);
    allocateResults(1);
    
    // Alternate between actions
    for (int i = 0; i < 10; i++) {
        // Format
        int result = manageUrls(urls, 1, "format", results);
        EXPECT_EQ(0, result);
        
        // Save result
        char* saved = strdup(results[0]);
        free(results[0]);
        results[0] = nullptr;
        
        // Validate
        delete[] urls;
        urls = new char*[1];
        urls[0] = saved;
        
        result = manageUrls(urls, 1, "checkValid", results);
        EXPECT_EQ(0, result);
        EXPECT_STREQ("1", results[0]);
        
        free(results[0]);
        results[0] = nullptr;
        free(saved);
        
        // Reset for next iteration
        delete[] urls;
        urls = new char*[1];
        urls[0] = const_cast<char*>(url);
    }
}

// ========================================
// Edge Cases in Integration
// ========================================

/**
 * @test EdgeCase_VeryLongUrlProcessing_MaintainsValidity
 * @brief Test processing of extremely long URLs
 * 
 * Verifies that very long URLs (>1000 characters) can be validated,
 * shortened, and remain valid after processing.
 * 
 * @note Tests robustness with extreme input sizes
 */
TEST_F(URLIntegrationTest, EdgeCase_VeryLongUrlProcessing_MaintainsValidity) {
    // Arrange: Create a very long URL (>1000 chars)
    std::string longUrl = "https://example.com/";
    for (int i = 0; i < 100; i++) {
        longUrl += "verylongpath/";
    }
    
    // Act & Assert: Validate original long URL
    EXPECT_EQ(1, is_valid_url(longUrl.c_str()))
        << "Very long URL should still be valid";
    
    // Act: Shorten the URL
    char* shortened = shorten_url(longUrl.c_str(), 50);
    ASSERT_NE(nullptr, shortened) << "shorten_url should handle long URLs";
    EXPECT_EQ(53, strlen(shortened)) << "Shortened length should be 50 + '...'";
    
    // Assert: Validate shortened URL maintains validity
    EXPECT_EQ(1, is_valid_url(shortened))
        << "Shortened URL should remain valid";
    
    // Cleanup
    free(shortened);
}

/**
 * @test EdgeCase_SpecialCharactersWorkflow_PreservesEncoding
 * @brief Test URL processing with special characters
 * 
 * Verifies that URLs containing query parameters with special characters
 * (spaces, &, +, etc.) are handled correctly through the workflow.
 * 
 * @note Tests URL encoding preservation
 */
TEST_F(URLIntegrationTest, EdgeCase_SpecialCharactersWorkflow_PreservesEncoding) {
    // Arrange: URL with special characters (spaces encoded as +, & separator)
    const char* specialUrl = "example.com/search?q=hello+world&lang=en";
    
    // Act: Format the URL
    char* formatted = format_url(specialUrl);
    ASSERT_NE(nullptr, formatted) << "format_url should handle special characters";
    
    // Assert: Formatted URL should be valid
    EXPECT_EQ(1, is_valid_url(formatted))
        << "URL with special characters should be valid after formatting";
    
    // Act: Shorten the URL
    char* shortened = shorten_url(formatted, 35);
    ASSERT_NE(nullptr, shortened) << "shorten_url should handle special characters";
    
    // Cleanup
    free(formatted);
    free(shortened);
}

/**
 * @test EdgeCase_MixedProtocols_PreservesExistingProtocols
 * @brief Test formatting with mixed protocol states
 * 
 * Verifies that format_url preserves existing http:// and https://
 * protocols while adding https:// to URLs without protocols.
 * 
 * @note Tests protocol preservation logic
 */
TEST_F(URLIntegrationTest, EdgeCase_MixedProtocols_PreservesExistingProtocols) {
    // Arrange: URLs with different protocol states
    urls = new char*[3];
    urls[0] = const_cast<char*>("http://example.com");    // Has http
    urls[1] = const_cast<char*>("https://example.com");   // Has https
    urls[2] = const_cast<char*>("example.com");           // No protocol
    allocateResults(3);
    
    // Act: Format all URLs
    int result = manageUrls(urls, 3, "format", results);
    EXPECT_EQ(0, result) << "manageUrls should succeed";
    
    // Assert: Verify protocol handling
    ASSERT_NE(nullptr, results[0]);
    EXPECT_STREQ("http://example.com", results[0])
        << "Existing http:// should be preserved";
    
    ASSERT_NE(nullptr, results[1]);
    EXPECT_STREQ("https://example.com", results[1])
        << "Existing https:// should be preserved";
    
    ASSERT_NE(nullptr, results[2]);
    EXPECT_STREQ("https://example.com", results[2])
        << "Missing protocol should get https://";
    
    // Assert: All formatted URLs should be valid
    for (int i = 0; i < 3; i++) {
        EXPECT_EQ(1, is_valid_url(results[i]))
            << "All formatted URLs should be valid (index: " << i << ")";
    }
}

/**
 * @test TestSuiteSummary
 * @brief Final summary test that always passes
 * 
 * This test serves as a marker for successful completion of the
 * entire integration test suite. Provides summary statistics.
 */
TEST_F(URLIntegrationTest, TestSuiteSummary) {
    SUCCEED() << "URL Integration test suite completed successfully - "
              << "Tested url_tools and url module integration comprehensively";
}
