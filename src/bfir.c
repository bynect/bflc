#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "bfir.h"

void bfir_instr_init(Bfir_Instr *instr, Bfir_Instr_Kind kind, int64_t arg) {
	instr->kind = kind;
	instr->arg = arg;
	instr->next = 0;
}

void bfir_instr_dump(Bfir_Instr *instr) {
	switch (instr->kind) {
		case BFIR_ADD:
			printf("add %ld", instr->arg);
			break;

		case BFIR_ADDP:
			printf("addp %ld", instr->arg);
			break;

		case BFIR_READ:
			printf("read");
			break;

		case BFIR_WRITE:
			printf("write %ld", instr->arg);
			break;

		case BFIR_JMPF:
			printf("jmpf");
			break;

		case BFIR_JMPB:
			printf("jmpb");
			break;

		default:
			assert(false && "Unreachable");
	}
}

static void pool_grow(Bfir_Pool *pool) {
	fprintf(stderr, "Handler for Bfir_Pool @%p:\n", pool);
	fprintf(stderr, "Failed to bump, %zu slots out of %zu were used\n", pool->len, pool->size);
	abort();
}

void bfir_pool_init(Bfir_Pool *pool, Bfir_Instr *instrs, size_t size, Bfir_Pool_Grow_F grow_f) {
	pool->instrs = instrs;
	pool->len = 0;
	pool->size = size;
	pool->grow_f = grow_f != NULL ? grow_f : pool_grow;

	assert(instrs != NULL && size != 0);
}

Bfir_Instr *bfir_pool_bump(Bfir_Pool *pool) {
	if (pool->len + 1 >= pool->size) pool->grow_f(pool);
	return &pool->instrs[pool->len++];
}

void bfir_entry_init(Bfir_Entry *entry, const char *name, Bfir_Pool *pool) {
	entry->name = name;
	entry->head = 0;
	entry->tail = 0;
	entry->pool = pool;

	assert(pool->len == 0 && "Pool should not be used");
	assert(pool->size != 0 && "Pool should not be empty");

	Bfir_Instr *ud = bfir_pool_bump(pool);
	ud->kind = BFIR_UD;
	ud->next = 0;
}

Bfir_Id bfir_entry_append(Bfir_Entry *entry, Bfir_Instr *instr) {
	Bfir_Id id = entry->pool->len;
	Bfir_Instr *xinstr = bfir_pool_bump(entry->pool);
	memcpy(xinstr, instr, sizeof(Bfir_Instr));

	// TODO: Improve this function
	if (entry->head == 0) entry->head = id;
	if (entry->tail != 0) bfir_entry_get(entry, entry->tail)->next = id;

	entry->tail = id;
	return id;
}

Bfir_Instr *bfir_entry_get(Bfir_Entry *entry, Bfir_Id id) {
	assert(id != 0 && "Id 0 is reserved");
	assert(id < entry->pool->len && "Id out of bounds");
	return &entry->pool->instrs[id];
}

void bfir_entry_dump(Bfir_Entry *entry) {
	printf("entry \"%s\" {\n", entry->name);

	Bfir_Instr *instr = bfir_entry_get(entry, entry->head);
	while (true) {
		putchar('\t');
		bfir_instr_dump(instr);
		putchar('\n');

		if (instr->next == 0) break;
		instr = bfir_entry_get(entry, instr->next);
	}

	printf("}\n");
}
