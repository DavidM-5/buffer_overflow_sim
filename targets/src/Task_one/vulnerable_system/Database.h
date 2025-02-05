#pragma once

#include "user_types.h"

int DataBase_init();
void DataBase_close();

int DataBase_addNewUser(UserData* user);
int DataBase_removeUser(UserData* user);

int DataBase_makeManager(UserData* user);
int DataBase_removeManager(UserData* user);

int DataBase_deactivateUser(UserData* user);
int DataBase_activateUser(UserData* user);

void DataBase_getUser(const char* username, UserData* dstUser);

void DataBase_printDatabse();
void DataBase_printDecryptedDatabse();
