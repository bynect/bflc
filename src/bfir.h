#ifndef BFIR_H
#define BFIR_H

#include <stdint.h>
#include <stddef.h>

typedef enum {
	BFIR_UD,
	BFIR_ADD,
	BFIR_ADDP,
	BFIR_READ,
	BFIR_WRITE,
	BFIR_JMPF,
	BFIR_JMPB,
} Bfir_Instr_Kind;

typedef size_t Bfir_Id;

typedef struct {
	Bfir_Instr_Kind kind;
	int64_t arg;
	Bfir_Id next;
} Bfir_Instr;

struct Bfir_Pool;

typedef void (Bfir_Pool_Grow_F)(struct Bfir_Pool *pool);

typedef struct Bfir_Pool {
	Bfir_Instr *instrs;
	size_t len;
	size_t size;
	Bfir_Pool_Grow_F *grow_f;
} Bfir_Pool;

typedef struct {
	const char *name;
	Bfir_Id head;
	Bfir_Id tail;
	Bfir_Pool *pool;
} Bfir_Entry;

void bfir_instr_init(Bfir_Instr *instr, Bfir_Instr_Kind kind, int64_t arg);

void bfir_instr_dump(Bfir_Instr *instr);

void bfir_pool_init(Bfir_Pool *pool, Bfir_Instr *instrs, size_t size, Bfir_Pool_Grow_F grow_f);

Bfir_Instr *bfir_pool_bump(Bfir_Pool *pool);

void bfir_entry_init(Bfir_Entry *entry, const char *name, Bfir_Pool *pool);

Bfir_Id bfir_entry_append(Bfir_Entry *entry, Bfir_Instr *instr);

Bfir_Instr *bfir_entry_get(Bfir_Entry *entry, Bfir_Id id);

void bfir_entry_dump(Bfir_Entry *entry);

#endif
