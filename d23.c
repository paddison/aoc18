#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define INP "./data/d23.txt"
#define TEST "./data/d23_test.txt"
#define TEST2 "./data/d23_test2.txt"

typedef struct Point Point;
struct Point {
    int x;
    int y;
    int z;
};

void point_print(Point pos) {
    printf("<%d,%d,%d>\n", pos.x, pos.y, pos.z);
}

/********************************************************************************
 * Drone functions
 *******************************************************************************/
typedef struct Drone Drone;
/*
 * pos: [x; y; z]
*/
struct Drone {
    Point pos;
    int r;
};

void drone_print(Drone* d) {
    printf("pos=<%d,%d,%d>, r=%d\n", d->pos.x, d->pos.y, d->pos.z, d->r);
}

Drone drone_scale(Drone drone, int scale) {
    Drone new_drone = { 0 };
    new_drone.pos.x = drone.pos.x / scale;
    new_drone.pos.y = drone.pos.y / scale;
    new_drone.pos.z = drone.pos.z / scale;
    new_drone.r = drone.r / scale;
    
    return new_drone;
}

/**
 * Find the index of the drone with the largest radius;
 */
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

/**
 * Manhattan distance point to point
 */
int manhattan_dist(Point a, Point b) {
    return abs(a.x - b.x) 
         + abs(a.y - b.y)
         + abs(a.z - b.z);
}

/**
 * Number of drones in range to point i
 */
size_t n_drones_in_range(Point point, size_t N, Drone drones[N]) {
    size_t n_in_range = 0;
    for (size_t i = 0; i < N; ++i) {
        Drone drone = drones[i];
        size_t distance = manhattan_dist(point, drone.pos);
        
        if (drone.r >= distance) ++n_in_range;
    }

    return n_in_range;
}

/**
* find the point with the most drones in range, within a radius of 100 manhattan distance.
*/
struct Point max_point_in_range_100(Point start, size_t N, Drone drones[N]) {
    size_t max = 0;
    Point max_point = start;

    for (int x = start.x - 10; x <= start.x + 10; ++x) {
        for (int y = start.y - 10; y <= start.y + 10; ++y) {
            for (int z = start.z - 10; z <= start.z + 10; ++z) {
                Point current = { .x = x, .y = y, .z = z};
                size_t n_in_range = n_drones_in_range(current, N, drones);

                if (n_in_range > max) {
                    max = n_in_range;
                    max_point = current;
                } else if (n_in_range == max) {
                    Point origin = { 0 };

                    if (manhattan_dist(current, origin) < manhattan_dist(max_point, origin)) {
                        max_point = current;
                    }
                }
            }    
        }    
    }
    
    return max_point;
}

size_t parse_input(char inp[static 1], Drone** drones) {
    size_t len = 0, cap = 1024;
    *drones = malloc(sizeof(Drone) * cap);
    char line[64];
    
    FILE* f = fopen(inp, "r");

    while (fgets(line, 64, f)) {
        Drone d;
        sscanf(line, "pos=<%d,%d,%d>, r=%d",
               &d.pos.x, &d.pos.y, &d.pos.z, &d.r);
        if (len == cap) {
            cap *= 2;
            *drones = realloc(drones, sizeof(Drone) * cap);
        }
        (*drones)[len++] = d;
    }

    fclose(f);

    return len;
}

/**
 * Find the drone with the largest radius and calculate how many drones
 * are in range with it.
 */
size_t solve_1(size_t N, Drone drones[N]) {
    size_t n_in_range = 0;

    size_t max_r = drone_max_r(N, drones);
    drone_print(&drones[max_r]);

    for (size_t i = 0; i < N; ++i) {
        int dist = manhattan_dist(drones[max_r].pos, drones[i].pos);
        if (drones[max_r].r >= dist) ++n_in_range;
    }

    return n_in_range;
}

/** 
 * Find the point which is in radius of the most drones.
 * It divides the whole map into segments, by dividing the input data
 * by 10 million. It then finds the segment with the most drones.
 * From that, the resolution is increased and the new best segment, 
 * starting from the last segment, is found.
 * This is repeated until the original  are reached.
 * This is done by decreasing the resolution of the search space,
 * by dividing it into 
 */
size_t solve_2(size_t N, Drone drones[N]) {
    Point start = { 0 };
    /* divide all points by 1 million.*/
    for (int i = 10000000; i > 0; i /= 10) {
        Drone new_drones[N];
        for (size_t n = 0; n < N; ++n) 
            new_drones[n] = drone_scale(drones[n], i);
        
        start = max_point_in_range_100(start, N, new_drones);
        start.x *= 10;
        start.y *= 10;
        start.z *= 10;
    }
    
    return (start.x / 10) + (start.y / 10) + (start.z / 10);
}

int main(void) {
    Drone* drones = 0;
    size_t N = parse_input(INP, &drones);
    printf("Part 1: %zu\n", solve_1(N, drones));
    printf("Part 2: %zu\n", solve_2(N, drones));
    
    return EXIT_SUCCESS;
}
