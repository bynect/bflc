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

#include "x86_64.h"
#include "../codegen.h"
#include "../labelstack.h"
#include "../reloc.h"

#include <stdio.h>
#include <string.h>

static bool
prologue_asm_x86_64(context_t *ctx, bytebuffer_t *buf,
                    instr_t *instr, error_t *err, void *extra)
{
    bool intel;
    context_get(ctx, CTX_INTELASM, &intel);

    if (intel)
    {
        bool intel_bin;
        context_get(ctx, CTX_INTELBIN, &intel_bin);

        bool f_write;
        context_get(ctx, CTX_FWRITE, &f_write);

        bool f_read;
        context_get(ctx, CTX_FREAD, &f_read);

        if (intel_bin && !(f_write && f_read))
        {
            error_node(err, "Binary output can use only syscalls", NULL);
            return false;
        }

        if (intel_bin)
        {
            const uint8_t prologue[] =
                "	bits	64\n"
                "	section	.text\n"
                "	align	16\n"
                "__prog:\n"
                "	push	rbp\n"
                "	mov	rbp, rsp\n"
                "	lea	rax, [rel __cellmem]\n"
                "	mov	[rel __cellptr], rax\n";

            bytebuffer_writes(buf, prologue, sizeof(prologue) - 1);
        }
        else
        {
            size_t cells;
            context_get(ctx, CTX_CELLS, &cells);

            char *func_name;
            context_get(ctx, CTX_FUNCNAME, &func_name);

            const char *format =
                "	bits	64\n"
                "	extern	putchar\n"
                "	extern	getchar\n"
                "\n"
                "	section	.bss\n"
                "	align	32\n"
                "__cellmem:\n"
                "	resb	%zu\n"
                "\n"
                "	section	.data\n"
                "	align	8\n"
                "__cellptr:\n"
                "	dq	__cellmem\n"
                "\n"
                "	section	.text\n"
                "	global	%s\n"
                "	align	16\n"
                "%s:\n"
                "	push	rbp\n"
                "	mov	rbp, rsp\n";

            const size_t size = snprintf(NULL, 0, format, cells,
                                        func_name, func_name);
            uint8_t prologue[size + 1];

            snprintf(prologue, size + 1, format, cells,
                    func_name, func_name);
            bytebuffer_writes(buf, prologue, size);
        }
    }
    else
    {
            size_t cells;
        context_get(ctx, CTX_CELLS, &cells);

        char *func_name;
        context_get(ctx, CTX_FUNCNAME, &func_name);

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
            "	.globl	%s\n"
            "	.type	%s, @function\n"
            "	.align	16\n"
            "%s:\n"
            "	pushq	%%rbp\n"
            "	movq	%%rsp, %%rbp\n";

        const size_t size = snprintf(NULL, 0, format, cells, cells,
                                    func_name, func_name, func_name);
        uint8_t prologue[size + 1];

        snprintf(prologue, size + 1, format, cells, cells,
                func_name, func_name, func_name);
        bytebuffer_writes(buf, prologue, size);
    }

    return true;
}

static bool
epilogue_asm_x86_64(context_t *ctx, bytebuffer_t *buf,
                    instr_t *instr, error_t *err, void *extra)
{
    char *func_name;
    context_get(ctx, CTX_FUNCNAME, &func_name);

    bool intel;
    context_get(ctx, CTX_INTELASM, &intel);

    if (intel)
    {
        bool intel_bin;
        context_get(ctx, CTX_INTELBIN, &intel_bin);

        if (intel_bin)
        {
            size_t cells;
            context_get(ctx, CTX_CELLS, &cells);

            const char *format =
                "	mov	eax, 0\n"
                "	pop	rbp\n"
                "	ret\n"
                "\n"
                "	align	32\n"
                "__cellmem:\n"
                "	times	%zu db 0\n"
                "\n"
                "	align	8\n"
                "__cellptr:\n"
                "	dq	0\n";

            const size_t size = snprintf(NULL, 0, format, cells);
            uint8_t epilogue[size + 1];

            snprintf(epilogue, size + 1, format, cells);
            bytebuffer_writes(buf, epilogue, size);
        }
        else
        {
            uint8_t epilogue[] =
                "	mov	eax, 0\n"
                "	pop	rbp\n"
                "	ret\n";

            bytebuffer_writes(buf, epilogue, sizeof(epilogue) - 1);
        }
    }
    else
    {
        const char *format =
            "	movl	$0, %%eax\n"
            "	popq	%%rbp\n"
            "	ret\n"
            "	.size	%s, .-%s\n";

        const size_t size = snprintf(NULL, 0, format, func_name, func_name);
        uint8_t epilogue[size + 1];

        snprintf(epilogue, size + 1, format, func_name, func_name);
        bytebuffer_writes(buf, epilogue, size);
    }

    return true;
}

