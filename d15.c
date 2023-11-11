#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define INP "d15.txt"

#define U(p) (p - width)
#define D(p) (p + width)
#define L(p) (p - 1)
#define R(p) (p + 1)

typedef struct MetaData MetaData;

struct MetaData {
    size_t len;
    size_t width;
    size_t height;
    size_t n_elves;
    size_t n_goblins;
};

enum Type {
    Elf,
    Goblin,
    Wall,
    Empty,
};

char type_to_char(enum Type t) {
    switch (t) {
        case Elf: return 'E';      
        case Goblin: return 'G';   
        case Wall: return '#';     
        case Empty: return '.';    
    }
}

typedef struct Unit Unit;

struct Unit {
    enum Type t;
    int hp;
};

size_t nearest_path(size_t width, size_t height, size_t start, enum Type target,
                    Unit map[width * height], size_t path[width * height]) {
#define ADD(N) path[N] = cur; queue[tail++] = N; visited[N] = true;

    size_t cur, head = 0, tail = 0;
    size_t queue[width * height * 4];
    bool visited[width * height];

    for (size_t i = 0; i < width * height; ++i) {
        visited[i] = false;
        path[i] = -1;
    }

    queue[tail++] = start;
    visited[start] = true;
    while (head != tail) {
        cur = queue[head++];
        size_t x = cur % width, y = cur / width;

        if (map[cur].t != Empty && cur != start) {
            if (map[cur].t == target) return cur;
            else continue;
        }
        
        if (y && !visited[U(cur)]) {
            ADD(U(cur))
        }
        if (x && !visited[L(cur)]) {
            ADD(L(cur))
        }
        if (x < width && !visited[R(cur)]) {
            ADD(R(cur))
        }
        if (y < height && !visited[D(cur)]) {
            ADD(D(cur))
        }
    }

    return -1;
}

size_t next_pos(size_t width, size_t height, Unit map[width * height], 
                 size_t start, enum Type target) {
    size_t path[width * height];
    size_t cur = nearest_path(width, height, start, target, map, path);

    if (cur == -1) return -1;

    while (path[cur] != start) cur = path[cur];

    if (map[cur].t != Empty) return -1;

    return cur;
}

size_t move(size_t width, size_t height, 
          Unit map[width * height], size_t pos, enum Type target) {
    size_t next = next_pos(width, height, map, pos, target); 

    if (next == -1) return pos;

    map[next] = map[pos];
    map[pos] = (Unit) { .t = Empty, .hp = 0 };
    return next;
}

size_t get_target(size_t width, size_t height, 
                Unit map[width * height], size_t pos, enum Type target) {
    size_t min = 201;
    size_t target_pos = -1;
    size_t x = pos % width, y = pos / width;

    if (y && map[U(pos)].t == target && map[U(pos)].hp < min) {
        target_pos = U(pos);
        min = map[U(pos)].hp;
    }
    if (x && map[L(pos)].t == target && map[L(pos)].hp < min) {
        target_pos = L(pos);
        min = map[L(pos)].hp;
    }
    if (x < width && map[R(pos)].t == target && map[R(pos)].hp < min) {
        target_pos = R(pos);
        min = map[R(pos)].hp;
    }
    if (y < height && map[D(pos)].t == target && map[D(pos)].hp < min) {
        target_pos = D(pos);
        min = map[D(pos)].hp;
    }

    return target_pos;
}

bool attack(size_t target, Unit map[static target + 1], int atk) {
    map[target].hp -= atk;
    if (map[target].hp <= 0) {
        map[target] = (Unit) { .t = Empty, .hp = 0 };
        return true;
    } 
    return false;
}

void determine_turns(size_t width, size_t height, Unit map[width * height], 
                     size_t n_units, size_t turns[n_units]) {
    size_t cur = 0;
    for (size_t pos = 0; pos < width * height; ++pos) {
        if (map[pos].t == Goblin || map[pos].t == Elf) {
            turns[cur++] = pos;
        }
    }
}

size_t simulate(size_t width, size_t height, Unit map[width * height],
              size_t n_goblins, size_t n_elves, bool p2, int atk) {
    size_t turns[n_goblins + n_elves];
    size_t n_turns = 0;

    while (1) {
        size_t n_units = n_goblins + n_elves;
        determine_turns(width, height, map, n_units, turns);

        for (size_t i = 0; i < n_units; ++i) {
            if (n_goblins == 0 || n_elves == 0) return n_turns;
            if (map[turns[i]].t == Empty) continue;

            enum Type target = map[turns[i]].t == Goblin ? Elf : Goblin;
            size_t next_pos = move(width, height, map, turns[i], target);
            size_t target_pos = get_target(width, height, map, next_pos, target);

            if (target_pos != -1 && 
                attack(target_pos, map, target == Elf ? 3 : atk)) {

                if (p2 && target == Elf) return -1;

                target == Elf ? --n_elves : --n_goblins;
            }
        }
        ++n_turns;
    }
}

int sum_hp(size_t len, Unit map[len]) {
    int hp = 0;
    for (size_t i = 0; i < len; ++i) {
        if (map[i].t == Goblin || map[i].t == Elf) {
            hp += map[i].hp;
        }
    }
    return hp;
}

MetaData* read_data(char file[static 1], MetaData* d, 
        size_t cap, Unit map[cap]) {
    
    FILE* f = fopen(INP, "r"); 
    char line[256];

    if (!f) {
        fprintf(stderr, "unable to open file\n");
        return 0;
    }

    while (fgets(line, 256, f)) {
        for (d->width = 0; line[d->width] != '\n'; ++d->width) {
            enum Type t;
            switch (line[d->width]) {
                case '.': t = Empty;                    break;
                case '#': t = Wall;                     break;
                case 'E': t = Elf;      ++d->n_elves;   break;
                case 'G': t = Goblin;   ++d->n_goblins; break;
                default: fprintf(stderr, "Tried to read invalid char");
                         return 0;
            }
            map[d->len++] = (Unit) { .t = t, .hp = 200 };
        }
        ++d->height;
    }

    fclose(f);

    return d;
}

int main(void) {
    const size_t CAP = 2048;
    Unit map_immutable[CAP] = { 0 };
    Unit map[CAP] = { 0 };
    MetaData d = { 0 };

    if (!read_data(INP, &d, CAP, map_immutable)) return EXIT_FAILURE;

    memcpy(map, map_immutable, sizeof(Unit) * d.len);

    size_t n_turns = simulate(d.width, 
            d.height, 
            map, 
            d.n_goblins, 
            d.n_elves, 
            false, 3);

    printf("Part 1: %zu\n", n_turns * sum_hp(d.len, map));

    for (int atk = 3;; ++atk) {
        memcpy(map, map_immutable, sizeof(Unit) * d.len);
        n_turns = simulate(d.width, 
                d.height, 
                map, 
                d.n_goblins, 
                d.n_elves, 
                true, atk);
        if (n_turns != -1) break;
    }
    
    printf("Part 2: %zu\n", n_turns * sum_hp(d.len, map));
    return EXIT_SUCCESS;
}
