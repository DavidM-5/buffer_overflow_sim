#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>


#define MAX_NAME_LENGTH 100
#define MAX_PASSWORD_LENGTH 100


typedef enum {
    QUIT,
    LOGIN,
    SIGN_UP,
    LOG_OUT,
    TOTAL_CHOICES,
    OTHER = -1
} Choice;

typedef enum {
    GUEST,
    USER,
    MANAGER,
    ADMIN
} Permission;

struct userData
{
    int id;
    char name[MAX_NAME_LENGTH];
};

struct session
{
    bool loggedIn;
    Permission permission;
    struct userData* user;    
};


void clearBuffer(){
    while (getchar() != '\n');  // Clears the input buffer until a newline is encountered
}

int getIdFromName(const char* name){
    FILE* file_users = fopen("users.txt", "r");

    if (file_users == NULL){
        printf("Cannot open users file.\n");
        exit(1);
    }

    char line[256];
    char currName[MAX_NAME_LENGTH];

    while ( fgets(line, sizeof(line), file_users) ) {
        int userId;
        if ( sscanf(line, "%d %s", &userId, currName) == 2 ){
            if (strncmp(currName, name, sizeof(currName)) == 0){
                fclose(file_users);
                return userId;
            }
        }
    }

    fclose(file_users);
    return -1;
}

Permission getPermissionFromId(int id){
    FILE* file_users = fopen("users.txt", "r");

    if (file_users == NULL){
        printf("Cannot open users file.\n");
        exit(1);
    }

    char line[256];
    char currName[MAX_NAME_LENGTH];
    char currPasswrd[MAX_PASSWORD_LENGTH];
    int perm;

    while ( fgets(line, sizeof(line), file_users) ) {
        int userId;
        if ( sscanf(line, "%d %s %s %d", &userId, currName, currPasswrd, &perm) == 4 ) {
            if ( id == userId ){
                fclose(file_users);
                return (Permission)perm;
            }
        }
    }

    fclose(file_users);
    return GUEST;
}

bool userNameExists(const char* name){
    FILE* file_users = fopen("users.txt", "r");

    if (file_users == NULL){
        printf("Cannot open users file.\n");
        exit(1);
    }

    char line[256];
    char currName[MAX_NAME_LENGTH];

    while ( fgets(line, sizeof(line), file_users) ) {
        int userId;
        if ( sscanf(line, "%d %s", &userId, currName) == 2 ){
            if (strncmp(currName, name, MAX_NAME_LENGTH) == 0){
                fclose(file_users);
                return true;
            }
        }
    }

    fclose(file_users);
    return false;
}

bool passwordsMatchToUser(const char* name, const char* passwrd){
    FILE* file_users = fopen("users.txt", "r");

    if (file_users == NULL){
        printf("Cannot open users file.\n");
        exit(1);
    }

    char line[256];
    char currName[MAX_NAME_LENGTH];
    char currPasswrd[MAX_PASSWORD_LENGTH];

    while ( fgets(line, sizeof(line), file_users) ) {
        int userId;
        if ( sscanf(line, "%d %s %s", &userId, currName, currPasswrd) == 3 ) {
            if ( strncmp(currName, name, MAX_PASSWORD_LENGTH) == 0 && strncmp(currPasswrd, passwrd, sizeof(currName)) == 0){
                fclose(file_users);
                return true;
            }
        }
    }

    fclose(file_users);
    return false;
}

void addNewUser(const char* name, const char* passwrd /*, Permission perm*/){
    // open the lastId file to know what id to assign to the new user
    FILE *lastIdFile = fopen("lastId.txt", "r+");
    if (!lastIdFile) {
        perror("Could not open file");
        return;
    }

    int currentUsedId;
    if (fscanf(lastIdFile, "%d", &currentUsedId) != 1) {
        printf("Failed to read number from the file.\n");
        fclose(lastIdFile);
        return;
    }

    currentUsedId++;

    // Move the file pointer to the beginning of the file
    rewind(lastIdFile);

    // append the new user to users.txt file
    FILE *usersFile = fopen("users.txt", "a"); 
    if (!usersFile) {
        perror("Could not open file");
        fclose(lastIdFile);
        fclose(usersFile);
        return;
    }

    char newUserData[5 + MAX_NAME_LENGTH + MAX_PASSWORD_LENGTH];
    sprintf(newUserData, "\n%-3d %s %s %d", currentUsedId, name, passwrd, USER);

    // Write to and close both files
    fprintf(lastIdFile, "%d", currentUsedId);
    fclose(lastIdFile);

    fprintf(usersFile, "%s", newUserData); 
    fclose(usersFile);
}

bool login(struct session* session){

    char name[MAX_NAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];

    printf("Name: ");
    scanf("%s", name);
    printf("Password: ");
    scanf("%s", password);

    if (!passwordsMatchToUser(name, password)){
        printf("Wrong name or password.\n");
        return false;
    }    

    session->loggedIn = true;
    
    session->user->id = getIdFromName(name);
    
    strncpy(session->user->name, name, MAX_NAME_LENGTH - 1);
    session->user->name[sizeof(session->user->name) - 1] = '\0';
    
    session->permission = getPermissionFromId(session->user->id);
    

    return true;
}

void sign_up(){
    char name[MAX_NAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    char confirmPasswrd[MAX_PASSWORD_LENGTH];

    bool firstTry = true;
    do {
        if (!firstTry) printf("Name already taken.\n");

        printf("Name: ");
        scanf("%s", name);

        firstTry = false;  
    } while ( userNameExists(name) );
    

    firstTry = true;
    bool passwordsMatch = false;
    do
    {
        if (!firstTry) printf("Passwords do not match. Try again:\n");

        printf("Password: ");
        scanf("%s", password);

        printf("Confirm Password: ");
        scanf("%s", confirmPasswrd);

        if (strcmp(password, confirmPasswrd) == 0) {
            passwordsMatch = true;
        }

        firstTry = false;

    } while (!passwordsMatch);

    addNewUser(name, password);
}

void log_out(struct session* session){
    if (session->loggedIn != false) {
        session->loggedIn = false;

        session->permission = GUEST;

        session->user->id = -1;
        memset(session->user->name, 0, MAX_NAME_LENGTH);
    }
}

int main( /* int argc, char const *argv[] */ )
{
    struct userData currentUser = {
        .id = -1,
        .name = {0}
    };
    struct session currentSession = {
        .loggedIn = false,
        .permission = GUEST,
        .user = &currentUser
    };


    Choice choice;
    char permission_names[4][10] = { "Guest", "User", "Manager", "Admin" };

    // Main loop
    while (1)
    {
        printf("\n\nLogged in as: %s  [ perm: %s ]\n", currentSession.user->name, permission_names[currentSession.permission]);
        printf("Welcome! Enter:\n");
        printf("    1 - LOGIN\n");
        printf("    2 - SIGN UP\n");
        printf("    3 - LOG OUT\n");
        printf("    0 - QUIT\n");

        printf("Your choice: ");
        scanf("%d", &choice);

        if (choice > TOTAL_CHOICES) choice = OTHER;

        switch (choice)
        {
        case QUIT:
            printf("Bye!\n");
            exit(0);
            break;

        case LOGIN:
            if (currentSession.loggedIn) {
                printf("Currently logged in, log out first.\n");
                break;
            }

            login(&currentSession);
            break;

        case SIGN_UP:
            sign_up();
            break;

        case LOG_OUT:
            log_out(&currentSession);
            break;

        default:
            
            break;
        }

        //clearBuffer();
    }

    return 0;
}
