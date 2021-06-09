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
#include "error.h"
#include "ir.h"

#include <stdio.h>

void
error_init(error_t *err, const char *pretty, const instr_t *instr)
{
    err->pretty = pretty;
    err->instr = instr;
    err->next = NULL;
}

void
error_node(context_t *ctx, error_t *err,
            const char *pretty, const instr_t *instr)
{
    if (err->pretty == NULL && err->instr == NULL)
    {
        error_init(err, pretty, instr);
    }
    else
    {
        error_t *node = ctx->mem->alloc_fn(sizeof(error_t), ctx->mem->extra);
        error_init(node, pretty, instr);

        while (err->next != NULL)
        {
            err = err->next;
        }
        err->next = node;
    }
}

void
error_free(context_t *ctx, error_t *err)
{
    err = err->next;
    while (err != NULL)
    {
        error_t *node = err;
        err = err->next;
        ctx->mem->free_fn(node, sizeof(error_t), ctx->mem->extra);
    }
}

bool
error_success(const error_t *err)
{
    return err->pretty == NULL && err->instr == NULL;
}

uint32_t
error_dump(const error_t *err)
{
    uint32_t count = 0;
    if (err->pretty != NULL || err->instr != NULL)
    {
        for (const error_t *node = err; node != NULL; node = node->next)
        {
            if (node->instr != NULL)
            {
                printf("%s on instr {", node->pretty);
                instr_dump(node->instr);
                printf("}\n");
            }
            else
            {
                printf("%s\n", node->pretty);
            }
            ++count;
        }
    }
    return count;
}
