#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INP "./data/d19.txt"
#define TEST "./data/d19_test.txt"

typedef int Registers[6];
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

size_t instruction_to_index(char instruction[static 4]) {
    if (!memcmp("addr", instruction, sizeof(char) * 4)) return 0;
    if (!memcmp("addi", instruction, sizeof(char) * 4)) return 1;
    if (!memcmp("mulr", instruction, sizeof(char) * 4)) return 2;
    if (!memcmp("muli", instruction, sizeof(char) * 4)) return 3;
    if (!memcmp("banr", instruction, sizeof(char) * 4)) return 4;
    if (!memcmp("bani", instruction, sizeof(char) * 4)) return 5;
    if (!memcmp("borr", instruction, sizeof(char) * 4)) return 6;
    if (!memcmp("bori", instruction, sizeof(char) * 4)) return 7;
    if (!memcmp("setr", instruction, sizeof(char) * 4)) return 8;
    if (!memcmp("seti", instruction, sizeof(char) * 4)) return 9;
    if (!memcmp("gtir", instruction, sizeof(char) * 4)) return 10;
    if (!memcmp("gtri", instruction, sizeof(char) * 4)) return 11;
    if (!memcmp("gtrr", instruction, sizeof(char) * 4)) return 12;
    if (!memcmp("eqir", instruction, sizeof(char) * 4)) return 13;
    if (!memcmp("eqri", instruction, sizeof(char) * 4)) return 14;
    if (!memcmp("eqrr", instruction, sizeof(char) * 4)) return 15;
    
    return SIZE_MAX;
}

typedef struct Program Program;

struct Program {
    size_t ip;
    Instruction* is;
    size_t len;
};

Program* program_init(Program* p, size_t cap) {
    p->ip = 0;
    p->is = malloc(sizeof(Instruction) * cap);
    p->len = 0;

    return p;
}

Program* program_parse(char input[static 1], Program* p, size_t cap) {
    size_t len = 0;
    char line[64];
    FILE* f = fopen(input, "r");

    if (!f) {
        fprintf(stderr, "Unable to open file\n");
        return 0;
    }

    /* set the instruction pointer */
    fgets(line, 64, f);
    p->ip = strtoull(line + 3, 0, 10);

    /* read in the program */
    while (fgets(line, 64, f)) {
        if (len == cap) {
            cap *= 2;
            p->is = realloc(p->is, sizeof(Instruction) * cap); 
        }

        p->is[len][0] = instruction_to_index(line);
        sscanf(line + 5, "%d %d %d", 
               &p->is[len][1], &p->is[len][2], &p->is[len][3]);
        ++len;
    }

    p->len = len;

    return p;
}

void program_print(Program* p) {
    printf("#ip %zu\n", p->ip);

    for (size_t i = 0; i < p->len; ++i)  {
        printf("%d %d %d %d\n", 
               p->is[i][0], p->is[i][1],p->is[i][2],p->is[i][3]);
    }
}

int program_execute(Program* p, Registers r) {
    while (r[p->ip] < p->len) {
        Instruction* cur = &p->is[r[p->ip]];
        OPS[*cur[0]](r, *cur);
        ++r[p->ip];
    }

    return r[0];
}

int main(void) {
    Program p;
    Registers r = { 0, 0, 0, 0, 0, 0 };
    size_t cap = 64;

    program_init(&p, cap);
    program_parse(INP, &p, cap);
    printf("Part 1: %d\n", program_execute(&p, r));

    /* 
     * The program takes the sum of all the numbers that evenly divide
     * the number stored in the fifth register, till that number.
     */
    size_t sum = 0;
    for (size_t i = 1; i <= 10551326; ++i) {
        if (10551326 % i == 0) sum += i;
    }

    printf("Part 2: %zu\n", sum);
    return EXIT_SUCCESS;
}
