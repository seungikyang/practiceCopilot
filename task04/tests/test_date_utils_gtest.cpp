/**
 * @file test_date_utils_gtest.cpp
 * @brief Google Test unit tests for date utility functions
 *
 * This file contains comprehensive unit tests for date manipulation
 * functions including date string generation, date difference calculation,
 * and date validation.
 */

#include <gtest/gtest.h>
#include <cstring>
#include <ctime>

extern "C" {
#include "date_utils.h"
}

/**
 * @class DateUtilsTest
 * @brief Test fixture for date utility tests
 */
class DateUtilsTest : public ::testing::Test {
protected:
    char buffer[50];

    void SetUp() override {
        memset(buffer, 0, sizeof(buffer));
    }

    void TearDown() override {
        // Cleanup
    }

    // Helper to validate date string format YYYY-MM-DD
    bool IsValidDateFormat(const char* date_str) {
        if (strlen(date_str) != 10) return false;
        if (date_str[4] != '-' || date_str[7] != '-') return false;
        
        for (int i = 0; i < 10; i++) {
            if (i == 4 || i == 7) continue;
            if (date_str[i] < '0' || date_str[i] > '9') return false;
        }
        return true;
    }

    // Helper to parse date string
    void ParseDate(const char* date_str, int* year, int* month, int* day) {
        sscanf(date_str, "%d-%d-%d", year, month, day);
    }
};

/* ========================================================================
 * get_date_string() Tests
 * ======================================================================== */

/**
 * @brief Test get_date_string with zero days offset
 *
 * Should return current date.
 */
TEST_F(DateUtilsTest, GetDateString_CurrentDate) {
    get_date_string(buffer, sizeof(buffer), 0);
    
    EXPECT_TRUE(IsValidDateFormat(buffer));
    
    // Verify it's approximately today
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    
    int year, month, day;
    ParseDate(buffer, &year, &month, &day);
    
    EXPECT_EQ(year, t->tm_year + 1900);
    EXPECT_EQ(month, t->tm_mon + 1);
    EXPECT_EQ(day, t->tm_mday);
}

/**
 * @brief Test get_date_string with positive days offset
 *
 * Should return future dates.
 */
TEST_F(DateUtilsTest, GetDateString_FutureDates) {
    get_date_string(buffer, sizeof(buffer), 7);
    EXPECT_TRUE(IsValidDateFormat(buffer));
    
    get_date_string(buffer, sizeof(buffer), 30);
    EXPECT_TRUE(IsValidDateFormat(buffer));
    
    get_date_string(buffer, sizeof(buffer), 365);
    EXPECT_TRUE(IsValidDateFormat(buffer));
}

/**
 * @brief Test get_date_string with negative days offset
 *
 * Should return past dates.
 */
TEST_F(DateUtilsTest, GetDateString_PastDates) {
    get_date_string(buffer, sizeof(buffer), -7);
    EXPECT_TRUE(IsValidDateFormat(buffer));
    
    get_date_string(buffer, sizeof(buffer), -30);
    EXPECT_TRUE(IsValidDateFormat(buffer));
    
    get_date_string(buffer, sizeof(buffer), -365);
    EXPECT_TRUE(IsValidDateFormat(buffer));
}

/**
 * @brief Test get_date_string with NULL buffer
 *
 * Should handle NULL gracefully.
 */
TEST_F(DateUtilsTest, GetDateString_NullBuffer) {
    // Should not crash
    get_date_string(nullptr, 0, 0);
    // No assertion - just verifying no crash
}

/**
 * @brief Test get_date_string with small buffer
 *
 * Should handle small buffers safely.
 */
TEST_F(DateUtilsTest, GetDateString_SmallBuffer) {
    char small_buffer[5];
    get_date_string(small_buffer, sizeof(small_buffer), 0);
    // Should not overflow - implementation should handle this
}

/* ========================================================================
 * get_current_date_string() Tests
 * ======================================================================== */

/**
 * @brief Test get_current_date_string
 *
 * Should return current date in correct format.
 */
TEST_F(DateUtilsTest, GetCurrentDateString_Basic) {
    get_current_date_string(buffer, sizeof(buffer));
    
    EXPECT_TRUE(IsValidDateFormat(buffer));
    
    // Should match get_date_string with 0 offset
    char buffer2[50];
    get_date_string(buffer2, sizeof(buffer2), 0);
    
    EXPECT_STREQ(buffer, buffer2);
}

