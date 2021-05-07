#ifndef BFLC_CODEGEN_H
#define BFLC_CODEGEN_H

#include "bytebuffer.h"
#include "ir.h"
#include "error.h"
#include "context.h"

typedef bool (*codegen_fn)(
    context_t *ctx, bytebuffer_t *buf, instr_t *instr, error_t *err, void *extra
);

typedef struct {
    codegen_fn prologue_fn;
    codegen_fn epilogue_fn;
    codegen_fn ptrinc_fn;
    codegen_fn ptrdec_fn;
    codegen_fn celinc_fn;
    codegen_fn celdec_fn;
    codegen_fn output_fn;
    codegen_fn input_fn;
    codegen_fn jmpbeg_fn;
    codegen_fn jmpend_fn;
    void *extra;
} codegen_t;

error_t codegen_run(
    context_t *ctx, codegen_t *codegen, bytebuffer_t *buf, ir_t *ir
);

#endif
