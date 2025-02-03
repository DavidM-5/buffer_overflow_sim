#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include "aes_utils.h"
#include "user_types.h"


static sqlite3 *db;


static void encryptStringData(const char* input, char* output, size_t outputSize) {
    if (!input || !output) {
        return;
    }

    unsigned char encryptedData[100];
    const char* keyString = "someSuperS3cur3Encrypt1onK3y!";
    unsigned char hashedKey[AES_KEY_LENGTH];

    // Hash the key to derive a fixed-length key
    hashString(keyString, hashedKey);

    int cipherLen = 0;

    // Encrypt the input string
    encryptString(input, hashedKey, encryptedData, &cipherLen);

    // Update the output with encrypted data
    memset(output, 0, outputSize);
    memcpy(output, encryptedData, cipherLen);
}

static void decryptStringData(const char* input, char* output, size_t outputSize) {
    if (!input || !output) {
        return;
    }

    unsigned char decryptedData[USERNAME_MAX_LENGHT + PASSWORD_MAX_LENGHT];
    const char* keyString = "someSuperS3cur3Encrypt1onK3y!";
    unsigned char hashedKey[AES_KEY_LENGTH];

    // Hash the key to derive a fixed-length key
    hashString(keyString, hashedKey);

    int cipherLen = 0;

    // Decrypt the input string
    decryptString(input, strlen((char*)input), hashedKey, decryptedData, &cipherLen);

    // Update the output with Decrypted data
    memset(output, 0, outputSize);
    memcpy(output, decryptedData, cipherLen);
}

static void encryptUserData(UserData* user) {
    if (!user)
        return;

    unsigned char encryptedUsername[128];
    unsigned char encryptedPassword[128];

    const char* keyString = "someSuperS3cur3Encrypt1onK3y!";
    unsigned char hashedKey[AES_KEY_LENGTH];

    // Hash the key to derive a fixed-length key
    hashString(keyString, hashedKey);

    int usernameCipherLen = 0, passwordCipherLen = 0;

    // Encrypt the username and password
    encryptString(user->username, hashedKey, encryptedUsername, &usernameCipherLen);
    encryptString(user->password, hashedKey, encryptedPassword, &passwordCipherLen);

    // Update the user structure with encrypted data
    memset(user->username, 0, strlen(user->username));
    memset(user->password, 0, strlen(user->password));
    memcpy(user->username, encryptedUsername, usernameCipherLen);
    memcpy(user->password, encryptedPassword, passwordCipherLen);
}

static void decryptUserData(UserData* user) {
    if (!user)
        return;

    unsigned char DecryptedUsername[128];
    unsigned char DecryptedPassword[128];

    const char* keyString = "someSuperS3cur3Encrypt1onK3y!";
    unsigned char hashedKey[AES_KEY_LENGTH];

    // Hash the key to derive a fixed-length key
    hashString(keyString, hashedKey);

    int usernameLen = 0, passwordLen = 0;

    // Decrypt the username and password using the decryptString function
    decryptString(user->username, strlen((char*)user->username), hashedKey, DecryptedUsername, &usernameLen);
    decryptString(user->password, strlen((char*)user->password), hashedKey, DecryptedPassword, &passwordLen);

    // Update the user structure with decrypted data
    memset(user->username, 0, strlen(user->username));
    memset(user->password, 0, strlen(user->password));
    memcpy(user->username, DecryptedUsername, usernameLen);
    memcpy(user->password, DecryptedPassword, passwordLen);
}


static int execQuery(const char *query, int (*callback)(void *, int, char **, char **), void *data, char **err_msg) {
    if (!query) {
        if (err_msg)
            *err_msg = "Invalid query: query is NULL or empty";

        return 1;
    }

    if (sqlite3_exec(db, query, callback, data, err_msg) != SQLITE_OK) {
        return 1;
    }
    
    if (err_msg)
        *err_msg = "Query executed successfully";

    return 0;
}

static int printDB(void *data, int argc, char **argv, char **azColName) {
    for(int i = 1; i < argc; i++) {
        printf("| %.*s |", (int)strcspn(argv[i] ? argv[i] : "NULL", "\n"), argv[i] ? argv[i] : "NULL");
    }
    printf("\n\n");
    return 0;
}

