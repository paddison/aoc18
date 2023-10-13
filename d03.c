#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

typedef struct Claim Claim;

struct Claim {
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t heigth;
};

void claim_init(char line[static 1], Claim* claim) {
    char* pos = strchr(line, '@');
    claim->x = strtoul(pos + 1, &pos, 10);
    assert(*pos == ',');
    claim->y = strtoul(pos + 1, &pos, 10);
    assert(*pos == ':');
    claim->width = strtoul(pos + 1, &pos, 10);
    assert(*pos == 'x');
    claim->heigth = strtoul(pos + 1, &pos, 10);
}

void claim_print(Claim* claim) {
    printf("Claim: %d,%d: %dx%d\n", 
            claim->x,
            claim->y,
            claim->width,
            claim->heigth);
}

void claim_add_to_grid(Claim const * const claim, 
                       size_t w, size_t h, char grid[w * h]) {
    for(size_t y = 0; y < claim->heigth; ++y) {
        for (size_t x = 0; x < claim->width; ++x) {
            ++grid[x + claim->x + (y + claim->y) * w]; 
        }
    }
}

bool claim_overlaps(Claim const*const claim,
                    size_t w, size_t h, char grid[w * h]) {
    for(size_t y = 0; y < claim->heigth; ++y) {
        for (size_t x = 0; x < claim->width; ++x) {
            if(grid[x + claim->x + (y + claim->y) * w] > 1) {
                return true;
            }
        }
    }
    return false;
}

int main(void) {
    FILE* data = fopen("d03.txt", "r");
    
    size_t lines = 0;
    char line[64];

    /* determine number of claims */
    while (fgets(line, 64, data)) {
        ++lines;
    }
    
    rewind(data);    

    /* read in claim values */
    Claim claims[lines];

    for (size_t i = 0; fgets(line, 64, data); ++i) {
        Claim claim = { 0 };
        claim_init(line, &claim);
        claims[i] = claim;
    }

    fclose(data);

    /* build grid created by claims */
    char grid[1000 * 1000] = { 0 };
    size_t overlapping = 0;
    for (size_t i = 0; i < lines; ++i) {
        claim_add_to_grid(claims + i, 1000, 1000, grid);
    }

    /* solve part 1: count overlapping */
    for (size_t i = 0; i < 1000 * 1000; ++i) {
        if (grid[i] > 1) {
            ++overlapping;
        }
    }
    printf("Part 1: %zu\n", overlapping);

    /* solve part 2: find claim that's not overlapping */
    for (size_t i = 0; i < lines; ++i) {
        if (!claim_overlaps(claims + i, 1000, 1000, grid)) {
            printf("Part 2: %zu\n", i + 1);
            break;
        }
    }
}
