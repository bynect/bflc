#ifndef BFLC_LABELSTACK_H
#define BFLC_LABELSTACK_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define LABELSTACK_BLOCK 32

typedef struct {
    uint32_t *labels;
    size_t len;
    size_t pos;
    uint32_t current;
} labelstack_t;

void labelstack_init(labelstack_t *labelstack, size_t min);

void labelstack_push(labelstack_t *labelstack, uint32_t label);

bool labelstack_pop(labelstack_t *labelstack, uint32_t *label);

void labelstack_free(labelstack_t *labelstack);

#endif
