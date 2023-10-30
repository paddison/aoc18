#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define M 10
#define L 510
#define INP "d12.txt"
#define TEST "d12_test.txt"

typedef struct Rule Rule;

struct Rule {
    char in[5];
    char out;
};

bool rule_matches(Rule* r, char plants[static 5]) {
    for (size_t i = 0; i < 5; ++i) {
        if (r->in[i] != plants[i]) return false;
    }

    return true;
}

int rules_apply(size_t n_rules, Rule rules[n_rules], 
                 char target[L], char source[L]) {  
    size_t count = 0;

    for (size_t i = 2; i < L - 5; ++i) {
        for (size_t r = 0; r < n_rules; ++r) {
            if (rule_matches(rules + r, source + i - 2)) {
                target[i] = rules[r].out;
                if (rules[r].out == '#') ++count;
                break;
            } else {
                target[i] = '.';
            }
        }
    }

    return count;
}

int64_t get_sum(char plants[L], int64_t shift) {
    int64_t sum = 0;
    for (size_t i = 0; i < L; ++i) {
        if (plants[i] == '#') sum += i - M + shift;
    }
    return sum;
}

void part_one(size_t n_rules, Rule rules[n_rules], char plants[L]) {
    char next[L];
    next[0] = next[1] = '.';

    for (size_t i = 0; i < 20; ++i) {
        if (i % 2) {
            rules_apply(n_rules, rules, plants, next);
        } else {
            rules_apply(n_rules, rules, next, plants);
        }
    }

    printf("Part 1: %lld\n", get_sum(plants, 0));
}

void part_two(size_t n_rules, Rule rules[n_rules], char plants[L]) {
    size_t count = 0;
    size_t no_changes = 0;
    size_t prev_count = 0;
    size_t n_iterations;
    char next[L];
    next[0] = next[1] = '.';

    for (size_t i = 0;; ++i) {
        if (i % 2) {
            count = rules_apply(n_rules, rules, plants, next);
        } else {
            count = rules_apply(n_rules, rules, next, plants);
        }
        if (count == prev_count) ++no_changes;
        else {
            prev_count = count;
            no_changes = 0;
        }
        if (no_changes == 100) {
            n_iterations = i;
            break;
        }
    }

    printf("Part 2: %lld\n", get_sum(plants, 50000000000 - n_iterations));
}

int main(void) {
    char plants[L]; 
    char plants2[L];
    Rule rules[50];
    size_t n_rules = 0;
    char line[200];

    FILE* f = fopen(INP, "r");

    if (!f) {
        fprintf(stderr, "Unable to open file");
        return EXIT_FAILURE;
    }

    /* parse initial state */
    fgets(line, 200, f);
    
    char* start = strchr(line, '#');
    for (size_t i = 0; i < L; ++i) plants[i] = '.';
    for (size_t i = 0; *(start + i) != '\n'; ++i) plants[M + i] = *(start + i);

    /* parse generator rules */
    fgets(line, 200, f);

    while (fgets(line, 200, f)) {
        Rule r = { 0 };
        sscanf(line, "%c%c%c%c%c => %c",
               &r.in[0], &r.in[1], &r.in[2], &r.in[3], &r.in[4], &r.out); 
        rules[n_rules++] = r;
    }

    fclose(f);

    memcpy(plants2, plants, L);

    part_one(n_rules, rules, plants);
    part_two(n_rules, rules, plants2);

    return EXIT_SUCCESS;
}
