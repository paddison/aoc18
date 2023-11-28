#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define TEST "./data/d24_test.txt"
#define INP "./data/d24.txt"

enum Type { RAD, BLUD, FIRE, SLASH, COLD };

enum Type str_to_type(char s[static 1]) {
    switch (s[0]) {
        case 'r': return RAD;
        case 'b': return BLUD;
        case 'f': return FIRE;
        case 's': return SLASH;
        default: return COLD;
    }
}

void type_print(enum Type t) {
    switch (t) {
        case RAD: printf("radiation"); break;
        case BLUD: printf("bludgeoning"); break;
        case FIRE: printf("fire"); break;
        case SLASH: printf("slashing"); break;
        case COLD: printf("cold"); break;
    }
}

typedef struct Group Group;

struct Group {
    int units;
    int hp;
    int init;
    int atk;
    enum Type atk_t;
    enum Type weak[3]; 
    size_t n_weak;
    enum Type immun[3];
    size_t n_immun;
};

void group_print(Group* g) {
    printf("%d units\t%d hp\t%d atk\t%d init\tatk_t: ", g->units, g->hp, g->atk, g->init);
    type_print(g->atk_t);
    printf("\nweaknesses:\t");
    for (size_t i = 0; i < g->n_weak; ++i) {
        type_print(g->weak[i]);
        printf(", ");
    }
    printf("\nimmunities:\t");
    for (size_t i = 0; i < g->n_immun; ++i) {
        type_print(g->immun[i]);
        printf(", ");
    }
    printf("\n");
}

int group_effective_power(Group* g) {
    return g->units * g->atk;
}

int group_cmp(const void* a, const void* b) {
    Group* aa = (Group*) a;
    Group* bb = (Group*) b;
    int ep_a = group_effective_power(aa);
    int ep_b = group_effective_power(bb);

    if (ep_a > ep_b) return -1;
    else if (ep_a < ep_b) return 1;
    else return bb->init - aa->init;
}

int calc_damage(Group* atk, Group* def) {
    int dmg = group_effective_power(atk);

    for (size_t i = 0; i < def->n_weak; ++i) {
        if (atk->atk_t == def->weak[i]) return dmg * 2;
    }

    for (size_t i = 0; i < def->n_immun; ++i) {
        if (atk->atk_t == def->immun[i]) return 0;
    }

    return dmg;
}

void choose_targets(size_t n_atk, Group atk[n_atk], size_t targets[n_atk], 
                    size_t n_def, Group def[n_def], size_t selected[n_def]) {
    // targets are targets for an attacker
    // selected stores which defending unit is already selected by an attacker
    for (size_t i = 0; i < n_atk; ++i) {
        // ignore groups which are dead
        if (atk[i].units <= 0) continue;
        //printf("%d\n", atk[i].init);
        int max = -1;
        size_t max_id = -1;
        for (size_t j = 0; j < n_def; ++j) {
            // ignore groups which are dead
            if (def[j].units <= 0 || selected[j]) continue;
            int dmg = calc_damage(&atk[i], &def[j]);
            if (!dmg) continue;
            if (dmg > max) { 
                max = dmg;
                max_id = j;
            } else if (dmg == max) {
                int ep_new = group_effective_power(&def[j]);
                int ep_old = group_effective_power(&def[max_id]);
                if (ep_new > ep_old) {
                    max = dmg;
                    max_id = j;
                } else if (ep_new == ep_old) {
                    if (def[j].init > def[max_id].init) {
                        max = dmg;
                        max_id = j;
                    }
                }
            }
        }
        if (max_id != -1) {
            //printf("%d chooses %d\n", atk[i].init, def[max_id].init);
            targets[i] = max_id;
            selected[max_id] = 1;
        }
    }
}

int target_selection(size_t n_immun, size_t n_infect, 
                     Group immun[n_immun], size_t immun_targets[n_immun], 
                     Group infect[n_infect], size_t infect_targets[n_infect]) {
    // which of the targets have been selected so far
    size_t immun_selected[n_infect];
    size_t infect_selected[n_immun];

    for (size_t i = 0; i < n_immun; ++i) {
        immun_targets[i] = -1;
        infect_selected[i] = 0;
    }
    for (size_t i = 0; i < n_infect; ++i) {
        infect_targets[i] = -1;
        immun_selected[i] = 0;
    }

    // groups are always sorted by effective power
    qsort(immun, n_immun, sizeof(Group), group_cmp);
    qsort(infect, n_infect, sizeof(Group), group_cmp);

    // target selection immune system
    choose_targets(n_immun, immun, immun_targets, n_infect, infect, immun_selected);
    choose_targets(n_infect, infect, infect_targets, n_immun, immun, infect_selected);

    return 0;
}

