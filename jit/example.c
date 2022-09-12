#include <stdio.h>
#include <assert.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "back/amd64.h"
#include "front/brainfuck.h"

int main(int argc, const char **argv) {
	// Open input file
	assert(argc == 2);
	FILE *file = fopen(argv[1], "rb");
	assert(file);

	In_Channel in;
	in_init_file(&in, file);

	// Init IR instruction pool
	const size_t instrs_len = 1024;
	Bfir_Instr instrs[instrs_len];

	Bfir_Pool pool;
	bfir_pool_init(&pool, instrs, instrs_len, NULL);

	Bfir_Entry entry;
	bfir_entry_init(&entry, "", &pool);

	// Parse input to IR
	brainfuck_front.parse_f(&in, &entry, NULL);
	fclose(file);

	// Init x86_64 backend
	const size_t labels_len = 128;
	Label_Id labels[labels_len];

	Label_Stack stack;
	label_stack_init(&stack, labels, labels_len);

	uint8_t cells[10000];
	Amd64_Layout mem = {
		.cells   = (uint64_t)cells,
		.getchar = (uint64_t)getchar,
		.putchar = (uint64_t)putchar,
	};

	Amd64_Aux aux;
	amd64_aux_init(&aux, &stack, &mem, AMD64_RELATIVE_CALL);

	// Memory map buffer
	const size_t mem_len = 4096;
    void *mem_ptr = mmap(NULL, mem_len, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (mem_ptr == MAP_FAILED || mem_ptr == NULL) {
		perror("mmap");
		return 1;
	}

	Byte_Buffer buffer;
	byte_buffer_init(&buffer, mem_ptr, mem_len);

	Out_Channel out;
	out_init_buffer(&out, &buffer);

	// Emit x86_64 machine code
	amd64_back.emit_f(&out, &entry, (void *)&aux);

	// Execute compiled machine code
	void (*func)() = mem_ptr;
	func();

	// Clean up buffer
	munmap(mem_ptr, mem_len);

	return 0;
}
