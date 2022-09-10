#ifndef BACK_AMD64_ASM_H
#define BACK_AMD64_ASM_H

#include "back.h"
#include "label.h"

typedef enum {
	AMD64_ASM_NONE,
	AMD64_ASM_READ_SYSCALL = 1 << 0,
	AMD64_ASM_WRITE_SYSCALL = 1 << 1,
	AMD64_ASM_SYSCALL = AMD64_ASM_READ_SYSCALL | AMD64_ASM_WRITE_SYSCALL,
} Amd64_Asm_Flag;

typedef struct {
	Back_Aux aux;
	Label_Stack *stack;
	size_t celln;
	Amd64_Asm_Flag flags;
} Amd64_Asm_Aux;

void amd64_asm_aux_init(Amd64_Asm_Aux *aux, Label_Stack *stack, size_t celln, Amd64_Asm_Flag flags);

void amd64_asm_emit(Out_Channel *out, Bfir_Entry *entry, Back_Aux *aux);

extern const Back_Info amd64_asm_back;

#endif
