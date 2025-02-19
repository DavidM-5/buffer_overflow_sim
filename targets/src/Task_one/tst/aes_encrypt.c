#include "aes_utils.h"

// Substitutes bytes using the S-box
void subBytes(unsigned char* state) {
    for (int i = 0; i < 16; i++) {
        state[i] = getSBoxValue(state[i]);
    }
}

// Shifts rows to the left
void shiftRows(unsigned char* state)
{
    int i;
    // iterate over the 4 rows and call shiftRow() with that row
    for (i = 0; i < 4; i++)
        shiftRow(state + i * 4, i);
}

void shiftRow(unsigned char* state, unsigned char nbr)
{
    int i, j;
    unsigned char tmp;
    // each iteration shifts the row to the left by 1
    for (i = 0; i < nbr; i++)
    {
        tmp = state[0];
        for (j = 0; j < 3; j++)
            state[j] = state[j + 1];
        state[3] = tmp;
    }
}

void addRoundKey(unsigned char* state, unsigned char* roundKey)
{
    int i;
    for (i = 0; i < 16; i++)
        state[i] = state[i] ^ roundKey[i];
}

unsigned char galois_multiplication(unsigned char a, unsigned char b)
{
    unsigned char p = 0;
    unsigned char counter;
    unsigned char hi_bit_set;
    for (counter = 0; counter < 8; counter++)
    {
        if ((b & 1) == 1)
            p ^= a;
        hi_bit_set = (a & 0x80);
        a <<= 1;
        if (hi_bit_set == 0x80)
            a ^= 0x1b;
        b >>= 1;
    }
    return p;
}

// Mix columns using Galois field multiplication
void mixColumns(unsigned char* state)
{
    int i, j;
    unsigned char column[4];

    // iterate over the 4 columns
    for (i = 0; i < 4; i++)
    {
        // construct one column by iterating over the 4 rows
        for (j = 0; j < 4; j++)
        {
            column[j] = state[(j * 4) + i];
        }

        // apply the mixColumn on one column
        mixColumn(column);

        // put the values back into the state
        for (j = 0; j < 4; j++)
        {
            state[(j * 4) + i] = column[j];
        }
    }
}

void mixColumn(unsigned char* column)
{
    unsigned char cpy[4];
    int i;
    for (i = 0; i < 4; i++)
    {
        cpy[i] = column[i];
    }
    column[0] = galois_multiplication(cpy[0], 2) ^
        galois_multiplication(cpy[3], 1) ^
        galois_multiplication(cpy[2], 1) ^
        galois_multiplication(cpy[1], 3);

    column[1] = galois_multiplication(cpy[1], 2) ^
        galois_multiplication(cpy[0], 1) ^
        galois_multiplication(cpy[3], 1) ^
        galois_multiplication(cpy[2], 3);

    column[2] = galois_multiplication(cpy[2], 2) ^
        galois_multiplication(cpy[1], 1) ^
        galois_multiplication(cpy[0], 1) ^
        galois_multiplication(cpy[3], 3);

    column[3] = galois_multiplication(cpy[3], 2) ^
        galois_multiplication(cpy[2], 1) ^
        galois_multiplication(cpy[1], 1) ^
        galois_multiplication(cpy[0], 3);
}


// Perform AES round operations
void aes_round(unsigned char* state, unsigned char* roundKey) {
    subBytes(state);
    shiftRows(state);
    mixColumns(state);
    addRoundKey(state, roundKey);
}

void createRoundKey(unsigned char* expandedKey, unsigned char* roundKey)
{
    int i, j;
    // iterate over the columns
    for (i = 0; i < 4; i++)
    {
        // iterate over the rows
        for (j = 0; j < 4; j++)
            roundKey[(i + (j * 4))] = expandedKey[(i * 4) + j];
    }
}

void aes_main(unsigned char* state, unsigned char* expandedKey, int nbrRounds)
{
    int i = 0;

    unsigned char roundKey[16];

    createRoundKey(expandedKey, roundKey);
    addRoundKey(state, roundKey);

    for (i = 1; i < nbrRounds; i++)
    {
        createRoundKey(expandedKey + 16 * i, roundKey);
        aes_round(state, roundKey);
    }

    createRoundKey(expandedKey + 16 * nbrRounds, roundKey);
    subBytes(state);
    shiftRows(state);
    addRoundKey(state, roundKey);
}

char aes_encrypt(unsigned char* input, unsigned char* output,unsigned char* key, int size)
{
    // the number of rounds
    int nbrRounds = 10;

    // the expanded keySize
    int expandedKeySize = (16 * (nbrRounds + 1));

    // the expanded key
    unsigned char* expandedKey = (unsigned char*)malloc(expandedKeySize * sizeof(unsigned char));

    // the 128 bit block to encode
    unsigned char block[16];

    int i, j;

    if (expandedKey == NULL)
    {
        return ERROR_MEMORY_ALLOCATION_FAILED;
    }
    else
    {
        /* Set the block values, for the block:
         * a0,0 a0,1 a0,2 a0,3
         * a1,0 a1,1 a1,2 a1,3
         * a2,0 a2,1 a2,2 a2,3
         * a3,0 a3,1 a3,2 a3,3
         * the mapping order is a0,0 a1,0 a2,0 a3,0 a0,1 a1,1 ... a2,3 a3,3
         */

         // iterate over the columns
        for (i = 0; i < 4; i++)
        {
            // iterate over the rows
            for (j = 0; j < 4; j++)
                block[(i + (j * 4))] = input[(i * 4) + j];
        }

        // expand the key into an 176, 208, 240 bytes key
        expandKey(expandedKey, key, size, expandedKeySize);

        // encrypt the block using the expandedKey
        aes_main(block, expandedKey, nbrRounds);

        // unmap the block again into the output
        for (i = 0; i < 4; i++)
        {
            // iterate over the rows
            for (j = 0; j < 4; j++)
                output[(i * 4) + j] = block[(i + (j * 4))];
        }

        // de-allocate memory for expandedKey
        free(expandedKey);
        expandedKey = NULL;
    }

    return SUCCESS;
}


