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

#define BFLC_INTERNAL
#include "context.h"

#include <string.h>

context_t *
context_new(mem_t *mem)
{
    context_t *ctx = mem->alloc_fn(sizeof(struct context), mem->extra);
    context_reset(ctx, mem);
    return ctx;
}

void
context_reset(context_t *ctx, mem_t *mem)
{
    memset(ctx, 0, sizeof(struct context));
    ctx->mem = mem;
}

bool
context_set(context_t *ctx, uint8_t option, void *value)
{
    switch (option)
    {
        case CTX_FWRITE:
            ctx->f_write = *(bool*)value;
            break;

        case CTX_FREAD:
            ctx->f_read = *(bool*)value;
            break;

        case CTX_FLIBC:
            ctx->f_libc = *(bool*)value;
            break;

        case CTX_FWRAP:
            ctx->f_wrap = *(bool*)value;
            break;

        case CTX_FWRAPPTR:
            ctx->f_wrap_ptr = *(bool*)value;
            break;

        case CTX_OASM:
            ctx->o_asm = *(bool*)value;
            break;

        case CTX_OMACH:
            ctx->o_mach = *(bool*)value;
            break;

        case CTX_CELLS:
            ctx->cells = *(size_t*)value;
            break;

        case CTX_FUNCNAME:
            ctx->func_name = (char*)value;
            break;

        case CTX_INTELASM:
            ctx->intel_asm = *(bool*)value;
            break;

        case CTX_INTELBIN:
            ctx->intel_bin= *(bool*)value;
            break;

        case CTX_EXTRA:
            ctx->extra = value;
            break;

        default:
            return false;
    }

    return true;
}

bool
context_get(context_t *ctx, uint8_t option, void *value)
{
    switch (option)
    {
        case CTX_FWRITE:
            *(bool*)value = ctx->f_write;
            break;

        case CTX_FREAD:
            *(bool*)value = ctx->f_read;
            break;

        case CTX_FLIBC:
            *(bool*)value = ctx->f_libc;
            break;

        case CTX_FWRAP:
            *(bool*)value = ctx->f_wrap;
            break;

        case CTX_FWRAPPTR:
            *(bool*)value = ctx->f_wrap_ptr;
            break;


        case CTX_OASM:
            *(bool*)value = ctx->o_asm;
            break;

        case CTX_OMACH:
            *(bool*)value = ctx->o_mach;
            break;

        case CTX_CELLS:
            *(size_t*)value = ctx->cells;
            break;

        case CTX_FUNCNAME:
            *(char**)value = ctx->func_name;
            break;

        case CTX_INTELASM:
            *(bool*)value = ctx->intel_asm;
            break;

        case CTX_INTELBIN:
            *(bool*)value = ctx->intel_bin;
            break;

        case CTX_EXTRA:
            *(void**)value = ctx->extra;
            break;

        default:
            return false;
    }

    return true;
}

void
context_set_mem(context_t *ctx, mem_t *mem)
{
    ctx->mem = mem;
}

mem_t *
context_get_mem(context_t *ctx)
{
    return ctx->mem;
}

void
context_free(context_t *ctx)
{
    ctx->mem->free_fn(ctx, sizeof(context_t), ctx->mem->free_fn);
}
