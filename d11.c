#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int powerlevel(size_t x, size_t y, int sn) {
    size_t rack_id = x + 10;
    return (((rack_id * y + sn) * rack_id / 100) % 10) - 5;
}

void fill_grid(int grid[300 * 300], int sn) {
    for (size_t y = 0; y < 300; ++y) {
        for (size_t x = 0; x < 300; ++x) {
            grid[y * 300 + x] = powerlevel(x + 1, y + 1, sn);
        }
    }
}

int calculate_square(int grid[300 * 300], size_t size, size_t x, size_t y) {
    assert(x <= 300 - size && y <= 300 - size);

    int sum = 0;

    for (size_t yy = y; yy < y + size; ++yy) {
        for (size_t xx = x; xx < x + size; ++xx) {
            sum += grid[yy * 300 + xx];
        }
    }

    return sum;
}

void largest_square(int grid[300 * 300], size_t coords[2]) {
    int largest = INT_MIN;

    for (size_t y = 0; y < 298; ++y) {
        for (size_t x = 0; x < 298; ++x) {
            int score = calculate_square(grid, 3, x, y);
            if (score > largest) {
                largest = score;
                coords[0] = x;
                coords[1] = y;
            }
        }
    }
}

void largest_square_size(int grid[300 * 300], size_t coords[3]) {
    int largest = INT_MIN;

    for (size_t y = 0; y < 300; ++y) {
        for (size_t x = 0; x < 300; ++x) {
            size_t max_size = 300 - (x > y ? x : y); 
            for (size_t size = 1; size <= max_size; ++size) {
                int score = calculate_square(grid, size, x, y);
                if (score > largest) {
                    largest = score;
                    coords[0] = x;
                    coords[1] = y;
                    coords[2] = size;
                }
            }
        }
    }
}

int main(void) {
    int grid[300 * 300];
    size_t coords[2];
    size_t coords2[3];
    fill_grid(grid, 5153);

    largest_square(grid, coords);
    printf("Part 1: %zu,%zu\n", coords[0] + 1, coords[1] + 1);

    largest_square_size(grid, coords2);

    printf("Part 2: %zu,%zu,%zu\n", coords2[0] + 1, coords2[1] + 1, coords2[2]);

    return EXIT_SUCCESS;
}
