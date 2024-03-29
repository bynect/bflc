#ifndef FRONT_H
#define FRONT_H

#include <stdbool.h>

#include "bfir.h"
#include "in.h"
#include "sign.h"
#include "error.h"

typedef struct {
	Signature sign;
} Front_Aux;

typedef Error (Front_Parse_F)(In_Channel *in, Bfir_Entry *entry, Front_Aux *aux);

typedef struct {
	Signature sign;
	Front_Parse_F *parse_f;
} Front_Info;

#endif
