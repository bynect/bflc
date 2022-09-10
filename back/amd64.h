#ifndef BACK_AMD64_H
#define BACK_AMD64_H

#include "back.h"
#include "label.h"

typedef uint64_t Data_Addr;
typedef uint64_t Func_Addr;

typedef struct {
	Data_Addr cells;
	Func_Addr putchar;
	Func_Addr getchar;
} Amd64_Layout;

typedef enum {
	AMD64_NONE,
	AMD64_READ_SYSCALL = 1 << 0,
	AMD64_WRITE_SYSCALL = 1 << 1,
	AMD64_SYSCALL = AMD64_READ_SYSCALL | AMD64_WRITE_SYSCALL,
	AMD64_RELATIVE_CALL = 1 << 2,
} Amd64_Flag;

typedef struct {
	Back_Aux aux;
	Label_Stack *stack;
	Amd64_Layout *mem;
	Amd64_Flag flags;
} Amd64_Aux;

void amd64_aux_init(Amd64_Aux *aux, Label_Stack *stack, Amd64_Layout *mem, Amd64_Flag flags);

void amd64_emit(Out_Channel *out, Bfir_Entry *entry, Back_Aux *aux);

extern const Back_Info amd64_back;

#endif
