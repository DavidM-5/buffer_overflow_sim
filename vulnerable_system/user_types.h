#pragma once

#include <stdbool.h>


typedef enum {
    USER_SUCCESS = 0,
    USER_EXISTS,
    USERNAME_TOO_SHORT,
    USERNAME_TOO_LONG,
    PASSWORD_TOO_SHORT,
    PASSWORD_TOO_LONG,
    INVALID_MANAGER_RIGHTS,
    USERNAME_NULL,
    PASSWORD_NULL,
    INVALID_PARAMETERS
} UserValidationError;

typedef struct {
    bool is_manager;
    bool is_active;
    char username[128];
    char password[128];
    UserValidationError error_code;
} UserData;
