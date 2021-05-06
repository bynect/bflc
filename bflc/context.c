#include "context.h"

#include <malloc.h>
#include <string.h>

struct context {
    bool f_write;
    bool f_open;
    bool f_libc;
    bool f_wrap;
    bool f_wrap_ptr;
    bool o_asm;
    bool o_mach;
    size_t cells;
    void *extra;
};

context_t *
context_new(void)
{
    context_t *ctx = calloc(1, sizeof(struct context));
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

        case CTX_FOPEN:
            *(bool*)value = ctx->f_open;
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

        case CTX_EXTRA:
            *(void**)value = ctx->extra;
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
