#include "folding.h"

#include <stdlib.h>

static void
instr_folding(instr_t *instr, uint8_t inc, uint8_t dec)
{
    intptr_t arg = 0;
    instr_t *node;
    instr_t *ptr = instr;

    if (instr->instr == inc)
    {
        arg += instr->arg;
        instr = instr->next;
    }
    else if (instr->instr == dec)
    {
        arg -= instr->arg;
        instr = instr->next;
    }

    while (instr != NULL)
    {
        if (instr->instr == inc)
        {
            arg += instr->arg;
            node = instr;
            instr = instr->next;
        }
        else if (instr->instr == dec)
        {
            arg -= instr->arg;
            node = instr;
            instr = instr->next;
        }
        else
        {
            break;
        }

        free(node);
    }

    if (arg == 0 && instr != NULL)
    {
        ptr->instr = instr->instr;
        ptr->arg = instr->arg;
        ptr->pos = instr->pos;
        ptr->next = instr->next;
        free(instr);
        return;
    }

    ptr->instr = arg >= 0 ? inc : dec;
    ptr->arg = labs(arg);
    ptr->next = instr;
}

error_t
pass_folding(context_t *ctx, ir_t *ir)
{
    error_t err;
    error_init(&err, NULL, NULL);

    instr_t *instr = ir->instrs;
    while (instr != NULL)
    {
        switch (instr->instr)
        {
            case INSTR_PTRINC:
            case INSTR_PTRDEC:
                instr_folding(instr, INSTR_PTRINC, INSTR_PTRDEC);
                break;

            case INSTR_CELINC:
            case INSTR_CELDEC:
                instr_folding(instr, INSTR_CELINC, INSTR_CELDEC);
                break;
        }
        instr = instr->next;
    }

    return err;
}
