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

#ifndef ARGS_H
#define ARGS_H

#include "libbflc/context.h"
#include "libbflc/error.h"

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define FLAVOR_INTELASM 1
#define FLAVOR_INTELBIN 2
#define FLAVOR_GNUASM 3

#define FLAG_WRITE (1 << 0)
#define FLAG_READ (1 << 1)
#define FLAG_LIBC (1 << 2)
#define FLAG_WRAP_CELL (1 << 3)
#define FLAG_WRAP_PTR (1 << 4)

typedef struct {
    const char *exe;
    const char *out;
    const char *in;
    const char *front;
    const char *back;
    bool validation;
    bool folding;
    const char *func_name;
    uint8_t flavor;
    size_t cells;
    uint32_t flags;
} args_t;

void args_parse(context_t *ctx, int argc, const char **argv,
                args_t *args, error_t *err);

void args_dump(const args_t *args);

#endif
