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
	BACK_BIN = 1 << 2,
	BACK_ELF = 1 << 3,
	BACK_JIT = 1 << 4,
} Back_Out;

typedef void (Back_Emit_F)(Out_Channel *out, Bfir_Entry *entry, Back_Aux *aux);

typedef struct {
	Signature sign;
	Back_Out outs;
	Back_Emit_F *emit_f;
} Back_Info;

#endif
