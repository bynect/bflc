#ifndef BFLC_CONTEXT_H
#define BFLC_CONTEXT_H

#include <stdint.h>
#include <stdbool.h>

#define CTX_FWRITE 1
#define CTX_FREAD 2
#define CTX_FLIBC 3
#define CTX_FWRAP 4
#define CTX_FWRAPPTR 5
#define CTX_OASM 6
#define CTX_OMACH 7
#define CTX_CELLS 8
#define CTX_FUNCNAME 9
#define CTX_INTELASM 10
#define CTX_INTELBIN 11
#define CTX_EXTRA 12

typedef struct context context_t;

context_t *context_new(void);

void context_reset(context_t *ctx);

bool context_set(context_t *ctx, uint8_t option, void *value);

bool context_get(context_t *ctx, uint8_t option, void *value);

void context_free(context_t *ctx);

#endif
