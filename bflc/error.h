#ifndef BFLC_ERROR_H
#define BFLC_ERROR_H

#include "ir.h"

typedef struct error {
    const char *pretty;
    const instr_t *instr;
    struct error *next;
} error_t;

#endif
