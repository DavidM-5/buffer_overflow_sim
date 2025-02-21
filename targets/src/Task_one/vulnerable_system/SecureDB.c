#include <stdio.h>
#include <string.h>
#include "sqlite3.h"
#include "aes_utils.h"
#include "user_types.h"


typedef struct {
    bool is_manager;
    bool is_active;
    char username[USERNAME_MAX_LENGHT*2];
    char password[PASSWORD_MAX_LENGHT*2];
    Error_code error_code;
} EncryptedUserData;




static sqlite3 *DB;



// Helper functions declarations
static int execQuery(const char *query, int (*callback)(void *, int, char **, char **), void *data, char **err_msg);

static int isUserValid(UserData* user);
static int isUserExists(void *data, int argc, char **argv, char **azColName);

static int printDecryptedDB(void *data, int argc, char **argv, char **azColName);



int SecureDB_init() {
    // Open database
    if (sqlite3_open("database/SystemDB.db", &DB)) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(DB));
        return 1;
    }

    return 0;
}
void SecureDB_close() {
    if (!DB) {
        return;
    }
    
    // Close database
    sqlite3_close(DB);
}

int SecureDB_addNewUser(UserData* user) {
    if (!DB)
        return 2;

    if (!user)
        return 0;


    // Check if user name and password are valid
    isUserValid(user);
    if (user->error_code != SUCCESS) {
        return 1;
    }


    EncryptedUserData userToAdd = {
                                    .username = {0},
                                    .password = {0},
                                    .is_manager = user->is_manager,
                                    .is_active = user->is_active,
                                    .error_code = SUCCESS
                                };

    
    // Encrypt username
    strncpy((char*)userToAdd.username, user->username, USERNAME_MAX_LENGHT - 1);
    Encrypt((unsigned char*)userToAdd.username, (unsigned char*)KEY);
    
    // Encrypt password
    strncpy((char*)userToAdd.password, user->password, PASSWORD_MAX_LENGHT - 1);
    Encrypt((unsigned char*)userToAdd.password, (unsigned char*)KEY);


    // Check if username already taken
    execQuery("SELECT * FROM users;", isUserExists, &userToAdd, NULL);
    
    if (userToAdd.error_code != SUCCESS) {
        user->error_code = userToAdd.error_code;
        return 1;
    }


    // Create the SQL INSERT statement
    char query[1024];
    snprintf(query, sizeof(query), 
        "INSERT INTO users (is_manager, is_active, username, password) "
        "VALUES (%d, %d, '%s', '%s');", 
        userToAdd.is_manager, userToAdd.is_active, userToAdd.username, userToAdd.password);

    execQuery(query, NULL, NULL, NULL);


    user->error_code = SUCCESS;
    return 0;
}
int SecureDB_removeUser(const char* username) {
    if (!DB)
        return 2;
    
    if (!username)
        return 1;


    // Encrypt username
    char userNameToRemove[USERNAME_MAX_LENGHT*2] = {0};
    strncpy((char*)userNameToRemove, username, USERNAME_MAX_LENGHT - 1);
    Encrypt((unsigned char*)userNameToRemove, (unsigned char*)KEY);
    

    char condition[512];
    snprintf(condition, sizeof(condition), "username = '%s'", userNameToRemove);

    char command[1024];
    snprintf(command, sizeof(command), "DELETE FROM users WHERE %s;", condition);

    if (execQuery(command, NULL, NULL, NULL) != 0) {
        return 1;
    }
    
    return 0;
}

