#include "x86_64.h"
#include "../codegen.h"
#include "../labelstack.h"

#include <stdio.h>

static bool
prologue_asm_x86_64(context_t *ctx, bytebuffer_t *buf,
                    instr_t *instr, error_t *err, void *extra)
{
    const char *format =
        "	.bss\n"
        "	.align	32\n"
        "	.type	__cellmem, @object\n"
        "	.size	__cellmem, %zu\n"
        "__cellmem:\n"
        "	.zero	%zu\n"
        "\n"
        "	.data\n"
        "	.align	8\n"
        "	.type	__cellptr, @object\n"
        "	.size	__cellptr, 8\n"
        "__cellptr:\n"
        "	.quad	__cellmem\n"
        "\n"
        "	.text\n"
        "	.globl	main\n"
        "	.type	main, @function\n"
        "	.align	16\n"
        "main:\n"
        "	pushq	%rbp\n"
        "	movq	%rsp, %rbp\n";

    size_t cells;
    context_get(ctx, CTX_CELLS, &cells);

    const size_t size = snprintf(NULL, 0, format, cells, cells);
    uint8_t prologue[size + 1];

    snprintf(prologue, size + 1, format, cells, cells);
    bytebuffer_writes(buf, prologue, size);

    return true;
}

static bool
epilogue_asm_x86_64(context_t *ctx, bytebuffer_t *buf,
                    instr_t *instr, error_t *err, void *extra)
{
    const uint8_t epilogue[] =
        "	movl	$0, %eax\n"
        "	popq	%rbp\n"
        "	ret\n"
        "	.size	main, .-main\n";

    bytebuffer_writes(buf, epilogue, sizeof(epilogue));

    return true;
}

static bool
ptrinc_asm_x86_64(context_t *ctx, bytebuffer_t *buf,
                instr_t *instr, error_t *err, void *extra)
{
    if (instr->arg < 0)
    {
        error_node(err, "Invalid instruction argument", instr);
        return false;
    }

    const char *format =
        "	movq	__cellptr(%%rip), %%rax\n"
        "	addq	$%ld, %%rax\n"
        "	movq	%%rax, __cellptr(%%rip)\n";

    const size_t size = snprintf(NULL, 0, format, instr->arg);
    uint8_t ptrinc[size + 1];

    snprintf(ptrinc, size + 1, format, instr->arg);
    bytebuffer_writes(buf, ptrinc, size);

    return true;
}

static bool
ptrdec_asm_x86_64(context_t *ctx, bytebuffer_t *buf,
                instr_t *instr, error_t *err, void *extra)
{
    if (instr->arg < 0)
    {
        error_node(err, "Invalid instruction argument", instr);
        return false;
    }

    const char *format =
        "	movq	__cellptr(%%rip), %%rax\n"
        "	subq	$%ld, %%rax\n"
        "	movq	%%rax, __cellptr(%%rip)\n";

    const size_t size = snprintf(NULL, 0, format, instr->arg);
    uint8_t ptrdec[size + 1];

    snprintf(ptrdec, size + 1, format, instr->arg);
    bytebuffer_writes(buf, ptrdec, size);

    return true;
}

static bool
celinc_asm_x86_64(context_t *ctx, bytebuffer_t *buf,
                instr_t *instr, error_t *err, void *extra)
{
    if (instr->arg < 0)
    {
        error_node(err, "Invalid instruction argument", instr);
        return false;
    }

    const char *format =
        "	movq	__cellptr(%%rip), %%rax\n"
        "	movzbl	(%%rax), %%edx\n"
        "	addl	$%ld, %%edx\n"
        "	movb	%%dl, (%%rax)\n";

    const size_t size = snprintf(NULL, 0, format, instr->arg);
    uint8_t celinc[size + 1];

    snprintf(celinc, size + 1, format, instr->arg);
    bytebuffer_writes(buf, celinc, size);

    return true;
}

