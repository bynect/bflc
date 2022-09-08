#ifndef BACK_H
#define BACK_H

#include <stdbool.h>

#include "bfir.h"
#include "out.h"
#include "sign.h"

typedef struct {
	Signature sign;
} Back_Aux;

typedef void (Back_Emit_F)(Out_Channel *out, Bfir_Entry *entry, Back_Aux *aux);

typedef struct {
	const char *name;
	Signature sign;
	Back_Emit_F *emit_f;
} Back_Info;

#endif
