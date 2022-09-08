#ifndef IN_H
#define IN_H

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>

typedef enum {
	IN_NONE,
	IN_FILE,
	IN_STRING,
	IN_SLICE,
} In_Channel_Kind;

typedef struct {
	In_Channel_Kind kind;
	union {
		FILE *file;
		struct {
			const uint8_t *bytes;
			size_t len;
		} slice;
	};
} In_Channel;

void in_init_none(In_Channel *in);

void in_init_file(In_Channel *in, FILE *file);

void in_init_string(In_Channel *in, const char *string);

void in_init_slice(In_Channel *in, const uint8_t *bytes, size_t len);

ssize_t in_read(In_Channel *in, uint8_t *bytes, size_t len);

//int in_scan(In_Channel *in, const char *fmt, ...) __attribute__((format(scanf, 2, 3)));

#endif
