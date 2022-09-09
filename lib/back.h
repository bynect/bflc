#ifndef BACK_H
#define BACK_H

#include <stdbool.h>

#include "bfir.h"
#include "out.h"
#include "sign.h"

typedef struct {
	Signature sign;
} Back_Aux;

typedef enum {
	BACK_NONE = 0,
	BACK_ASM = 1 << 0,
	BACK_OBJ = 1 << 1,
	BACK_JIT = 1 << 2,
} Back_Out;

typedef void (Back_Emit_F)(Out_Channel *out, Bfir_Entry *entry, Back_Aux *aux);

typedef struct {
	const char *name;
	const char *desc;
	Signature sign;
	Back_Out outs;
	Back_Emit_F *emit_f;
} Back_Info;

#endif
