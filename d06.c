#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define DUMMY 99

typedef struct Grid Grid;

struct Grid {
    size_t width;
    size_t height;
    size_t min_x;
    size_t min_y;
    size_t** grid;
    size_t n_coords;
    size_t* coords;
};

Grid* grid_init(size_t width, size_t height, 
                size_t min_x, size_t min_y,
                size_t n_coords, size_t* coords) {
    Grid* g = malloc(sizeof(Grid));

    size_t** grid = malloc(sizeof(size_t*) * height);

    for (size_t y = 0; y < height; ++y) {
        grid[y] = malloc(sizeof(size_t) * width);
        for (size_t x = 0; x < width; ++x) {
            grid[y][x] = -1;
        }
    }

    for (size_t i = 0; i < n_coords; ++i) {
        size_t x = coords[2 * i];
        size_t y = coords[2 * i + 1];
        
        grid[y - min_y][x - min_x] = i;
    }

    g->width = width;
    g->height = height;
    g->min_x = min_x;
    g->min_y = min_y;
    g->grid = grid;
    g->n_coords = n_coords;
    g->coords = coords;

    return g;
}

size_t manhattan(size_t a_x, size_t a_y, size_t b_x, size_t b_y) {
    size_t tmp;
    if (a_x < b_x) {
        tmp = b_x;
        b_x = a_x;
        a_x = tmp;
    }

    if (a_y < b_y) {
        tmp = b_y;
        b_y = a_y;
        a_y = tmp;
    }
    return a_x - b_x + a_y - b_y;
}

void grid_fill(Grid* grid) {
    for (size_t y = 0; y < grid->height; ++y) {
        for (size_t x = 0; x < grid->width; ++x) {
            if (grid->grid[y][x] == -1) {
                size_t a_x = x + grid->min_x;
                size_t a_y = y + grid->min_y;
                size_t min_dist = -1;
                size_t min_coord = -1;
                bool is_equal = false;

                for (size_t c = 0; c < grid->n_coords; ++c) {
                    size_t b_x = grid->coords[2 * c];
                    size_t b_y = grid->coords[2 * c + 1]; 
                    size_t dist = manhattan(a_x, a_y, b_x, b_y);

                    if (dist < min_dist) {
                        min_dist = dist;
                        min_coord = c;
                        is_equal = false;
                    } else if(dist == min_dist) {
                        is_equal = true;
                    }
                }

                if (!is_equal) grid->grid[y][x] = min_coord;
                else grid->grid[y][x] = DUMMY;
            }
        }
    }
}

bool grid_is_infinite_area(size_t area_id, Grid* grid) {
    for (size_t y = 0; y < grid->height; ++y) {
        if (grid->grid[y][0] == area_id || 
            grid->grid[y][grid->width - 1] == area_id) {
            return true;
        }
    }

    for (size_t x = 0; x < grid->width; ++x) {
        if (grid->grid[0][x] == area_id || 
            grid->grid[grid->height - 1][x] == area_id) {
            return true;
        }
    }

    return false;
}

size_t grid_largest_area(Grid* grid) {
    size_t largest[grid->n_coords];
    size_t max = 0;

    for (size_t c = 0; c < grid->n_coords; ++c) {
        largest[c] = 0;
    }

    for (size_t y = 0; y < grid->height; ++y) {
        for (size_t x = 0; x < grid->width; ++x) {
            if (grid->grid[y][x] != DUMMY) ++largest[grid->grid[y][x]];
        }
    }

    for (size_t c = 0; c < grid->n_coords; ++c) {
        if (largest[c] > max && !grid_is_infinite_area(c, grid)) { 
            max = largest[c];
        }
    }

    return max;
}

size_t grid_closest_region(Grid* grid, size_t min_dist) {
    size_t region_size = 0;

    for (size_t y = 0; y < grid->height; ++y) {
        for (size_t x = 0; x < grid->width; ++x) {
            size_t a_x = x + grid->min_x;
            size_t a_y = y + grid->min_y;
            size_t total_dist = 0;

            for (size_t c = 0; c < grid->n_coords; ++c) {
                size_t b_x = grid->coords[c * 2];
                size_t b_y = grid->coords[c * 2 + 1];
                total_dist += manhattan(a_x, a_y, b_x, b_y);
            }

            if (total_dist < min_dist) ++region_size;
        }
    }

    return region_size;
}

void grid_print(Grid* grid) {
    for (size_t y = 0; y < grid->height; ++y) {
        for (size_t x = 0; x < grid->width; ++x) {
            printf("%2zu ", grid->grid[y][x]);
        }
        printf("\n");
    }
}

int main(void) {
    char line[64];
    FILE* f = fopen("d06.txt", "r");
    size_t lines = 0;

    while(fgets(line, 64, f)) {
        ++lines;
    }

    rewind(f);

    size_t* coords = malloc(sizeof(size_t) * lines * 2);

    for (size_t i = 0; fgets(line, 64, f); ++i) {
        size_t x, y;
        sscanf(line, "%zu, %zu", &x, &y);
        coords[2 * i] = x;
        coords[2 * i + 1] = y;
    }

    fclose(f);

    size_t min_x, min_y = SIZE_MAX;
    size_t max_x, max_y = 0;

    for (size_t i = 0; i < lines; ++i) {
        size_t x = coords[i * 2];
        size_t y = coords[i * 2 + 1];

        if (x < min_x) min_x = x;
        if (x > max_x) max_x = x;
        if (y < min_y) min_y = y;
        if (y > max_y) max_y = y;
    }

    size_t width = max_x - min_x + 1;
    size_t height = max_y - min_y + 1;

    Grid* grid = grid_init(width, height, min_x, min_y, lines, coords);

    grid_fill(grid);

    printf("Part 1: %zu\n", grid_largest_area(grid));
    printf("Part 2: %zu\n", grid_closest_region(grid, 10000));

    return EXIT_SUCCESS;
}
