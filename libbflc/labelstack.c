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
#include "labelstack.h"

void
labelstack_init(context_t *ctx, labelstack_t *labelstack, size_t min)
{
    min *= sizeof(uint32_t);
    labelstack->labels = ctx->mem->alloc_fn(min, ctx->mem->extra);
    labelstack->len = min;
    labelstack->pos = 0;
    labelstack->current = 0;
}

void
labelstack_reset(context_t *ctx, labelstack_t *labelstack, size_t min)
{
    min *= sizeof(uint32_t);
    if (labelstack->labels != NULL)
    {
        if (labelstack->len < min)
        {
            labelstack->labels = ctx->mem->realloc_fn(
                labelstack->labels, labelstack->len, min, ctx->mem->extra
            );
            labelstack->len = min;
        }
        labelstack->pos = 0;
        labelstack->current = 0;
        return;
    }

    labelstack->labels = ctx->mem->alloc_fn(min, ctx->mem->extra);
    labelstack->len = min;
    labelstack->pos = 0;
    labelstack->current = 0;
}

void
labelstack_push(context_t *ctx, labelstack_t *labelstack, uint32_t label)
{
    if ((labelstack->pos + 1) * sizeof(uint32_t) >= labelstack->len)
    {
        labelstack->labels = ctx->mem->realloc_fn(
            labelstack->labels,
            labelstack->len,
            labelstack->len + LABELSTACK_BLOCK * sizeof(uint32_t),
            ctx->mem->extra
        );
        labelstack->len += LABELSTACK_BLOCK * sizeof(uint32_t);
    }

    labelstack->labels[++labelstack->pos] = label;
}

bool
labelstack_pop(labelstack_t *labelstack, uint32_t *label)
{
    if (labelstack->pos == 0)
    {
        *label = 0;
        return false;
    }

    *label = labelstack->labels[labelstack->pos--];
    return true;
}

void
labelstack_free(context_t *ctx, labelstack_t *labelstack)
{
    ctx->mem->free_fn(labelstack->labels, labelstack->len, ctx->mem->extra);
    labelstack->labels = NULL;
    labelstack->len = 0;
    labelstack->pos = 0;
    labelstack->current = 0;
}
