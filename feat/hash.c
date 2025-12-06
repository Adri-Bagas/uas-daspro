#include <stdio.h>
#include "./hash.h"

/**
 * This function implements the djb2 algorithm, a string hashing algorithm.
 * It takes a string as input and returns a hash value.
 *
 * The algorithm works by iterating through the string character by character,
 * and for each character, it adds the character value to the hash value.
 * The hash value is then multiplied by 33 (or equivalently, shifted left by
 * 5 bits and added to itself) to distribute the hash values evenly.
 *
 * The function returns the final hash value.
 */
unsigned long hash(unsigned char *str) {
    unsigned long hash = 5381;
    int c;

    // Iterate through the string character by character
    while ((c = *str++)) {
        // hash * 33 + c
        // The shift (hash << 5) is equivalent to hash * 32.
        // Adding 'hash' makes it hash * 33.
        hash = ((hash << 5) + hash) + c; 
    }

    // fprintf(stderr, "%d", hash);

    return hash;
}