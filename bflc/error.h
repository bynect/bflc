#ifndef BFLC_ERROR_H
#define BFLC_ERROR_H

#include "ir.h"

typedef struct {
    const char *pretty;
    const instr_t *instr;
} error_t;

#endif
