#ifndef BFLC_X86_64_H
#define BFLC_X86_64_H

#include "../bytebuffer.h"
#include "../ir.h"
#include "../error.h"
#include "../context.h"

error_t emit_asm_x86_64(context_t *ctx, bytebuffer_t *buf, ir_t *ir);

error_t emit_mach_x86_64(context_t *ctx, bytebuffer_t *buf, ir_t *ir);

#endif
