#include <assert.h>
#include <string.h>

#include "buffer.h"

void byte_buffer_init(Byte_Buffer *buffer, uint8_t *bytes, size_t size) {
	buffer->bytes = bytes;
	buffer->len = 0;
	buffer->size = size;

	assert(bytes != NULL);
}

void byte_buffer_write(Byte_Buffer *buffer, const uint8_t *bytes, size_t len) {
	assert(buffer->bytes != bytes); // restrict
	memcpy(buffer->bytes, bytes, len);
}

void byte_buffer_ensure(Byte_Buffer *buffer, size_t len) {
	//if (buffer->len + len > buffer->size) buffer->grow(buffer);
	assert(buffer->len + len > buffer->size);
}
