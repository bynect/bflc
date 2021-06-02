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

#ifndef BFLC_BACK_X86_64_H
#define BFLC_BACK_X86_64_H

#include "../bytebuffer.h"
#include "../ir.h"
#include "../error.h"
#include "../context.h"

error_t emit_asm_x86_64(context_t *ctx, bytebuffer_t *buf, ir_t *ir);

error_t emit_mach_x86_64(context_t *ctx, bytebuffer_t *buf, ir_t *ir);

#endif
