#include "driver.h"
#include "units.h"
#include "error.h"
#include "bytebuffer.h"
#include "ir.h"
#include "mid/folding.h"
#include "mid/validation.h"

#include <string.h>
#include <stdio.h>
#include <malloc.h>

int
driver_main(context_t *ctx, args_t *args)
{
    const frontend_t *front = NULL;
    for (uint32_t i = 0; i < MAX_FRONTENDS; ++i)
    {
        if (!strcmp(args->front, frontends[i].name))
        {
            front = frontends + i;
            break;
        }
    }

    if (front == NULL)
    {
        printf("Frontend not found\n");
    }

    const backend_t *back = NULL;
    for (uint32_t i = 0; i < MAX_BACKENDS; ++i)
    {
        if (!strcmp(args->back, backends[i].name))
        {
            back = backends + i;
            break;
        }
    }

    if (back == NULL)
    {
        printf("Backend not found\n");
    }

    if (front == NULL || back == NULL)
    {
        return 1;
    }

    FILE *in = fopen(args->in, "rb");
    if (in == NULL)
    {
        printf("Failed to open input file\n");
        return 1;
    }

    fseek(in, 0, SEEK_END);
    size_t in_size = ftell(in);
    rewind(in);

    char *src = malloc(in_size);
    fread(src, 1, in_size, in);
    fclose(in);

    driver_context(ctx, args);

    ir_t ir;
    ir_init(&ir);
    error_t err = front->scan_fn(src, in_size, &ir);

    if (error_dump(&err) > 0)
    {
        error_free(&err);
        ir_free(&ir);
        return 1;
    }
    else
    {
        printf("Program scanned\n");
    }

    if (args->validation)
    {
        err = pass_validation(ctx, &ir);

        if (error_dump(&err) > 0)
        {
            error_free(&err);
            ir_free(&ir);
            return 1;
        }
        else
        {
            printf("Program validated\n");
        }
    }

    if (args->folding)
    {
        err = pass_folding(ctx, &ir);

        if (error_dump(&err) > 0)
        {
            error_free(&err);
            ir_free(&ir);
            return 1;
        }
        else
        {
            printf("Program folded\n");
        }
    }

    bytebuffer_t buf;
    bytebuffer_init(&buf, BYTEBUFFER_BLOCK);

    err = back->asm_fn(ctx, &buf, &ir);

    if (error_dump(&err) > 0)
    {
        error_free(&err);
        ir_free(&ir);
        bytebuffer_free(&buf);
        return 1;
    }
    else
    {
        printf("Program compiled\n");
    }

    FILE *out = fopen(args->out, "wb");
    fwrite(buf.bytes, 1, buf.pos, out);
    fclose(out);

    free(src);
    ir_free(&ir);
    bytebuffer_free(&buf);
    return 0;
}

void
driver_context(context_t *ctx, args_t *args)
{
    bool true_value = true;
    bool false_value = false;

    bool *fwrite = args->flags & FLAG_WRITE ? &true_value : &false_value;
    bool *fread = args->flags & FLAG_WRITE ? &true_value : &false_value;
    bool *flibc = args->flags & FLAG_LIBC ? &true_value : &false_value;
    bool *fwrap = args->flags & FLAG_WRAP_CELL ? &true_value : &false_value;
    bool *fwrapptr = args->flags & FLAG_WRAP_PTR ? &true_value : &false_value;

    context_set(ctx, CTX_FWRITE, fwrite);
    context_set(ctx, CTX_FREAD, fread);
    context_set(ctx, CTX_FLIBC, flibc);
    context_set(ctx, CTX_FWRAP, fwrap);
    context_set(ctx, CTX_FWRAPPTR, fwrapptr);

    context_set(ctx, CTX_OASM, &true_value);
    context_set(ctx, CTX_OMACH, &false_value);
    context_set(ctx, CTX_CELLS, &args->cells);
    context_set(ctx, CTX_FUNCNAME, (char*)args->func_name);

    bool intelbin = args->flavor == FLAVOR_INTELBIN;
    bool intel = args->flavor == FLAVOR_INTELASM || intelbin;
    context_set(ctx, CTX_INTELASM, &intel);
    context_set(ctx, CTX_INTELBIN, &intelbin);

    context_set(ctx, CTX_EXTRA, NULL);
}
