#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <assert.h>

typedef struct tm tm;

typedef struct Entry Entry;

struct Entry {
    tm date;
    char* info;
};

void entry_destroy(Entry* e) {
    free(e->info);
}

int entry_sort(const void* a, const void* b) {
    const Entry* e1 = a;
    const Entry* e2 = b;
    
    tm d1 = e1->date;
    tm d2 = e2->date;

    time_t date1 = mktime(&d1);
    time_t date2 = mktime(&d2);

    return date1 - date2;
}

typedef struct Table Table;

struct Table {
    size_t** guards;
    size_t len;
};

Table* table_init(size_t len) {
    Table* t = malloc(sizeof(Table));

    *t = (Table) {
        .len = len,
        .guards = calloc(len, sizeof(size_t*))
    };

    return t;
}

void table_destroy(Table* t) {
    for (size_t i = 0; i < t->len; ++i) {
        if (t->guards[i]) free(t->guards[i]);
    }

    free(t->guards);
    t->len = 0;
}

size_t table_max_asleep_guard(Table* t) {
    size_t max_guard = 0;
    size_t max_time_asleep = 0;

    for (size_t i = 0; i < t->len; ++i) {
        size_t* guard = t->guards[i];
        if (guard) {
            size_t time_asleep = 0;
            for (size_t j = 0; j < 60; ++j) {
                time_asleep += guard[j];
            }
            if (time_asleep > max_time_asleep) {
                max_guard = i;
                max_time_asleep = time_asleep;
            }
        }
    }

    return max_guard;
}

size_t table_most_asleep_minute(Table* t, size_t guard) {
    size_t max_minute = 0;
    size_t max_minute_asleep = 0;
    size_t* guard_table = t->guards[guard];

    for (size_t i = 0; i < 60; ++i) {
        if (guard_table[i] > max_minute_asleep) {
            max_minute = i;
            max_minute_asleep = guard_table[i];
        }
    }

    return max_minute;
}

int main(void) {
    FILE* data = fopen("d04.txt", "r");

    if (!data) {
        fprintf(stderr, "unable to open file");
        return EXIT_FAILURE;
    }
    
    char line[64];
    size_t n_entries = 1085;
    Entry entries[n_entries];

    /* read in data, so it can be sorted by date */
    for (size_t i = 0; fgets(line, 64, data); ++i) {
        tm date;
        char* info = malloc(32);

        sscanf(line, "[%d-%d-%d %d:%d] %[A-Za-z#0-9 ]", 
                &date.tm_year,
                &date.tm_mon,
                &date.tm_mday,
                &date.tm_hour,
                &date.tm_min,
                info);

        entries[i] = (Entry) { .date = date, .info = info };
    }

    fclose(data);

    /* sort entries so we can analyze them */
    qsort(entries, n_entries, sizeof(Entry), entry_sort);

    /* Build table for guards sleep schedule */
    Table* table = table_init(4000);
    int id = 0;
    int sleep_start = 0;

    for(size_t i = 0; i < n_entries; ++i) {
        Entry entry = entries[i];
        if (strchr(entry.info, 'G')) {
            /* get guard id */
            sscanf(entry.info, "Guard #%d begins shift", &id);
            assert(id < table->len);
            if (!table->guards[id]) {
                table->guards[id] = calloc(60, sizeof(size_t));
            }
        } else if (strspn(entry.info, "falls")) {
            sleep_start = entry.date.tm_min;
        } else {
            /* note time guard was asleep */
            for (size_t i = sleep_start; i < entry.date.tm_min; ++i) {
                (table->guards[id])[i] += 1;
            }
        }
    }

    /* deallocate entries */
    for (size_t i = 0; i < n_entries; ++i)
        entry_destroy(entries + i);

    /* solve part 1 */
    size_t guard = table_max_asleep_guard(table);
    size_t minute = table_most_asleep_minute(table, guard);

    printf("Part 1: %zu\n", guard * minute);

    /* solve part 2 */
    size_t max_minute = 0;
    size_t max_min_asleep = 0;
    size_t max_guard = 0;

    for (size_t i = 0; i < table->len; ++i) {
        size_t* guard = table->guards[i];
        if (guard) {
            for (size_t j = 0; j < 60; ++j) {
                if (guard[j] > max_min_asleep) {
                    max_minute = j;
                    max_min_asleep = guard[j];
                    max_guard = i;
                }
            }
        }
    }

    printf("Part 2: %zu\n", max_guard * max_minute);

    /* deallocate table */
    table_destroy(table);

    return EXIT_SUCCESS;
}
