/**
 * @file url.c
 * @brief Implementation of high-level URL management interface
 * 
 * This module provides batch URL processing by utilizing url_tools functions.
 * Acts as a facade for bulk operations on URL arrays.
 * 
 * @see url.h for public API documentation
 * @see url_tools.h for underlying URL manipulation functions
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "url.h"
#include "url_tools.h"

/* Action string constants */
static const char* ACTION_CHECK_VALID = "checkValid";
static const char* ACTION_FORMAT = "format";
static const char* ACTION_SHORTEN = "shorten";

/* Default shorten length */
static const int DEFAULT_SHORTEN_LENGTH = 30;

/**
 * @brief Manages multiple URLs based on the specified action.
 * 
 * @param urls Array of URL strings (can contain NULL entries)
 * @param urlCount Number of URLs in the array
 * @param action Action to perform (must be valid action string)
 * @param results Pre-allocated array to store results
 * @return 0 on success, -1 on error
 */
int manageUrls(char** urls, int urlCount, const char* action, char** results) {
    // Validate input parameters
    if (urls == NULL || action == NULL || results == NULL) {
        return -1;
    }

    if (urlCount <= 0) {
        return -1;
    }

    // Process each URL based on the action
    for (int i = 0; i < urlCount; i++) {
        // Handle NULL URL entries
        if (urls[i] == NULL) {
            results[i] = NULL;
            continue;
        }

        // Perform action based on type
        if (strcmp(action, ACTION_CHECK_VALID) == 0) {
            // Validate URL and store result as "1" or "0" string
            const int isValid = is_valid_url(urls[i]);
            
            // Allocate 2 bytes: 1 for digit, 1 for null terminator
            results[i] = (char*)malloc(2);
            if (results[i] != NULL) {
                results[i][0] = isValid ? '1' : '0';
                results[i][1] = '\0';
            }
        } 
        else if (strcmp(action, ACTION_FORMAT) == 0) {
            // Format the URL (add protocol if missing)
            results[i] = format_url(urls[i]);
        }
        else if (strcmp(action, ACTION_SHORTEN) == 0) {
            // Shorten URL to default length
            results[i] = shorten_url(urls[i], DEFAULT_SHORTEN_LENGTH);
        }
        else {
            // Unknown action - return error
            return -1;
        }
    }

    return 0;
}
