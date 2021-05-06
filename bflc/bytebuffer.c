#include "bytebuffer.h"

#include <malloc.h>
#include <string.h>

void
bytebuffer_init(bytebuffer_t *buf, size_t min)
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
}

void
bytebuffer_write(bytebuffer_t *buf, uint8_t byte)
{
    if (buf->pos + 1 >= buf->len)
    {
        buf->len += BYTEBUFFER_BLOCK;
        buf->bytes = realloc(buf->bytes, buf->len);
    }

    buf->bytes[buf->pos++] = byte;
}

void
bytebuffer_writes(bytebuffer_t *buf, uint8_t *bytes, size_t len)
{
    if (buf->pos + len >= buf->len)
    {
        buf->len += len + BYTEBUFFER_BLOCK;
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
