#ifndef BFLC_FRONT_BRAINFUCK_H
#define BFLC_FRONTBRAINFUCK_H

#include "../ir.h"
#include "../error.h"

error_t scan_brainfuck(const char *src, size_t len, ir_t *ir);

#endif
