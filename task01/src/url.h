#ifndef URL_H
#define URL_H

/**
 * Manage URLs based on the specified action.
 *
 * @param urls Array of URL strings to manage.
 * @param urlCount Number of URLs in the array.
 * @param action Action to perform on the URLs. Options are 'checkValid', 'shorten', 'format'.
 * @param results Array to store the results (must be pre-allocated).
 * @return 0 on success, -1 on error.
 */
int manageUrls(char** urls, int urlCount, const char* action, char** results);

#endif /* URL_H */

