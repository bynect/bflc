#include "brainfuck.h"
#include "bfir.h"

bool parse_brainfuck(const char *src, Bfir_Entry *entry) {
	while (true) {
		Bfir_Instr instr;
		switch (*src++) {
			case '\0':
				return true;

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
	return false;
}

Bflc_Front brainfuck_front = {
	"brainfuck",
	parse_brainfuck,
};
