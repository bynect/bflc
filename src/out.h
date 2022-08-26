#ifndef OUT_H
#define OUT_H

#include <stdio.h>

// TODO: Add other kinds of output channel (eg memory, fd, ...)
typedef struct {
	FILE *file;
} Out_Channel;

//void out_write(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
#define out_write(out, ...) fprintf(out->file, __VA_ARGS__)

#endif