void execute_attack(Group* atk, Group* def) {
    // BAM BAM
    int dmg = calc_damage(atk, def);
    int units_killed = dmg / def->hp;
    printf("%d attacks %d: %d damage, killing %d units\n", atk->init, def->init, dmg, units_killed);
    def->units -= units_killed;
}

void attack(size_t n_immun, size_t n_infect,
            Group immun[n_immun], size_t immun_targets[n_immun],
            Group infect[n_infect], size_t infect_targets[n_infect]) {
    // since we cannot sort the groups again, we manually detect which group is next
    // initiative is sort of an id, so just start from n_immun + n_infect and go down
    // calculate battle plan for each Group
    
    for (size_t i = n_immun + n_infect; i > 0; --i) {
        for (size_t j = 0; j < n_immun; ++j) {
            if (immun[j].init == i && immun[j].units > 0 && immun_targets[j] != -1) 
                execute_attack(&immun[j], &infect[immun_targets[j]]);
        }
        for (size_t j = 0; j < n_infect; ++j) {
            if (infect[j].init == i && infect[j].units > 0 && infect_targets[j] != -1) 
                execute_attack(&infect[j], &immun[infect_targets[j]]);
        }
    }
    printf("\n");
}

void do_round(size_t n_immun, size_t n_infect, 
              Group immun[n_immun], Group infect[n_infect]) {
    size_t immun_targets[n_immun];
    size_t infect_targets[n_infect];
    target_selection(n_immun, n_infect, immun, immun_targets, infect, infect_targets);
    attack(n_immun, n_infect, immun, immun_targets, infect, infect_targets);
}

int calc_score(size_t n, Group win[n]) {
    int result = 0;
    for (size_t i = 0; i < n; ++i) {
        if (win[i].units > 0) result += win[i].units;
    }
    return result;
}

int has_lost(size_t n, Group g[n]) {
    for (size_t i = 0; i < n; ++i) {
        if (g[i].units > 0) return 0;
    }
    return 1;
}

int do_battle(size_t n_immun, size_t n_infect, 
              Group immun[n_immun], Group infect[n_infect]) {
    while (1) {
        do_round(n_immun, n_infect, immun, infect); 
        if (has_lost(n_immun, immun)) return calc_score(n_infect, infect);
        if (has_lost(n_infect, infect)) return calc_score(n_immun, immun);
    }
}

void parse_group(char line[static 1], Group* g) {
        char atk_t[64] = { 0 };
        g->n_immun = 0;
        g->n_weak = 0;
        char* weak = malloc(256);
        if (strchr(line, '(')) {
            sscanf(line, "%d units each with %d hit points (%[a-z ;,]) with an attack that does %d %s damage at initiative %d",
                   &g->units, &g->hp, weak, &g->atk, atk_t, &g->init);
            char* weaknesses[10];
            char** p;
            enum Type* to_parse;
            size_t* to_increment;
            for (p = weaknesses; (*p = strsep(&weak, ", ;")) != NULL;) {
                if (!strcmp(*p, "to") || !strcmp(*p, "") ) { 
                    continue;
                } else if (!strcmp(*p, "weak")) {
                    to_parse = g->weak;
                    to_increment = &g->n_weak;
                } else if (!strcmp(*p, "immune")) {
                    to_parse = g->immun;
                    to_increment = &g->n_immun;
                } else  { 
                    to_parse[(*to_increment)++] = str_to_type(*p);
                }
            }
            
        } else {
            sscanf(line, "%d units each with %d hit points with an attack that does %d %s damage at initiative %d",
                   &g->units, &g->hp, &g->atk, atk_t, &g->init);
            g->n_immun = 0;
            g->n_weak = 0;
        }
        g->atk_t = str_to_type(atk_t);
        group_print(g);
        printf("\n");
        free(weak);
}

size_t parse_input(char input[static 1], size_t n_immun, size_t n_infect, Group immun[n_immun], Group infect[n_infect]) {
    char line[256] = { 0 };
    char atk_t[64] = { 0 };
    FILE* f = fopen(input, "r");

    if (!f) {
        fprintf(stderr, "Unable to open file\n");
        return 0;
    }
    fgets(line, 256 ,f);
    printf("%s", line);
    //assert(!strcmp(line, "Immune System:"));

    for (size_t i = 0; i < n_immun; ++i) {
        fgets(line, 256 ,f);
        parse_group(line, &immun[i]);
    }

    fgets(line, 256, f);
    fgets(line, 256, f);
    
    printf("%s", line);

    for (size_t i = 0; i < n_infect; ++i) {
        fgets(line, 256, f);
        parse_group(line, &infect[i]);
    }
   
    return 1;
}

int main(void) {
    const size_t N = 2;
    Group immun[N] = { 0 };
    Group infect[N] = { 0 };
    parse_input(TEST, N, N, immun, infect);

    printf("Part 1: %d\n", do_battle(N, N, immun, infect));

    return EXIT_SUCCESS;
}
