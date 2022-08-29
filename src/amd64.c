#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "back.h"
#include "bfir.h"
#include "label.h"
#include "out.h"
#include "amd64.h"

#define CELLP "rbx"
#define CELLN 30000

static void amd64_asm_instr(Out_Channel *out, Bfir_Instr *instr, Label_Stack *stack) {
	uint32_t label;
	switch (instr->kind) {
		case BFIR_ADD:
			out_print(out, "\tmovzx	edx, BYTE [%s]\n", CELLP);
			out_print(out, "\tadd edx, %ld\n", instr->arg);
			out_print(out, "\tmov [%s], BYTE dl\n", CELLP);
			break;

		case BFIR_ADDP:
			out_print(out, "\tadd %s, %ld\n", CELLP, instr->arg);
			break;

		case BFIR_READ:
			out_print(out, "\tcall [rel getchar wrt ..got]\n");
			out_print(out, "\tmov BYTE [%s], al\n", CELLP);
			break;

		case BFIR_WRITE:
			out_print(out, "\tmovzx edi, BYTE [%s]\n", CELLP);
			out_print(out, "\tcall [rel putchar wrt ..got]\n");
			break;

		case BFIR_JMPF:
			label = label_stack_fresh(stack);
			out_print(out, ".L%uf:\n", label);
			out_print(out, "\tmov al, BYTE [%s]\n", CELLP);
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

static void amd64_asm_entry(Out_Channel *out, Bfir_Entry *entry, Label_Stack *stack) {
	out_print(out, "\tbits 64\n");
	out_print(out, "\textern putchar\n");
	out_print(out, "\textern getchar\n\n");

	out_print(out, "\tsection .bss\n");
	out_print(out, "\talign 32\n");
	out_print(out, "__cells:\n");
	out_print(out, "\tresb %d\n\n", CELLN);

	//out_print(out, "\tsection .data\n");
	//out_print(out, "\talign 8\n");
	//out_print(out, "__cellp:\n");
	//out_print(out, "\tdq __cells\n\n");

	out_print(out, "\tsection .text\n");
	out_print(out, "\tglobal %s\n", entry->name);
	out_print(out, "\talign 16\n");
	out_print(out, "%s:\n", entry->name);
	out_print(out, "\tpush rbp\n");
	out_print(out, "\tmov rbp, rsp\n");
	out_print(out, "\tpush rbx\n");
	out_print(out, "\tmov %s, __cells\n", CELLP);

	Bfir_Instr *instr = bfir_entry_get(entry, entry->head);
	while (true) {
		amd64_asm_instr(out, instr, stack);
		if (instr->next == 0) break;
		instr = bfir_entry_get(entry, instr->next);
	}
	assert(stack->len == 0 && "Unpaired jumps");

	out_print(out, "\tmov rax, 0\n");
	out_print(out, "\tpop rbx\n");
	out_print(out, "\tpop rbp\n");
	out_print(out, "\tret\n");
}

static bool amd64_asm_emit(Back_Emitter *back, Out_Channel *out, Bfir_Entry *entry) {
	assert(back->sign.quad == 0x866400aa);

	amd64_asm_entry(out, entry, ((Amd64_Asm_Emitter *)back)->stack);
	return true;
}

void amd64_asm_init(Amd64_Asm_Emitter *back, Label_Stack *stack) {
	back->back.sign.quad = 0x866400aa;
	back->back.emit_f = amd64_asm_emit;
	back->stack = stack;
}
