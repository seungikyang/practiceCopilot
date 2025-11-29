/**
 * @file test_discount_gtest.cpp
 * @brief Google Test unit tests for discount calculation module
 *
 * This file contains comprehensive unit tests for the discount
 * calculation functions including edge cases and boundary testing.
 */

#include <gtest/gtest.h>
#include <cmath>

extern "C" {
#include "getDiscount.h"
}

/**
 * @class DiscountTest
 * @brief Test fixture for discount calculation tests
 */
class DiscountTest : public ::testing::Test {
protected:
    // Tolerance for floating point comparisons
    const float EPSILON = 0.01f;

    void SetUp() override {
        // Setup runs before each test
    }

    void TearDown() override {
        // Cleanup runs after each test
    }

    // Helper function to compare floats
    bool FloatEquals(float a, float b) {
        return std::fabs(a - b) < EPSILON;
    }
};

/* ========================================================================
 * Regular Customer Tests
 * ======================================================================== */

/**
 * @brief Test regular customer with price under $100
 *
 * Regular customers get no discount on purchases under $100.
 */
TEST_F(DiscountTest, RegularCustomer_UnderThreshold) {
    float result = get_discounted_price(50.0f, "regular");
    EXPECT_TRUE(FloatEquals(result, 50.0f));
    
    result = get_discounted_price(99.99f, "regular");
    EXPECT_TRUE(FloatEquals(result, 99.99f));
}

/**
 * @brief Test regular customer with price at $100
 *
 * At exactly $100, no discount is applied.
 */
TEST_F(DiscountTest, RegularCustomer_AtThreshold) {
    float result = get_discounted_price(100.0f, "regular");
    EXPECT_TRUE(FloatEquals(result, 100.0f));
}

/**
 * @brief Test regular customer with price over $100
 *
 * Regular customers get 10% discount on purchases over $100.
 */
TEST_F(DiscountTest, RegularCustomer_OverThreshold) {
    float result = get_discounted_price(150.0f, "regular");
    EXPECT_TRUE(FloatEquals(result, 135.0f));  // 150 * 0.9
    
    result = get_discounted_price(200.0f, "regular");
    EXPECT_TRUE(FloatEquals(result, 180.0f));  // 200 * 0.9
}

/* ========================================================================
 * Member Customer Tests
 * ======================================================================== */

/**
 * @brief Test member customer with price under $100
 *
 * Member customers get 5% discount on all purchases.
 */
TEST_F(DiscountTest, MemberCustomer_UnderThreshold) {
    float result = get_discounted_price(50.0f, "member");
    EXPECT_TRUE(FloatEquals(result, 47.5f));  // 50 * 0.95
    
    result = get_discounted_price(80.0f, "member");
    EXPECT_TRUE(FloatEquals(result, 76.0f));  // 80 * 0.95
}

/**
 * @brief Test member customer with price at $100
 *
 * At $100, member gets 5% discount.
 */
TEST_F(DiscountTest, MemberCustomer_AtThreshold) {
    float result = get_discounted_price(100.0f, "member");
    EXPECT_TRUE(FloatEquals(result, 95.0f));  // 100 * 0.95
}

/**
 * @brief Test member customer with price over $100
 *
 * Member customers get 15% discount on purchases over $100.
 */
TEST_F(DiscountTest, MemberCustomer_OverThreshold) {
    float result = get_discounted_price(150.0f, "member");
    EXPECT_TRUE(FloatEquals(result, 127.5f));  // 150 * 0.85
    
    result = get_discounted_price(200.0f, "member");
    EXPECT_TRUE(FloatEquals(result, 170.0f));  // 200 * 0.85
}

/* ========================================================================
 * VIP Customer Tests
 * ======================================================================== */

/**
 * @brief Test VIP customer with price under $100
 *
 * VIP customers get 10% discount on all purchases.
 */
TEST_F(DiscountTest, VipCustomer_UnderThreshold) {
    float result = get_discounted_price(50.0f, "vip");
    EXPECT_TRUE(FloatEquals(result, 45.0f));  // 50 * 0.9
    
    result = get_discounted_price(75.0f, "vip");
    EXPECT_TRUE(FloatEquals(result, 67.5f));  // 75 * 0.9
}

/**
 * @brief Test VIP customer with price at $100
 *
 * At $100, VIP gets 10% discount.
 */
TEST_F(DiscountTest, VipCustomer_AtThreshold) {
    float result = get_discounted_price(100.0f, "vip");
    EXPECT_TRUE(FloatEquals(result, 90.0f));  // 100 * 0.9
}

/**
 * @brief Test VIP customer with price over $100
 *
 * VIP customers get 20% discount on purchases over $100.
 */
TEST_F(DiscountTest, VipCustomer_OverThreshold) {
    float result = get_discounted_price(150.0f, "vip");
    EXPECT_TRUE(FloatEquals(result, 120.0f));  // 150 * 0.8
    
    result = get_discounted_price(250.0f, "vip");
    EXPECT_TRUE(FloatEquals(result, 200.0f));  // 250 * 0.8
}

/* ========================================================================
 * Edge Cases and Invalid Input Tests
 * ======================================================================== */

/**
 * @brief Test with zero price
 *
 * Zero price should return zero.
 */
TEST_F(DiscountTest, ZeroPrice) {
    EXPECT_TRUE(FloatEquals(get_discounted_price(0.0f, "regular"), 0.0f));
    EXPECT_TRUE(FloatEquals(get_discounted_price(0.0f, "member"), 0.0f));
    EXPECT_TRUE(FloatEquals(get_discounted_price(0.0f, "vip"), 0.0f));
}

