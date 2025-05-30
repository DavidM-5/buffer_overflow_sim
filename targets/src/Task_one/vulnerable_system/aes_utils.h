#ifndef AES_H
#define AES_H

#define AES_BLOCK_SIZE 16
#define MAX_LENGTH 30
// #define KEY "Sup3r5Ecur3K37!@"
/* 128-bit key will be loaded at runtime from a separate file */


#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

enum errorCode {
    AES_SUCCESS = 0,
    ERROR_AES_UNKNOWN_KEYSIZE,
    ERROR_MEMORY_ALLOCATION_FAILED,
};

extern unsigned char KEY[16];

/** 
 * Read exactly 16 bytes into KEY[] from a binary file at offset 100.
 * @param filename  path to the key file (must be at least 116 bytes).
 * @return AES_SUCCESS (0) on success, or an errorCode on failure.
*/
int load_key(const char *filename);

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

char* base64_encode_buf(const unsigned char* input, size_t len, char* output, size_t outSize);
unsigned char base64_reverse(char c);
unsigned char* base64_decode(const char* input, size_t* out_len);

void Encrypt(const unsigned char* plaintext, unsigned char* key, char* output, size_t outputSize);
void Decrypt(const char* base64Ciphertext, unsigned char* key, char* output, size_t outputSize);

#endif // AES_H