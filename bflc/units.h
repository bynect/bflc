/*
    Copyright 2021 @bynect

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#ifndef BFLC_UNIT_H
#define BFLC_UNIT_H

#include "libbflc/bytebuffer.h"
#include "libbflc/ir.h"
#include "libbflc/context.h"
#include "libbflc/error.h"

typedef error_t (*emit_fn)(context_t *ctx, bytebuffer_t *buf, ir_t *ir);

typedef error_t (*scan_fn)(
    context_t *ctx, const char *src, size_t len, ir_t *ir
);

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
