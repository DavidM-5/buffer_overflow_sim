#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sqlite3.h>
#include "aes_utils.h"
#include "user_types.h"


sqlite3 *db;


int DataBase_init() {
    // Open database
    if (sqlite3_open("database\\SystemDB.db", &db)) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    return 0;
}

void DataBase_close() {
    // Close database
    sqlite3_close(db);
}

int DataBase_execQuery(const char *query, int (*callback)(void *, int, char **, char **), void *data, char **err_msg) {
    if (!query) {
        *err_msg = "Invalid query: query is NULL or empty";
        return 1;
    }

    if (sqlite3_exec(db, query, callback, data, err_msg) != SQLITE_OK) {
        return 1;
    }
    
    *err_msg = "Query executed successfully";
    return 0;
}


void encryptUserData(UserData* user) {
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

void decryptUserData(UserData* user) {
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


// Helper function to convert error code to string
const char* getUserValidationError(UserValidationError error) {
    switch(error) {
        case USER_SUCCESS:
            return "Success";
        case USER_EXISTS:
            return "Username already exists";
        case USERNAME_TOO_SHORT:
            return "Username is too short";
        case USERNAME_TOO_LONG:
            return "Username is too long";
        case PASSWORD_TOO_SHORT:
            return "Password is too short";
        case PASSWORD_TOO_LONG:
            return "Password is too long";
        case INVALID_MANAGER_RIGHTS:
            return "Insufficient rights to create manager account";
        case USERNAME_NULL:
            return "Username is NULL";
        case PASSWORD_NULL:
            return "Password is NULL";
        case INVALID_PARAMETERS:
            return "Invalid parameters";
        default:
            return "Unknown error";
    }
}

static int callback1(void *data, int argc, char **argv, char **azColName) {
    for(int i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

static int canAddNewUser(void *data, int argc, char **argv, char **azColName) {
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

    if(strlen(cbd->username) > 128) {
        cbd->error_code = USERNAME_TOO_LONG;
        return 0;
    }
    
    if(strlen(cbd->password) < 6) {
        cbd->error_code = PASSWORD_TOO_SHORT;
        return 0;
    }

    if(strlen(cbd->password) > 128) {
        cbd->error_code = PASSWORD_TOO_LONG;
        return 0;
    }

    // Check if username already exists
    for(int i = 0; i < argc; i++) {
        if(strcmp(azColName[i], "username") == 0 && 
           argv[i] != NULL && 
           strcmp(argv[i], cbd->username) == 0) {
            cbd->error_code = USER_EXISTS;
            return 0;
        }
    }

    cbd->error_code = USER_SUCCESS;
    return 0;
}

int main() {
    DataBase_init();

    char *err_msg = NULL;

    // Execute query
    const char *sql = "SELECT * FROM users";
    


    UserData usr = {
        .is_manager = true,
        .is_active = true,
        .username = "dudun",
        .password = "123456",
        .error_code = USER_SUCCESS
    };
    

    encryptUserData(&usr);
    printf("Username: %s \n", usr.username);
    printf("Password: %s \n", usr.password);

    printf("\n");

    decryptUserData(&usr);
    printf("Username: %s \n", usr.username);
    printf("Password: %s \n", usr.password);


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
    */


    DataBase_close();

    return 0;
}