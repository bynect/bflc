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

#ifndef BFLC_CONTEXT_H
#define BFLC_CONTEXT_H

#include "mem.h"

#include <stdint.h>
#include <stdbool.h>

#define CTX_FWRITE 1
#define CTX_FREAD 2
#define CTX_FLIBC 3
#define CTX_FWRAP 4
#define CTX_FWRAPPTR 5
#define CTX_OASM 6
#define CTX_OMACH 7
#define CTX_CELLS 8
#define CTX_FUNCNAME 9
#define CTX_INTELASM 10
#define CTX_INTELBIN 11
#define CTX_EXTRA 12

#ifdef BFLC_INTERNAL

struct context {
    bool f_write;
    bool f_read;
    bool f_libc;
    bool f_wrap;
    bool f_wrap_ptr;
    bool o_asm;
    bool o_mach;
    size_t cells;
    char *func_name;
    bool intel_asm;
    bool intel_bin;
    void *extra;
    mem_t *mem;
};

#endif

typedef struct context context_t;

context_t *context_new(mem_t *mem);

void context_reset(context_t *ctx, mem_t *mem);

bool context_set(context_t *ctx, uint8_t option, void *value);

bool context_get(context_t *ctx, uint8_t option, void *value);

void context_set_mem(context_t *ctx, mem_t *mem);

mem_t *context_get_mem(context_t *ctx);

void context_free(context_t *ctx);

#endif
