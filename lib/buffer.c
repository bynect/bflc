#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "buffer.h"

void byte_buffer_init(Byte_Buffer *buffer, uint8_t *bytes, size_t size) {
	assert(buffer != NULL);
	assert(bytes != NULL);

	buffer->bytes = bytes;
	buffer->len = 0;
	buffer->size = size;
}

void byte_buffer_write(Byte_Buffer *buffer, const uint8_t *bytes, size_t len) {
	assert(buffer->bytes != bytes); // restrict
	byte_buffer_ensure(buffer, len);
	memcpy(buffer->bytes + buffer->len, bytes, len);
	buffer->len += len;
}

void byte_buffer_ensure(Byte_Buffer *buffer, size_t len) {
	if (buffer->len + len > buffer->size) {
		//buffer->grow(buffer);
		printf("%zu + %zu > %zu\n", buffer->len, len, buffer->size);
		assert(false);
	}
}
