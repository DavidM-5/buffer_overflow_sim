#pragma once

#include "user_types.h"

int DataBase_init();
void DataBase_close();

int DataBase_execQuery(const char *query, int (*callback)(void *, int, char **, char **), void *data, char **err_msg);

// Helper function to convert error code to string
const char* getUserValidationError(UserValidationError error);