/**
 * @brief Test with NULL customer type
 *
 * NULL customer type should return original price.
 */
TEST_F(DiscountTest, NullCustomerType) {
    float result = get_discounted_price(100.0f, nullptr);
    EXPECT_TRUE(FloatEquals(result, 100.0f));
}

/**
 * @brief Test with invalid customer type
 *
 * Invalid customer types should return original price.
 */
TEST_F(DiscountTest, InvalidCustomerType) {
    float result = get_discounted_price(100.0f, "invalid");
    EXPECT_TRUE(FloatEquals(result, 100.0f));
    
    result = get_discounted_price(100.0f, "");
    EXPECT_TRUE(FloatEquals(result, 100.0f));
    
    result = get_discounted_price(100.0f, "REGULAR");  // Case sensitive
    EXPECT_TRUE(FloatEquals(result, 100.0f));
}

/**
 * @brief Test with very large prices
 *
 * Verify calculations work correctly with large numbers.
 */
TEST_F(DiscountTest, LargePrices) {
    float result = get_discounted_price(10000.0f, "vip");
    EXPECT_TRUE(FloatEquals(result, 8000.0f));  // 10000 * 0.8
    
    result = get_discounted_price(99999.0f, "member");
    EXPECT_TRUE(FloatEquals(result, 84999.15f));  // 99999 * 0.85
}

/**
 * @brief Test with small prices
 *
 * Verify calculations work correctly with small numbers.
 */
TEST_F(DiscountTest, SmallPrices) {
    float result = get_discounted_price(0.01f, "vip");
    EXPECT_TRUE(FloatEquals(result, 0.009f));  // 0.01 * 0.9
    
    result = get_discounted_price(1.50f, "member");
    EXPECT_TRUE(FloatEquals(result, 1.425f));  // 1.5 * 0.95
}

/**
 * @brief Test prices around $100 threshold
 *
 * Verify boundary conditions around the $100 threshold.
 */
TEST_F(DiscountTest, ThresholdBoundary) {
    // Just under threshold
    float result = get_discounted_price(100.00f, "regular");
    EXPECT_TRUE(FloatEquals(result, 100.0f));
    
    // Just over threshold
    result = get_discounted_price(100.01f, "regular");
    EXPECT_TRUE(FloatEquals(result, 90.009f));  // 100.01 * 0.9
    
    // Member at threshold
    result = get_discounted_price(100.00f, "member");
    EXPECT_TRUE(FloatEquals(result, 95.0f));  // 100 * 0.95
    
    result = get_discounted_price(100.01f, "member");
    EXPECT_TRUE(FloatEquals(result, 85.0085f));  // 100.01 * 0.85
}

/* ========================================================================
 * Alternative Implementation Tests (v2)
 * ======================================================================== */

/**
 * @brief Test that v2 implementation produces same results as v1
 *
 * Verify both implementations are equivalent.
 */
TEST_F(DiscountTest, V2_EquivalenceTest) {
    float prices[] = {50.0f, 100.0f, 150.0f, 200.0f};
    const char* types[] = {"regular", "member", "vip"};
    
    for (float price : prices) {
        for (const char* type : types) {
            float result_v1 = get_discounted_price(price, type);
            float result_v2 = get_discounted_price_v2(price, type);
            EXPECT_TRUE(FloatEquals(result_v1, result_v2))
                << "Mismatch for price=" << price << " type=" << type
                << " v1=" << result_v1 << " v2=" << result_v2;
        }
    }
}

/**
 * @brief Test v2 with edge cases
 */
TEST_F(DiscountTest, V2_EdgeCases) {
    EXPECT_TRUE(FloatEquals(get_discounted_price_v2(0.0f, "vip"), 0.0f));
    EXPECT_TRUE(FloatEquals(get_discounted_price_v2(100.0f, nullptr), 100.0f));
    EXPECT_TRUE(FloatEquals(get_discounted_price_v2(100.0f, "invalid"), 100.0f));
}

/* ========================================================================
 * Comprehensive Matrix Test
 * ======================================================================== */

/**
 * @brief Comprehensive test of all combinations
 *
 * Tests all customer types with various price points.
 */
TEST_F(DiscountTest, ComprehensiveMatrix) {
    struct TestCase {
        float price;
        const char* type;
        float expected;
    };
    
    TestCase cases[] = {
        // Regular customers
        {50.0f, "regular", 50.0f},
        {100.0f, "regular", 100.0f},
        {150.0f, "regular", 135.0f},
        
        // Member customers
        {50.0f, "member", 47.5f},
        {100.0f, "member", 95.0f},
        {150.0f, "member", 127.5f},
        
        // VIP customers
        {50.0f, "vip", 45.0f},
        {100.0f, "vip", 90.0f},
        {150.0f, "vip", 120.0f},
        
        // Edge cases
        {0.0f, "regular", 0.0f},
        {100.01f, "regular", 90.009f},
        {99.99f, "member", 94.9905f},
    };
    
    for (const auto& test : cases) {
        float result = get_discounted_price(test.price, test.type);
        EXPECT_TRUE(FloatEquals(result, test.expected))
            << "Failed for price=" << test.price << " type=" << test.type
            << " expected=" << test.expected << " got=" << result;
    }
}

/* ========================================================================
 * Main Test Runner
 * ======================================================================== */

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