static bool
ptrinc_asm_x86_64(context_t *ctx, bytebuffer_t *buf,
                instr_t *instr, error_t *err, void *extra)
{
    if (instr->arg < 0)
    {
        error_node(err, "Invalid instruction argument", instr);
        return true;
    }

    bool intel;
    context_get(ctx, CTX_INTELASM, &intel);

    char *format;

    if (intel)
    {
        format =
            "	mov	rax, [rel __cellptr]\n"
            "	add	rax, %ld\n"
            "	mov	[rel __cellptr], rax\n";
    }
    else
    {
        format =
            "	movq	__cellptr(%%rip), %%rax\n"
            "	addq	$%ld, %%rax\n"
            "	movq	%%rax, __cellptr(%%rip)\n";
    }

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
        return true;
    }

    bool intel;
    context_get(ctx, CTX_INTELASM, &intel);

    char *format;

    if (intel)
    {
        format =
            "	mov	rax, [rel __cellptr]\n"
            "	sub	rax, %ld\n"
            "	mov	[rel __cellptr], rax\n";
    }
    else
    {
        format =
            "	movq	__cellptr(%%rip), %%rax\n"
            "	subq	$%ld, %%rax\n"
            "	movq	%%rax, __cellptr(%%rip)\n";
    }

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
        return true;
    }

    bool intel;
    context_get(ctx, CTX_INTELASM, &intel);

    char *format;

    if (intel)
    {
        format =
            "	mov	rax, [rel __cellptr]\n"
            "	movzx	edx, BYTE [rax]\n"
            "	add	edx, %ld\n"
            "	mov	[rax], BYTE dl\n";
    }
    else
    {
        format =
            "	movq	__cellptr(%%rip), %%rax\n"
            "	movzbl	(%%rax), %%edx\n"
            "	addl	$%ld, %%edx\n"
            "	movb	%%dl, (%%rax)\n";
    }

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
        return true;
    }

    bool intel;
    context_get(ctx, CTX_INTELASM, &intel);

    char *format;

    if (intel)
    {
        format =
            "	mov	rax, [rel __cellptr]\n"
            "	movzx	edx, BYTE [rax]\n"
            "	sub	edx, %ld\n"
            "	mov	[rax], BYTE dl\n";
    }
    else
    {
        format =
            "	movq	__cellptr(%%rip), %%rax\n"
            "	movzbl	(%%rax), %%edx\n"
            "	subl	$%ld, %%edx\n"
            "	movb	%%dl, (%%rax)\n";
    }

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
    bool f_write;
    context_get(ctx, CTX_FWRITE, &f_write);

    bool intel;
    context_get(ctx, CTX_INTELASM, &intel);

    if (intel)
    {
        if (f_write)
        {
            const uint8_t output[] =
                "	mov	rax, 1\n"
                "	mov	rdi, 1\n"
                "	mov	rsi, [rel __cellptr]\n"
                "	mov	rdx, 1\n"
                "	syscall\n";

            bytebuffer_writes(buf, output, sizeof(output) - 1);
        }
        else
        {
            const uint8_t output[] =
                "	mov	rax, [rel __cellptr]\n"
                "	movzx	eax, BYTE [rax]\n"
                "	movsx	eax, al\n"
                "	mov	edi, eax\n"
                "	call	[rel putchar wrt ..got]\n";

            bytebuffer_writes(buf, output, sizeof(output) - 1);
        }
    }
    else
    {
        if (f_write)
        {
            const uint8_t output[] =
                "	movq	$1, %rax\n"
                "	movq	$1, %rdi\n"
                "	movq	__cellptr(%rip), %rsi\n"
                "	movq	$1, %rdx\n"
                "	syscall\n";

            bytebuffer_writes(buf, output, sizeof(output) - 1);
        }
        else
        {
            const uint8_t output[] =
                "	movq	__cellptr(%rip), %rax\n"
                "	movzbl	(%rax), %eax\n"
                "	movsbl	%al, %eax\n"
                "	movl	%eax, %edi\n"
                "	call	putchar@PLT\n";

            bytebuffer_writes(buf, output, sizeof(output) - 1);
        }
    }

    return true;
}

