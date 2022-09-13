#ifndef FRONT_BRAINFUCK_H
#define FRONT_BRAINFUCK_H

#include "front.h"

Error brainfuck_parse(In_Channel *in, Bfir_Entry *entry, Front_Aux *aux);

extern const Front_Info brainfuck_front;

#endif
