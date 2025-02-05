#pragma once

typedef enum {
    WELCOME_PAGE = 0, 
    OPERATION_SELECT, 
    TOTAL_PAGES
} Page_Type;


const char* welcome_page = "Welcome! Select one of the following:\n"
                           "1 - Login\n"
                           "2 - Sign up\n"
                           "0 - Exit\n"
                           "\n";


const char* op_selection = "Select operation:\n"
                           "1 - Log out\n"
                           "2 - List users\n"
                           "3 - Elevate user permissions\n"
                           "4 - Remove user\n"
                           "0 - Exit\n"
                           "\n";
                           