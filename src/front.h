#ifndef FRONT_H
#define FRONT_H

#include <stdbool.h>

#include "bfir.h"
#include "in.h"
#include "sign.h"

typedef struct {
	Signature sign;
} Front_Aux;

typedef void (Front_Parse_F)(In_Channel *in, Bfir_Entry *entry, Front_Aux *aux);

typedef struct {
	const char *name;
	Signature sign;
	Front_Parse_F *parse_f;
} Front_Info;

#endif
