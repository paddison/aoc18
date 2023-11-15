#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define SAMPLES "./data/d16_samples.txt"
#define PROGRAM "./data/d16_program.txt"

typedef int Registers[4];
typedef int Instruction[4];
typedef void(*Opcode) (Registers, Instruction);

void addr(Registers r, Instruction i) { r[i[3]] = r[i[1]] + r[i[2]]; }
void addi(Registers r, Instruction i) { r[i[3]] = r[i[1]] + i[2]; }
void mulr(Registers r, Instruction i) { r[i[3]] = r[i[1]] * r[i[2]]; }
void muli(Registers r, Instruction i) { r[i[3]] = r[i[1]] * i[2]; }
void banr(Registers r, Instruction i) { r[i[3]] = r[i[1]] & r[i[2]]; }
void bani(Registers r, Instruction i) { r[i[3]] = r[i[1]] & i[2]; }
void borr(Registers r, Instruction i) { r[i[3]] = r[i[1]] | r[i[2]]; }
void bori(Registers r, Instruction i) { r[i[3]] = r[i[1]] | i[2]; }
void setr(Registers r, Instruction i) { r[i[3]] = r[i[1]]; }
void seti(Registers r, Instruction i) { r[i[3]] = i[1]; }
void gtir(Registers r, Instruction i) { r[i[3]] = i[1] > r[i[2]] ? 1 : 0; }
void gtri(Registers r, Instruction i) { r[i[3]] = r[i[1]] > i[2] ? 1 : 0; }
void gtrr(Registers r, Instruction i) { r[i[3]] = r[i[1]] > r[i[2]] ? 1 : 0; }
void eqir(Registers r, Instruction i) { r[i[3]] = i[1] == r[i[2]] ? 1 : 0; }
void eqri(Registers r, Instruction i) { r[i[3]] = r[i[1]] == i[2] ? 1 : 0; }
void eqrr(Registers r, Instruction i) { r[i[3]] = r[i[1]] == r[i[2]] ? 1 : 0; }

static const Opcode OPS[16] = { addr, addi, mulr, muli, banr, bani, borr, bori, 
                                setr, seti, gtir, gtri, gtrr, eqir, eqri, eqrr };

typedef struct Sample Sample;

struct Sample {
    Registers in;
    Instruction i;
    Registers out;
};

typedef struct Data Data;

struct Data {
    Sample* s;
    size_t len;
};

Data* data_init(size_t cap, Data* d) {
    if (!d) return 0;
    if (!cap) cap = 512;

    Sample* s = malloc(sizeof(Sample) * cap);
    d->s = s;
    d->len = 0;

    return d;
}

typedef struct Program Program;

struct Program {
    Instruction* i;
    size_t len;
};

Program* program_init(size_t cap, Program* p) {
    if (!p) return 0;
    if (!cap) cap = 512;

    p->i = malloc(sizeof(Instruction) * cap);
    p->len = 0;

    return p;
}

Data* parse_samples(char input[static 1], size_t cap, Data* d) {
    char line[256];
    FILE* f = fopen(input, "r");
    
    if (!f) {
        fprintf(stderr, "Unable to open file");
        return 0;
    }

    while (fgets(line, 256, f)) {
        Sample s = { 0 };
        sscanf(line, "Before: [%d, %d, %d, %d]", 
                &s.in[0], &s.in[1], &s.in[2], &s.in[3]);

        fgets(line, 256, f);
        sscanf(line, "%d %d %d %d", 
                &s.i[0], &s.i[1], &s.i[2], &s.i[3]);

        fgets(line, 256, f);
        sscanf(line, "After: [%d, %d, %d, %d]", 
                &s.out[0], &s.out[1], &s.out[2], &s.out[3]);
        if (d->len == cap) { 
            cap *= 2;
            d->s = realloc(d->s, sizeof(Sample) * cap);
        }
        fgets(line, 256, f);
        d->s[d->len++] = s; 

    }

    fclose(f);

    return d;
}

Program* parse_program(char input[static 1], size_t cap, Program* p) {
    char line[16];
    FILE* f = fopen(input, "r");

    if (!f) {
        fprintf(stderr, "Unable to open file");
        return 0;
    }

    while (fgets(line, 16, f)) {
        if (p->len  == cap) {
            cap *= 2;
            p->i = realloc(p->i, sizeof(Instruction) * cap);
        }

        sscanf(line, "%d %d %d %d", 
               &p->i[p->len][0], 
               &p->i[p->len][1], 
               &p->i[p->len][2], 
               &p->i[p->len][3]);

        ++p->len;
    } 

    return p;
}

bool register_equals(Registers a, Registers b) {
    return a[0] == b[0] && a[1] == b[1] && a[2] == b[2] && a[3] == b[3];
}

size_t n_behaves_correctly(Sample* s, uint16_t codes[16]) {
    size_t n = 0;
    Registers in;

    for (size_t i = 0; i < 16; ++i) {
        memcpy(in, s->in, sizeof(Registers));
        OPS[i](in, s->i); 
        if (register_equals(in, s->out)) ++n; 
        else codes[i] &= ~(1 << s->i[0]);
    }

    return n;
}

size_t n_three_or_more_opcodes(Data* d, uint16_t codes[16]) {
    size_t n = 0;

    for (size_t i = 0; i < d->len; ++i) {
        if (n_behaves_correctly(&d->s[i], codes) >= 3) ++n;
    }

    return n;
}

bool all_opcodes_determined(uint16_t codes[16]) {
    for (size_t i = 0; i < 16; ++i)  {
        if ((codes[i] & codes[i] - 1)) return false;
    }
    return true;
}

void determine_opcodes(uint16_t codes[16]) {
    while (1) {
        for (size_t i = 0; i < 16; ++i) {
            if (!(codes[i] & codes[i] - 1)) {
                for (size_t j = 0; j < 16; ++j) {
                    if (i == j) continue;
                    codes[j] &= ~codes[i];
                }
            }
            if (all_opcodes_determined(codes)) return;
        }
    }
}

size_t log2_int(uint16_t n) {
    size_t log2 = SIZE_MAX;
    
    while (n) { n >>= 1; ++log2; }

    return log2;
}

void map_opcode_to_functions(size_t opcode_to_codes[16], uint16_t codes[16]) {
    for (size_t i = 0; i < 16; ++i) {
        opcode_to_codes[log2_int(codes[i])] = i;
    }
}

int execute_program(size_t map[16], Program* p, Registers r) {
    for (size_t i = 0; i < p->len; ++i) {
        OPS[map[p->i[i][0]]](r, p->i[i]); 
    }
    
    return r[0];
}

int main(void) {
    Data d;
    Program p;
    Registers r = { 0 };
    size_t cap = 512;
    uint16_t codes[16];
    size_t opcode_to_functions[16];

    data_init(cap, &d);
    program_init(cap, &p);

    if (!parse_samples(SAMPLES, cap, &d)) return EXIT_FAILURE;
    if (!parse_program(PROGRAM, cap, &p)) return EXIT_FAILURE;

    for (size_t i = 0; i < 16; ++i) codes[i] = 65535;

    printf("Part 1: %zu\n", n_three_or_more_opcodes(&d, codes));

    determine_opcodes(codes);
    map_opcode_to_functions(opcode_to_functions, codes);
    execute_program(opcode_to_functions, &p, r);

    printf("Part 2: %d\n", r[0]);

    return EXIT_SUCCESS;
}
