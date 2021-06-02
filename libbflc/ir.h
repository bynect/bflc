/*
    Copyright 2021 @bynect

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

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

uint32_t ir_dump(const ir_t *ir);

#endif