int SecureDB_makeManager(const char* username) {
    if (!DB) 
        return 2;
    
    if (!username)
        return 1;

    
    // Encrypt username
    char userNameToMakeMangr[USERNAME_MAX_LENGHT*2] = {0};
    strncpy((char*)userNameToMakeMangr, username, USERNAME_MAX_LENGHT - 1);
    Encrypt((unsigned char*)userNameToMakeMangr, (unsigned char*)KEY);
    

    char condition[512];
    snprintf(condition, sizeof(condition), "username = '%s'", userNameToMakeMangr);

    char query[1024];
    snprintf(query, sizeof(query), "UPDATE users SET is_manager = 1 WHERE %s;", condition);
    
    if (execQuery(query, NULL, NULL, NULL) != 0) {
        return 1;
    }

    return 0;
}
int SecureDB_removeManager(const char* username) {
    if (!DB) 
        return 2;
    
    if (!username)
        return 1;

    
    // Encrypt username
    char userNameToMakeMangr[USERNAME_MAX_LENGHT*2] = {0};
    strncpy((char*)userNameToMakeMangr, username, USERNAME_MAX_LENGHT - 1);
    Encrypt((unsigned char*)userNameToMakeMangr, (unsigned char*)KEY);
    

    char condition[512];
    snprintf(condition, sizeof(condition), "username = '%s'", userNameToMakeMangr);

    char query[1024];
    snprintf(query, sizeof(query), "UPDATE users SET is_manager = 0 WHERE %s;", condition);

    if (execQuery(query, NULL, NULL, NULL) != 0) {
        return 1;
    }

    return 0;
}

int SecureDB_activateUser(const char* username) {
    if (!DB) 
        return 2;
    
    if (!username)
        return 1;

    
    // Encrypt username
    char userNameToMakeMangr[USERNAME_MAX_LENGHT*2] = {0};
    strncpy((char*)userNameToMakeMangr, username, USERNAME_MAX_LENGHT - 1);
    Encrypt((unsigned char*)userNameToMakeMangr, (unsigned char*)KEY);
    

    char condition[512];
    snprintf(condition, sizeof(condition), "username = '%s'", userNameToMakeMangr);

    char query[1024];
    snprintf(query, sizeof(query), "UPDATE users SET is_active = 1 WHERE %s;", condition);

    if (execQuery(query, NULL, NULL, NULL) != 0) {
        return 1;
    }

    return 0;
}
int SecureDB_deactivateUser(const char* username) {
    if (!DB) 
        return 2;
    
    if (!username)
        return 1;

    
    // Encrypt username
    char userNameToMakeMangr[USERNAME_MAX_LENGHT*2] = {0};
    strncpy((char*)userNameToMakeMangr, username, USERNAME_MAX_LENGHT - 1);
    Encrypt((unsigned char*)userNameToMakeMangr, (unsigned char*)KEY);
    

    char condition[512];
    snprintf(condition, sizeof(condition), "username = '%s'", userNameToMakeMangr);

    char query[1024];
    snprintf(query, sizeof(query), "UPDATE users SET is_active = 0 WHERE %s;", condition);

    if (execQuery(query, NULL, NULL, NULL) != 0) {
        return 1;
    }

    return 0;
}

void SecureDB_getUser(const char* username, UserData* dstUser) {
    if (!DB || !username || !dstUser) {
        return;
    }


    // Encrypt username for database lookup
    char encryptedUserNameForLookup[USERNAME_MAX_LENGHT * 2] = { 0 };
    strncpy((char*)encryptedUserNameForLookup, username, USERNAME_MAX_LENGHT - 1);
    Encrypt((unsigned char*)encryptedUserNameForLookup, (unsigned char*)KEY);

    // Prepare the query with the encrypted username
    char query[1024];
    snprintf(query, sizeof(query), 
        "SELECT is_manager, is_active, username, password FROM users WHERE username = '%s';", 
        encryptedUserNameForLookup);


    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(DB, query, -1, &stmt, NULL) != SQLITE_OK) {
        // fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        dstUser->error_code = UNKNOWN_ERROR;
        return;
    }

    // Execute the query
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        // Retrieve data from the row
        dstUser->is_manager = sqlite3_column_int(stmt, 0);
        dstUser->is_active = sqlite3_column_int(stmt, 1);


        // Copy encrypted username and password
        const unsigned char *encryptedUsername = sqlite3_column_text(stmt, 2);
        const unsigned char *encryptedPassword = sqlite3_column_text(stmt, 3);


        char decryptedPassword[PASSWORD_MAX_LENGHT] = { 0 };
        strncpy((char*)decryptedPassword, encryptedPassword, PASSWORD_MAX_LENGHT - 1);
        Decrypt((unsigned char*)decryptedPassword, (unsigned char*)KEY);


        // Copy decrypted username and password to destination user
        strcpy(dstUser->username, username);
        memcpy(dstUser->password, decryptedPassword, PASSWORD_MAX_LENGHT);

        dstUser->error_code = SUCCESS;
    }
    else {
        // No user found
        // fprintf(stderr, "No user found with username: %s\n", username);
        dstUser->error_code = USER_NOT_FOUND;
    }

    // Finalize the statement
    sqlite3_finalize(stmt);
}

