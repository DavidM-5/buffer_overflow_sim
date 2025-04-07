#pragma once

#include "user_types.h"


int SecureDB_init();
void SecureDB_close();

int SecureDB_addNewUser(UserData* user);
int SecureDB_removeUser(const char* username);

int SecureDB_makeManager(const char* username);
int SecureDB_removeManager(const char* username);

int SecureDB_activateUser(const char* username);
int SecureDB_deactivateUser(const char* username);

void SecureDB_getUser(const char* username, UserData* dstUser);

void SecureDB_printDB();
