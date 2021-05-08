#include "codegen.h"

error_t
codegen_run(context_t *ctx, codegen_t *codegen, bytebuffer_t *buf, ir_t *ir)
{
    error_t err;
    error_init(&err, NULL, NULL);

    bytebuffer_reset(buf, BYTEBUFFER_BLOCK);

    codegen->prologue_fn(ctx, buf, NULL, &err, codegen->extra);

    for (instr_t *instr = ir->instrs; instr != NULL; instr = instr->next)
    {
        switch (instr->instr)
        {
            case INSTR_PTRINC:
                codegen->ptrinc_fn(
                    ctx, buf, instr, &err, codegen->extra
                );
                break;

            case INSTR_PTRDEC:
                codegen->ptrdec_fn(
                    ctx, buf, instr, &err, codegen->extra
                );
                break;

            case INSTR_CELINC:
                codegen->celinc_fn(
                    ctx, buf, instr, &err, codegen->extra
                );
                break;

            case INSTR_CELDEC:
                codegen->celdec_fn(
                    ctx, buf, instr, &err, codegen->extra
                );
                break;

            case INSTR_OUTPUT:
                codegen->output_fn(
                    ctx, buf, instr, &err, codegen->extra
                );
                break;

            case INSTR_INPUT:
                codegen->input_fn(
                    ctx, buf, instr, &err, codegen->extra
                );
                break;

            case INSTR_JMPBEG:
                codegen->jmpbeg_fn(
                    ctx, buf, instr, &err, codegen->extra
                );
                break;

            case INSTR_JMPEND:
                codegen->jmpend_fn(
                    ctx, buf, instr, &err, codegen->extra
                );
                break;

            default:
                error_node(&err, "Invalid instruction", instr);
                break;
        }
    }

    codegen->epilogue_fn(ctx, buf, NULL, &err, codegen->extra);

    return err;
}
