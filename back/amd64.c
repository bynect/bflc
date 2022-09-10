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
static const uint8_t amd64_add_rbx_8[] = { 0x48, 0x83, 0xc3 };

// add rbx, (imm32)
static const uint8_t amd64_add_rbx_32[] = { 0x48, 0x81, 0xc3 };

// mov rsi, rbx
static const uint8_t amd64_mov_rsi_rbx[] = { 0x48, 0x89, 0xde };

// movzx edi, BYTE PTR [rbx]
static const uint8_t amd64_movzx_edi_rbx[] = { 0x0f, 0xb6, 0x3b };

// mov BYTE PTR [rbx], al
static const uint8_t amd64_mov_rbx_al[] = { 0x88, 0x03 };

// mov eax, (imm32)
static const uint8_t amd64_mov_eax[] = { 0xb8 };

// mov edi, (imm32)
static const uint8_t amd64_mov_edi[] = { 0xbf };

// mov edx, (imm32)
static const uint8_t amd64_mov_edx[] = { 0xba };

// mov rax, (imm64)
static const uint8_t amd64_mov_rax[] = { 0x48, 0xb8 };

// call rax
static const uint8_t amd64_call_rax[] = { 0xff, 0xd0 };

// call (rel32)
static const uint8_t amd64_call_rel[] = { 0xe8 };

// syscall
static const uint8_t amd64_syscall[] = { 0x0f, 0x05 };

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

