#ifndef AMD64_H
#define AMD64_H

#include "back.h"
#include "label.h"

typedef struct {
	Back_Aux aux;
	Label_Stack *stack;
} Amd64_Aux;

void amd64_aux_init(Amd64_Aux *aux, Label_Stack *stack);

void amd64_emit(Out_Channel *out, Bfir_Entry *entry, Back_Aux *aux);

extern const Back_Info amd64_back;

#endif
