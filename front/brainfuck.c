#include <assert.h>

#include "brainfuck.h"
#include "bfir.h"
#include "front.h"
#include "in.h"

void brainfuck_parse(In_Channel *in, Bfir_Entry *entry, Front_Aux *aux) {
	assert(aux == NULL || aux->sign.quad == brainfuck_front.sign.quad);

	uint8_t c;
	while (in_read(in, &c, 1) == 1) {
		Bfir_Instr instr;

		switch (c) {
			case '+':
				bfir_instr_init(&instr, BFIR_ADD, 1);
				break;

			case '-':
				bfir_instr_init(&instr, BFIR_ADD, -1);
				break;

			case '>':
				bfir_instr_init(&instr, BFIR_ADDP, 1);
				break;

			case '<':
				bfir_instr_init(&instr, BFIR_ADDP, -1);
				break;

			case '.':
				bfir_instr_init(&instr, BFIR_WRITE, 1);
				break;

			case ',':
				bfir_instr_init(&instr, BFIR_READ, 1);
				break;

			case '[':
				bfir_instr_init(&instr, BFIR_JMPF, 0);
				break;

			case ']':
				bfir_instr_init(&instr, BFIR_JMPB, 0);
				break;

			default:
				 continue;
		}
		bfir_entry_append(entry, &instr);
	}
}

const Front_Info brainfuck_front = {
	.name = "brainfuck",
	.desc = "Brainfuck frontend",
	.sign.quad = 0xbaff0000,
	.parse_f = brainfuck_parse,
};
