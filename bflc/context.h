#ifndef BFLC_CONTEXT_H
#define BFLC_CONTEXT_H

#include <stdint.h>
#include <stdbool.h>

#define CTX_FWRITE 1
#define CTX_FOPEN 2
#define CTX_FLIBC 3
#define CTX_OASM 4
#define CTX_OMACH 5
#define CTX_EXTRA 6

typedef struct context context_t;

context_t *context_new(void);

void context_reset(context_t *ctx);

bool context_set(context_t *ctx, uint8_t option, void *value);

bool context_get(context_t *ctx, uint8_t option, void **value);

void context_free(context_t *ctx);

#endif
