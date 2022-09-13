#ifndef FRONT_BFIR_H
#define FRONT_BFIR_H

#include "front.h"

Error bfir_parse(In_Channel *in, Bfir_Entry *entry, Front_Aux *aux);

extern const Front_Info bfir_front;

#endif
