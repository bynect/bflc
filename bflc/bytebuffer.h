#ifndef BFLC_BYTEBUFFER_H
#define BFLC_BYTEBUFFER_H

#include <stdint.h>
#include <stddef.h>

#define BYTEBUFFER_BLOCK 64

typedef struct {
    uint8_t *bytes;
    size_t len;
    size_t pos;
} bytebuffer_t;

void bytebuffer_init(bytebuffer_t *buf, size_t min);

void bytebuffer_write(bytebuffer_t *buf, uint8_t byte);

void bytebuffer_writes(bytebuffer_t *buf, uint8_t *bytes, size_t len);

void bytebuffer_free(bytebuffer_t *buf);

#endif
