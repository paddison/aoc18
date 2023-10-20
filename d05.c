#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

size_t react(char polymer[static 1]) {
    size_t len = strlen(polymer) + 1;
    size_t diff = 'a' - 'A';

    for (int i = 1; i < len; ++i) {
        if (abs(polymer[i - 1] - polymer[i]) == diff) {
            memmove(polymer + i - 1, polymer + i + 1, len  - i);
            i = 0;
            len = strlen(polymer) + 1;
        }
    }

    return strlen(polymer);
}

size_t remove_unit(char unit, size_t len, char target[len], char polymer[len]) {
    assert(unit < 123 && unit >= 97);
    size_t removed = 0;
    for (size_t i = 0; i < len; ++i) {
        if (polymer[i] != unit && polymer[i] != unit - 32) {
            target[removed++] = polymer[i];
        }
    }

    return removed; 
}

int main(void) {
    uint32_t char_count = 0;
    size_t const cap = 50001;
    char polymer[cap] = { 0 }; 
    char cpy[cap]; 
    FILE* f = fopen("d05.txt", "r");

    int read = fread(polymer, sizeof(char), cap, f);

    //assert(read == cap - 1);

    polymer[cap - 1] = 0;

    strcpy(cpy, polymer); 
    printf("Part 1: %zu\n", react(cpy));

    size_t min = SIZE_MAX;

    for (size_t c = 'a'; c <= 'z'; ++c) {
        char cpy[cap] = { 0 };
        size_t removed = remove_unit(c, strlen(polymer), cpy, polymer);
        size_t res = react(cpy);
        if (res < min) 
            min = res;
    }

    printf("Part 2: %zu\n", min);

    return EXIT_SUCCESS;
}
