#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "back.h"
#include "bfir.h"
#include "label.h"
#include "out.h"
#include "amd64.h"

// push rbp
// mov rbp, rsp
// push rbx
// mov rbx, (imm64)
static const uint8_t amd64_prologue[] = {
	0x55,
	0x48, 0x89, 0xe5,
	0x53,
	0x48, 0xbb,
};

// mov rax, 0
// pop rbx
// pop rbp
// ret
static const uint8_t amd64_epilogue[] = {
	0x48, 0xc7, 0xc0, 0x00, 0x00, 0x00, 0x00,
	0x5b,
	0x5d,
	0xc3,
};

// add BYTE PTR [rbx], (imm8)
static const uint8_t amd64_add[] = { 0x80, 0x03 };

// add rbx, (imm8)
static const uint8_t amd64_addp_8[] = { 0x48, 0x83, 0xc3 };

// add rbx, (imm32)
static const uint8_t amd64_addp_32[] = { 0x48, 0x81, 0xc3 };

// movzx edi, BYTE PTR [rbx]
static const uint8_t amd64_movzx_edi[] = { 0x0f, 0xb6, 0x3b };

// mov BYTE PTR [rbx], al
static const uint8_t amd64_mov_al[] = { 0x88, 0x03 };

// mov rax, (imm64)
static const uint8_t amd64_mov_rax[] = { 0x48, 0xb8 };

// call rax
static const uint8_t amd64_call_rax[] = { 0xff, 0xd0 };

// mov al, BYTE PTR [rbx]
// test al, al
// jz (rel32)
static const uint8_t amd64_test[] = {
	0x8a, 0x03,
	0x84, 0xc0,
	0x0f, 0x84,
};

// jmp (rel32)
static const uint8_t amd64_jmp[] = { 0xe9 };

static void amd64_write64(Out_Channel *out,  uint64_t quad) {
	uint8_t bytes[8] = {
		quad >> (8 * 0),
		quad >> (8 * 1),
		quad >> (8 * 2),
		quad >> (8 * 3),
		quad >> (8 * 4),
		quad >> (8 * 5),
		quad >> (8 * 6),
		quad >> (8 * 7),
	};
	out_write(out, bytes, 8);
}

static void amd64_write32(Out_Channel *out,  uint32_t dword) {
	uint8_t bytes[4] = {
		dword >> (8 * 0),
		dword >> (8 * 1),
		dword >> (8 * 2),
		dword >> (8 * 3),
	};
	out_write(out, bytes, 4);
}

static void amd64_instr(Out_Channel *out, Bfir_Instr *instr, Label_Stack *stack, Amd64_Layout *mem) {
	int32_t target = 0;

	switch (instr->kind) {
		case BFIR_ADD:
			assert(instr->arg <= UINT8_MAX);
			out_write(out, amd64_add, sizeof(amd64_add));
			uint8_t arg = instr->arg;
			out_write(out, &arg, 1);
			break;

		case BFIR_ADDP:
			if (instr->arg <= UINT8_MAX) {
				out_write(out, amd64_addp_8, sizeof(amd64_addp_8));
				uint8_t arg = instr->arg;
				out_write(out, &arg, 1);
			} else {
				assert(instr->arg <= UINT32_MAX);
				out_write(out, amd64_addp_32, sizeof(amd64_addp_32));
				uint32_t arg = instr->arg;
				amd64_write32(out, arg);
			}
			break;

		case BFIR_READ:
			out_write(out, amd64_mov_rax, sizeof(amd64_mov_rax));
			amd64_write64(out, (uint64_t)mem->getchar);
			out_write(out, amd64_call_rax, sizeof(amd64_call_rax));
			out_write(out, amd64_mov_al, sizeof(amd64_mov_al));
			break;

		case BFIR_WRITE:
			out_write(out, amd64_movzx_edi, sizeof(amd64_movzx_edi));
			out_write(out, amd64_mov_rax, sizeof(amd64_mov_rax));
			amd64_write64(out, (uint64_t)mem->putchar);
			out_write(out, amd64_call_rax, sizeof(amd64_call_rax));
			break;

		case BFIR_JMPF:
			out_write(out, amd64_test, sizeof(amd64_test));
			assert(label_stack_pop(stack, (uint32_t *)&target));
			amd64_write32(out, target);
			break;

		case BFIR_JMPB:
			out_write(out, amd64_jmp, sizeof(amd64_jmp));
			assert(label_stack_pop(stack, (uint32_t *)&target));
			amd64_write32(out, target);
			break;

		default:
			assert(false && "Unreachable");
	}
}

