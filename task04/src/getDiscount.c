#include <stdio.h>
#include <string.h>

// 고객 타입을 enum으로 정의하여 문자열 비교 제거
typedef enum {
    CUSTOMER_REGULAR = 0,
    CUSTOMER_MEMBER = 1,
    CUSTOMER_VIP = 2,
    CUSTOMER_UNKNOWN = 3
} CustomerType;

// 문자열을 enum으로 변환하는 헬퍼 함수
static inline CustomerType parse_customer_type(const char* customer_type) {
    // 첫 글자 확인으로 빠른 분기
    if (customer_type == NULL) return CUSTOMER_UNKNOWN;
    
    switch (customer_type[0]) {
        case 'r': // regular
            if (strcmp(customer_type, "regular") == 0) return CUSTOMER_REGULAR;
            break;
        case 'm': // member
            if (strcmp(customer_type, "member") == 0) return CUSTOMER_MEMBER;
            break;
        case 'v': // vip
            if (strcmp(customer_type, "vip") == 0) return CUSTOMER_VIP;
            break;
    }
    return CUSTOMER_UNKNOWN;
}

// 룩업 테이블을 사용한 최적화된 버전
float get_discounted_price(float price, const char* customer_type) {
    // 할인율 룩업 테이블 [customer_type][is_over_100]
    static const float discount_table[4][2] = {
        {1.0f, 0.9f},    // REGULAR: 할인없음, 10% 할인
        {0.95f, 0.85f},  // MEMBER:  5% 할인, 15% 할인
        {0.9f, 0.8f},    // VIP:     10% 할인, 20% 할인
        {1.0f, 1.0f}     // UNKNOWN: 할인없음
    };
    
    CustomerType type = parse_customer_type(customer_type);
    int is_over_100 = (price > 100.0f) ? 1 : 0;
    
    return price * discount_table[type][is_over_100];
}

// 레거시 호환성을 위한 대안: 문자열 비교 최소화 버전
float get_discounted_price_v2(float price, const char* customer_type) {
    if (customer_type == NULL) return price;
    
    // 분기 예측 최적화: 가장 빈번한 케이스를 먼저 처리
    char first_char = customer_type[0];
    
    // 단일 문자 비교로 먼저 필터링
    if (first_char == 'r' && customer_type[1] == 'e') { // regular
        return (price > 100.0f) ? price * 0.9f : price;
    }
    
    if (first_char == 'm' && customer_type[1] == 'e') { // member
        return (price > 100.0f) ? price * 0.85f : price * 0.95f;
    }
    
    if (first_char == 'v' && customer_type[1] == 'i') { // vip
        return (price > 100.0f) ? price * 0.8f : price * 0.9f;
    }
    
    return price;
}



