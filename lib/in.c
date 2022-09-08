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
	assert(in != NULL);
	assert(in->kind >= IN_NONE && in->kind <= IN_SLICE);

	if (in->kind == IN_NONE) return 0;

	if (in->kind == IN_FILE) return fread(bytes, 1, len, in->file);
	else {
		size_t slen = in->slice.len > len ? len : in->slice.len;
		if (slen == 0) return 0;

		memcpy(bytes, in->slice.bytes, slen);
		in->slice.bytes += slen;
		in->slice.len -= slen;
		return slen;
	}

	return -1;
}