static bool
input_asm_x86_64(context_t *ctx, bytebuffer_t *buf,
                instr_t *instr, error_t *err, void *extra)
{
    bool f_read;
    context_get(ctx, CTX_FREAD, &f_read);

    bool intel;
    context_get(ctx, CTX_INTELASM, &intel);

    if (intel)
    {
        if (f_read)
        {
            const uint8_t input[] =
                "	mov	rax, 0\n"
                "	mov	rdi, 0\n"
                "	mov	rsi, [rel __cellptr]\n"
                "	mov	rdx, 1\n"
                "	syscall\n";

            bytebuffer_writes(buf, input, sizeof(input) - 1);
        }
        else
        {
            const uint8_t input[] =
                "	call	[rel getchar wrt ..got]\n"
                "	mov	edx, eax\n"
                "	mov	rax, [rel __cellptr]\n"
                "	mov	BYTE [rax], dl\n";

            bytebuffer_writes(buf, input, sizeof(input) - 1);
        }
    }
    else
    {
        if (f_read)
        {
            const uint8_t input[] =
                "	movq	$0, %rax\n"
                "	movl	$0, %rdi\n"
                "	movl	__cellptr(%rip), %rsi\n"
                "	movl	$1, %rdx\n"
                "	syscall\n";

            bytebuffer_writes(buf, input, sizeof(input) - 1);
        }
        else
        {
            const uint8_t input[] =
                "	call	getchar@PLT\n"
                "	movl	%eax, %edx\n"
                "	movq	__cellptr(%rip), %rax\n"
                "	movb	%dl, (%rax)\n";

            bytebuffer_writes(buf, input, sizeof(input) - 1);
        }
    }

    return true;
}

