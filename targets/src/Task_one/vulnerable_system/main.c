#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include "sqlite3.h"
#include "Pages.h"
#include "user_types.h"
#include "SecureDB.h"


/*

NOTE!
-----
USERNAME_MAX_LENGHT = 10
PASSWORD_MAX_LENGHT = 10
-----

*/

typedef struct {
  bool logged_in;
  UserData user;
} Session;


const char* pages[TOTAL_PAGES];

char lastMessage[256] = "";

void init_pages() {
  pages[WELCOME_PAGE] = welcome_page;
  pages[OPERATION_SELECT] = op_selection;
}

void clear_screen() {
  #ifdef _WIN32
    system("cls");  // Windows
  #else
    system("clear"); // Linux and macOS
  #endif
}

void print_users() {
  SecureDB_printDB();
}

void print_and_flush(const char *format, ...) {
  va_list args;
  va_start(args, format);  // Initialize the va_list with the first argument (format)

  vprintf(format, args);   // Print using vprintf, which takes a va_list

  fflush(stdout);          // Flush the output buffer to ensure it's written immediately

  va_end(args);            // Clean up the va_list
}

void init() {
  init_pages();
  SecureDB_init();
}

void close() {
  SecureDB_close();
}

// Helper function to convert error code to string
const char* errorToString(Error_code err) {
  switch(err) {
    case SUCCESS:
      return "Success";
    case USER_EXISTS:
      return "Username already exists";
    case USER_NOT_FOUND:
      return "User not found";
    case USER_NOT_ACTIVE:
      return "User is not active";
    case WRONG_PASSWORD:
      return "Wrong password";
    case NOT_LOGGED_IN:
      return "Not logged in";
    case USERNAME_TOO_SHORT:
      return "Username is too short";
    case USERNAME_TOO_LONG:
      return "Username is too long";
    case PASSWORDS_NOT_MATCH:
      return "Passwords do not match";
    case PASSWORD_TOO_SHORT:
      return "Password is too short";
    case PASSWORD_TOO_LONG:
      return "Password is too long";
    case INVALID_MANAGER_RIGHTS:
      return "Insufficient rights";
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


Error_code login(Session* session) {
  __asm__ volatile ("" ::: "memory"); // Prevent reordering
  char username[USERNAME_MAX_LENGHT] = {0};
  __asm__ volatile ("" ::: "memory"); // Prevent reordering
  
  print_and_flush("Username: ");
  gets(username); // <-- ?

  UserData user;

  SecureDB_getUser(username, &user);

  if (user.error_code == USER_NOT_FOUND) {
    return USER_NOT_FOUND;
  }

  if (!user.is_active) {
    return USER_NOT_ACTIVE;
  }

  print_and_flush("Password: ");

  char password[PASSWORD_MAX_LENGHT];
  if (fgets(password, PASSWORD_MAX_LENGHT, stdin))
    password[strcspn(password, "\n")] = '\0';

  if (strcmp(password, user.password) != 0) {
    return WRONG_PASSWORD;
  }

  session->logged_in = true;
  session->user = user;

  return SUCCESS;
}

void logout(Session* session) {
  session->logged_in = false;
  
  // Reset the user
  memset(&session->user, 0, sizeof(session->user));
}

// keep session NULL if dont want to automatically login after signup 
Error_code signup(Session* session) {
  UserData newUser = {
    .is_active = true,
    .is_manager = false,
    .error_code = SUCCESS
  };

  print_and_flush("Username: ");
  fgets(newUser.username, USERNAME_MAX_LENGHT, stdin);
  newUser.username[strcspn(newUser.username, "\n")] = '\0'; // Removes the newline character
  

  print_and_flush("Password: ");
  fgets(newUser.password, PASSWORD_MAX_LENGHT, stdin);
  newUser.password[strcspn(newUser.password, "\n")] = '\0'; // Removes the newline character


  char confPassword[PASSWORD_MAX_LENGHT];
  print_and_flush("Confirm password: ");
  fgets(confPassword, PASSWORD_MAX_LENGHT, stdin);
  confPassword[strcspn(confPassword, "\n")] = '\0'; // Removes the newline character


  // compare passwords
  if (strcmp(newUser.password, confPassword) != 0)
    return PASSWORDS_NOT_MATCH;


  SecureDB_addNewUser(&newUser);


  if (newUser.error_code != SUCCESS)
    return newUser.error_code;

  if (!session) 
    return SUCCESS;


  session->logged_in = true;
  session->user = newUser;

  return SUCCESS;
}

Error_code remove_manager(UserData* user) {
  if (!user)
    return INVALID_PARAMETERS;

  if (SecureDB_removeManager(user->username) != 0)
    return UNKNOWN_ERROR;

  return SUCCESS;
}

void handle_page(Page_Type* page, Session* session, bool* clearFlag) {
  *clearFlag = true;

  if (!page || !session) {
    return;
  }

  char choice;
  choice = getchar();
  int c;
  while ((c = getchar()) != '\n' && c != EOF);

  Error_code err;

  if (*page == WELCOME_PAGE) {
    
    switch (choice)
    {
    case '0':
      exit(0);

    case '1':
      err = login(session);

      if (err != SUCCESS) {
        snprintf(lastMessage, sizeof(lastMessage), "%s", errorToString(err));
        return;
      }

      *page = OPERATION_SELECT;
      break;

    case '2':
      err = signup(session);

      if (err != SUCCESS) {
        snprintf(lastMessage, sizeof(lastMessage), "%s", errorToString(err));
        return;
      }

      *page = OPERATION_SELECT;
      break;
    
    default:
      snprintf(lastMessage, sizeof(lastMessage), "%s", errorToString(INVALID_PARAMETERS));
      break;
    }

  }
  else if (*page == OPERATION_SELECT) {

    switch (choice)
    {
    case '0':
      exit(0);

    case '1':
      logout(session);
      *page = WELCOME_PAGE;
      break;

    case '2': // List users
      if (!session->user.is_manager) {
        snprintf(lastMessage, sizeof(lastMessage), "%s", errorToString(INVALID_MANAGER_RIGHTS));
        break;
      }

      print_users();
      print_and_flush("\n");

      *clearFlag = false;
      break;

    case '3':
      if (!session->user.is_manager) {
        snprintf(lastMessage, sizeof(lastMessage), "%s", errorToString(INVALID_MANAGER_RIGHTS));
        break;
      }

      print_and_flush("User: ");

      char userToElevate[USERNAME_MAX_LENGHT] = {0};

      fgets(userToElevate, USERNAME_MAX_LENGHT, stdin);
      userToElevate[strcspn(userToElevate, "\n")] = '\0'; // Removes the newline character
      
      if (SecureDB_makeManager(userToElevate) != 0) {
        snprintf(lastMessage, sizeof(lastMessage), "%s", errorToString(UNKNOWN_ERROR));
        return;
      }

      snprintf(lastMessage, sizeof(lastMessage), "%s is now manager.", userToElevate);
      break;

    case '4':
      if (!session->user.is_manager) {
        snprintf(lastMessage, sizeof(lastMessage), "%s", errorToString(INVALID_MANAGER_RIGHTS));
        break;
      }

      print_and_flush("User: ");

      char userToRemove[USERNAME_MAX_LENGHT] = {0};

      fgets(userToRemove, USERNAME_MAX_LENGHT, stdin);
      userToRemove[strcspn(userToRemove, "\n")] = '\0'; // Removes the newline character

      if (SecureDB_deactivateUser(userToRemove) != 0) {
        snprintf(lastMessage, sizeof(lastMessage), "%s", errorToString(UNKNOWN_ERROR));
        return;
      }

      snprintf(lastMessage, sizeof(lastMessage), "Removed %s.", userToRemove);
      break;
    
    default:
      snprintf(lastMessage, sizeof(lastMessage), "%s", errorToString(INVALID_PARAMETERS));
      break;
    }

  }
}

int main(int argc, char const *argv[])
{
  init();

  Page_Type currentPage = WELCOME_PAGE;

  bool clearSceenFlag = true;

  UserData usr = {
    .is_manager = true,
    .is_active = true,
    .username = "DavidM",
    .password = "123456"
  };
  SecureDB_addNewUser(&usr);
  print_users();

  Session session = {
    .logged_in = false,
    .user = {}
  };
  
  while (true) {
    if (clearSceenFlag)
      // clear_screen();

    if (strlen(lastMessage) > 0) {
      print_and_flush("%s\n\n", lastMessage);
      lastMessage[0] = '\0';
    }

    print_and_flush("-> %s", pages[currentPage]);

    handle_page(&currentPage, &session, &clearSceenFlag);
  }
  

  return 0;
}