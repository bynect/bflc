#ifndef BFLC_IR_H
#define BFLC_IR_H

#include <stdint.h>
#include <stddef.h>

#define INSTR_PTRINC 1
#define INSTR_PTRDEC 2
#define INSTR_CELINC 3
#define INSTR_CELDEC 4
#define INSTR_OUTPUT 5
#define INSTR_INPUT 6
#define INSTR_JMPBEG 7
#define INSTR_JMPEND 8

typedef struct {
    uint32_t line;
    uint32_t column;
    size_t offset;
} pos_t;

typedef struct instr {
    uint8_t instr;
    intptr_t arg;
    pos_t pos;
    struct instr *next;
} instr_t;

typedef struct {
    instr_t *instrs;
} ir_t;

void ir_init(ir_t *ir);

void ir_node(ir_t *ir, uint8_t instr, intptr_t arg, pos_t pos);

void ir_free(ir_t *ir);

void instr_dump(const instr_t *instr);

void ir_dump(const ir_t *ir);

#endif
