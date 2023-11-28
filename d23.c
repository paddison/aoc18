#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define INP "./data/d23.txt"
#define TEST "./data/d23_test.txt"
#define TEST2 "./data/d23_test2.txt"

typedef struct Drone Drone;

struct Drone {
    int pos[3];
    int r;
};

void drone_print(Drone* d) {
    printf("pos=<%d,%d,%d>, r=%d\n", d->pos[0], d->pos[1], d->pos[2], d->r);
}

size_t drone_max_r(size_t N, Drone drones[N]) {
    size_t max = 0;
    size_t d = -1;

    for (size_t i = 0; i < N; ++i) {
        if (drones[i].r > max) {
            max = drones[i].r;
            d = i;
        }
    }
    return d;
}

int drone_dist(Drone from, Drone to) {
    return abs(from.pos[0] - to.pos[0]) 
         + abs(from.pos[1] - to.pos[1])
         + abs(from.pos[2] - to.pos[2]);
}

size_t solve_1(size_t N, Drone drones[N]) {
    size_t n_in_range = 0;

    size_t max_r = drone_max_r(N, drones);
    drone_print(&drones[max_r]);

    for (size_t i = 0; i < N; ++i) {
        int dist = drone_dist(drones[max_r], drones[i]);
        if (drones[max_r].r >= dist) ++n_in_range;
    }

    return n_in_range;
}

size_t most_reached_drone(size_t N, Drone drones[N]) {
    size_t reached[N];
    size_t max = 0;
    size_t min_r = SIZE_MAX;
    size_t most_reached = -1;

    for (size_t i = 0; i < N; ++i) {
        if (drones[i].r < min_r) min_r = drones[i].r;
        reached[i] = 0;
        for (size_t j = 0; j < N; ++j) {
            int dist = drone_dist(drones[j], drones[i]);
            if (drones[j].r >= dist) ++reached[i];
        }
        if (reached[i] > max) {
            max = reached[i]; most_reached = i;
        }
    }

    printf("min: %zu\n", min_r);
    printf("max: %zu\n", max);
    return most_reached;
}

size_t n_in_range(Drone d, size_t N, Drone drones[N]) {
    size_t n = 0;

    for (size_t i = 0; i < N; ++i) {
        if (drones[i].r >= drone_dist(drones[i], d)) ++n;
    }

    return n;
}

size_t parse_input(char inp[static 1], Drone** drones) {
    size_t len = 0, cap = 1024;
    *drones = malloc(sizeof(Drone) * cap);
    char line[64];
    
    FILE* f = fopen(inp, "r");

    while (fgets(line, 64, f)) {
        Drone d;
        sscanf(line, "pos=<%d,%d,%d>, r=%d",
               &d.pos[0], &d.pos[1], &d.pos[2], &d.r);
        if (len == cap) {
            cap *= 2;
            *drones = realloc(drones, sizeof(Drone) * cap);
        }
        (*drones)[len++] = d;
    }

    fclose(f);

    return len;
}

int main(void) {
    Drone* drones = 0;
    size_t N = parse_input(INP, &drones);
    printf("Part 1: %zu\n", solve_1(N, drones));

    printf("%zu\n", most_reached_drone(N, drones));
    drone_print(&drones[716]);

    /*
    Drone d;
    d.r = 1000000;
    size_t max = 0;

    while (1) {
        for (int i = -d.r; i <= d.r; ++i) {
            for (int j = -abs((abs(i) - d.r)); abs(i) + abs(j) <= d.r; ++j) {
                int k = abs((abs(i) + abs(j) - d.r));
                d.pos[0] = i;
                d.pos[1] = j;
                d.pos[2] = k;
                size_t n = n_in_range(d, N, drones);
                if (n > max) {
                    max = n;
                    printf("<%d,%d,%d>: %zu\n", i, j, k, n);
                }
                d.pos[2] = -k;
                n = n_in_range(d, N, drones) ;
                if (n > max) {
                    max = n;
                    printf("<%d,%d,%d>: %zu\n", i, j, k, n);
                }
                assert(abs(i) + abs(j) + abs(k) == d.r);
            }
        }
        ++d.r;
        //if (d.r > 50) break;
    }
*/


    return EXIT_SUCCESS;
}
