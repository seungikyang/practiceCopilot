/**
 * @file url_tools.c
 * @brief Implementation of URL manipulation and validation utilities
 * 
 * Provides core functionality for validating, formatting, and shortening URLs.
 * All functions follow defensive programming practices with NULL checking.
 * 
 * @see url_tools.h for public API documentation
 */

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "url_tools.h"

/* Protocol constants */
#define HTTP_PREFIX "http://"
#define HTTPS_PREFIX "https://"
#define HTTP_PREFIX_LEN 7
#define HTTPS_PREFIX_LEN 8
#define ELLIPSIS "..."
#define ELLIPSIS_LEN 3

/**
 * @brief Checks if the given string is a valid URL.
 * 
 * @param url The URL string to validate (can be NULL)
 * @return 1 if valid, 0 if invalid or NULL
 */
int is_valid_url(const char* url) {
    // Validate input parameter
    if (url == NULL) {
        return 0;
    }
    
    // Check for http:// or https:// prefix
    const char* afterProtocol = NULL;
    if (strncmp(url, HTTP_PREFIX, HTTP_PREFIX_LEN) == 0) {
        afterProtocol = url + HTTP_PREFIX_LEN;
    } else if (strncmp(url, HTTPS_PREFIX, HTTPS_PREFIX_LEN) == 0) {
        afterProtocol = url + HTTPS_PREFIX_LEN;
    } else {
        // No valid protocol found
        return 0;
    }
    
    // Validate that there's content after the protocol
    if (*afterProtocol == '\0') {
        return 0;
    }
    
    // Validate that domain contains at least one dot
    return (strchr(afterProtocol, '.') != NULL) ? 1 : 0;
}

/**
 * @brief Formats a URL by ensuring it has a protocol prefix.
 * 
 * @param url The URL to format (can be NULL)
 * @return Newly allocated formatted string, or NULL on error
 */
char* format_url(const char* url) {
    // Validate input parameter
    if (url == NULL) {
        return NULL;
    }
    
    char* formatted = NULL;
    
    // Check if URL already has a protocol
    if (strncmp(url, HTTP_PREFIX, HTTP_PREFIX_LEN) == 0 || 
        strncmp(url, HTTPS_PREFIX, HTTPS_PREFIX_LEN) == 0) {
        // URL already has protocol, duplicate it
        formatted = strdup(url);
    } else {
        // Add https:// prefix
        size_t url_len = strlen(url);
        // Allocate: HTTPS_PREFIX_LEN + url_len + 1 for null terminator
        formatted = (char*)malloc(HTTPS_PREFIX_LEN + url_len + 1);
        if (formatted != NULL) {
            strcpy(formatted, HTTPS_PREFIX);
            strcat(formatted, url);
        }
    }
    
    return formatted;
}

/**
 * @brief Shortens a URL to the specified length.
 * 
 * @param url The URL to shorten (can be NULL)
 * @param length Maximum length before truncation
 * @return Newly allocated shortened string, or NULL on error
 */
char* shorten_url(const char* url, int length) {
    // Validate input parameters
    if (url == NULL) {
        return NULL;
    }
    
    // Handle negative length
    if (length < 0) {
        length = 0;
    }
    
    const int url_len = (int)strlen(url);
    char* shortened = NULL;
    
    if (url_len > length) {
        // URL needs truncation
        // Allocate: length + ELLIPSIS_LEN + 1 for null terminator
        shortened = (char*)malloc(length + ELLIPSIS_LEN + 1);
        if (shortened != NULL) {
            // Copy first 'length' characters
            strncpy(shortened, url, length);
            shortened[length] = '\0';  // Ensure null termination
            // Append ellipsis
            strcat(shortened, ELLIPSIS);
        }
    } else {
        // URL is short enough, duplicate it
        shortened = strdup(url);
    }
    
    return shortened;
}
