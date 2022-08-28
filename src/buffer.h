#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
	uint8_t *bytes;
	size_t len;
	size_t size;
} Byte_Buffer;

void byte_buffer_init(Byte_Buffer *buffer, uint8_t *bytes, size_t size);

void byte_buffer_write(Byte_Buffer *buffer, const uint8_t *bytes, size_t len);

void byte_buffer_ensure(Byte_Buffer *buffer, size_t len);

#endif
