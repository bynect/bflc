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

#include "codegen.h"

error_t
codegen_run(context_t *ctx, codegen_t *codegen, bytebuffer_t *buf, ir_t *ir)
{
    error_t err;
    error_init(&err, NULL, NULL);

    bytebuffer_reset(ctx, buf, BYTEBUFFER_BLOCK);
    bool run = true;

    if (codegen->prologue_fn)
    {
        run = codegen->prologue_fn(ctx, buf, NULL, &err, codegen->extra);
    }

    for (instr_t *instr = ir->instrs; instr != NULL; instr = instr->next)
    {
        if (!run)
        {
            return err;
        }

        switch (instr->instr)
        {
            case INSTR_PTRINC:
                run = codegen->ptrinc_fn(
                    ctx, buf, instr, &err, codegen->extra
                );
                break;

            case INSTR_PTRDEC:
                run = codegen->ptrdec_fn(
                    ctx, buf, instr, &err, codegen->extra
                );
                break;

            case INSTR_CELINC:
                run = codegen->celinc_fn(
                    ctx, buf, instr, &err, codegen->extra
                );
                break;

            case INSTR_CELDEC:
                run = codegen->celdec_fn(
                    ctx, buf, instr, &err, codegen->extra
                );
                break;

            case INSTR_OUTPUT:
                run = codegen->output_fn(
                    ctx, buf, instr, &err, codegen->extra
                );
                break;

            case INSTR_INPUT:
                run = codegen->input_fn(
                    ctx, buf, instr, &err, codegen->extra
                );
                break;

            case INSTR_JMPBEG:
                run = codegen->jmpbeg_fn(
                    ctx, buf, instr, &err, codegen->extra
                );
                break;

            case INSTR_JMPEND:
                run = codegen->jmpend_fn(
                    ctx, buf, instr, &err, codegen->extra
                );
                break;

            default:
                error_node(ctx, &err, "Invalid instruction", instr);
                break;
        }
    }

    if (codegen->epilogue_fn)
    {
        codegen->epilogue_fn(ctx, buf, NULL, &err, codegen->extra);
    }

    return err;
}
