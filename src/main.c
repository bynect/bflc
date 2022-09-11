#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "opt.h"
#include "drive.h"

#include "front/brainfuck.h"
#include "front/fuckbees.h"

#include "back/amd64_asm.h"
#include "back/amd64.h"
#include "back/bfir.h"

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
			{
				.names = (const char *[]) { "brainfuck", NULL },
				.desc = "Brainfuck frontend",
				.info = &brainfuck_front,
				.aux = NULL
			},
			{
				.names = (const char *[]) { "fuckbees", NULL },
				.desc = "fuckbeEs frontend",
				.info = &fuckbees_front,
				.aux = NULL
			},
		},
		.backs = {
			{
				.names = (const char *[]) { "amd64_asm", "amd64", "x86_64", NULL },
				.desc = "X86_64 assembly backend",
				.info = &amd64_asm_back,
				.aux = (void *)&amd64_asm_aux
			},
			{
				.names = (const char *[]) { "bfir", "ir", NULL },
				.desc = "Brainfuck-like Intermediate Representation",
				.info = &bfir_back,
				.aux = NULL,
			},
		},
		.verbose = false,
		.debug = false,
		.cell_n = 30000,
	};

	return driver_run(&drive, argc, argv);
}
