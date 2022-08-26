#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "back.h"
#include "bfir.h"
#include "label.h"
#include "out.h"

#define CELLP "rbx"
#define CELLN 30000

void x86_64_asm_instr(Out_Channel *out, Bfir_Instr *instr, Label_Stack *stack) {
	uint32_t label;
	switch (instr->kind) {
		case BFIR_ADD:
			out_write(out, "\tmovzx	edx, BYTE [%s]\n", CELLP);
            out_write(out, "\tadd edx, %ld\n", instr->arg);
            out_write(out, "\tmov [%s], BYTE dl\n", CELLP);
			break;

		case BFIR_ADDP:
			out_write(out, "\tadd %s, %ld\n", CELLP, instr->arg);
			break;

		case BFIR_READ:
			out_write(out, "\tcall [rel getchar wrt ..got]\n");
			out_write(out, "\tmov BYTE [%s], al\n", CELLP);
			break;

		case BFIR_WRITE:
			out_write(out, "\tmovzx edi, BYTE [%s]\n", CELLP);
			out_write(out, "\tcall [rel putchar wrt ..got]\n");
			break;

		case BFIR_JMPF:
			label = label_stack_fresh(stack);
			out_write(out, ".L%uf:\n", label);
			out_write(out, "\tmov al, BYTE [%s]\n", CELLP);
			out_write(out, "\ttest al, al\n");
			out_write(out, "\tje .L%ub\n", label);
			label_stack_push(stack, label);
			break;

		case BFIR_JMPB:
			assert(label_stack_pop(stack, &label) == true && "Unpaired jumps");
			out_write(out, "\tjmp .L%uf\n", label);
			out_write(out, ".L%ub:\n", label);
			break;

		default:
			assert(false && "Unreachable");
	}
}

bool x86_64_asm_emit(Out_Channel *out, Bfir_Entry *entry, Label_Stack *stack) {
	out_write(out, "\tbits 64\n");
	out_write(out, "\textern putchar\n");
	out_write(out, "\textern getchar\n\n");

	out_write(out, "\tsection .bss\n");
	out_write(out, "\talign 32\n");
	out_write(out, "__cells:\n");
	out_write(out, "\tresb %d\n\n", CELLN);

	//out_write(out, "\tsection .data\n");
	//out_write(out, "\talign 8\n");
	//out_write(out, "__cellp:\n");
	//out_write(out, "\tdq __cells\n\n");

	out_write(out, "\tsection .text\n");
	out_write(out, "\tglobal %s\n", entry->name);
	out_write(out, "\talign 16\n");
	out_write(out, "%s:\n", entry->name);
	out_write(out, "\tpush rbp\n");
	out_write(out, "\tmov rbp, rsp\n");
	out_write(out, "\tpush rbx\n");
	out_write(out, "\tmov %s, __cells\n", CELLP);

	Bfir_Instr *instr = bfir_entry_get(entry, entry->head);
	while (true) {
		x86_64_asm_instr(out, instr, stack);
		if (instr->next == 0) break;
		instr = bfir_entry_get(entry, instr->next);
	}
	assert(stack->len == 0 && "Unpaired jumps");

	out_write(out, "\tmov rax, 0\n");
	out_write(out, "\tpop rbx\n");
	out_write(out, "\tpop rbp\n");
	out_write(out, "\tret\n");

	return true;
}

Bflc_Back x86_64_asm_back = {
	"x86_64_asm",
	x86_64_asm_emit,
};
