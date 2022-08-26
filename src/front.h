#ifndef FRONT_H
#define FRONT_H

#include <stdbool.h>

#include "bfir.h"

typedef bool (Bflc_Front_Parse_F)(const char *src, Bfir_Entry *entry);

typedef struct {
	const char *name;
	Bflc_Front_Parse_F *parse_f;
} Bflc_Front;

#endif
