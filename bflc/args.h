#ifndef ARGS_H
#define ARGS_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "error.h"

#define FLAVOR_INTELASM 1
#define FLAVOR_INTELBIN 2
#define FLAVOR_GNUASM 3

#define FLAG_WRITE (1 << 0)
#define FLAG_READ (1 << 1)
#define FLAG_LIBC (1 << 2)
#define FLAG_WRAP_CELL (1 << 3)
#define FLAG_WRAP_PTR (1 << 4)

typedef struct {
    const char *exe;
    const char *out;
    const char *in;
    const char *front;
    const char *back;
    bool validation;
    bool folding;
    const char *func_name;
    uint8_t flavor;
    size_t cells;
    uint32_t flags;
} args_t;

void args_parse(int argc, char **argv, args_t *args, error_t *err);

void args_dump(const args_t *args);

#endif
