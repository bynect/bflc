#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "opt.h"
#include "drive.h"

#include "brainfuck.h"
#include "amd64.h"
#include "amd64_asm.h"

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
			{ &brainfuck_front, NULL },
		},
		.backs = {
			{ &amd64_asm_back, (void *)&amd64_asm_aux },
			{ &amd64_back, NULL },
		},
		.verbose = false,
	};

	return driver_run(&drive, argc, argv);
}
