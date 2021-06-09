#ifndef BFLC_DRIVER_H
#define BFLC_DRIVER_H

#include "args.h"
#include "libbflc/context.h"

#include <stdint.h>
#include <stddef.h>

int driver_main(context_t *ctx, args_t *args);

void driver_context(context_t *ctx, args_t *args);

#endif
