#include <stdio.h>
#include <stdlib.h>

#define INP "./data/d25.txt"

#define B_SIZE 2048

typedef struct Point Point;
struct Point {
    int x;
    int y;
    int z;
    int t;
};

void point_print(Point point) {
    printf("%d, %d, %d, %d\n", point.x, point.y, point.z, point.t);
}

int point_manhattan_distance(Point a, Point b) {
    return abs(a.x - b.x) 
         + abs(a.y - b.y)
         + abs(a.z - b.z)
         + abs(a.t - b.t);
}

typedef struct Node Node;
struct Node {
    size_t parent;
    size_t rank;
};

void subsets_init(size_t N, Node subsets[N]) {
    for (size_t i = 0; i < N; ++i) {
        subsets[i].parent = i;
        subsets[i].rank = 0;
    }
}

/* Returns the root of the subset 'id' belongs to */
size_t subsets_find(size_t id, Node* subsets) {
    if (subsets[id].parent != id)
        subsets[id].parent = subsets_find(subsets[id].parent, subsets);
    
    return subsets[id].parent;
}

size_t subsets_number_of_partitions(size_t N, Node subsets[N]) {
    size_t number_of_partitions = 0;

    for (size_t i = 0; i < N; ++i) {
        if (subsets[i].parent == i) ++number_of_partitions;
    }
    
    return number_of_partitions;
}

/* Creates a union from the subsets of a and b */
void subsets_union(size_t a, size_t b, Node* subsets) {
    size_t root_a = subsets_find(a, subsets);
    size_t root_b = subsets_find(b, subsets);

    /* both a and b are in the same set */
    if (root_a == root_b) return;

    /* swap variables, since root_a is always assumed to be the root */
    if (subsets[root_a].rank < subsets[root_b].rank) {
        // swap variables without tmp because im cool
        root_a = root_a ^ root_b; 
        root_b = root_a ^ root_b;         
        root_a = root_a ^ root_b; 
    }

    subsets[root_b].parent = root_a;

    /* Increment the rank of a if both subtrees have equal rank */
    if (subsets[root_a].rank == subsets[root_b].rank) ++subsets[root_a].rank;
}

size_t parse_input(char inp[static 1], Point points[B_SIZE]) {
    size_t len = 0;
    char line[64];
    
    FILE* f = fopen(inp, "r");

    while (fgets(line, 64, f)) {
        sscanf(line, "%d, %d, %d, %d",
               &points[len].x, &points[len].y, &points[len].z, &points[len].t);
        ++len;
    }

    fclose(f);

    return len;
}

void determine_subsets(size_t N, Point points[static N], Node subsets[N]) {
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = i + 1; j < N; ++j) {
            int distance = point_manhattan_distance(points[i], points[j]);
            if (distance <= 3) subsets_union(i, j, subsets); 
        }
    }
}

int main(void) {
    Point points[B_SIZE] = { 0 };
    
    size_t len = parse_input(INP, points);
    Node subsets[len];
    
    subsets_init(len, subsets);
    determine_subsets(len, points, subsets);
    size_t number_of_partitions = subsets_number_of_partitions(len, subsets);
    printf("Part: 1: %d\n", number_of_partitions);
    
    return EXIT_SUCCESS;
}