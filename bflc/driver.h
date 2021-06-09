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

#ifndef BFLC_DRIVER_H
#define BFLC_DRIVER_H

#include "args.h"
#include "libbflc/context.h"

#include <stdint.h>
#include <stddef.h>

int driver_main(context_t *ctx, args_t *args);

void driver_context(context_t *ctx, args_t *args);

#endif
