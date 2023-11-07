#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MOVE_ELF(e) (*e = (r->items[*e] + 1 + *e) % r->len)

typedef struct Ring Ring;

struct Ring {
    uint8_t* items;
    size_t cap;
    size_t len;
};

Ring* ring_init(Ring* r, size_t cap) {
    if (!r) r = malloc(sizeof(Ring));
    if (!cap) cap = 64;

    r->cap = cap;
    r->items = malloc(cap);
    r->len = 0;

    return r;
}

void ring_push(Ring* r, uint8_t item) {
    if (!r) return;

    if (r->len == r->cap) {
        r->cap *= 2;
        r->items = realloc(r->items, r->cap);
    }

    r->items[r->len++] = item;
}

void add_recipe(Ring* r, size_t e1, size_t e2) {
    size_t sum = r->items[e1] + r->items[e2];
    if (sum < 10) { 
        ring_push(r, sum);
    } else {
        ring_push(r, sum / 10);
        ring_push(r, sum % 10);
    }
}

void add_recipes(Ring*r, size_t* e1, size_t* e2, size_t n_recipes) {
    for (size_t i = 0; i < n_recipes; ++i) {
        add_recipe(r, *e1, *e2);            
        MOVE_ELF(e1);
        MOVE_ELF(e2);
    }
}

int main(void) {
    const size_t MAX = 598701;
    const uint8_t ID[6] = { 5, 9, 8, 7, 0, 1 }; 
    Ring r;
    size_t e1 = 0; size_t e2 = 1;

    ring_init(&r, 1024);
    ring_push(&r, 3);
    ring_push(&r, 7);

    /* Part 1 */
    add_recipes(&r, &e1, &e2, MAX + 10);

    printf("Part 1: ");
    for (size_t i = MAX; i < MAX + 10; ++i) {
        printf("%d", r.items[i]);
    }
    printf("\n");

    /* Part 2 */
    for (size_t count = 0;; count = r.len) {
        add_recipes(&r, &e1, &e2, count);

        for (size_t i = count; i < r.len - 6; ++i) {
            if (!memcmp(r.items + i, ID, 6)) {
                printf("Part 2: %zu\n", i);
                return EXIT_SUCCESS;
            }
        }
    }

    return EXIT_FAILURE;
}
