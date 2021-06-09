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

#ifndef BFLC_BYTEBUFFER_H
#define BFLC_BYTEBUFFER_H

#include "context.h"

#include <stdint.h>
#include <stddef.h>

#define BYTEBUFFER_BLOCK 64

typedef struct {
    uint8_t *bytes;
    size_t len;
    size_t pos;
} bytebuffer_t;

void bytebuffer_init(context_t *ctx, bytebuffer_t *buf, size_t min);

void bytebuffer_reset(context_t *ctx, bytebuffer_t *buf, size_t min);

void bytebuffer_write(context_t *ctx, bytebuffer_t *buf, uint8_t byte);

void bytebuffer_writes(context_t *ctx, bytebuffer_t *buf, const uint8_t *bytes, size_t len);

void bytebuffer_free(context_t *ctx, bytebuffer_t *buf);

#endif
