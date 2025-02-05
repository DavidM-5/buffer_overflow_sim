#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/sha.h>


#define AES_KEYLEN 32 // 256-bit AES key
#define AES_BLOCKLEN 16 // Block size is 16 bytes

// Expose AES_KEYLEN for external use
const int AES_KEY_LENGTH = AES_KEYLEN;
const int AES_BLOCK_LENGTH = AES_BLOCKLEN;
const int SHA256_LENGTH = SHA256_DIGEST_LENGTH;

// Function to handle errors
void handleErrors(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void hashString(const char *input, unsigned char output[SHA256_DIGEST_LENGTH]) {
    SHA256((unsigned char *)input, strlen(input), output);
}

void encryptString(const unsigned char *plaintext, const unsigned char *key, unsigned char *ciphertext, int *ciphertext_len) {
    EVP_CIPHER_CTX *ctx;

    // Initialize the context
    if (!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors("Error initializing EVP context");

    // Initialize AES-256-CBC encryption
    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, key + AES_BLOCKLEN))
        handleErrors("Error initializing AES-256-CBC encryption");

    int len;

    // Encrypt the plaintext
    if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, strlen((char *)plaintext)))
        handleErrors("Error encrypting plaintext");
    *ciphertext_len = len;

    // Finalize the encryption
    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
        handleErrors("Error finalizing encryption");
    *ciphertext_len += len;

    // Clean up
    EVP_CIPHER_CTX_free(ctx);
}

void decryptString(const unsigned char *ciphertext, int ciphertext_len, const unsigned char *key, unsigned char *plaintext, int *plaintext_len) {
    EVP_CIPHER_CTX *ctx;

    // Initialize the context
    if (!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors("Error initializing EVP context");

    // Initialize AES-256-CBC decryption
    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, key + AES_BLOCKLEN))
        handleErrors("Error initializing AES-256-CBC decryption");

    int len;

    // Decrypt the ciphertext
    if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        handleErrors("Error decrypting ciphertext");
    *plaintext_len = len;

    // Finalize the decryption
    if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
        handleErrors("Error finalizing decryption");
    *plaintext_len += len;

    // Null-terminate the plaintext
    plaintext[*plaintext_len] = '\0';

    // Clean up
    EVP_CIPHER_CTX_free(ctx);
}