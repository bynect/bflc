#include "ir.h"

#include <malloc.h>
#include <stdio.h>

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

void
instr_dump(const instr_t *instr)
{
    const char *instrs[] = {
        "ptrinc", "ptrdec",
        "celinc", "celdec",
        "output", "input",
        "jmpbeg", "jmpend"
    };

    printf(
        "%d (%ld) [line %u; column %u; offset %zu]",
        instrs[instr->instr - 1], instr->arg,
        instr->pos.line, instr->pos.column, instr->pos.offset
    );
}

void
ir_dump(const ir_t *ir)
{
    for (instr_t *instr = ir->instrs; instr != NULL; instr = instr->next)
    {
        instr_dump(instr);
        printf("\n");
    }
}
