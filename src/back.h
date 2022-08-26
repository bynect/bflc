#ifndef BACK_H
#define BACK_H

#include <stdbool.h>

#include "bfir.h"
#include "label.h"
#include "out.h"

typedef bool (Bflc_Back_Emit_F)(Out_Channel *out, Bfir_Entry *entry, Label_Stack *stack);

typedef struct {
	const char *name;
	Bflc_Back_Emit_F *emit_f;
} Bflc_Back;

#endif
