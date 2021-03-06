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

#include "driver.h"
#include "args.h"

int
main(int argc, const char **argv)
{
    mem_t mem;
    mem_default(&mem);
    context_t *ctx = context_new(&mem);

    args_t args;
    error_t err = {NULL, NULL};
    args_parse(ctx, argc, argv, &args, &err);

    if (!error_success(&err))
    {
        error_dump(&err);
        error_free(ctx, &err);
        context_free(ctx);
        return 1;
    }

    int res = driver_main(ctx, &args);
    context_free(ctx);
    return res;
}
