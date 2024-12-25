#define _CRT_SECURE_NO_WARNINGSexi

#include <stdio.h>
#include <string.h>

// Function the user shouldn't be able to access directly
void secret_function() {
    printf("\n[+] Secret Function Accessed! Sensitive information: PASSWORD123\n");
}

// Vulnerable function in a loop
void vulnerable_function() {
    char buffer[8];
    printf("Enter your input: ");
    gets(buffer); // <<-- Unsafe gets()

    printf("You entered: %s\n", buffer);
}

int main() {
    printf("Buffer Overflow Demo - Main Loop\n");
    printf("Try to trigger the secret function!\n");
    
    vulnerable_function(); // <<-- Vulnerable function
    
    return 0;
}
