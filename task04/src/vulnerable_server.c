#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>

// 환경변수 또는 설정 파일에서 자격증명을 읽어오는 함수
typedef struct {
    char server[256];
    char user[64];
    char password[256];
    char database[64];
} DatabaseConfig;

static int load_database_config(DatabaseConfig *config) {
    // 환경변수에서 읽기 (실제 운영 환경에서는 암호화된 설정 파일 사용)
    const char *server = getenv("DB_SERVER");
    const char *user = getenv("DB_USER");
    const char *password = getenv("DB_PASSWORD");
    const char *database = getenv("DB_NAME");
    
    if (!server || !user || !password || !database) {
        fprintf(stderr, "Error: Database credentials not found in environment variables\n");
        fprintf(stderr, "Please set: DB_SERVER, DB_USER, DB_PASSWORD, DB_NAME\n");
        return -1;
    }
    
    strncpy(config->server, server, sizeof(config->server) - 1);
    strncpy(config->user, user, sizeof(config->user) - 1);
    strncpy(config->password, password, sizeof(config->password) - 1);
    strncpy(config->database, database, sizeof(config->database) - 1);
    
    config->server[sizeof(config->server) - 1] = '\0';
    config->user[sizeof(config->user) - 1] = '\0';
    config->password[sizeof(config->password) - 1] = '\0';
    config->database[sizeof(config->database) - 1] = '\0';
    
    return 0;
}

// 입력 검증 함수
static int validate_search_input(const char *search) {
    if (search == NULL) {
        return -1;
    }
    
    size_t len = strlen(search);
    if (len == 0 || len > 100) {
        fprintf(stderr, "Error: Invalid search length (must be 1-100 characters)\n");
        return -1;
    }
    
    // 위험한 문자 검사 (추가 보안)
    for (size_t i = 0; i < len; i++) {
        char c = search[i];
        // 알파벳, 숫자, 공백, 하이픈, 언더스코어만 허용
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || 
              (c >= '0' && c <= '9') || c == ' ' || c == '-' || c == '_')) {
            fprintf(stderr, "Error: Invalid characters in search term\n");
            return -1;
        }
    }
    
    return 0;
}

// 안전한 데이터베이스 연결 함수
static MYSQL* connect_database(const DatabaseConfig *config) {
    MYSQL *conn = mysql_init(NULL);
    if (conn == NULL) {
        fprintf(stderr, "Error: mysql_init() failed\n");
        return NULL;
    }
    
    // SSL/TLS 연결 활성화 (선택사항)
    // mysql_ssl_set(conn, NULL, NULL, NULL, NULL, NULL);
    
    if (!mysql_real_connect(conn, config->server, config->user, 
                           config->password, config->database, 0, NULL, 0)) {
        fprintf(stderr, "Error: Connection failed: %s\n", mysql_error(conn));
        mysql_close(conn);
        return NULL;
    }
    
    return conn;
}

// 🔒 SQL Injection 방지: Prepared Statement 사용
int query_database(const char *search) {
    DatabaseConfig config;
    MYSQL *conn = NULL;
    MYSQL_STMT *stmt = NULL;
    MYSQL_BIND bind_param[1];
    MYSQL_BIND bind_result[1];
    MYSQL_RES *res_meta = NULL;
    char result_name[256];
    unsigned long length;
    my_bool is_null;
    my_bool error;
    int ret = -1;
    
    // 입력 검증
    if (validate_search_input(search) != 0) {
        return -1;
    }
    
    // 설정 로드
    if (load_database_config(&config) != 0) {
        return -1;
    }
    
    // 데이터베이스 연결
    conn = connect_database(&config);
    if (conn == NULL) {
        goto cleanup;
    }
    
    // Prepared Statement 생성 (SQL Injection 방지)
    const char *query = "SELECT name FROM users WHERE name = ? LIMIT 10";
    stmt = mysql_stmt_init(conn);
    if (stmt == NULL) {
        fprintf(stderr, "Error: mysql_stmt_init() failed\n");
        goto cleanup;
    }
    
    if (mysql_stmt_prepare(stmt, query, strlen(query)) != 0) {
        fprintf(stderr, "Error: mysql_stmt_prepare() failed: %s\n", 
                mysql_stmt_error(stmt));
        goto cleanup;
    }
    
    // 파라미터 바인딩
    memset(bind_param, 0, sizeof(bind_param));
    bind_param[0].buffer_type = MYSQL_TYPE_STRING;
    bind_param[0].buffer = (char *)search;
    bind_param[0].buffer_length = strlen(search);
    bind_param[0].is_null = 0;
    bind_param[0].length = NULL;
    
    if (mysql_stmt_bind_param(stmt, bind_param) != 0) {
        fprintf(stderr, "Error: mysql_stmt_bind_param() failed: %s\n", 
                mysql_stmt_error(stmt));
        goto cleanup;
    }
    
    // 쿼리 실행
    if (mysql_stmt_execute(stmt) != 0) {
        fprintf(stderr, "Error: mysql_stmt_execute() failed: %s\n", 
                mysql_stmt_error(stmt));
        goto cleanup;
    }
    
    // 결과 바인딩
    memset(bind_result, 0, sizeof(bind_result));
    bind_result[0].buffer_type = MYSQL_TYPE_STRING;
    bind_result[0].buffer = result_name;
    bind_result[0].buffer_length = sizeof(result_name);
    bind_result[0].is_null = &is_null;
    bind_result[0].length = &length;
    bind_result[0].error = &error;
    
    if (mysql_stmt_bind_result(stmt, bind_result) != 0) {
        fprintf(stderr, "Error: mysql_stmt_bind_result() failed: %s\n", 
                mysql_stmt_error(stmt));
        goto cleanup;
    }
    
    // 결과 가져오기
    printf("\n=== Search Results ===\n");
    int count = 0;
    while (mysql_stmt_fetch(stmt) == 0) {
        if (!is_null) {
            printf("User: %.*s\n", (int)length, result_name);
            count++;
        }
    }
    
    if (count == 0) {
        printf("No users found matching '%s'\n", search);
    } else {
        printf("Total: %d user(s) found\n", count);
    }
    
    ret = 0; // 성공
    
cleanup:
    // 리소스 정리
    if (stmt != NULL) {
        mysql_stmt_close(stmt);
    }
    if (conn != NULL) {
        mysql_close(conn);
    }
    
    // 메모리에서 민감한 정보 제거
    memset(&config, 0, sizeof(config));
    
    return ret;
}

int main() {
    char search[101]; // 버퍼 크기 증가
    
    printf("=== Secure User Search System ===\n");
    printf("Enter name to search (alphanumeric, space, -, _ only): ");
    
    // fgets 사용으로 버퍼 오버플로우 방지
    if (fgets(search, sizeof(search), stdin) == NULL) {
        fprintf(stderr, "Error: Failed to read input\n");
        return 1;
    }
    
    // 개행문자 제거
    size_t len = strlen(search);
    if (len > 0 && search[len - 1] == '\n') {
        search[len - 1] = '\0';
    }
    
    // 데이터베이스 쿼리 실행
    if (query_database(search) != 0) {
        fprintf(stderr, "Error: Database query failed\n");
        return 1;
    }
    
    return 0;
}