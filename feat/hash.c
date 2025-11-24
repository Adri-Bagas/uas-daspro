#include <stdio.h>
#include "./hash.h"

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

    fprintf(stderr, "%d", hash);

    return hash;
}