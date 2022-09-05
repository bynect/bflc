#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "back.h"
#include "bfir.h"
#include "label.h"
#include "out.h"
#include "amd64.h"

static void amd64_instr(Out_Channel *out, Bfir_Instr *instr, Label_Stack *stack) {
	assert(false);
}

static void amd64_entry(Out_Channel *out, Bfir_Entry *entry, Label_Stack *stack) {
	Bfir_Instr *instr = bfir_entry_get(entry, entry->head);
	while (true) {
		amd64_instr(out, instr, stack);
		if (instr->next == 0) break;
		instr = bfir_entry_get(entry, instr->next);
	}
}

void amd64_aux_init(Amd64_Aux *aux, Label_Stack *stack) {
	assert(aux != NULL);
	assert(stack != NULL);

	aux->aux.sign = amd64_back.sign;
	aux->stack = stack;
}

void amd64_emit(Out_Channel *out, Bfir_Entry *entry, Back_Aux *aux) {
	assert(out != NULL && entry != NULL && aux != NULL);
	assert(aux->sign.quad == amd64_back.sign.quad);
	amd64_entry(out, entry, ((Amd64_Aux *)aux)->stack);
}

const Back_Info amd64_back = {
	.name = "amd64",
	.sign.quad = 0x86640000,
	.emit_f = amd64_emit,
};
