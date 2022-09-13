#ifndef BACK_BFIR_H
#define BACK_BFIR_H

#include "back.h"

Error bfir_emit(Out_Channel *out, Bfir_Entry *entry, Back_Aux *aux);

extern const Back_Info bfir_back;

#endif
