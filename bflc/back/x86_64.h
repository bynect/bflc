#ifndef BFLC_X86_64
#define BFLC_X86_64

#include "../bytebuffer.h"
#include "../ir.h"
#include "../error.h"

error_t emit_asm_x86_64(bytebuffer_t *buf, ir_t *ir, size_t len);

error_t emit_mach_x86_64(bytebuffer_t *buf, ir_t *ir, size_t len);

#endif
