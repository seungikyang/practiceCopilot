#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "url_tools.h"

int is_valid_url(const char* url) {
    if (url == NULL) {
        return 0;
    }
    
    // Check for http:// or https:// prefix
    if (strncmp(url, "http://", 7) != 0 && strncmp(url, "https://", 8) != 0) {
        return 0;
    }
    
    // Simple validation - has at least one character after the protocol
    // and contains a dot
    const char* afterProtocol = url;
    if (strncmp(url, "http://", 7) == 0) {
        afterProtocol = url + 7;
    } else {
        afterProtocol = url + 8;
    }
    
    if (*afterProtocol == '\0') {
        return 0;
    }
    
    return (strchr(afterProtocol, '.') != NULL);
}

char* format_url(const char* url) {
    if (url == NULL) {
        return NULL;
    }
    
    char* formatted;
    if (strncmp(url, "http://", 7) != 0 && strncmp(url, "https://", 8) != 0) {
        formatted = (char*) malloc(strlen(url) + 9); // 8 for "https://" + 1 for null terminator
        if (formatted != NULL) {
            strcpy(formatted, "https://");
            strcat(formatted, url);
        }
    } else {
        formatted = strdup(url);
    }
    
    return formatted;
}

char* shorten_url(const char* url, int length) {
    if (url == NULL) {
        return NULL;
    }
    
    int urlLen = strlen(url);
    char* shortened;
    
    if (urlLen > length) {
        shortened = (char*) malloc(length + 4); // Length + "..." + null terminator
        if (shortened != NULL) {
            strncpy(shortened, url, length);
            shortened[length] = '\0';
            strcat(shortened, "...");
        }
    } else {
        shortened = strdup(url);
    }
    
    return shortened;
}
