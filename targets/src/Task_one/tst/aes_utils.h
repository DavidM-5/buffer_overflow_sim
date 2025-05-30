#ifndef AES_H
#define AES_H

#define AES_BLOCK_SIZE 16
#define MAX_LENGTH 100
#define KEY "Sup3r5Ecur3K37!@"


#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

enum errorCode {
    SUCCESS = 0,
    ERROR_AES_UNKNOWN_KEYSIZE,
    ERROR_MEMORY_ALLOCATION_FAILED,
};

// Function declarations
unsigned char getSBoxValue(unsigned char num);
unsigned char getSBoxInvert(unsigned char num);
void rotate(unsigned char* word);
unsigned char getRconValue(unsigned char num);
void core(unsigned char* word, int iteration);
void expandKey(unsigned char* expandedKey, unsigned char* key, int size, size_t expandedKeySize);
void subBytes(unsigned char* state);
void shiftRows(unsigned char* state);
void shiftRow(unsigned char* state, unsigned char nbr);
void addRoundKey(unsigned char* state, unsigned char* roundKey);
unsigned char galois_multiplication(unsigned char a, unsigned char b);
void mixColumns(unsigned char* state);
void mixColumn(unsigned char* column);
void aes_round(unsigned char* state, unsigned char* roundKey);
void createRoundKey(unsigned char* expandedKey, unsigned char* roundKey);
void aes_main(unsigned char* state, unsigned char* expandedKey, int nbrRounds);
char aes_encrypt(unsigned char* input, unsigned char* output, unsigned char* key, int size);
void invSubBytes(unsigned char* state);
void invShiftRows(unsigned char* state);
void invShiftRow(unsigned char* state, unsigned char nbr);
void invMixColumns(unsigned char* state);
void invMixColumn(unsigned char* column);
void aes_invRound(unsigned char* state, unsigned char* roundKey);
void aes_invMain(unsigned char* state, unsigned char* expandedKey, int nbrRounds);
char aes_decrypt(unsigned char* input, unsigned char* output, unsigned char* key, int size);

void Encrypt(unsigned char* plaintext, unsigned char* key);
void Decrypt(unsigned char* ciphertext, unsigned char* key);

#endif // AES_H
