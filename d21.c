#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

int main(void) {
    size_t e = 0, b;
    size_t cap = 16384;
    size_t seen[cap];

    for (size_t i = 0; i < cap; ++i) {
        b = 65536 | e;
        e = 16031208;         
        for (; b > 0; b /= 256) {
            e = ((e + (b % 256) % 16777216) * 65899) % 16777216;
        }

        for (size_t j = 0; j < i; ++j) {
            if (seen[j] == e) { 
                printf("Part 2: %zu\n", seen[i - 1]);
                return 1;
            }
        }
        seen[i] = e;
        if (i == 0) printf("Part 1: %zu\n", e);
    }
}
