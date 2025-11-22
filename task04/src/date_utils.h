#ifndef DATE_UTILS_H
#define DATE_UTILS_H

#include <time.h>
#include <stddef.h>

/**
 * @brief 날짜 문자열을 생성한다 (YYYY-MM-DD 형식)
 * 
 * @param date_str 날짜 문자열을 저장할 버퍼
 * @param size 버퍼 크기
 * @param days_to_add 현재 날짜에 추가할 일수
 */
void get_date_string(char *date_str, size_t size, int days_to_add);

/**
 * @brief 두 날짜 사이의 일수 차이를 계산한다.
 * 
 * @param date1 첫 번째 날짜 (YYYY-MM-DD 형식)
 * @param date2 두 번째 날짜 (YYYY-MM-DD 형식)
 * @return int 날짜 차이 (date2 - date1)
 */
int date_difference(const char *date1, const char *date2);

/**
 * @brief 날짜 문자열의 유효성을 검증한다.
 * 
 * @param date_str 검증할 날짜 문자열 (YYYY-MM-DD 형식)
 * @return int 유효하면 1, 유효하지 않으면 0 반환
 */
int is_valid_date_string(const char *date_str);

/**
 * @brief 현재 날짜를 문자열로 반환한다.
 * 
 * @param date_str 날짜 문자열을 저장할 버퍼
 * @param size 버퍼 크기
 */
void get_current_date_string(char *date_str, size_t size);

#endif /* DATE_UTILS_H */
