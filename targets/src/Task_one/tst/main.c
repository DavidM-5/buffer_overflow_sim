#include <stdio.h>
#include "aes_utils.h"


int main(int argc, char const *argv[])
{   
    char username[MAX_LENGTH] = "Donald J. Pump";
    printf("Not Encrypted:\n%s\n", username);

    char encryptedUserName[MAX_LENGTH * 2] = { 0 };
    strncpy((char*)encryptedUserName, username, MAX_LENGTH - 1);
    Encrypt((unsigned char*)encryptedUserName, (unsigned char*)KEY);

    printf("Encrypted:\n%s\n", encryptedUserName);
    
    
    char decryptedUserName[MAX_LENGTH] = { 0 };
    strncpy((char*)decryptedUserName, username, MAX_LENGTH - 1);
    Decrypt((unsigned char*)decryptedUserName, (unsigned char*)KEY);

    printf("Decrypted:\n%s\n", decryptedUserName);

    return 0;
}