static bool
jmpbeg_asm_x86_64(context_t *ctx, bytebuffer_t *buf,
                instr_t *instr, error_t *err, void *extra)
{
    labelstack_t *labelstack = extra;

    uint32_t current = labelstack->current;
    uint32_t target = current + 1;

    bool intel;
    context_get(ctx, CTX_INTELASM, &intel);

    char *format;

    if (intel)
    {
        format =
            ".L%u:\n"
            "	mov	rax, [rel __cellptr]\n"
            "	mov	al, BYTE [rax]\n"
            "	test	al, al\n"
            "	je	.L%u\n";
    }
    else
    {
        format =
            ".L%u:\n"
            "	movq	__cellptr(%%rip), %%rax\n"
            "	movb	(%%rax), %%al\n"
            "	testb	%%al, %%al\n"
            "	je	.L%u\n";
    }

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
        "	jmp	.L%u\n"
        ".L%u:\n";

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

static void
addr_patch_mach_x86_64(bytebuffer_t *buf, uint32_t base,
                        uint8_t type, uint32_t off, void *extra)
{
    uint32_t addr = base - (off + 7); // pc off

    buf->bytes[off + 0] = (uint8_t)(addr >> 0);
    buf->bytes[off + 1] = (uint8_t)(addr >> 8);
    buf->bytes[off + 2] = (uint8_t)(addr >> 16);
    buf->bytes[off + 3] = (uint8_t)(addr >> 24);
}

static bool
prologue_mach_x86_64(context_t *ctx, bytebuffer_t *buf,
                    instr_t *instr, error_t *err, void *extra)
{
    bool f_write;
    context_get(ctx, CTX_FWRITE, &f_write);

    bool f_read;
    context_get(ctx, CTX_FREAD, &f_read);

    if (!(f_write && f_read))
    {
        error_node(err, "Binary output can use only syscalls", NULL);
        return false;
    }

    const uint8_t prologue[] = {
        0x55, // pushq %rbp
        0x48, 0x89, 0xe5, // movq %rsp, %rbp
        0x48, 0x8d, 0x05, 0xaf, 0xaf, 0xaf, 0xaf, // leaq cellmem(%rip), %rax
        0x48, 0x89, 0x05, 0xaf, 0xaf, 0xaf, 0xaf // movq %rax, cellptr(%rip)
    };
    bytebuffer_writes(buf, prologue, sizeof(prologue));

    reloc_t *reloc = extra;
    reloc_write(reloc, RELOC_PATCH_MEM, 7);
    reloc_write(reloc, RELOC_PATCH_PTR, 14);

    return true;
}

static bool
epilogue_mach_x86_64(context_t *ctx, bytebuffer_t *buf,
                    instr_t *instr, error_t *err, void *extra)
{
    const uint8_t epilogue[] = {
        0xb8, 0x00, 0x00, 0x00, 0x00,// movq $0, %eax
        0x5d, // popq %rbp
        0xc3 // ret
    };
    bytebuffer_writes(buf, epilogue, sizeof(epilogue));

    while ((buf->pos % 16) != 0)
    {
        bytebuffer_write(buf, 0x90); // pad
    }

    uint32_t cellmem = buf->pos + 3;

    size_t cells;
    context_get(ctx, CTX_CELLS, &cells);

    while (cells--)
    {
        bytebuffer_write(buf, 0x00);
    }

    while ((buf->pos % 32) != 0)
    {
        bytebuffer_write(buf, 0x90); // pad
    }

    uint32_t cellptr = buf->pos + 3;

    uint8_t ptr[8] = {0};
    bytebuffer_writes(buf, ptr, sizeof(ptr));

    reloc_t *reloc = extra;
    reloc_patch(
        reloc, buf, cellmem, cellptr, addr_patch_mach_x86_64, NULL
    );

    return true;
}

static bool
instr_mach_x86_64(context_t *ctx, bytebuffer_t *buf,
                instr_t *instr, error_t *err, void *extra)
{
    reloc_t *reloc = extra;
    uint32_t addr = 0;

    uint8_t ptrbin[] = {
        0x48, 0x8b, 0x05, 0xaf, 0xaf, 0xaf, 0xaf, // movq __cellptr(%rip), %rax
        0x48, 0x83, 0x00, 0x00, // op $arg, %rax
        0x48, 0x89, 0x05, 0xaf, 0xaf, 0xaf, 0xaf // movq %rax, __cellptr(%rip)
    };

    uint8_t celbin[] = {
        0x48, 0x8b, 0x05, 0xaf, 0xaf, 0xaf, 0xaf, // movq __cellptr(%rip), %rax
        0x0f, 0xb6, 0x10, // movzbl (%rax), %edx
        0x83, 0x00, 0x00, // op $arg, %edx
        0x88, 0x10 // movb %dl, (%rax)
    };

    uint8_t sysbin[] = {
        0xb8, 0x00, 0x00, 0x00, 0x00, // movl $arg, %eax
        0xbf, 0x00, 0x00, 0x00, 0x00, // movl $fd, %edi
        0x48, 0x8b, 0x35, 0xaf, 0xaf, 0xaf, 0xaf, // movq __cellptr(%rip), %rsi
        0xba, 0x01, 0x00, 0x00, 0x00, // movl $1, %edx
        0x0f, 0x05 // syscall
    };

    const uint8_t jmpbin[] = {
        0x48, 0x8b, 0x05, 0xaf, 0xaf, 0xaf, 0xaf, // movq __cellptr(%rip), %rax
        0x8a, 0x00, // movb (%rax), %al
        0x84, 0xc0, // testb %al, %al
        0x0f, 0x84, 0xaf, 0xaf, 0xaf, 0xaf // je addr
    };

    switch (instr->instr)
    {
        case INSTR_PTRINC:
            reloc_write(reloc, RELOC_PATCH_PTR, buf->pos + 3);
            reloc_write(reloc, RELOC_PATCH_PTR, buf->pos + 14);

            ptrbin[9] = 0xc0; // add
            ptrbin[10] = instr->arg; // imm
            bytebuffer_writes(buf, ptrbin, sizeof(ptrbin));
            break;

        case INSTR_PTRDEC:
            reloc_write(reloc, RELOC_PATCH_PTR, buf->pos + 3);
            reloc_write(reloc, RELOC_PATCH_PTR, buf->pos + 14);

            ptrbin[9] = 0xe8; // sub
            ptrbin[10] = instr->arg; // imm
            bytebuffer_writes(buf, ptrbin, sizeof(ptrbin));
            break;

        case INSTR_CELINC:
            reloc_write(reloc, RELOC_PATCH_PTR, buf->pos + 3);

            celbin[11] = 0xc2; // add
            celbin[12] = instr->arg; // imm
            bytebuffer_writes(buf, celbin, sizeof(celbin));
            break;

        case INSTR_CELDEC:
            reloc_write(reloc, RELOC_PATCH_PTR, buf->pos + 3);

            celbin[11] = 0xea; // sub
            celbin[12] = instr->arg; // imm
            bytebuffer_writes(buf, celbin, sizeof(celbin));
            break;

        case INSTR_OUTPUT:
            reloc_write(reloc, RELOC_PATCH_PTR, buf->pos + 13);

            sysbin[1] = 0x01; // write
            sysbin[6] = 0x01; // stdout
            bytebuffer_writes(buf, sysbin, sizeof(sysbin));
            break;

        case INSTR_INPUT:
            reloc_write(reloc, RELOC_PATCH_PTR, buf->pos + 13);

            sysbin[1] = 0x00; // read
            sysbin[6] = 0x00; // stdin
            bytebuffer_writes(buf, sysbin, sizeof(sysbin));
            break;

        case INSTR_JMPBEG:
            reloc_write(reloc, RELOC_PATCH_PTR, buf->pos + 3);

            addr = buf->pos;
            bytebuffer_writes(buf, jmpbin, sizeof(jmpbin));

            labelstack_push(&reloc->labelstack, addr);
            break;

        case INSTR_JMPEND:
            labelstack_pop(&reloc->labelstack, &addr);
            int32_t off = -(buf->pos + 5 - addr);

            bytebuffer_write(buf, 0xe9); // jmp addr
            bytebuffer_write(buf, (uint8_t)(off >> 0));
            bytebuffer_write(buf, (uint8_t)(off >> 8));
            bytebuffer_write(buf, (uint8_t)(off >> 16));
            bytebuffer_write(buf, (uint8_t)(off >> 24));

            off = buf->pos - (addr + sizeof(jmpbin));
            buf->bytes[addr + 13] = (uint8_t)(off >> 0);
            buf->bytes[addr + 14] = (uint8_t)(off >> 8);
            buf->bytes[addr + 15] = (uint8_t)(off >> 16);
            buf->bytes[addr + 16] = (uint8_t)(off >> 24);
            break;

        default:
            return false;
    }

    return true;
}

error_t
emit_mach_x86_64(context_t *ctx, bytebuffer_t *buf, ir_t *ir)
{
    reloc_t reloc;
    reloc_init(&reloc, RELOC_BLOCK);

    codegen_t codegen;
    codegen.prologue_fn = prologue_mach_x86_64;
    codegen.epilogue_fn = epilogue_mach_x86_64;
    codegen.ptrinc_fn = instr_mach_x86_64;
    codegen.ptrdec_fn = instr_mach_x86_64;
    codegen.celinc_fn = instr_mach_x86_64;
    codegen.celdec_fn = instr_mach_x86_64;
    codegen.output_fn = instr_mach_x86_64;
    codegen.input_fn = instr_mach_x86_64;
    codegen.jmpbeg_fn = instr_mach_x86_64;
    codegen.jmpend_fn = instr_mach_x86_64;
    codegen.extra = &reloc;

    error_t err = codegen_run(ctx, &codegen, buf, ir);
    reloc_free(&reloc);

    return err;
}
