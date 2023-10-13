#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>

typedef struct entry entry;
struct entry {
    int items[5];
    size_t size;
};

void init_groups(size_t N, entry shift_groups[N]) {
    entry def = {
        .items = { 0 },
        .size = 0
    };

    for (size_t i = 0; i < N; ++i) {
        shift_groups[i] = def;
    }
}

int cmp(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

size_t count_lines(FILE* f) {
    size_t lines = 0;
    while (!feof(f)) {
         if (fgetc(f) == '\n') {
            ++lines;
        }   
    }
    return lines;
}

int parse_line_to_int(FILE* f) {
    char buf[10] = { 0 };
    char* ptr;
    fgets(buf, 10, f);
    return strtol(buf, &ptr, 10);
}

int get_solution_1(size_t N, int A[N]) {
    int sum = 0;
    for (size_t i = 0; i < N; ++i) {
        sum += A[i];
    }
    return sum;
}

int get_solution_2(size_t N, int A[N]) {
    int seen[N];
    int sum = 0;
    size_t seen_len = 0;
    // determine sums after first iteration
    for (size_t i = 0; i < N; ++i) {
        seen[i] = sum;
        sum += A[i];
    }

    // determine the shift:
    int shift = sum;
    assert(shift == 472);
    // divide into groups
    entry shift_groups[shift];
    init_groups(shift, shift_groups);

    for (size_t i = 0; i < N; ++i) {
        size_t index = (seen[i] % shift + shift) % shift;
        entry* e = &shift_groups[index];
        e->items[e->size++] = seen[i];
    }

    // sort the elements for each index
    // min entries
    int mins[shift][2];
    for (size_t i = 0; i < shift; ++i) {
        entry* e = &shift_groups[i];
        if (e->size > 1) {
            qsort(e->items, e->size, sizeof(int), cmp);
            // now the element with minimum distance is at the first positions
            mins[i][0] = e->items[1] - e->items[0];
            mins[i][1] = e->items[1];
        } else {
            mins[i][0] = INT_MAX;
        }
    }

    int min_item = 0;
    int min_distance = INT_MAX - 1; 

    for (size_t i = 0; i < shift; ++i) {
        if (mins[i][0] < min_distance) {
            min_item = mins[i][1];
        } else if (mins[i][0] == min_distance) {
            for (size_t j = 0; j < N; ++j) {
                if (A[j] == min_item) {
                    break;
                } else if (A[j] == mins[i][1]) {
                    min_item =  mins[i][1];
                }
            }            
        }
    }

    return min_item;

    printf("unable to find repeating element\n");
    exit(EXIT_FAILURE);
}

int main(void) {
    FILE* f = fopen("d0.txt", "r");    
    size_t N = count_lines(f);
    int A[N]; 

    rewind(f);

    for (int i = 0; i < N; ++i) {
        A[i] = parse_line_to_int(f);
    }    

    int p1 = get_solution_1(N, A);
    printf("P1: %i\n", p1);

    int p2 = get_solution_2(N, A);
    printf("P2: %i\n", p2);

    return EXIT_SUCCESS;
}
