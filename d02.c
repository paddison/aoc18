#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

typedef enum CharCount CharCount;

enum CharCount {
    NONE,
    TWO,
    THREE,
    BOTH
};

int cmp_char(const void* a, const void* b) {
    const char* l = a;
    const char* r = b;
    if (!*l) {
        return 1;
    } else if (!*r) {
        return -1;
    } else {
        return *l > *r ? 1 : -1;
    }
}

CharCount count_line(size_t n, char s[n]) {
    size_t two_count = 0;
    size_t three_count = 0;
    size_t cur_count = 1;

    qsort(s, n, sizeof(char), cmp_char);

    for (size_t i = 1; i < n - 1; ++i) {
        if (s[i] == s[i - 1]) {
            ++cur_count;
        } else {
            if (cur_count == 2) 
                ++two_count;
            else if (cur_count == 3) 
                ++three_count;
            cur_count = 1;
        }

    }

    if (two_count && three_count) 
        return BOTH;
    else if (two_count && !three_count) 
        return TWO;
    else if (!two_count && three_count) 
        return THREE;
    else 
        return NONE;
}

int find_diffs(size_t n, char s1[n], char s2[n]) {
    int pos = -1;
    for (int i = 0; i < n; ++i) {
        if (s1[i] != s2[i]) {
            if (pos == -1) {
                pos = i;
            } else {
                return -1;
            }
        }

    }
    return pos;
}

void copy_diff_string(size_t n, char s[n], char diff[n - 1], int pos) {
    assert(pos != -1);

    for (size_t i = 0; i < n; ++i) {
        if (i != pos) diff[i] = s[i];
    }
}

int main(int argc, char* argv[argc]) {
    FILE* input = fopen("d02.txt", "r");
    size_t n = 5;
    size_t cap = 30;

    char** ids = malloc(sizeof(char*) * cap);
    size_t line_count = 0;

    size_t twos = 0;
    size_t threes = 0;

    char diff_string[n - 1];

    while(true) {
        char* line  = malloc(sizeof(char) * n);

        ssize_t res = getline(&line, &n, input);
        if (res == -1) {
            free(line);
            break;
        }
        ids[line_count] = line;
        ++line_count;

        if (line_count == cap) {
            cap *= 2;
            ids = realloc(ids, sizeof(char*) * cap);
        }
    }

    /* Part 2 */
    for (size_t i = 0; i < line_count; ++i) {
        for (size_t j = i + 1; j < line_count; ++j) {
            size_t pos = find_diffs(n, ids[i], ids[j]);
            if (pos != -1) {
                copy_diff_string(n, ids[i], diff_string, pos); 
            }
        }
    }

    /* Part 1 */
    for (size_t i = 0; i < line_count; ++i) {
        switch (count_line(n, ids[i])) {
            case TWO: ++twos;
                      break;
            case THREE: ++threes;
                        break;
            case BOTH: ++twos;
                       ++threes;
                       break;
            case NONE: break;
        }

    }

    printf("Part 1: %zu\n", twos * threes);

    printf("Part 2: %s", diff_string);
    return EXIT_SUCCESS;
}
