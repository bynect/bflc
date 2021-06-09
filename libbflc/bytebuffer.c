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
#include "bytebuffer.h"

#include <string.h>

void
bytebuffer_init(context_t *ctx, bytebuffer_t *buf, size_t min)
{
    buf->bytes = ctx->mem->alloc_fn(min, ctx->mem->extra);
    buf->len = min;
    buf->pos = 0;
}

void
bytebuffer_reset(context_t *ctx, bytebuffer_t *buf, size_t min)
{
    if (buf->bytes != NULL)
    {
        if (buf->len < min)
        {
            buf->bytes = ctx->mem->realloc_fn(
                buf->bytes, buf->len, min, ctx->mem->extra
            );
            buf->len = min;
        }
        buf->pos = 0;
        return;
    }

    buf->bytes = ctx->mem->alloc_fn(min, ctx->mem->extra);
    buf->len = min;
    buf->pos = 0;
}

void
bytebuffer_write(context_t *ctx, bytebuffer_t *buf, uint8_t byte)
{
    if (buf->pos + 1 >= buf->len)
    {
        buf->bytes = ctx->mem->realloc_fn(
            buf->bytes, buf->len, buf->len * 2, ctx->mem->extra
        );
        buf->len *= 2;
    }

    buf->bytes[buf->pos++] = byte;
}

void
bytebuffer_writes(context_t *ctx, bytebuffer_t *buf,
                const uint8_t *bytes, size_t len)
{
    if (buf->pos + len >= buf->len)
    {
        buf->bytes = ctx->mem->realloc_fn(
            buf->bytes, buf->len, buf->len * 2 + len, ctx->mem->extra
        );
        buf->len = buf->len * 2 + len;
    }

    memcpy(buf->bytes + buf->pos, bytes, len);
    buf->pos += len;
}

void
bytebuffer_free(context_t *ctx, bytebuffer_t *buf)
{
    ctx->mem->free_fn(buf->bytes, buf->len, ctx->mem->extra);
    buf->bytes = NULL;
    buf->len = 0;
    buf->pos = 0;
}
