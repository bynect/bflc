#ifndef AMD64_H
#define AMD64_H

#include "back.h"
#include "label.h"

typedef uint8_t *Data_Ptr;
typedef void (*Func_Ptr)();

typedef struct {
	Data_Ptr cells;
	Func_Ptr putchar;
	Func_Ptr getchar;
} Amd64_Layout;

typedef struct {
	Back_Aux aux;
	Label_Stack *stack;
	Amd64_Layout *mem;
} Amd64_Aux;

void amd64_aux_init(Amd64_Aux *aux, Label_Stack *stack, Amd64_Layout *mem);

void amd64_emit(Out_Channel *out, Bfir_Entry *entry, Back_Aux *aux);

extern const Back_Info amd64_back;

#endif
