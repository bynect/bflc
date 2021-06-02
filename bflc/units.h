#ifndef BFLC_UNIT_H
#define BFLC_UNIT_H

#include "bytebuffer.h"
#include "ir.h"
#include "error.h"
#include "context.h"

typedef error_t (*emit_fn)(context_t *ctx, bytebuffer_t *buf, ir_t *ir);

typedef error_t (*scan_fn)(const char *src, size_t len, ir_t *ir);

typedef struct {
    const char *name;
    emit_fn asm_fn;
    emit_fn mach_fn;
} backend_t;

typedef struct {
    const char *name;
    scan_fn scan_fn;
} frontend_t;

#define MAX_BACKENDS 1
#define MAX_FRONTENDS 2

extern const backend_t backends[];
extern const frontend_t frontends[];

#endif