#include "ir.h"

#include <malloc.h>

void
ir_init(ir_t *ir, size_t min)
{
    ir->instrs = malloc(min);
    ir->len = min;
    ir->end = 0;
}

void
ir_reset(ir_t *ir, size_t min)
{
    if (ir->instrs != NULL)
    {
        if (ir->len < min)
        {
            ir->instrs = realloc(ir->instrs, min);
            ir->len = min;
            ir->end = 0;
        }
        return;
    }

    ir->instrs = malloc(min);
    ir->len = min;
    ir->end = 0;
}

void
ir_ensure(ir_t *ir, size_t len)
{
    if (ir->len < len)
    {
        ir->len = len + IR_BLOCK;
        ir->instrs = realloc(ir->instrs, ir->len);
    }
}

void
ir_free(ir_t *ir)
{
    free(ir->instrs);
    ir->instrs = NULL;
    ir->len = 0;
    ir->end = 0;
}
