#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

#include "opt.h"
#include "drive.h"

#include "front/brainfuck.h"
#include "front/fuckbees.h"
#include "front/bfir.h"

#include "back/amd64_asm.h"
#include "back/amd64.h"
#include "back/bfir.h"

static const char *drive_flags[] = {
	"",
	"read_syscall",
	"write_syscall",
};

static void drive_flag_amd64_asm(Back_Aux *aux, Driver_Flag flag, Opt_Value value) {
	assert(value.kind == OPT_VALUE_NONE);
	switch (flag) {
		case DRIVE_FLAG_NONE:
			break;

		case DRIVE_FLAG_READ_SYSCALL:
			((Amd64_Asm_Aux *)aux)->flags |= AMD64_ASM_READ_SYSCALL;
			break;

		case DRIVE_FLAG_WRITE_SYSCALL:
			((Amd64_Asm_Aux *)aux)->flags |= AMD64_ASM_WRITE_SYSCALL;
			break;

		default:
			assert(false && "Unreachable");
	}
}

static void drive_flag_bfir(Back_Aux *aux, Driver_Flag flag, Opt_Value value) {
	driver_error("bfir backend does not support %s flag\n", drive_flags[flag]);
}

int main(int argc, const char **argv) {
	const size_t labels_len = 256;
	Label_Id labels[labels_len];

	Label_Stack stack;
	label_stack_init(&stack, labels, labels_len);

	Amd64_Asm_Aux amd64_asm_aux;
	amd64_asm_aux_init(&amd64_asm_aux, &stack, 30000, AMD64_ASM_NONE);

	// TODO: amd64 can only be used to jit at the moment
	//Amd64_Aux amd64_aux;
	//amd64_aux_init(&amd64_aux, &stack, NULL, AMD64_NONE);

	Driver drive = {
		.fronts = {
			[DRIVE_FRONT_BRAINFUCK] = {
				.names = (const char *[]) { "brainfuck", NULL },
				.desc = "Brainfuck frontend",
				.info = &brainfuck_front,
				.aux = NULL
			},
			[DRIVE_FRONT_FUCKBEES] = {
				.names = (const char *[]) { "fuckbees", NULL },
				.desc = "fuckbeEs frontend",
				.info = &fuckbees_front,
				.aux = NULL
			},
			[DRIVE_FRONT_BFIR] = {
				.names = (const char *[]) { "bfir", NULL },
				.desc = "bfir frontend",
				.info = &bfir_front,
				.aux = NULL,
			},
		},
		.backs = {
			[DRIVE_BACK_AMD64_ASM] = {
				.names = (const char *[]) { "amd64_asm", "amd64", "x86_64", NULL },
				.desc = "X86_64 assembly backend",
				.file_ext = "asm",
				.info = &amd64_asm_back,
				.aux = (void *)&amd64_asm_aux,
				.flag_f = drive_flag_amd64_asm,
			},
			[DRIVE_BACK_BFIR] = {
				.names = (const char *[]) { "bfir", "ir", NULL },
				.desc = "Brainfuck-like Intermediate Representation",
				.file_ext = "bfir",
				.info = &bfir_back,
				.aux = NULL,
				.flag_f = drive_flag_bfir,
			},
		},
		.verbose = false,
		.debug = false,
		.validate = true,
		.cell_n = 30000,
	};

	return driver_run(&drive, argc, argv);
}