static bool
celdec_asm_x86_64(context_t *ctx, bytebuffer_t *buf,
                instr_t *instr, error_t *err, void *extra)
{
    if (instr->arg < 0)
    {
        error_node(err, "Invalid instruction argument", instr);
        return false;
    }

    const char *format =
        "	movq	__cellptr(%%rip), %%rax\n"
        "	movzbl	(%%rax), %%edx\n"
        "	subl	$%ld, %%edx\n"
        "	movb	%%dl, (%%rax)\n";

    const size_t size = snprintf(NULL, 0, format, instr->arg);
    uint8_t celdec[size + 1];

    snprintf(celdec, size + 1, format, instr->arg);
    bytebuffer_writes(buf, celdec, size);

    return true;
}

static bool
output_asm_x86_64(context_t *ctx, bytebuffer_t *buf,
                instr_t *instr, error_t *err, void *extra)
{
    const uint8_t output[] =
        "	movq	__cellptr(%rip), %rax\n"
        "	movzbl	(%rax), %eax\n"
        "	movsbl	%al, %eax\n"
        "	movl	%eax, %edi\n"
        "	call	putchar@PLT\n";

    bytebuffer_writes(buf, output, sizeof(output) - 1);

    return true;
}

static bool
input_asm_x86_64(context_t *ctx, bytebuffer_t *buf,
                instr_t *instr, error_t *err, void *extra)
{
    const uint8_t input[] =
        "	call	getchar@PLT\n"
        "	movl	%eax, %edx\n"
        "	movq	__cellptr(%rip), %rax\n"
        "	movb	%dl, (%rax)\n";

    bytebuffer_writes(buf, input, sizeof(input) - 1);

    return true;
}

static bool
jmpbeg_asm_x86_64(context_t *ctx, bytebuffer_t *buf,
                instr_t *instr, error_t *err, void *extra)
{
    labelstack_t *labelstack = extra;

    uint32_t current = labelstack->current;
    uint32_t target = current + 1;

    const char *format =
        "L%u:\n"
        "	movq	__cellptr(%%rip), %%rax\n"
        "	movb	(%%rax), %%al\n"
        "	testb	%%al, %%al\n\n"
        "	je	L%u\n";

    const size_t size = snprintf(NULL, 0, format, current, target);
    uint8_t jmpbeg[size + 1];

    snprintf(jmpbeg, size + 1, format, current, target);
    bytebuffer_writes(buf, jmpbeg, size);

    labelstack_push(labelstack, labelstack->current++);
    labelstack_push(labelstack, labelstack->current++);

    return true;
}

static bool
jmpend_asm_x86_64(context_t *ctx, bytebuffer_t *buf,
                instr_t *instr, error_t *err, void *extra)
{
    labelstack_t *labelstack = extra;

    uint32_t next;
    labelstack_pop(labelstack, &next);

    uint32_t target;
    labelstack_pop(labelstack, &target);

    const char *format =
        "	jmp L%u\n"
        "L%u:\n";

    const size_t size = snprintf(NULL, 0, format, target, next);
    uint8_t jmpend[size + 1];

    snprintf(jmpend, size + 1, format, target, next);
    bytebuffer_writes(buf, jmpend, size);

    return true;
}

error_t
emit_asm_x86_64(context_t *ctx, bytebuffer_t *buf, ir_t *ir)
{
    labelstack_t labelstack;
    labelstack_init(&labelstack, LABELSTACK_BLOCK);

    codegen_t codegen;
    codegen.prologue_fn = prologue_asm_x86_64;
    codegen.epilogue_fn = epilogue_asm_x86_64;
    codegen.ptrinc_fn = ptrinc_asm_x86_64;
    codegen.ptrdec_fn = ptrdec_asm_x86_64;
    codegen.celinc_fn = celinc_asm_x86_64;
    codegen.celdec_fn = celdec_asm_x86_64;
    codegen.output_fn = output_asm_x86_64;
    codegen.input_fn = input_asm_x86_64;
    codegen.jmpbeg_fn = jmpbeg_asm_x86_64;
    codegen.jmpend_fn = jmpend_asm_x86_64;
    codegen.extra = &labelstack;

    error_t err = codegen_run(ctx, &codegen, buf, ir);
    labelstack_free(&labelstack);

    return err;
}

error_t
emit_mach_x86_64(context_t *ctx, bytebuffer_t *buf, ir_t *ir)
{
    error_t err;
    error_init(&err, NULL, NULL);
    return err;
}
