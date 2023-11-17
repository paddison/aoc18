#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST "./data/d18_test.txt"
#define INP "./data/d18.txt"

typedef struct Period Period;

struct Period {
    size_t start;
    size_t len;
};

typedef struct Map Map;

struct Map {
    char* m;
    size_t w;
    size_t h;
};

Map* map_init(Map* map, size_t size) {
    if (!map) return map;

    map->m = malloc(sizeof(char) * size * size);
    map->w = size;
    map->h = size;

    return map;
}

void map_print(Map* map) {
    for (size_t y = 0; y < map->h; ++y) {
        for (size_t x = 0; x < map->w; ++x) {
            printf("%c", map->m[y * map->w + x]);
        }
        printf("\n");
    }
}

void map_count_adjacent(Map* map, size_t x, size_t y, size_t adj[3]) {
    size_t ystart = !y ? y : y - 1;
    size_t yend = y == map->h - 1 ? y : y + 1;
    size_t xstart = !x ? x : x - 1;
    size_t xend = x == map->w - 1 ? x : x + 1;

    for (size_t yy = ystart; yy <= yend; ++yy) {
        for (size_t xx = xstart; xx <= xend; ++xx) {
            if (yy == y && xx == x) continue;

            switch (map->m[yy * map->w + xx]) {
                case '.': ++adj[0]; break;
                case '|': ++adj[1]; break;
                case '#': ++adj[2]; break;
                default: fprintf(stderr, "Invalid char");
            }
        }
    }
}

void map_step(Map* map) {
    char* next = malloc(sizeof(char) * map->w * map->h);

    for (size_t y = 0; y < map->h; ++y) {
        for (size_t x = 0; x < map->w; ++x) {
            size_t adj[3] = { 0, 0, 0 };
            size_t pos = y * map->w + x;

            map_count_adjacent(map, x, y, adj);

            switch (map->m[pos]) {
                case '.': if (adj[1] >= 3) next[pos] = '|';
                          else next[pos] = '.';
                          break;
                case '|': if (adj[2] >= 3) next[pos] = '#';
                          else next[pos] = '|';
                          break;
                case '#': if (adj[1] && adj[2]) next[pos] = '#';
                          else next[pos] = '.';
                          break;
            }
        }
    }

    free(map->m);
    map->m = next;
}

Map* parse_input(char input[static 1], Map* map) {
    char line[64];
    size_t y = 0;
    FILE* f = fopen(input, "r");

    if (!f) {
        fprintf(stderr, "Unable to open file");
        return 0;
    }

    while (fgets(line, 64, f)) {
        for (size_t i = 0; line[i] != '\n'; ++i) {
            map->m[map->w * y + i] = line[i];
        }
        ++y;
    }

    return map;
}

size_t calc_n_trees_n_lumbers(Map* map) {
    size_t n_trees = 0;
    size_t n_lumber = 0;

    for (size_t y = 0; y < map->h; ++y) {
        for (size_t x = 0; x < map->w; ++x) {
            size_t pos = y * map->h + x;
            if (map->m[pos] == '#') ++n_lumber;
            else if (map->m[pos] == '|') ++n_trees;
        }
    }

    return n_trees * n_lumber;
}

Period* period_determine(Period* period, Map* map, 
                         size_t len, size_t numbers[len]) {
    for (size_t i = 0; i < len; ++i) {
        numbers[i] = calc_n_trees_n_lumbers(map);
        map_step(map);
        if (i == 10) printf("Part 1: %zu\n", numbers[i]);
    }
    
    for (size_t i = 0; i < len; ++i) {
        for (size_t j = i + 1; j < len; ++j) {
            if (numbers[i] == numbers[j]) {
                size_t start = numbers[i];

                for (size_t k = 1; numbers[k + i] == numbers[k + j]; ++k) {
                    if (numbers[k + i] == start && numbers[k + j] == start) {
                        period->start = i;
                        period->len = k;
                        return period;
                    }

                }
            }
        }
    }

    return 0;
}


int main(void) {
    Map map;
    Period period;
    size_t numbers[1000];

    map_init(&map, 50);
    parse_input(INP, &map);
    period_determine(&period, &map, 1000, numbers);

    printf("Part 2: %zu\n", 
            numbers[period.start + ((1000000000 - period.start) % period.len)]);

    return EXIT_SUCCESS;
}
