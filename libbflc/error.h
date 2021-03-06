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

#ifndef BFLC_ERROR_H
#define BFLC_ERROR_H

#include "ir.h"
#include "context.h"

#include <stdbool.h>

typedef struct error {
    const char *pretty;
    const instr_t *instr;
    struct error *next;
} error_t;

void error_init(error_t *err, const char *pretty, const instr_t *instr);

void error_node(context_t *ctx, error_t *err,
                const char *pretty, const instr_t *instr);

void error_free(context_t *ctx, error_t *err);

bool error_success(const error_t *err);

uint32_t error_dump(const error_t *err);

#endif
