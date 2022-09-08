#ifndef OUT_H
#define OUT_H

#include <stdio.h>

#include "buffer.h"

typedef enum {
	OUT_NONE,
	OUT_FILE,
	OUT_BUFFER,
} Out_Channel_Kind;

typedef struct {
	Out_Channel_Kind kind;
	union {
		FILE *file;
		Byte_Buffer *buffer;
	};
} Out_Channel;

void out_init_none(Out_Channel *out);

void out_init_file(Out_Channel *out, FILE *file);

void out_init_buffer(Out_Channel *out, Byte_Buffer *buffer);

void out_write(Out_Channel *out, const uint8_t *bytes, size_t len);

void out_print(Out_Channel *out, const char *fmt, ...) __attribute__((format(printf, 2, 3)));

#endif
