#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#define NNODES 26
#define MIN_AMOUNT 60
#define NUM_WORKERS 5
#define INPUT "d07.txt"

int cmp_size_t(const void* a, const void* b) {
    size_t* sa = (size_t*) a;
    size_t* sb = (size_t*) b;

    if (*sa > *sb)      return 1;
    else if (*sa < *sb) return -1;
    else                return 0;
}

typedef struct Worker Worker;

struct Worker {
    size_t item;
    size_t time;
};

typedef struct Queue Queue;

struct Queue {
    size_t* q;
    size_t len;
    size_t cap;
};

Queue* queue_init(size_t cap) {
    if (!cap) cap = 101;

    Queue* queue = malloc(sizeof(Queue));
    size_t * q = malloc(sizeof(size_t) * cap);

    queue->q = q;
    queue->len = 0;
    queue->cap = cap;

    return queue;
}

void queue_destroy(Queue* queue) {
    if (queue) {
        free(queue->q);
        free(queue);
    }

    queue = 0;
}

size_t queue_push(Queue* queue, size_t element) {
    if (!queue) return -1;

    if (queue->len == queue->cap) {
        queue->cap *= 2;
        queue->q = realloc(queue->q, queue->cap);
    }

    queue->q[queue->len++] = element;

    qsort(queue->q, queue->len, sizeof(size_t), cmp_size_t);

    return queue->len;
}

size_t queue_pop(Queue* queue) {
    if (!queue) return -1;
    if (queue->len == 0) return -1;

    size_t element = queue->q[0];

    --queue->len;
    memmove(queue->q, &queue->q[1], queue->len * sizeof(size_t));

    return element;
}

size_t graph_add_roots_to_queue(Queue* q, size_t nnodes, 
                                size_t graph[nnodes][nnodes]) {

    // root is node with no incoming edges, meaning it is not 
    // contained in any column of the adj matrix
    for(size_t i = 0; i < nnodes; ++i) {
        bool found_root = true;
        for(size_t j = 0; j < nnodes; ++j) {
            if (graph[j][i]) {
                found_root = false;
                break;
            }
        }
        if (found_root) queue_push(q, i); 
        else found_root = true;
    }
    
    return q->len;
}

bool graph_can_enqueue_node(size_t nnodes, size_t graph[nnodes][nnodes],
                              size_t node, size_t visited[nnodes]) {

    if (visited[node]) return false;

    for (size_t inc = 0; inc < NNODES; ++inc) {
        if (graph[inc][node] && !visited[inc]) {
            return false;
        }
    }

    return true;
}

bool all_done(size_t nnodes, size_t visited[nnodes]) {
    for (size_t i = 0; i < nnodes; ++i) {
        if (!visited[i]) return false;
    }

    return true;
}

void solve_1(size_t graph[NNODES][NNODES]) {
    Queue* q = queue_init(100);
    size_t n_roots = graph_add_roots_to_queue(q, NNODES, graph);

    printf("%zu roots in graph.\n", n_roots);

    size_t visited[NNODES] = { 0 };
    size_t element;

    printf("Part 1: ");
    while ((element = queue_pop(q)) != -1) {
        if (visited[element]) continue;

        visited[element] = 1;
        printf("%c", (char) element + 'A');

        for (size_t n = 0; n < NNODES; ++n) {
            if (graph[element][n] && 
                graph_can_enqueue_node(NNODES, graph, n, visited)) {
                queue_push(q, n);
            }
        }
    }
    printf("\n");

    queue_destroy(q);
}

void solve_2(size_t graph[NNODES][NNODES]) {
    Queue* q = queue_init(100);
    size_t visited[NNODES] = { 0 };
    Worker workers[NUM_WORKERS] = { 0 };
    size_t t = 0;

    for (size_t w = 0; w < NUM_WORKERS; ++w) workers[w].item = -1;

    graph_add_roots_to_queue(q, NNODES, graph);

    while (!all_done(NNODES, visited)) {
        for (size_t j = 0; j < NUM_WORKERS; ++j) {
            Worker* w = workers + j; 
            size_t element;
            if (w->time > t) continue; 
            // complete task if there is one
            if (w->item != -1) {
                visited[w->item] = 1;

                for (size_t n = 0; n < NNODES; ++n) {
                    if (graph[w->item][n] && 
                        graph_can_enqueue_node(NNODES, graph, n, visited)) {
                        queue_push(q, n);
                        j = 0;
                    }
                }
                w->item = -1;
            }

            // grab the next task from the queue if there is one
            if ((element = queue_pop(q)) != -1 && !visited[element]) {
                w->item = element;
                w->time = t + element + 1 + MIN_AMOUNT;
            }
        }
        ++t;
    }
    --t;

    queue_destroy(q);
    printf("Part 2: %zu\n", t);
}

int main(void) {
    char line[64];
    size_t graph[NNODES][NNODES] = { 0 };     
    FILE* f = fopen(INPUT, "r"); 

    if (!f) {
        fprintf(stderr, "Unable to open file");
        return EXIT_FAILURE;
    }

    while(fgets(line, 64, f)) {
        char tail, head;
        sscanf(line, "Step %c must be finished before step %c can begin.", 
                &tail, &head);
        graph[tail - 'A'][head - 'A'] = 1;
    }

    fclose(f);

    solve_1(graph);
    solve_2(graph);

    return EXIT_SUCCESS;
}
