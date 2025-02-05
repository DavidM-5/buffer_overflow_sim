#pragma once

extern int AES_KEY_LENGTH;
extern int AES_BLOCK_LENGTH;
extern int SHA256_LENGTH;

void hashString(const char *input, unsigned char output[SHA256_LENGTH]);

void encryptString(const unsigned char *plaintext, const unsigned char *key, unsigned char *ciphertext, int *ciphertext_len);

void decryptString(const unsigned char *ciphertext, int ciphertext_len, const unsigned char *key, unsigned char *plaintext, int *plaintext_len);
