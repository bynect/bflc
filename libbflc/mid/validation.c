#include "validation.h"

error_t
pass_validation(context_t *ctx, ir_t *ir)
{
    error_t err;
    error_init(&err, NULL, NULL);

    int32_t jmps = 0;
    instr_t *jmpbeg;
    instr_t *jmpend;

    size_t cellptr = 0;
    size_t cellmax;
    context_get(ctx, CTX_CELLS, &cellmax);

    bool wrap_ptr;
    context_get(ctx, CTX_FWRAPPTR, &wrap_ptr);

    for (instr_t *instr = ir->instrs; instr != NULL; instr = instr->next)
    {
        switch (instr->instr)
        {
            case INSTR_PTRINC:
                if (cellptr == cellmax && !wrap_ptr)
                {
                    error_node(
                        &err, "Cell pointer out of upper bounds", instr
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
                        &err, "Cell pointer out of lower bounds", instr
                    );
                }
                else
                {
                    --cellptr;
                }
                break;

            case INSTR_JMPBEG:
                ++jmps;
                jmpbeg = instr;
                break;

            case INSTR_JMPEND:
                --jmps;
                jmpend = instr;
                break;
        }
    }

    if (jmps != 0)
    {
        error_node(
            &err, "Unmatched jump", jmps > 0 ? jmpbeg : jmpend
        );
    }

    return err;
}