static int printDecryptedDb(void *data, int argc, char **argv, char **azColName) {
    UserData usr;

    for(int i = 0; i < argc; i++) {
        // Check the column names to assign the values to the corresponding fields
        if (strcmp(azColName[i], "username") == 0) {
            strncpy(usr.username, argv[i], USERNAME_MAX_LENGHT - 1);
            usr.username[USERNAME_MAX_LENGHT - 1] = '\0';  // Ensure null termination
        }
        else if (strcmp(azColName[i], "password") == 0) {
            strncpy(usr.password, argv[i], PASSWORD_MAX_LENGHT - 1);
            usr.password[PASSWORD_MAX_LENGHT - 1] = '\0';  // Ensure null termination
        }
        else if (strcmp(azColName[i], "is_manager") == 0) {
            usr.is_manager = (strcmp(argv[i], "1") == 0);
        }
        else if (strcmp(azColName[i], "is_active") == 0) {
            usr.is_active = (strcmp(argv[i], "1") == 0);
        }
    }

    decryptUserData(&usr);
    printf("| %d || %d || %s || %s |", usr.is_manager, usr.is_active, usr.username, usr.password);

    printf("\n\n");
    return 0;
}

static int isUserValid(void *data, int argc, char **argv, char **azColName) {
    UserData *cbd = (UserData*)data;

    if (!cbd->username) {
        cbd->error_code = USERNAME_NULL;
        return 0;
    }
    
    if (!cbd->password) {
        cbd->error_code = PASSWORD_NULL;
        return 0;
    }

    if(strlen(cbd->username) < 3) {
        cbd->error_code = USERNAME_TOO_SHORT;
        return 0;
    }

    if(strlen(cbd->username) >= USERNAME_MAX_LENGHT) {
        cbd->error_code = USERNAME_TOO_LONG;
        return 0;
    }

    if(strlen(cbd->password) < 6) {
        cbd->error_code = PASSWORD_TOO_SHORT;
        return 0;
    }

    if(strlen(cbd->password) >= PASSWORD_MAX_LENGHT) {
        cbd->error_code = PASSWORD_TOO_LONG;
        return 0;
    }
    
    UserData tempUser = *cbd;
    encryptUserData(&tempUser);


    // Check if username already exists
    for(int i = 0; i < argc; i++) {
        if(strcmp(azColName[i], "username") == 0 && 
           argv[i] != NULL && 
           strcmp(argv[i], tempUser.username) == 0) {
            cbd->error_code = USER_EXISTS;
            return 0;
        }
    }
    
    cbd->error_code = SUCCESS;
    return 0;
}


int DataBase_makeManager(UserData* user) {
    if (!db) {
        return 2;
    }
    
    UserData usr = *user;
    encryptUserData(&usr);
    
    char condition[150];
    snprintf(condition, sizeof(condition), "username = '%s'", usr.username);

    char query[300];
    snprintf(query, sizeof(query), "UPDATE users SET is_manager = 1 WHERE %s;", condition);

    if (!execQuery(query, NULL, NULL, NULL)) {
        return 1;
    }

    return 0;
}

int DataBase_removeManager(UserData* user) {
    if (!db) {
        return 2;
    }
    
    UserData usr = *user;
    encryptUserData(&usr);
    
    char condition[150];
    snprintf(condition, sizeof(condition), "username = '%s'", usr.username);

    char query[300];
    snprintf(query, sizeof(query), "UPDATE users SET is_manager = 0 WHERE %s;", condition);

    if (!execQuery(query, NULL, NULL, NULL)) {
        return 1;
    }

    return 0;
}

int DataBase_addNewUser(UserData* user) {
    if (!db) {
        return 2;
    }

    if (!user)
        return 0;

    if (strcmp(user->username, "Obamna") == 0)
        return 1;

    execQuery("SELECT * FROM users;", isUserValid, user, NULL);

    if (user->error_code != SUCCESS) {
        return 1;
    }

    UserData userToAdd = *user;

    encryptUserData(&userToAdd);
    char query[512];
    // Create the SQL INSERT statement
    snprintf(query, sizeof(query), 
        "INSERT INTO users (is_manager, is_active, username, password) "
        "VALUES (%d, %d, '%s', '%s');", 
        userToAdd.is_manager, userToAdd.is_active, userToAdd.username, userToAdd.password);

    execQuery(query, NULL, NULL, NULL);

    return 0;
}

