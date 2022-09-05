#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

#include "buffer.h"
#include "out.h"

void out_init_none(Out_Channel *out) {
	assert(out != NULL);

	out->kind = OUT_NONE;
}

void out_init_file(Out_Channel *out, FILE *file) {
	assert(out != NULL);
	assert(file != NULL);

	out->kind = OUT_FILE;
	out->file = file;
}

void out_init_buffer(Out_Channel *out, Byte_Buffer *buffer) {
	assert(out != NULL);
	assert(buffer != NULL);

	out->kind = OUT_BUFFER;
	out->buffer = buffer;
}

void out_write(Out_Channel *out, const uint8_t *bytes, size_t len) {
	assert(out != NULL);
	assert(out->kind >= OUT_NONE && out->kind <= OUT_BUFFER);

	if (out->kind == OUT_NONE) return;
	assert(out->kind == OUT_FILE || out->kind == OUT_BUFFER);

	if (out->kind == OUT_FILE) assert(len == fwrite(bytes, 1, len, out->file));

	byte_buffer_write(out->buffer, bytes, len);
}

void out_print(Out_Channel *out, const char *fmt, ...) {
	assert(out != NULL);
	assert(fmt != NULL);
	assert(out->kind >= OUT_NONE && out->kind <= OUT_BUFFER);

	if (out->kind == OUT_NONE) return;

	va_list args;
	va_start(args, fmt);

	if (out->kind == OUT_FILE) vfprintf(out->file, fmt, args);
	else {
		va_list args2;
		va_copy(args2, args);

		size_t len = vsnprintf(NULL, 0, fmt, args);
		byte_buffer_ensure(out->buffer, len);

		assert(len == vsnprintf((char *)out->buffer->bytes, len, fmt, args2));
		out->buffer->len += len;
		va_end(args2);
	}

	va_end(args);
}
