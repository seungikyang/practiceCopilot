#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "date_utils.h"

void get_date_string(char *date_str, size_t size, int days_to_add) {
    if (date_str == NULL || size == 0) {
        fprintf(stderr, "Invalid parameters for get_date_string\n");
        return;
    }
    
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    
    if (t == NULL) {
        fprintf(stderr, "Failed to get local time\n");
        return;
    }
    
    // 지정된 일수를 더함
    t->tm_mday += days_to_add;
    mktime(t); // 날짜 정규화 (예: 2월 30일 -> 3월 2일)
    
    strftime(date_str, size, "%Y-%m-%d", t);
}

int date_difference(const char *date1, const char *date2) {
    if (date1 == NULL || date2 == NULL) {
        fprintf(stderr, "Invalid parameters for date_difference\n");
        return 0;
    }
    
    struct tm tm1 = {0}, tm2 = {0};
    
    // 날짜 문자열 파싱
    if (sscanf(date1, "%d-%d-%d", &tm1.tm_year, &tm1.tm_mon, &tm1.tm_mday) != 3) {
        fprintf(stderr, "Invalid date format for date1: %s\n", date1);
        return 0;
    }
    
    if (sscanf(date2, "%d-%d-%d", &tm2.tm_year, &tm2.tm_mon, &tm2.tm_mday) != 3) {
        fprintf(stderr, "Invalid date format for date2: %s\n", date2);
        return 0;
    }
    
    // 년도와 월 보정
    tm1.tm_year -= 1900;
    tm1.tm_mon -= 1;
    tm2.tm_year -= 1900;
    tm2.tm_mon -= 1;
    
    // time_t로 변환
    time_t t1 = mktime(&tm1);
    time_t t2 = mktime(&tm2);
    
    if (t1 == -1 || t2 == -1) {
        fprintf(stderr, "Failed to convert date to time_t\n");
        return 0;
    }
    
    // 일 단위 차이 계산
    return (int)difftime(t2, t1) / (60 * 60 * 24);
}

int is_valid_date_string(const char *date_str) {
    if (date_str == NULL) {
        return 0;
    }
    
    int year, month, day;
    if (sscanf(date_str, "%d-%d-%d", &year, &month, &day) != 3) {
        return 0;
    }
    
    // 기본 범위 검증
    if (year < 1900 || year > 3000) {
        return 0;
    }
    
    if (month < 1 || month > 12) {
        return 0;
    }
    
    if (day < 1 || day > 31) {
        return 0;
    }
    
    // 더 정확한 날짜 검증은 mktime을 사용
    struct tm tm = {0};
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    
    time_t t = mktime(&tm);
    if (t == -1) {
        return 0;
    }
    
    // 정규화된 날짜가 입력과 동일한지 확인
    struct tm *normalized = localtime(&t);
    if (normalized->tm_year + 1900 != year ||
        normalized->tm_mon + 1 != month ||
        normalized->tm_mday != day) {
        return 0;
    }
    
    return 1;
}

void get_current_date_string(char *date_str, size_t size) {
    get_date_string(date_str, size, 0);
}