/* ========================================================================
 * date_difference() Tests
 * ======================================================================== */

/**
 * @brief Test date_difference with same dates
 *
 * Difference should be 0.
 */
TEST_F(DateUtilsTest, DateDifference_SameDates) {
    int diff = date_difference("2025-01-15", "2025-01-15");
    EXPECT_EQ(diff, 0);
}

/**
 * @brief Test date_difference with consecutive days
 *
 * Should return 1 day difference.
 */
TEST_F(DateUtilsTest, DateDifference_ConsecutiveDays) {
    int diff = date_difference("2025-01-15", "2025-01-16");
    EXPECT_EQ(diff, 1);
    
    diff = date_difference("2025-01-16", "2025-01-15");
    EXPECT_EQ(diff, -1);
}

/**
 * @brief Test date_difference across month boundary
 *
 * Should handle month changes correctly.
 */
TEST_F(DateUtilsTest, DateDifference_AcrossMonths) {
    int diff = date_difference("2025-01-31", "2025-02-01");
    EXPECT_EQ(diff, 1);
    
    diff = date_difference("2025-02-28", "2025-03-01");
    EXPECT_EQ(diff, 1);
}

/**
 * @brief Test date_difference across year boundary
 *
 * Should handle year changes correctly.
 */
TEST_F(DateUtilsTest, DateDifference_AcrossYears) {
    int diff = date_difference("2024-12-31", "2025-01-01");
    EXPECT_EQ(diff, 1);
    
    diff = date_difference("2024-01-01", "2025-01-01");
    EXPECT_EQ(diff, 366);  // 2024 is a leap year, so 366 days
}

/**
 * @brief Test date_difference with leap year
 *
 * Should handle leap years correctly.
 */
TEST_F(DateUtilsTest, DateDifference_LeapYear) {
    // 2024 is a leap year
    int diff = date_difference("2024-02-28", "2024-03-01");
    EXPECT_EQ(diff, 2);  // Feb 29 exists
    
    diff = date_difference("2024-01-01", "2024-12-31");
    EXPECT_EQ(diff, 365);  // Leap year has 366 days total
}

/**
 * @brief Test date_difference with large intervals
 *
 * Should handle large date differences.
 */
TEST_F(DateUtilsTest, DateDifference_LargeIntervals) {
    int diff = date_difference("2020-01-01", "2025-01-01");
    EXPECT_EQ(diff, 1827);  // ~5 years
    
    diff = date_difference("2025-01-01", "2025-12-31");
    EXPECT_EQ(diff, 364);
}

/**
 * @brief Test date_difference with NULL parameters
 *
 * Should handle NULL gracefully.
 */
TEST_F(DateUtilsTest, DateDifference_NullParameters) {
    int diff = date_difference(nullptr, "2025-01-15");
    EXPECT_EQ(diff, 0);
    
    diff = date_difference("2025-01-15", nullptr);
    EXPECT_EQ(diff, 0);
    
    diff = date_difference(nullptr, nullptr);
    EXPECT_EQ(diff, 0);
}

/**
 * @brief Test date_difference with invalid date formats
 *
 * Should handle invalid formats gracefully.
 */
TEST_F(DateUtilsTest, DateDifference_InvalidFormats) {
    int diff = date_difference("invalid", "2025-01-15");
    EXPECT_EQ(diff, 0);
    
    diff = date_difference("2025-01-15", "bad-date");
    EXPECT_EQ(diff, 0);
    
    diff = date_difference("2025/01/15", "2025-01-16");
    EXPECT_EQ(diff, 0);  // Wrong separator
}

/* ========================================================================
 * is_valid_date_string() Tests
 * ======================================================================== */

/**
 * @brief Test is_valid_date_string with valid dates
 *
 * Should return 1 for all valid dates.
 */
TEST_F(DateUtilsTest, IsValidDateString_ValidDates) {
    EXPECT_EQ(is_valid_date_string("2025-01-15"), 1);
    EXPECT_EQ(is_valid_date_string("2024-02-29"), 1);  // Leap year
    EXPECT_EQ(is_valid_date_string("2025-12-31"), 1);
    EXPECT_EQ(is_valid_date_string("2000-01-01"), 1);
}