void SecureDB_printDB() {
    execQuery("SELECT * FROM users;", printDecryptedDB, NULL, NULL);
}


// Helper functions definitions

static int execQuery(const char *query, int (*callback)(void *, int, char **, char **), void *data, char **err_msg) {
    if (!query) {
        if (err_msg)
            *err_msg = "Invalid query: query is NULL or empty";

        return 1;
    }

    if (sqlite3_exec(DB, query, callback, data, err_msg) != SQLITE_OK) {
        return 1;
    }
    
    if (err_msg)
        *err_msg = "Query executed successfully";

    return 0;
}

static int isUserValid(UserData* user) {
    if (!user->username) {
        user->error_code = USERNAME_NULL;
        return 0;
    }
    
    if (!user->password) {
        user->error_code = PASSWORD_NULL;
        return 0;
    }

    if(strlen(user->username) < 3) {
        user->error_code = USERNAME_TOO_SHORT;
        return 0;
    }

    if(strlen(user->username) >= USERNAME_MAX_LENGHT) {
        user->error_code = USERNAME_TOO_LONG;
        return 0;
    }

    if(strlen(user->password) < 3) {
        user->error_code = PASSWORD_TOO_SHORT;
        return 0;
    }

    if(strlen(user->password) >= PASSWORD_MAX_LENGHT) {
        user->error_code = PASSWORD_TOO_LONG;
        return 0;
    }

    user->error_code = SUCCESS;
    return 0;
}

static int isUserExists(void *data, int argc, char **argv, char **azColName) {
    EncryptedUserData *cbd = (EncryptedUserData*)data;

    // Check if username already exists
    for(int i = 0; i < argc; i++) {
        if(strcmp(azColName[i], "username") == 0 && 
           argv[i] != NULL && 
           strcmp(argv[i], cbd->username) == 0) {
            cbd->error_code = USER_EXISTS;
            return 1;
        }
    }

    cbd->error_code = SUCCESS;
    return 0;
}

static int printDecryptedDB(void *data, int argc, char **argv, char **azColName) {
    EncryptedUserData encryptedUser;

    for(int i = 0; i < argc; i++) {
        // Check the column names to assign the values to the corresponding fields
        if (strcmp(azColName[i], "username") == 0) {
            strncpy(encryptedUser.username, argv[i], USERNAME_MAX_LENGHT * 2);
        }
        else if (strcmp(azColName[i], "password") == 0) {
            strncpy(encryptedUser.password, argv[i], PASSWORD_MAX_LENGHT * 2);
        }
        else if (strcmp(azColName[i], "is_manager") == 0) {
            encryptedUser.is_manager = (strcmp(argv[i], "1") == 0);
        }
        else if (strcmp(azColName[i], "is_active") == 0) {
            encryptedUser.is_active = (strcmp(argv[i], "1") == 0);
        }
    }

    // Decrypt user data
    char decryptedUserName[USERNAME_MAX_LENGHT] = { 0 };
    strncpy((char*)decryptedUserName, encryptedUser.username, USERNAME_MAX_LENGHT - 1);
    Decrypt((unsigned char*)decryptedUserName, (unsigned char*)KEY);
    
    char decryptedPassword[PASSWORD_MAX_LENGHT] = { 0 };
    strncpy((char*)decryptedPassword, encryptedUser.password, PASSWORD_MAX_LENGHT - 1);
    Decrypt((unsigned char*)decryptedPassword, (unsigned char*)KEY);


    printf("| %-7s || %-10s || %-16s || %-16s |\n", 
            encryptedUser.is_manager ? "manager" : "user", 
            encryptedUser.is_active ? "active" : "not active", 
            decryptedUserName, 
            decryptedPassword);
    
    printf("____________________________________________________________________\n");

    return 0;
}
