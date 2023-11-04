#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define TEST "d13_test2.txt"
#define INP "d13.txt"

enum dir {
    R,
    D,
    L,
    U
};

typedef struct Cart Cart;

struct Cart {
    size_t pos[2];
    size_t dir;
    size_t last_turn;
    bool crashed;
};

char get_dir(Cart* c) {
    if (c->dir == R) return '>';
    else if (c->dir == D) return 'v';
    else if (c->dir == L) return '<';
    else return '^';
}

void cart_move(Cart* cart, char** tracks) {
    if (cart->dir == R) ++cart->pos[0];
    else if (cart->dir == D) ++cart->pos[1];
    else if (cart->dir == L) --cart->pos[0];
    else --cart->pos[1];

    switch (tracks[cart->pos[1]][cart->pos[0]]) {
        case '\\':
            if (cart->dir == R || cart->dir == L) ++cart->dir;
            else --cart->dir;
            break;
        case '/':
            if (cart->dir == R || cart->dir == L) cart->dir = (cart->dir + 3) % 4;
            else cart->dir = (cart->dir + 1) % 4;
            break;
        case '+':
            if (cart->last_turn == 0) cart->dir = (cart->dir + 3) % 4;
            else if (cart->last_turn == 2) cart->dir = (cart->dir + 1) % 4;
            cart->last_turn = (cart->last_turn + 1) % 3;
            break;
    }
}

int cart_sort(const void* a, const void* b) {
    Cart* ca = (Cart*) a;
    Cart* cb = (Cart*) b;
    if (ca->pos[1] == cb->pos[1]) {
        return (int) ca->pos[0] - (int) cb->pos[0];
    }
    return (int) ca->pos[1] - (int) cb->pos[1];
}

size_t cart_crashes(size_t n_carts, Cart carts[n_carts], 
                  size_t cart) {
    for (size_t i = 0; i < n_carts; ++i) {
        if (i == cart || carts[i].crashed) continue;
        if (carts[cart].pos[0] == carts[i].pos[0] &&
            carts[cart].pos[1] == carts[i].pos[1]) {
            return i;
        };
    }
    return -1;
}

int main(void) {
    char line[512];
    char** track = malloc(sizeof(char*) * 512);
    Cart carts[20];
    size_t n_carts = 0;
    size_t crashed = 0;
    size_t other_crash;
    FILE* f = fopen(INP, "r");

    if (!f) {
        fprintf(stderr, "Unable to open file");
        return EXIT_FAILURE;
    }

    for (size_t y = 0; fgets(line, 512, f); ++y) {
        char* trackline = malloc(sizeof(char) * 512);
        for (size_t x = 0; line[x]; ++x) {
            switch(line[x]) {
                case '>': carts[n_carts++] = (Cart) {
                            .pos = { x, y },
                            .dir = 0,
                            .last_turn = 0,
                            .crashed = false,
                          };
                          trackline[x] = '-';
                          break;
                case 'v': carts[n_carts++] = (Cart) {
                            .pos = { x, y },
                            .dir = 1,
                            .last_turn = 0,
                            .crashed = false,
                          };
                          trackline[x] = '|';
                          break;
                case '<': carts[n_carts++] = (Cart) {
                            .pos = { x, y },
                            .dir = 2,
                            .last_turn = 0,
                            .crashed = false,
                          };
                          trackline[x] = '-';
                          break;
                case '^': carts[n_carts++] = (Cart) {
                            .pos = { x, y },
                            .dir = 3,
                            .last_turn = 0,
                            .crashed = false,
                          };
                          trackline[x] = '|';
                          break;
                case '\n': trackline[x] = 0;
                           break;
                default: trackline[x] = line[x];
            }
        }
        track[y] = trackline;
    }

    while (1) {
        if (n_carts - crashed == 1) break;

        qsort(carts, n_carts, sizeof(Cart), cart_sort);

        for (size_t i = 0; i < n_carts; ++i) {
            if (carts[i].crashed) continue;

            cart_move(carts + i, track);

            if ((other_crash = cart_crashes(n_carts, carts, i)) != -1) {
                if (!crashed) {
                    printf("Part 1: %zu,%zu\n", 
                            carts[i].pos[0], 
                            carts[i].pos[1]);
                }

                carts[i].crashed = true;
                carts[other_crash].crashed = true;
                crashed += 2;
            }
        }
    }

    for (size_t i = 0; i < n_carts; ++i) {
        if (!carts[i].crashed) { 
            printf("Part 2: %zu,%zu\n", 
                    carts[i].pos[0], 
                    carts[i].pos[1]); 
            break;
        }
    }

    return EXIT_SUCCESS;
}   
