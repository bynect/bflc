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

#ifndef BFLC_RELOC_H
#define BFLC_RELOC_H

#include "labelstack.h"
#include "bytebuffer.h"

#define RELOC_BLOCK 64

#define RELOC_PATCH_MEM 1
#define RELOC_PATCH_PTR 2

typedef struct {
    labelstack_t labelstack;
    size_t off_len;
    size_t off_pos;
    struct {
        uint8_t type;
        uint32_t off;
    } *offs;
} reloc_t;

typedef void (*addr_patch_fn) (bytebuffer_t *buf, uint32_t base,
                                uint8_t type, uint32_t off, void *extra);

void reloc_init(reloc_t *reloc, size_t min);

void reloc_reset(reloc_t *reloc, size_t min);

void reloc_write(reloc_t *reloc, uint8_t type, uint32_t off);

void reloc_patch(reloc_t *reloc, bytebuffer_t *buf, uint32_t cellmem,
                uint32_t cellptr, addr_patch_fn addr_patch, void *extra);

void reloc_free(reloc_t *reloc);

#endif