static void amd64_instr(Out_Channel *out, Bfir_Instr *instr, Label_Stack *stack, Amd64_Layout *mem, Amd64_Flag flags) {
	int32_t target = 0;

	switch (instr->kind) {
		case BFIR_ADD:
			out_write(out, amd64_add, sizeof(amd64_add));
			uint8_t arg = instr->arg;
			out_write(out, &arg, 1);
			break;

		case BFIR_ADDP:
			if (instr->arg <= UINT8_MAX) {
				out_write(out, amd64_add_rbx_8, sizeof(amd64_add_rbx_8));
				uint8_t arg = instr->arg;
				out_write(out, &arg, 1);
			} else {
				out_write(out, amd64_add_rbx_32, sizeof(amd64_add_rbx_32));
				uint32_t arg = instr->arg;
				amd64_write32(out, arg);
			}
			break;

		case BFIR_READ:
			if (flags & AMD64_READ_SYSCALL) {
				// sys_read
				out_write(out, amd64_mov_eax, sizeof(amd64_mov_eax));
				amd64_write32(out, 0);

				// stdin
				out_write(out, amd64_mov_edi, sizeof(amd64_mov_edi));
				amd64_write32(out, 0);

				// buffer
				out_write(out, amd64_mov_rsi_rbx, sizeof(amd64_mov_rsi_rbx));

				// len
				out_write(out, amd64_mov_edx, sizeof(amd64_mov_edx));
				amd64_write32(out, instr->arg);

				out_write(out, amd64_syscall, sizeof(amd64_syscall));
			} else {
				if (flags & AMD64_RELATIVE_CALL) {
					out_write(out, amd64_call_rel, sizeof(amd64_call_rel));
					uint32_t off = (uint64_t)mem->getchar - (uint64_t)(out->buffer->bytes + out->buffer->len + 4);
					amd64_write32(out, off);
				} else {
					out_write(out, amd64_mov_rax, sizeof(amd64_mov_rax));
					amd64_write64(out, (uint64_t)mem->getchar);
					out_write(out, amd64_call_rax, sizeof(amd64_call_rax));
				}
				out_write(out, amd64_mov_rbx_al, sizeof(amd64_mov_rbx_al));
			}
			break;

		case BFIR_WRITE:
			if (flags & AMD64_WRITE_SYSCALL) {
				// sys_write
				out_write(out, amd64_mov_eax, sizeof(amd64_mov_eax));
				amd64_write32(out, 1);

				// stdout
				out_write(out, amd64_mov_edi, sizeof(amd64_mov_edi));
				amd64_write32(out, 1);

				// buffer
				out_write(out, amd64_mov_rsi_rbx, sizeof(amd64_mov_rsi_rbx));

				// len
				out_write(out, amd64_mov_edx, sizeof(amd64_mov_edx));
				amd64_write32(out, instr->arg);

				out_write(out, amd64_syscall, sizeof(amd64_syscall));
			} else {
				out_write(out, amd64_movzx_edi_rbx, sizeof(amd64_movzx_edi_rbx));
				if (flags & AMD64_RELATIVE_CALL) {
					out_write(out, amd64_call_rel, sizeof(amd64_call_rel));
					uint32_t off = (uint64_t)mem->putchar - (uint64_t)(out->buffer->bytes + out->buffer->len + 4);
					amd64_write32(out, off);
				} else {
					out_write(out, amd64_mov_rax, sizeof(amd64_mov_rax));
					amd64_write64(out, (uint64_t)mem->putchar);
					out_write(out, amd64_call_rax, sizeof(amd64_call_rax));
				}
			}
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

uint32_t amd64_compute(Bfir_Entry *entry, Label_Stack *stack, Amd64_Flag flags) {
	uint32_t ip = sizeof(amd64_prologue) + 8;
	uint32_t target = 0;

	const uint32_t syscall = sizeof(amd64_mov_eax) + 4 + sizeof(amd64_mov_edi) + 4 + sizeof(amd64_mov_rsi_rbx) + sizeof(amd64_mov_edx) + 4 + sizeof(amd64_syscall);

	Bfir_Instr *instr = bfir_entry_get(entry, entry->head);
	while (true) {
		switch (instr->kind) {
			case BFIR_ADD:
				assert(instr->arg <= UINT8_MAX);
				ip += sizeof(amd64_add) + 1;
				break;

			case BFIR_ADDP:
				assert(instr->arg <= UINT32_MAX);
				if (instr->arg <= UINT8_MAX) ip += sizeof(amd64_add_rbx_8) + 1;
				else ip += sizeof(amd64_add_rbx_32) + 4;
				break;

			case BFIR_READ:
				if (flags & AMD64_READ_SYSCALL) ip += syscall;
				else {
					if (flags & AMD64_RELATIVE_CALL) ip += sizeof(amd64_call_rel) + 4;
					else ip += sizeof(amd64_mov_rax) + 8 + sizeof(amd64_call_rax);
					ip += sizeof(amd64_mov_rbx_al);
				}
				break;

			case BFIR_WRITE:
				if (flags & AMD64_WRITE_SYSCALL) ip += syscall;
				else {
					ip += sizeof(amd64_movzx_edi_rbx);
					if (flags & AMD64_RELATIVE_CALL) ip += sizeof(amd64_call_rel) + 4;
					else ip += sizeof(amd64_mov_rax) + 8 + sizeof(amd64_call_rax);
				}
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

	return ip + sizeof(amd64_epilogue);
}

static void amd64_entry(Out_Channel *out, Bfir_Entry *entry, Label_Stack *stack, Amd64_Layout *mem, Amd64_Flag flags) {
	uint32_t ip = amd64_compute(entry, stack, flags);
	label_stack_reverse(stack);

	// For relative addressing there must be no reallocation
	if (flags & AMD64_RELATIVE_CALL) assert(out->kind == OUT_BUFFER && "Relative addressing works only in memory");
	if (out->kind == OUT_BUFFER) byte_buffer_ensure(out->buffer, ip);

	out_write(out, amd64_prologue, sizeof(amd64_prologue));
	amd64_write64(out, (uint64_t)mem->cells);

	Bfir_Instr *instr = bfir_entry_get(entry, entry->head);
	while (true) {
		amd64_instr(out, instr, stack, mem, flags);
		if (instr->next == 0) break;
		instr = bfir_entry_get(entry, instr->next);
	}

	out_write(out, amd64_epilogue, sizeof(amd64_epilogue));
}

void amd64_aux_init(Amd64_Aux *aux, Label_Stack *stack, Amd64_Layout *mem, Amd64_Flag flags) {
	assert(aux != NULL);
	assert(stack != NULL);
	assert(mem != NULL);

	aux->aux.sign = amd64_back.sign;
	aux->stack = stack;
	aux->mem = mem;
	aux->flags = flags;
}

void amd64_emit(Out_Channel *out, Bfir_Entry *entry, Back_Aux *aux) {
	assert(out != NULL && entry != NULL && aux != NULL);
	assert(aux->sign.quad == amd64_back.sign.quad);
	amd64_entry(out, entry, ((Amd64_Aux *)aux)->stack, ((Amd64_Aux *)aux)->mem, ((Amd64_Aux *)aux)->flags);
}

const Back_Info amd64_back = {
	.sign.quad = 0x8664000086640000,
	.emit_f = amd64_emit,
};
