#include <assert.h>

#include "bfir.h"
#include "front.h"
#include "in.h"

Error bfir_parse(In_Channel *in, Bfir_Entry *entry, Front_Aux *aux) {
	assert(aux == NULL || aux->sign.quad == bfir_front.sign.quad);

	assert(false);
}

const Front_Info bfir_front = {
	.sign.quad = 0x1122334400bb00bb,
	.parse_f = bfir_parse,
};
