#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "in.h"

void in_init_none(In_Channel *in) {
	assert(in != NULL);
	in->kind = IN_NONE;
}

void in_init_file(In_Channel *in, FILE *file) {
	assert(in != NULL);
	assert(file != NULL);

	in->kind = IN_FILE;
	in->file = file;
}

void in_init_string(In_Channel *in, const char *string) {
	assert(in != NULL);
	assert(string != NULL);

	in->kind = IN_STRING;
	in->slice.bytes = (uint8_t *)string;
	in->slice.len = strlen(string);
}

void in_init_slice(In_Channel *in, const uint8_t *bytes, size_t len) {
	assert(in != NULL);
	assert(bytes != NULL);

	in->kind = IN_STRING;
	in->slice.bytes = bytes;
	in->slice.len = len;
}

ssize_t in_read(In_Channel *in, uint8_t *bytes, size_t len) {
	return -1;
}
