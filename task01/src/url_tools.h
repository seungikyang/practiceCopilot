#ifndef URL_TOOLS_H
#define URL_TOOLS_H

/**
 * Checks if the given string matches a URL pattern.
 *
 * @param url The URL string to check.
 * @return 1 if valid, 0 if invalid.
 */
int is_valid_url(const char* url);

/**
 * Ensures the URL starts with 'https://'.
 *
 * @param url The URL to format.
 * @return A newly allocated string with the formatted URL. Caller must free it.
 */
char* format_url(const char* url);

/**
 * Truncates the URL to the specified length and appends '...'.
 *
 * @param url The URL to shorten.
 * @param length Maximum length before truncation.
 * @return A newly allocated string with the shortened URL. Caller must free it.
 */
char* shorten_url(const char* url, int length);

#endif /* URL_TOOLS_H */