int DataBase_removeUser(UserData* user) {
    if (!db) {
        return 2;
    }
    
    if (!user)
        return 1;

    UserData usr = *user;
    encryptUserData(&usr);
    
    char condition[150];
    snprintf(condition, sizeof(condition), "username = '%s'", usr.username);

    char command[300];
    snprintf(command, sizeof(command), "DELETE FROM users WHERE %s;", condition);

    if (!execQuery(command, NULL, NULL, NULL)) {
        return 1;
    }
    
    return 0;
}

int DataBase_deactivateUser(UserData* user) {
    if (!db) {
        return 2;
    }

    UserData usr = *user;
    encryptUserData(&usr);
    
    char condition[150];
    snprintf(condition, sizeof(condition), "username = '%s'", usr.username);

    char query[300];
    snprintf(query, sizeof(query), "UPDATE users SET is_active = 0 WHERE %s;", condition);

    if (!execQuery(query, NULL, NULL, NULL)) {
        return 1;
    }

    return 0;
}

int DataBase_activateUser(UserData* user) {
    if (!db) {
        return 2;
    }
    
    UserData usr = *user;
    encryptUserData(&usr);
    
    char condition[150];
    snprintf(condition, sizeof(condition), "username = '%s'", usr.username);

    char query[300];
    snprintf(query, sizeof(query), "UPDATE users SET is_active = 1 WHERE %s;", condition);

    if (!execQuery(query, NULL, NULL, NULL)) {
        return 1;
    }

    return 0;
}

void DataBase_getUser(const char* username, UserData* dstUser)  {
    if (!db || !username || !dstUser) {
        return;
    }

    // Encrypt the username for database lookup
    char encryptedUsername[USERNAME_MAX_LENGHT];
    encryptStringData(username, encryptedUsername, sizeof(encryptedUsername));

    // Prepare the query with the encrypted username
    char query[256];
    snprintf(query, sizeof(query), 
        "SELECT is_manager, is_active, username, password FROM users WHERE username = '%s';", 
        encryptedUsername);

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) != SQLITE_OK) {
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

        // Reset and copy encrypted data
        memset(dstUser->username, 0, sizeof(dstUser->username));
        memset(dstUser->password, 0, sizeof(dstUser->password));
        
        if (encryptedUsername) {
            strncpy(dstUser->username, (const char*)encryptedUsername, sizeof(dstUser->username) - 1);
        }
        
        if (encryptedPassword) {
            strncpy(dstUser->password, (const char*)encryptedPassword, sizeof(dstUser->password) - 1);
        }

        // Decrypt the user data
        decryptUserData(dstUser);

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

int DataBase_init() {
    // Open database
    if (sqlite3_open("database\\SystemDB.db", &db)) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    return 0;
}

void DataBase_close() {
    if (!db) {
        return;
    }
    
    // Close database
    sqlite3_close(db);
}

void DataBase_printDatabse() {
    execQuery("SELECT * FROM users;", printDB, NULL, NULL);
}

void DataBase_printDecryptedDatabse() {
    execQuery("SELECT * FROM users;", printDecryptedDb, NULL, NULL);
}


/*
int main() {
    DataBase_init();

    char *err_msg = NULL;

    // Execute query
    const char *sql = "SELECT * FROM users";
    


    UserData usr = {
        .is_manager = false,
        .is_active = true,
        .username = "Orangu",
        .password = "password456",
        .error_code = USER_SUCCESS
    };

    // DataBase_addNewUser(&usr);

    UserData data;

    DataBase_printDecryptedDatabse();
    
    // execQuery("SELECT * FROM users;", printDB, NULL, NULL);

    /*
    DataBase_getUser("dudun", &data);

    printf("is_manager: %d\n", data.is_manager);
    printf("is_active: %d\n", data.is_active);
    printf("username: %s\n", data.username);
    printf("password: %s\n", data.password);
    */

    /*
    DataBase_addNewUser(&usr);

    execQuery("SELECT * FROM users;", printDB, &usr, NULL);

    printf("\n\n=============================\n\n");

    DataBase_deactivateUser(&usr);

    execQuery("SELECT * FROM users;", printDB, &usr, NULL);
    */



    /*
    DataBase_execQuery(sql, callback1, &usr, &err_msg);
    printf("%s \n", err_msg);
    */

    /*
    if (sqlite3_exec(db, sql, canAddNewUser, &usr, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    }

    printf("%s\n", getUserValidationError(usr.error_code));
    


    DataBase_close();

    return 0;
}
*/
