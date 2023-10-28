#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#define INP "d10.txt"
#define TEST "d10_test.txt"

typedef struct Point Point;

struct Point {
    int pos[2];
    int vel[2];
};

void point_step(Point* p) {
    p->pos[0] += p->vel[0];
    p->pos[1] += p->vel[1];
}

void point_bounding_box(int box[4], size_t len, Point points[len]) {
    int min_x = INT_MAX;
    int max_x = INT_MIN;
    int min_y = INT_MAX;
    int max_y = INT_MIN;

    for (size_t i = 0; i < len; ++i) {
        if (points[i].pos[0] < min_x) min_x = points[i].pos[0];
        if (points[i].pos[0] > max_x) max_x = points[i].pos[0];
        if (points[i].pos[1] < min_y) min_y = points[i].pos[1];
        if (points[i].pos[1] > max_y) max_y = points[i].pos[1];
    }

    box[0] = min_x;
    box[1] = max_x;
    box[2] = min_y;
    box[3] = max_y;
}

void point_print_message(int bb[4], size_t len, Point points[len]) {
    int width = bb[1] - bb[0] + 1;
    int height = bb[3] - bb[2] + 1;

    char message[height][width + 1];

    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) message[i][j] = ' ';
        message[i][width] = 0;
    }

    for (size_t i = 0; i < len; ++i) {
        int y = points[i].pos[1] - bb[2];
        int x = points[i].pos[0] - bb[0];
        message[y][x] = '#';
    }

    for (size_t i = 0; i < height; ++i) {
        printf("%s\n", message[i]);
    } 
}

void point_dbg(const Point* p) {
    printf("p: %2d, %2d\tv: %2d, %2d\n", 
            p->pos[0], p->pos[1], p->vel[0], p->vel[1]);
}

int main(void) {
    size_t len = 0;
    char line[128];
    FILE* f = fopen(INP, "r");

    if (!f) {
        fprintf(stderr, "Unable to open file");
        return EXIT_FAILURE;
    }

    while (fgets(line, 100, f)) ++len; 

    rewind(f);

    Point points[len]; 
    int bounding_box[4] = { 0 };
    
    for (size_t i = 0; fgets(line, 128, f); ++i) {
        Point p = { 0 };
        sscanf(line, "position=<%d, %d> velocity=<%d, %d>", 
                p.pos, p.pos + 1, p.vel, p.vel + 1);
        points[i] = p;
    }
    
    
    for (size_t t = 1;; ++t) {
        for (size_t i = 0; i < len; ++i) point_step(points + i);

        point_bounding_box(bounding_box, len, points);

        if (bounding_box[3] - bounding_box[2] + 1 < 12) {
            printf("Part 1:\n");
            point_print_message(bounding_box, len, points);
            printf("Part 2: %zu\n", t);
            break;
        }
    }

    return EXIT_SUCCESS;
}
