#ifndef DRIVE_H
#define DRIVE_H

#include "opt.h"
#include "back.h"
#include "front.h"

#define DRIVER_FRONTS 3
#define DRIVER_BACKS 2

typedef struct {
	struct {
		const char **names;
		const char *desc;
		const Front_Info *info;
		Front_Aux *aux;
	} fronts[DRIVER_FRONTS];
	struct {
		const char **names;
		const char *desc;
		const Back_Info *info;
		Back_Aux *aux;
	} backs[DRIVER_BACKS];
	bool verbose;
	bool debug;
	size_t cell_n;
} Driver;

int driver_run(Driver *drive, int argc, const char **argv);

void driver_error(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

#endif
