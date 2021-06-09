/*
    Copyright 2021 @bynect

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#define BFLC_INTERNAL
#include "ir.h"

#include <stdio.h>

void
ir_init(ir_t *ir)
{
    ir->instrs = NULL;
}

void
ir_node(context_t *ctx, ir_t *ir, uint8_t instr, intptr_t arg, pos_t pos)
{
    instr_t *node = ctx->mem->alloc_fn(sizeof(instr_t), ctx->mem->extra);
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
ir_free(context_t *ctx, ir_t *ir)
{
    instr_t *instr = ir->instrs;
    while (instr != NULL)
    {
        instr_t *node = instr;
        instr = instr->next;
        ctx->mem->free_fn(node, sizeof(instr_t), ctx->mem->extra);
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
        "%s (%ld) [line %u; column %u; offset %zu]",
        instrs[instr->instr - 1], instr->arg,
        instr->pos.line, instr->pos.column, instr->pos.offset
    );
}

uint32_t
ir_dump(const ir_t *ir)
{
    uint32_t count = 0;
    for (instr_t *instr = ir->instrs; instr != NULL; instr = instr->next)
    {
        instr_dump(instr);
        printf("\n");
        ++count;
    }
    return count;
}
