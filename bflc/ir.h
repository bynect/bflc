#ifndef BFLC_IR_H
#define BFLC_IR_H

#include <stdint.h>
#include <stddef.h>

#define INSTR_PTRINC 1
#define INSTR_PTRDEC 2
#define INSTR_CELINC 3
#define INSTR_CELDEC 4
#define INSTR_JMPBEG 5
#define INSTR_JMPEND 6

typedef struct {
    uint8_t instr;
    uint32_t line;
    uint32_t column;
    size_t offset;
} ir_t;

#endif