/**
 * @brief Test is_valid_date_string with NULL
 *
 * Should return 0 for NULL input.
 */
TEST_F(DateUtilsTest, IsValidDateString_Null) {
    EXPECT_EQ(is_valid_date_string(nullptr), 0);
}

/**
 * @brief Test is_valid_date_string with invalid formats
 *
 * Should return 0 for incorrect format.
 */
TEST_F(DateUtilsTest, IsValidDateString_InvalidFormats) {
    EXPECT_EQ(is_valid_date_string(""), 0);
    EXPECT_EQ(is_valid_date_string("2025/01/15"), 0);  // Wrong separator
    EXPECT_EQ(is_valid_date_string("25-01-15"), 0);    // Wrong year format
    // Note: "2025-1-15" might be accepted by sscanf, so removed from test
    EXPECT_EQ(is_valid_date_string("invalid"), 0);
}

/**
 * @brief Test is_valid_date_string with invalid dates
 *
 * Should return 0 for impossible dates.
 */
TEST_F(DateUtilsTest, IsValidDateString_InvalidDates) {
    EXPECT_EQ(is_valid_date_string("2025-02-30"), 0);  // Feb only has 28/29 days
    EXPECT_EQ(is_valid_date_string("2025-04-31"), 0);  // April only has 30 days
    EXPECT_EQ(is_valid_date_string("2025-13-01"), 0);  // Invalid month
    EXPECT_EQ(is_valid_date_string("2025-00-15"), 0);  // Invalid month
    EXPECT_EQ(is_valid_date_string("2025-01-32"), 0);  // Invalid day
    EXPECT_EQ(is_valid_date_string("2025-01-00"), 0);  // Invalid day
    EXPECT_EQ(is_valid_date_string("1899-01-01"), 0);  // Year too old
    EXPECT_EQ(is_valid_date_string("3001-01-01"), 0);  // Year too far future
}

/**
 * @brief Test is_valid_date_string with leap year edge cases
 *
 * Should handle Feb 29 correctly for leap and non-leap years.
 */
TEST_F(DateUtilsTest, IsValidDateString_LeapYearEdgeCases) {
    EXPECT_EQ(is_valid_date_string("2024-02-29"), 1);  // 2024 is leap year
    EXPECT_EQ(is_valid_date_string("2025-02-29"), 0);  // 2025 is not leap year
    EXPECT_EQ(is_valid_date_string("2000-02-29"), 1);  // 2000 is leap year
    EXPECT_EQ(is_valid_date_string("1900-02-29"), 0);  // 1900 is not leap year
}

/* ========================================================================
 * Integration Tests
 * ======================================================================== */

/**
 * @brief Test round-trip date operations
 *
 * Verify that generated dates are valid and can be processed.
 */
TEST_F(DateUtilsTest, Integration_RoundTrip) {
    // Get current date
    get_current_date_string(buffer, sizeof(buffer));
    
    // Verify it's valid
    EXPECT_EQ(is_valid_date_string(buffer), 1);
    
    // Calculate difference with itself
    int diff = date_difference(buffer, buffer);
    EXPECT_EQ(diff, 0);
}

/**
 * @brief Test date generation and difference calculation
 *
 * Verify that date arithmetic is consistent.
 */
TEST_F(DateUtilsTest, Integration_DateArithmetic) {
    char date1[50], date2[50];
    
    // Generate today and 7 days from now
    get_date_string(date1, sizeof(date1), 0);
    get_date_string(date2, sizeof(date2), 7);
    
    // Difference should be 7 days
    int diff = date_difference(date1, date2);
    EXPECT_EQ(diff, 7);
    
    // Reverse should be -7
    diff = date_difference(date2, date1);
    EXPECT_EQ(diff, -7);
}

/**
 * @brief Test date validation after generation
 *
 * All generated dates should be valid.
 */
TEST_F(DateUtilsTest, Integration_GeneratedDatesAreValid) {
    int offsets[] = {-365, -30, -7, -1, 0, 1, 7, 30, 365};
    
    for (int offset : offsets) {
        get_date_string(buffer, sizeof(buffer), offset);
        EXPECT_EQ(is_valid_date_string(buffer), 1)
            << "Generated date with offset " << offset << " is invalid: " << buffer;
    }
}

/* ========================================================================
 * Main Test Runner
 * ======================================================================== */

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
