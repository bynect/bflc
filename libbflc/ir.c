#include "ir.h"

#include <malloc.h>

void
ir_init(ir_t *ir)
{
    ir->instrs = NULL;
}

void
ir_node(ir_t *ir, uint8_t instr, intptr_t arg, pos_t pos)
{
    instr_t *node = malloc(sizeof(instr_t));
    node->instr = instr;
    node->arg = arg;
    node->pos = pos;
    node->next = NULL;

    if (ir->instrs == NULL)
    {
        ir->instrs = node;
    }
    else
    {
        instr_t *ptr = ir->instrs;
        while (ptr->next != NULL)
        {
            ptr = ptr->next;
        }
        ptr->next = node;
    }
}

void
ir_free(ir_t *ir)
{
    instr_t *instr = ir->instrs;
    while (instr != NULL)
    {
        instr_t *node = instr;
        instr = instr->next;
        free(node);
    }
}
