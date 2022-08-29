#ifndef AMD64_ASM_H
#define AMD64_ASM_H

#include "back.h"
#include "label.h"

typedef struct {
	Back_Emitter back;
	Label_Stack *stack;
} Amd64_Asm_Emitter;

void amd64_asm_init(Amd64_Asm_Emitter *back, Label_Stack *stack);

#endif
