#include "validation.h"

error_t
ir_validate(context_t *ctx, ir_t *ir)
{
    error_t err;
    error_init(&err, NULL, NULL);

    int32_t jmps = 0;
    size_t jmpbeg = 0;
    size_t jmpend = 0;

    size_t cellptr = 0;
    size_t cellmax;
    context_get(ctx, CTX_CELLS, &cellmax);

    bool wrap_ptr;
    context_get(ctx, CTX_FWRAPPTR, &wrap_ptr);

    for (size_t i = 0; i < ir->end; ++i)
    {
        switch (ir->instrs[i].instr)
        {
            case INSTR_PTRINC:
                if (cellptr == cellmax && !wrap_ptr)
                {
                    error_node(
                        &err, "Cell pointer out of lower bounds", ir->instrs + i
                    );
                }
                else
                {
                    ++cellptr;
                }
                break;

            case INSTR_PTRDEC:
                if (cellptr == 0 && !wrap_ptr)
                {
                    error_node(
                        &err, "Cell pointer out of upper bounds", ir->instrs + i
                    );
                }
                else
                {
                    --cellptr;
                }
                break;

            case INSTR_JMPBEG:
                ++jmps;
                jmpbeg = i;
                break;

            case INSTR_JMPEND:
                --jmps;
                jmpend = i;
                break;
        }
    }

    if (jmps != 0)
    {
        size_t jmp = jmps > 0 ? jmpbeg : jmpend;
        error_node(
            &err, "Unmatched jump", ir->instrs + jmp
        );
    }

    return err;
}
