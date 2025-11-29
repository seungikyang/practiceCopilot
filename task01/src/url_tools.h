/**
 * @file url_tools.h
 * @brief URL manipulation and validation utilities
 * 
 * This header provides functions for validating, formatting, and shortening URLs.
 * All functions that allocate memory document caller's responsibility to free.
 * 
 * @note All functions handle NULL inputs gracefully
 * @see .github/COPILOT_INSTRUCTIONS.md for coding guidelines
 */

#ifndef URL_TOOLS_H
#define URL_TOOLS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Checks if the given string is a valid URL.
 * 
 * Validates if the URL starts with http:// or https:// protocol and contains
 * at least one dot (.) in the domain portion.
 *
 * @param url The URL string to validate. Can be NULL.
 * @return 1 if valid, 0 if invalid or NULL.
 * 
 * @note This performs basic validation, not comprehensive RFC 3986 compliance
 * @example
 *   int valid = is_valid_url("https://example.com");  // Returns 1
 *   int invalid = is_valid_url("example.com");        // Returns 0 (no protocol)
 */
int is_valid_url(const char* url);

/**
 * @brief Formats a URL by ensuring it has a protocol prefix.
 * 
 * If the URL does not start with http:// or https://, prepends "https://".
 * Otherwise, returns a copy of the original URL.
 *
 * @param url The URL to format. Can be NULL.
 * @return A newly allocated string with the formatted URL, or NULL if input is NULL
 *         or memory allocation fails.
 * 
 * @warning Caller must free the returned string using free()
 * @note Always check return value for NULL before using
 * 
 * @example
 *   char* formatted = format_url("example.com");
 *   if (formatted != NULL) {
 *       printf("%s\n", formatted);  // Prints: https://example.com
 *       free(formatted);
 *   }
 */
char* format_url(const char* url);

/**
 * @brief Shortens a URL to the specified length and appends "...".
 * 
 * Truncates the URL to the specified maximum length and appends "..." if truncation
 * occurs. If the URL is already shorter than or equal to the specified length,
 * returns a copy of the original URL.
 *
 * @param url The URL to shorten. Can be NULL.
 * @param length Maximum length before truncation. Must be >= 0.
 * @return A newly allocated string with the shortened URL, or NULL if input is NULL
 *         or memory allocation fails.
 * 
 * @warning Caller must free the returned string using free()
 * @note The returned string length will be (length + 3) if truncation occurs
 * 
 * @example
 *   char* short_url = shorten_url("https://example.com/very/long/path", 20);
 *   if (short_url != NULL) {
 *       printf("%s\n", short_url);  // Prints: https://example.com/...
 *       free(short_url);
 *   }
 */
char* shorten_url(const char* url, int length);

#ifdef __cplusplus
}
#endif

#endif /* URL_TOOLS_H */
