#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

size_t parse_tree(size_t cur, size_t len, size_t nums[len], size_t* sum) {
    size_t n_parsed = 2; // header data
    size_t n_children = nums[cur];
    size_t n_metadata = nums[cur + 1];
    size_t value = 0;
    size_t child_values[n_children];

    if (n_children) {
        // children propagate their values upwards
        for (size_t child = 0; child < n_children; ++child) {
            size_t data = parse_tree(cur + n_parsed, len, nums, sum);
            n_parsed += (data >> 32);
            child_values[child] = data & 0xFFFF; 
        }

        for (size_t i = 0; i < n_metadata; ++i) {
            // read in child values specified by metadata
            size_t id = nums[cur + n_parsed + i] - 1;
            if (id < n_children) value += child_values[id];
        }
    } else {
        // no children -> value is sum of metadata
        for (size_t i = 0; i < n_metadata; ++i) {
            value += nums[cur + n_parsed + i];
        }
    }

    for (size_t i = 0; i < n_metadata; ++i) {
        // part 1
        *sum += nums[cur + n_parsed + i];
    }

    assert(sizeof(size_t) == 8);

    return ((n_parsed + n_metadata) << 32) | value;
}


int main(void) {
    char buf[1024];
    size_t cap = 32;
    size_t len = 0;
    size_t* nums = malloc(sizeof(size_t) * cap);

    while(fgets(buf, 1024, stdin) > 0) {
        if (buf[0] == '\n') break;

        char* cur = buf;

        for (char* next = 0; cur[0]; cur = next) {
            size_t n = strtoul(cur, &next, 10);

            if (cur == next) break;

            if (len == cap) {
                cap <<= 1;
                nums = realloc(nums, sizeof(size_t) * cap);
            }

            nums[len++] = n;
        };
    }

    size_t sum = 0;

    size_t value = parse_tree(0, len, nums, &sum) & 0xFFFF;

    printf("Part 1: %zu\n", sum);
    printf("Part 2: %zu\n", value);

    return EXIT_SUCCESS;
}
