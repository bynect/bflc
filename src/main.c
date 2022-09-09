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
	Driver drive = {
		.fronts = {
			{ &brainfuck_front, NULL },
		},
		.backs = {
			{ &amd64_asm_back, NULL },
			{ &amd64_back, NULL },
		},
		.verbose = false,
	};

	return driver_run(&drive, argc, argv);
}
