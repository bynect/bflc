#include <assert.h>

#include "valid.h"

void valid_pass(Bfir_Entry *entry, Middle_Aux *aux) {
	if (entry->head == 0) return;

	uint32_t level = 0;

	Bfir_Instr *instr = bfir_entry_get(entry, entry->head);
	while (true) {
		switch (instr->kind) {
			case BFIR_ADD:
				break;

			case BFIR_ADDP:
				break;

			case BFIR_READ:
				break;

			case BFIR_WRITE:
				break;

			case BFIR_JMPF:
				++level;
				break;

			case BFIR_JMPB:
				assert(level != 0);
				--level;
				break;

			default:
				assert(false && "Unreachable");
		}

		if (instr->next == 0) break;
		instr = bfir_entry_get(entry, instr->next);
	}

	assert(level == 0 && "Unpaired jumps");
}

const Middle_Info valid_middle = {
	.sign.quad = 0x0011002200330044,
	.pass_f = valid_pass,
};
