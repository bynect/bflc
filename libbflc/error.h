#ifndef BFLC_ERROR_H
#define BFLC_ERROR_H

#include <stdbool.h>
#include "ir.h"

typedef struct error {
    const char *pretty;
    const instr_t *instr;
    struct error *next;
} error_t;

void error_init(error_t *err, const char *pretty, const instr_t *instr);

void error_node(error_t *err, const char *pretty, const instr_t *instr);

void error_free(error_t *err);

bool error_success(const error_t *err);

void error_dump(const error_t *err);

#endif
