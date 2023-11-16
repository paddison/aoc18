#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#define INP "./data/d17.txt"
#define TEST "./data/d17_test.txt"

typedef struct Map Map;

struct Map {
    char* m;
    size_t w;
    size_t h;
    size_t s;
};

void map_print(Map* m) {
    for (size_t y = 0; y < m->h; ++y) {
        for (size_t x = 0; x < m->w; ++x) {
            printf("%c", m->m[y * m->w + x]);
        }
        printf("\n");
    }
}

Map* map_init(Map* map, size_t len, size_t coords[len][4], size_t bounds[4]) {
    size_t width = bounds[1] - bounds[0] + 3;
    size_t height = bounds[3] + 1 - bounds[2];
    size_t shift = bounds[0];
    char* m = malloc(sizeof(char) * width * height);

    for (size_t i = 0; i < width * height; ++i) {
        m[i] = '.';
    }

    for (size_t i = 0; i < len; ++i) {
        for (size_t y = coords[i][2] - bounds[2]; y < coords[i][3] + 1 - bounds[2]; ++y) {
            for (size_t x = coords[i][0]; x < coords[i][1] + 1; ++x) {
                m[y * width + x + 1 - shift] = '#';
            }
        }
    }

    map->m = m;
    map->w = width;
    map->h = height;
    map->s = shift;
    
    return map;
}

void determine_bounds(size_t len, size_t coordinates[len][4], size_t bounds[4]) {  
    size_t min_x = SIZE_MAX, min_y = SIZE_MAX;
    size_t max_x = 0, max_y = 0;

    for (size_t i = 0; i < len; ++i) {
        if (coordinates[i][0] < min_x) min_x = coordinates[i][0]; 
        else if (coordinates[i][1] > max_x) max_x = coordinates[i][1]; 
        else if (coordinates[i][2] < min_y) min_y = coordinates[i][2]; 
        else if (coordinates[i][3] > max_y) max_y = coordinates[i][3]; 
    }

    bounds[0] = min_x;
    bounds[1] = max_x;
    bounds[2] = min_y;
    bounds[3] = max_y;
}

Map* parse_map(char input[static 1], Map* map) {
    size_t cap = 256;
    char line[cap];
    FILE* f = fopen(input, "r");
    size_t bounds[4];
    size_t coordinates[2048][4];
    size_t len = 0;

    if (!f) {
        fprintf(stderr, "Unable to open file");
        return 0;
    }

    while (fgets(line, cap, f)) {
        if (line[0] == 'x') {
            sscanf(line, "x=%zu, y=%zu..%zu", 
                   &coordinates[len][0], 
                   &coordinates[len][2], 
                   &coordinates[len][3]);

            coordinates[len][1] = coordinates[len][0];
        } else {
            sscanf(line, "y=%zu, x=%zu..%zu",
                   &coordinates[len][2], 
                   &coordinates[len][0], 
                   &coordinates[len][1]);

            coordinates[len][3] = coordinates[len][2];
        }
        ++len;
    }

    determine_bounds(len, coordinates, bounds);
    map_init(map, len, coordinates, bounds);

    return map;
}

bool can_fill(Map* m, size_t cur, size_t dir) {
    while (cur && (cur % m->w) + 1 != m->w) {
        if (m->m[cur] == '.') break;
        if (m->m[cur] == '#') return true;
        cur += dir;
    }

    return false;
}

size_t dfs(Map* m, size_t cur) {
    size_t count = 1;
    size_t down = cur + m->w;
    size_t left = cur - 1;
    size_t right = cur + 1;

    m->m[cur] = '|';

    /* go down */
    if (m->m[down] == '.') count += dfs(m, down);

    if (m->m[down] == '#' || m->m[down] == '~') {
        /* go left */
        if (m->m[left] == '.') count += dfs(m, left);

        if (m->m[left] == '#' && can_fill(m, cur, 1)) {
            for (size_t fill = cur; m->m[fill] != '#'; ++fill) {
                m->m[fill] = '~';
            }
        }

        /* go right */
        if (m->m[right] == '.') count += dfs(m, right);

        if (m->m[right] == '#' && can_fill(m, cur, SIZE_MAX)) {
            for (size_t fill = cur; m->m[fill] != '#'; --fill) {
                m->m[fill] = '~';
            }
        }
    }

    return count;
}

size_t count_water_squares(Map* m) {
    return dfs(m, 500 - m->s + 1);
}

int main(void) {
    Map map;
    size_t water_left = 0;

    if (!parse_map(INP, &map)) EXIT_FAILURE; 

    printf("Part 1: %zu\n", count_water_squares(&map));

    for (size_t i = 0; i < map.w * map.h; ++i) {
        if (map.m[i] == '~') ++water_left; 
    }

    printf("Part 2: %zu\n", water_left);

    return EXIT_SUCCESS;    
}
