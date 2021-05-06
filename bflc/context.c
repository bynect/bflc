#include "context.h"

#include <malloc.h>
#include <string.h>

struct context {
    bool f_write;
    bool f_open;
    bool f_libc;
    bool o_asm;
    bool o_mach;
    void *extra;
};

context_t *
context_new(void)
{
    context_t *ctx = realloc(NULL, sizeof(struct context));
    return ctx;
}

void
context_reset(context_t *ctx)
{
    memset(ctx, 0, sizeof(struct context));
}

bool
context_set(context_t *ctx, uint8_t option, void *value)
{
    switch (option)
    {
        case CTX_FWRITE:
            ctx->f_write = *(bool*)value;
            break;

        case CTX_FOPEN:
            ctx->f_open = *(bool*)value;
            break;

        case CTX_FLIBC:
            ctx->f_libc = *(bool*)value;
            break;

        case CTX_OASM:
            ctx->o_asm = *(bool*)value;
            break;

        case CTX_OMACH:
            ctx->o_mach = *(bool*)value;
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
context_get(context_t *ctx, uint8_t option, void **value)
{
    bool *ptr = *value;

    switch (option)
    {
        case CTX_FWRITE:
            *ptr = ctx->f_write;
            break;

        case CTX_FOPEN:
            *ptr = ctx->f_open;
            break;

        case CTX_FLIBC:
            *ptr = ctx->f_libc;
            break;

        case CTX_OASM:
            *ptr = ctx->o_asm;
            break;

        case CTX_OMACH:
            *ptr = ctx->o_mach;
            break;

        case CTX_EXTRA:
            *value = ctx->extra;
            break;

        default:
            return false;
    }

    return true;
}

void
context_free(context_t *ctx)
{
    free(ctx);
}