void amd64_labels(Bfir_Entry *entry, Label_Stack *stack) {
	uint32_t ip = sizeof(amd64_prologue) + 8;
	uint32_t target = 0;

	Bfir_Instr *instr = bfir_entry_get(entry, entry->head);
	while (true) {
		switch (instr->kind) {
			case BFIR_ADD:
				ip += sizeof(amd64_add) + 1;
				break;

			case BFIR_ADDP:
				if (instr->arg <= UINT8_MAX) ip += sizeof(amd64_addp_8) + 1;
				else ip += sizeof(amd64_addp_32) + 4;
				break;

			case BFIR_READ:
				ip += sizeof(amd64_mov_rax) + 8 + sizeof(amd64_call_rax) + sizeof(amd64_mov_al);
				break;

			case BFIR_WRITE:
				ip += sizeof(amd64_movzx_edi) + sizeof(amd64_mov_rax) + 8 + sizeof(amd64_call_rax);
				break;

			case BFIR_JMPF:
				ip += sizeof(amd64_test) + 4;
				label_stack_push(stack, ip);
				break;

			case BFIR_JMPB:
				assert(label_stack_pop(stack, &target));
				int32_t tmp = (int32_t)ip - target;
				label_stack_push(stack, *(uint32_t *)&tmp); // lf -> lb
				ip += sizeof(amd64_jmp) + 4;
				tmp = (int32_t)target - ip;
				label_stack_push(stack, *(uint32_t *)&tmp); // lb -> lf
				break;

			default:
				assert(false && "Unreachable");
		}

		if (instr->next == 0) break;
		instr = bfir_entry_get(entry, instr->next);
	}
}

static void amd64_entry(Out_Channel *out, Bfir_Entry *entry, Label_Stack *stack, Amd64_Layout *mem) {
	amd64_labels(entry, stack);
	label_stack_reverse(stack);

	for (int i = 0; i < stack->len; i++) {
		printf("LABEL %d: %d\n", i , *(int32_t *)&stack->labels[i]);
	}

	out_write(out, amd64_prologue, sizeof(amd64_prologue));
	amd64_write64(out, (uint64_t)mem->cells);

	Bfir_Instr *instr = bfir_entry_get(entry, entry->head);
	while (true) {
		amd64_instr(out, instr, stack, mem);
		if (instr->next == 0) break;
		instr = bfir_entry_get(entry, instr->next);
	}

	out_write(out, amd64_epilogue, sizeof(amd64_epilogue));
}

void amd64_aux_init(Amd64_Aux *aux, Label_Stack *stack, Amd64_Layout *mem) {
	assert(aux != NULL);
	assert(stack != NULL);
	assert(mem != NULL);

	aux->aux.sign = amd64_back.sign;
	aux->stack = stack;
	aux->mem = mem;
}

void amd64_emit(Out_Channel *out, Bfir_Entry *entry, Back_Aux *aux) {
	assert(out != NULL && entry != NULL && aux != NULL);
	assert(aux->sign.quad == amd64_back.sign.quad);
	amd64_entry(out, entry, ((Amd64_Aux *)aux)->stack, ((Amd64_Aux *)aux)->mem);
}

const Back_Info amd64_back = {
	.name = "amd64",
	.sign.quad = 0x86640000,
	.emit_f = amd64_emit,
};
