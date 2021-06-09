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

#ifndef BFLC_MEM_H
#define BFLC_MEM_H

#include <stdint.h>
#include <stddef.h>

typedef void *(*mem_alloc_fn) (size_t size, void *extra);

typedef void *(*mem_realloc_fn) (
    void *ptr, size_t prev, size_t size, void *extra
);

typedef void (*mem_free_fn) (void *ptr, size_t prev, void *extra);

typedef struct {
    mem_alloc_fn alloc_fn;
    mem_realloc_fn realloc_fn;
    mem_free_fn free_fn;
    void *extra;
} mem_t;

void mem_default(mem_t *mem);

#endif
