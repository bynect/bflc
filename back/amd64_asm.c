#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "back.h"
#include "bfir.h"
#include "label.h"
#include "out.h"
#include "amd64_asm.h"

static const char *amd64_cellp = "rbx";

static void amd64_asm_instr(Out_Channel *out, Bfir_Instr *instr, Label_Stack *stack, Label_Id *fresh, Amd64_Asm_Flag flags) {
	Label_Id label;
	switch (instr->kind) {
		case BFIR_ADD:
			out_print(out, "\tadd BYTE [%s], %ld\n", amd64_cellp, instr->arg);
			break;

		case BFIR_ADDP:
			out_print(out, "\tadd %s, %ld\n", amd64_cellp, instr->arg);
			break;

		case BFIR_READ:
			if (flags & AMD64_ASM_READ_SYSCALL) {
				out_print(out, "\tmov rax, 0\n");
				out_print(out, "\tmov rdi, 0\n");
				out_print(out, "\tmov rsi, rbx\n");
				out_print(out, "\tmov rdx, %ld\n", instr->arg);
				out_print(out, "\tsyscall\n");
			} else {
				assert(instr->arg == 1);
				out_print(out, "\tcall [rel getchar wrt ..got]\n");
				out_print(out, "\tmov BYTE [%s], al\n", amd64_cellp);
			}
			break;

		case BFIR_WRITE:
			if (flags & AMD64_ASM_WRITE_SYSCALL) {
				out_print(out, "\tmov rax, 1\n");
				out_print(out, "\tmov rdi, 1\n");
				out_print(out, "\tmov rsi, rbx\n");
				out_print(out, "\tmov rdx, %ld\n", instr->arg);
				out_print(out, "\tsyscall\n");
			} else {
				assert(instr->arg == 1);
				out_print(out, "\tmovzx edi, BYTE [%s]\n", amd64_cellp);
				out_print(out, "\tcall [rel putchar wrt ..got]\n");
			}
			break;

		case BFIR_JMPF:
			label = ++(*fresh);
			out_print(out, ".L%uf:\n", label);
			out_print(out, "\tmov al, BYTE [%s]\n", amd64_cellp);
			out_print(out, "\ttest al, al\n");
			out_print(out, "\tje .L%ub\n", label);
			label_stack_push(stack, label);
			break;

		case BFIR_JMPB:
			assert(label_stack_pop(stack, &label) == true && "Unpaired jumps");
			out_print(out, "\tjmp .L%uf\n", label);
			out_print(out, ".L%ub:\n", label);
			break;

		default:
			assert(false && "Unreachable");
	}
}

static void amd64_asm_entry(Out_Channel *out, Bfir_Entry *entry, Label_Stack *stack, size_t celln, Amd64_Asm_Flag flags) {
	out_print(out, "\tbits 64\n");
	if (!(flags & AMD64_ASM_READ_SYSCALL)) out_print(out, "\textern getchar\n\n");
	if (!(flags & AMD64_ASM_WRITE_SYSCALL)) out_print(out, "\textern putchar\n");

	out_print(out, "\tsection .bss\n");
	out_print(out, "\talign 32\n");
	out_print(out, "__cells:\n");
	out_print(out, "\tresb %zu\n\n", celln);

	out_print(out, "\tsection .text\n");
	out_print(out, "\tglobal %s\n", entry->name);
	out_print(out, "\talign 16\n");
	out_print(out, "%s:\n", entry->name);
	out_print(out, "\tpush rbp\n");
	out_print(out, "\tmov rbp, rsp\n");
	out_print(out, "\tpush rbx\n");
	out_print(out, "\tmov %s, __cells\n", amd64_cellp);

	Label_Id fresh = 0;
	Bfir_Instr *instr = bfir_entry_get(entry, entry->head);

	while (true) {
		amd64_asm_instr(out, instr, stack, &fresh, flags);
		if (instr->next == 0) break;
		instr = bfir_entry_get(entry, instr->next);
	}

	assert(stack->len == 0 && "Unpaired jumps");

	out_print(out, "\tmov rax, 0\n");
	out_print(out, "\tpop rbx\n");
	out_print(out, "\tpop rbp\n");
	out_print(out, "\tret\n");
}

void amd64_asm_aux_init(Amd64_Asm_Aux *aux, Label_Stack *stack, size_t celln, Amd64_Asm_Flag flags) {
	assert(aux != NULL);
	assert(stack != NULL);
	assert(celln != 0);

	aux->aux.sign = amd64_asm_back.sign;
	aux->stack = stack;
	aux->celln = celln;
	aux->flags = flags;
}

void amd64_asm_emit(Out_Channel *out, Bfir_Entry *entry, Back_Aux *aux) {
	assert(out != NULL && entry != NULL && aux != NULL);
	assert(aux->sign.quad == amd64_asm_back.sign.quad);
	amd64_asm_entry(out, entry, ((Amd64_Asm_Aux *)aux)->stack, ((Amd64_Asm_Aux *)aux)->celln, ((Amd64_Asm_Aux *)aux)->flags);
}

const Back_Info amd64_asm_back = {
	.name = "amd64-asm",
	.desc = "X86_64 assembly backend",
	.outs = BACK_ASM,
	.sign.quad = 0x866400008664aaaa,
	.emit_f = amd64_asm_emit,
};
