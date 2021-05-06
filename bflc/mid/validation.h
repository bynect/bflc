#ifndef BFLC_VALIDATION_H
#define BFLC_VALIDATION_H

#include "../ir.h"
#include "../error.h"
#include "../context.h"

error_t ir_validate(context_t *ctx, ir_t *ir);

#endif
