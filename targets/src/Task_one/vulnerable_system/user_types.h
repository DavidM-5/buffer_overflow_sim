#pragma once

#include <stdbool.h>

#define USERNAME_MAX_LENGHT 100
#define PASSWORD_MAX_LENGHT 100

typedef enum {
    SUCCESS = 0,
    USER_EXISTS,
    USER_NOT_FOUND,
    USER_NOT_ACTIVE,
    WRONG_PASSWORD,
    NOT_LOGGED_IN,
    USERNAME_TOO_SHORT,
    USERNAME_TOO_LONG,
    PASSWORD_TOO_SHORT,
    PASSWORD_TOO_LONG,
    PASSWORDS_NOT_MATCH,
    INVALID_MANAGER_RIGHTS,
    USERNAME_NULL,
    PASSWORD_NULL, 
    INVALID_PARAMETERS,
    UNKNOWN_ERROR
} Error_code;

typedef struct {
    bool is_manager;
    bool is_active;
    char username[USERNAME_MAX_LENGHT];
    char password[PASSWORD_MAX_LENGHT];
    Error_code error_code;
} UserData;


// Helper function to convert error code to string
const char* errorToString(Error_code err);
