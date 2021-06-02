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

#include "labelstack.h"

#include <malloc.h>

void
labelstack_init(labelstack_t *labelstack, size_t min)
{
    min *= sizeof(uint32_t);
    labelstack->labels = malloc(min);
    labelstack->len = min;
    labelstack->pos = 0;
    labelstack->current = 0;
}

void
labelstack_reset(labelstack_t *labelstack, size_t min)
{
    min *= sizeof(uint32_t);
    if (labelstack->labels != NULL)
    {
        if (labelstack->len < min)
        {
            labelstack->labels = realloc(labelstack->labels, min);
            labelstack->len = min;
        }
        labelstack->pos = 0;
        labelstack->current = 0;
        return;
    }

    labelstack->labels = malloc(min);
    labelstack->len = min;
    labelstack->pos = 0;
    labelstack->current = 0;
}

void
labelstack_push(labelstack_t *labelstack, uint32_t label)
{
    if (labelstack->pos + 1 >= labelstack->len)
    {
        labelstack->len += LABELSTACK_BLOCK * sizeof(uint32_t);
        labelstack->labels = realloc(labelstack->labels, labelstack->len);
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
labelstack_free(labelstack_t *labelstack)
{
    free(labelstack->labels);
    labelstack->labels = NULL;
    labelstack->len = 0;
    labelstack->pos = 0;
    labelstack->current = 0;
}
