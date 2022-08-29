#ifndef AMD64_ASM_H
#define AMD64_ASM_H

#include "back.h"
#include "label.h"

typedef struct {
	Back_Aux aux;
	Label_Stack *stack;
} Amd64_Asm_Aux;

void amd64_asm_aux_init(Amd64_Asm_Aux *aux, Label_Stack *stack);

void amd64_asm_emit(Out_Channel *out, Bfir_Entry *entry, Back_Aux *aux);

extern const Back_Info amd64_asm_back;

#endif
