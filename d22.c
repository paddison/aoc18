#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define TEST 10, 10, 510
#define INP 11, 722, 10689

enum Tile { ROCKY, WET, NARROW };

enum Tool { NONE, TORCH, GEAR };

typedef struct State State;

struct State {
    size_t pos;
    size_t steps;
    enum Tool tool;
};

State state_new(size_t pos, size_t steps, enum Tool tool) {
    return (State) { .pos = pos, .steps = steps, .tool = tool };
}

typedef struct BinaryHeap BHeap;

struct BinaryHeap {
    State* items;
    size_t len;
    size_t cap;
};

BHeap* bheap_init(BHeap* h, size_t cap) {
    h->items = malloc(sizeof(State) * cap);
    h->cap = cap;
    h->len = 0;
    return h;
}

void bheap_insert(BHeap* h, State item) {
    if (h->len == h->cap) {
        h->cap *= 2;
        h->items = realloc(h->items, sizeof(State) * h->cap);
    }
    size_t i = h->len;

    h->items[h->len++] = item;

    while (h->items[i].steps < h->items[(i - 1) / 2].steps) {
        State tmp = h->items[i];
        h->items[i] = h->items[(i - 1) / 2];
        h->items[(i - 1) / 2] = tmp;
        i = (i - 1) / 2;
    }
}

State bheap_remove(BHeap* h) {
    State ret = h->items[0];
    size_t i = 0;
    h->items[i] = h->items[--h->len];

    while (1) {
        size_t min = i, left = i * 2 + 1, right = i * 2 + 2;

        if (left < h->len && h->items[min].steps > h->items[left].steps) {
            min = left;
        }

        if (right < h->len && h->items[min].steps > h->items[right].steps) {
            min = right;
        }

        if (min != i) {
            State tmp = h->items[min];
            h->items[min] = h->items[i];
            h->items[i] = tmp;
            i = min;
        } else {
            break;
        }
    }

    return ret;
}

typedef struct Map Map;

struct Map {
    size_t* m;
    size_t width;
    size_t height;
    size_t depth;
    size_t t_x;
    size_t t_y;
};

Map* map_init(Map* map, size_t t_x, size_t t_y, size_t depth) {
    size_t width = t_x + 40;
    size_t height = t_y + 40;
    map->m = malloc(sizeof(size_t) * width * height);
    map->width = width;
    map->height = height;
    map->depth = depth;
    map->t_x = t_x;
    map->t_y = t_y;

    return map;
}

size_t map_get(Map* map, size_t x, size_t y) {
    return  map->m[map->width * y + x];
}

size_t map_geological_index(Map* map, size_t x, size_t y) {
    if (y == 0 && x == 0) return 0;
    if (y == map->t_y && x == map->t_x) return 0;
    if (y == 0) return x * 16807;
    if (x == 0) return y * 48271;
    
    return map_get(map, x - 1, y) * map_get(map, x, y - 1);
}

void map_determine_erosion_levels(Map* map) {
    for (size_t y = 0; y < map->height; ++y) {
        for (size_t x = 0; x < map->width; ++x) {
            map->m[map->width * y + x] = 
                (map_geological_index(map, x, y) + map->depth) % 20183;
        }
    }
}

size_t map_determine_risk_level(Map* map) {
    size_t risk_level = 0;
    for (size_t y = 0; y <= map->t_y; ++y) {
        for (size_t x = 0; x <= map->t_x; ++x) {
            risk_level += map->m[map->width * y + x] % 3;
        }
    }
    return risk_level;
}

size_t dijkstra(Map* map, size_t goal) {
    BHeap queue;
    // stored as position * 3 + equipped tool
    size_t visited[map->width * map->height * 3];

    for (size_t i = 0; i < map->width * map->height * 3; ++i) visited[i] = SIZE_MAX; 

    bheap_init(&queue, map->width * map->height);
    bheap_insert(&queue, state_new(0, 0, TORCH));

    while(queue.len) {
        // find min element in queue
        State cur;
        size_t next_positions[4];
        size_t n_next_positions = 0;
       
        cur = bheap_remove(&queue);

        if (visited[cur.pos * 3 + cur.tool] <= cur.steps) continue;
        if (cur.pos == goal && cur.tool == TORCH) return cur.steps;

        visited[cur.pos * 3 + cur.tool] = cur.steps;
        // switch the tool 
        for (size_t tool = 0; tool < 3; ++tool) {
            if (tool != cur.tool && map->m[cur.pos] % 3 != tool) {
                bheap_insert(&queue, state_new(cur.pos, cur.steps + 7, tool));
            }
        }

        // up
        if (cur.pos / map->width > 0 ) 
            next_positions[n_next_positions++] = cur.pos - map->width;
        // left
        if (cur.pos % map->width ) 
            next_positions[n_next_positions++] = cur.pos - 1; 
        // down
        if (cur.pos / map->width < map->height - 1) 
            next_positions[n_next_positions++] = cur.pos + map->width; 
        // right
        if (cur.pos % map->width < map->width - 1) 
            next_positions[n_next_positions++] = cur.pos + 1; 

        for (size_t i = 0; i < n_next_positions; ++i) {
            if (map->m[next_positions[i]] % 3 != cur.tool) {
                bheap_insert(&queue, state_new(next_positions[i], cur.steps + 1, cur.tool));
            }
        }
    }
    return -1;
}

int main(void) {
    Map map;

    map_init(&map, INP);
    map_determine_erosion_levels(&map);

    printf("Part 1: %zu\n", map_determine_risk_level(&map));
    printf("Part 2: %zu\n", dijkstra(&map, map.t_y * map.width + map.t_x));
    return EXIT_SUCCESS;
}
