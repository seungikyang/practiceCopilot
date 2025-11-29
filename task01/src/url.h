/**
 * @file url.h
 * @brief High-level URL management interface
 * 
 * Provides batch URL processing capabilities using url_tools functions.
 * This module serves as a facade for url_tools, enabling batch operations.
 * 
 * @note All array parameters must be properly allocated by caller
 * @see url_tools.h for underlying URL manipulation functions
 * @see .github/COPILOT_INSTRUCTIONS.md for coding guidelines
 */

#ifndef URL_H
#define URL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Manages multiple URLs based on the specified action.
 * 
 * Performs batch operations on an array of URLs. Supported actions:
 * - "checkValid": Validates each URL, returns "1" or "0" as string
 * - "format": Adds protocol prefix if missing
 * - "shorten": Truncates URLs to 30 characters with "..." suffix
 *
 * @param urls Array of URL strings to process. Individual URLs can be NULL.
 *             Must not be NULL if urlCount > 0.
 * @param urlCount Number of URLs in the array. Must be > 0.
 * @param action Action to perform. Must be one of: "checkValid", "format", "shorten".
 *               Case-sensitive. Must not be NULL.
 * @param results Pre-allocated array to store results. Must have space for urlCount entries.
 *                Must not be NULL. Caller is responsible for freeing each non-NULL result.
 * 
 * @return 0 on success, -1 on error
 * @retval 0 Success - all URLs processed
 * @retval -1 Error - invalid parameters or unknown action
 * 
 * @warning Caller must free each non-NULL entry in results array using free()
 * @note NULL URLs in input array will result in NULL in results array
 * @note For "checkValid" action, results are "1" (valid) or "0" (invalid) strings
 * @note Memory allocation failures for individual URLs will result in NULL entries
 * 
 * @example
 *   char* urls[] = {"example.com", "https://test.org"};
 *   char* results[2] = {NULL, NULL};
 *   
 *   if (manageUrls(urls, 2, "format", results) == 0) {
 *       // Process results
 *       for (int i = 0; i < 2; i++) {
 *           if (results[i] != NULL) {
 *               printf("%s\n", results[i]);
 *               free(results[i]);
 *           }
 *       }
 *   }
 */
int manageUrls(char** urls, int urlCount, const char* action, char** results);

#ifdef __cplusplus
}
#endif

#endif /* URL_H */

