#ifndef BACK_H
#define BACK_H

#include <stdbool.h>

#include "bfir.h"
#include "label.h"
#include "out.h"

typedef union {
	uint8_t bytes[8];
	uint64_t quad;
} Signature;

struct Back_Emitter;

typedef bool (Back_Emitter_F)(struct Back_Emitter *back, Out_Channel *out, Bfir_Entry *entry);

typedef struct Back_Emitter {
	Signature sign;
	Back_Emitter_F *emit_f;
} Back_Emitter;

#endif
