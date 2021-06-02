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

#include "bytebuffer.h"

#include <malloc.h>
#include <string.h>

void
bytebuffer_init(bytebuffer_t *buf, size_t min)
{
    buf->bytes = malloc(min);
    buf->len = min;
    buf->pos = 0;
}

void
bytebuffer_reset(bytebuffer_t *buf, size_t min)
{
    if (buf->bytes != NULL)
    {
        if (buf->len < min)
        {
            buf->bytes = realloc(buf->bytes, min);
            buf->len = min;
        }
        buf->pos = 0;
        return;
    }

    buf->bytes = malloc(min);
    buf->len = min;
    buf->pos = 0;
}

void
bytebuffer_write(bytebuffer_t *buf, uint8_t byte)
{
    if (buf->pos + 1 >= buf->len)
    {
        buf->len *= 2;
        buf->bytes = realloc(buf->bytes, buf->len);
    }

    buf->bytes[buf->pos++] = byte;
}

void
bytebuffer_writes(bytebuffer_t *buf, const uint8_t *bytes, size_t len)
{
    if (buf->pos + len >= buf->len)
    {
        buf->len *= 2;
        buf->len += len;
        buf->bytes = realloc(buf->bytes, buf->len);
    }

    memcpy(buf->bytes + buf->pos, bytes, len);
    buf->pos += len;
}

void
bytebuffer_free(bytebuffer_t *buf)
{
    free(buf->bytes);
    buf->bytes = NULL;
    buf->len = 0;
    buf->pos = 0;
}
