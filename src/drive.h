#ifndef DRIVE_H
#define DRIVE_H

#include "opt.h"
#include "back.h"
#include "front.h"

enum {
	DRIVE_FRONT_BRAINFUCK,
	DRIVE_FRONT_FUCKBEES,
	DRIVE_FRONT_BFIR,
	DRIVE_FRONT_LAST,
};

enum {
	DRIVE_BACK_AMD64_ASM,
	DRIVE_BACK_BFIR,
	DRIVE_BACK_LAST,
};

typedef enum {
	DRIVE_FLAG_NONE,
	DRIVE_FLAG_READ_SYSCALL,
	DRIVE_FLAG_WRITE_SYSCALL,
} Driver_Flag;

typedef void (Driver_Flag_F)(Back_Aux *aux, Driver_Flag flags, Opt_Value value);

typedef struct {
	struct {
		const char **names;
		const char *desc;
		const Front_Info *info;
		Front_Aux *aux;
	} fronts[DRIVE_FRONT_LAST];
	struct {
		const char **names;
		const char *desc;
		const char *file_ext;
		const Back_Info *info;
		Back_Aux *aux;
		Driver_Flag_F *flag_f;
	} backs[DRIVE_BACK_LAST];
	bool verbose;
	bool debug;
	size_t cell_n;
} Driver;

int driver_run(Driver *drive, int argc, const char **argv);

void driver_error(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

#endif
