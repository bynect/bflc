#ifndef BFLC_RELOC_H
#define BFLC_RELOC_H

#include "labelstack.h"
#include "bytebuffer.h"

#define RELOC_BLOCK 64

#define RELOC_PATCH_MEM 1
#define RELOC_PATCH_PTR 2

typedef struct {
    labelstack_t labelstack;
    size_t off_len;
    size_t off_pos;
    struct {
        uint8_t type;
        uint32_t off;
    } *offs;
} reloc_t;

typedef void (*addr_patch_fn) (bytebuffer_t *buf, uint32_t base,
                                uint8_t type, uint32_t off, void *extra);

void reloc_init(reloc_t *reloc, size_t min);

void reloc_reset(reloc_t *reloc, size_t min);

void reloc_write(reloc_t *reloc, uint8_t type, uint32_t off);

void reloc_patch(reloc_t *reloc, bytebuffer_t *buf, uint32_t cellmem,
                uint32_t cellptr, addr_patch_fn addr_patch, void *extra);

void reloc_free(reloc_t *reloc);

#endif
