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

#ifndef BFLC_LABELSTACK_H
#define BFLC_LABELSTACK_H

#include "context.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define LABELSTACK_BLOCK 32

typedef struct {
    uint32_t *labels;
    size_t len;
    size_t pos;
    uint32_t current;
} labelstack_t;

void labelstack_init(context_t *ctx, labelstack_t *labelstack, size_t min);

void labelstack_reset(context_t *ctx, labelstack_t *labelstack, size_t min);

void labelstack_push(context_t *ctx, labelstack_t *labelstack, uint32_t label);

bool labelstack_pop(labelstack_t *labelstack, uint32_t *label);

void labelstack_free(context_t *ctx, labelstack_t *labelstack);

#endif
