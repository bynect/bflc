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
#include "reloc.h"

void
reloc_init(context_t *ctx, reloc_t *reloc, size_t min)
{
    labelstack_init(ctx, &reloc->labelstack, LABELSTACK_BLOCK);
    min *= sizeof(*(reloc->offs));

    reloc->offs = ctx->mem->alloc_fn(min, ctx->mem->extra);
    reloc->off_len = min;
    reloc->off_pos = 0;
}

void
reloc_reset(context_t *ctx, reloc_t *reloc, size_t min)
{
    labelstack_reset(ctx, &reloc->labelstack, LABELSTACK_BLOCK);
    min *= sizeof(*reloc->offs);

    if (reloc->offs != NULL)
    {
        if (reloc->off_len < min)
        {
            reloc->offs = ctx->mem->realloc_fn(
                reloc->offs, reloc->off_len, min, ctx->mem->extra
            );
            reloc->off_len = min;
        }
        reloc->off_pos = 0;
        return;
    }

    reloc->offs = ctx->mem->alloc_fn(min, ctx->mem->extra);
    reloc->off_len = min;
    reloc->off_pos = 0;
}

void
reloc_write(context_t *ctx, reloc_t *reloc, uint8_t type, uint32_t off)
{
    size_t off_pos = (reloc->off_pos + 1) * sizeof(*reloc->offs);
    if (off_pos >= reloc->off_len)
    {
        reloc->offs = ctx->mem->realloc_fn(
            reloc->offs,
            reloc->off_len,
            reloc->off_len + RELOC_BLOCK * sizeof(*reloc->offs),
            ctx->mem->extra
        );
        reloc->off_len += RELOC_BLOCK * sizeof(*reloc->offs);
    }

    reloc->offs[reloc->off_pos].type = type;
    reloc->offs[reloc->off_pos].off = off;
    ++reloc->off_pos;
}

void
reloc_patch(reloc_t *reloc, bytebuffer_t *buf, uint32_t cellmem,
            uint32_t cellptr, addr_patch_fn addr_patch, void *extra)
{
    for (size_t i = 0; i < reloc->off_pos; ++i)
    {
        addr_patch(
            buf, reloc->offs[i].type == RELOC_PATCH_MEM ? cellmem : cellptr,
            reloc->offs[i].type, reloc->offs[i].off, extra
        );
    }
}

void
reloc_free(context_t *ctx, reloc_t *reloc)
{
    labelstack_free(ctx, &reloc->labelstack);
    ctx->mem->free_fn(reloc->offs, reloc->off_len, ctx->mem->extra);
    reloc->offs = NULL;
    reloc->off_len = 0;
    reloc->off_pos = 0;
}
