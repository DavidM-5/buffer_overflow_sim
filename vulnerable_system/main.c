#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include "Pages.h"

typedef enum{false, true} bool;

enum Page_Type {PAGE1, PAGE2, PAGE2_3, TOTAL_PAGES};

const char* pages[TOTAL_PAGES];

void init_pages() {
    pages[PAGE1] = page1;
    pages[PAGE2] = page2;
    pages[PAGE2_3] = page2_3;
}

void clear_screen() {
    #ifdef _WIN32
        system("cls");  // Windows
    #else
        system("clear"); // Linux and macOS
    #endif
}

bool login() {

}

void logout() {

}

bool signup() {

}

void print_users_data() {

}

void elevate_permissions() {

}

void remove_user() {

}


int main(int argc, char const *argv[])
{
    init_pages();
    enum Page_Type currentPage = PAGE1;

    while (true) {
        clear_screen();

        printf(pages[currentPage]);

        char str[150];
        scanf("%s", str);

        if (str[0] == '0')
            break;

    }
    

    return 0;
}
