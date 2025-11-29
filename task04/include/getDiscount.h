/**
 * @file getDiscount.h
 * @brief Customer discount calculation module
 *
 * This module provides functions for calculating discounted prices
 * based on customer type and purchase amount.
 *
 * @author Library Management System Team
 * @date 2025
 */

#ifndef GET_DISCOUNT_H
#define GET_DISCOUNT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Calculate discounted price based on customer type and amount
 *
 * Discount rules:
 * - Regular customers: 10% discount on purchases over $100
 * - Member customers: 5% discount on all purchases, 15% on purchases over $100
 * - VIP customers: 10% discount on all purchases, 20% on purchases over $100
 *
 * @param price Original price (must be non-negative)
 * @param customer_type Customer type ("regular", "member", or "vip")
 * @return float Discounted price
 *
 * @warning Returns original price if customer_type is invalid
 *
 * @example
 * float price = get_discounted_price(150.0f, "vip");  // Returns 120.0
 * float price2 = get_discounted_price(80.0f, "member");  // Returns 76.0
 */
float get_discounted_price(float price, const char* customer_type);

/**
 * @brief Alternative optimized version with minimal string comparisons
 *
 * This version minimizes string comparisons by checking the first
 * two characters before performing full comparison.
 *
 * @param price Original price
 * @param customer_type Customer type string
 * @return float Discounted price
 */
float get_discounted_price_v2(float price, const char* customer_type);

#ifdef __cplusplus
}
#endif

#endif /* GET_DISCOUNT_H */
