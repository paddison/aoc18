#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define ELVES 418
#define MARBLES 71339

typedef struct N N;

struct N {
    uint32_t v;
    N* prev;
    N* next;
};

N* N_new(uint32_t v) {
    N* n = malloc(sizeof(N));
    n->prev = n;
    n->next = n;
    n->v = v;
    return n;
}

N* N_insert(uint32_t v, N* n) {
    N* insert = N_new(v);
    insert->v = v;
    insert->next = n->next;
    insert->prev = n;
    n->next->prev = insert;
    n->next = insert;

    return insert;
}

uint32_t N_remove(N* n) {
    uint32_t v = n->v;
    n->prev->next = n->next;
    n->next->prev = n->prev;
    free(n);
    return v; 
}

void N_print(N* n) {
    N* start = n;
    do {
        printf("%u ", n->v);
        n = n->next;
    } while (n != start);
    printf("\n");
}

size_t linked_list(size_t marbles) {
    N* n = N_new(0);
    N* next = n;
    uint32_t scores[ELVES] = { 0 };
    size_t high_score = 0;
    
    for (size_t i = 1; i < marbles + 1; ++i) {
        if (i % 23) {
            next = N_insert(i, next->next);
        } else {
            N* nnext;
            for (size_t i = 0; i < 7; ++i) next = next->prev;
            nnext = next->next;
            scores[i % ELVES] += N_remove(next) + i;
            next = nnext;
        }
    }

    for (size_t i = 0; i < ELVES; ++i) {
        if (scores[i] > high_score) high_score = scores[i];
    } 

    return high_score;
}

int main(void) {
    
    printf("Part 1: %zu\n", linked_list(MARBLES));
    printf("Part 2: %zu\n", linked_list(MARBLES * 100));

    return EXIT_SUCCESS;
}

