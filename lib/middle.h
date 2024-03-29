#ifndef MIDDLE_H
#define MIDDLE_H

#include <stdbool.h>

#include "bfir.h"
#include "sign.h"
#include "error.h"

typedef struct {
	Signature sign;
} Middle_Aux;

typedef Error (Middle_Pass_F)(Bfir_Entry *entry, Middle_Aux *aux);

typedef struct {
	Signature sign;
	Middle_Pass_F *pass_f;
} Middle_Info;

#endif
