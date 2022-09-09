#ifndef DRIVE_H
#define DRIVE_H

#include "opt.h"
#include "back.h"
#include "front.h"

#define DRIVER_FRONTS 1
#define DRIVER_BACKS 2

typedef struct {
	struct {
		const Front_Info *info;
		Front_Aux *aux;
	} fronts[DRIVER_FRONTS];
	struct {
		const Back_Info *info;
		Back_Aux *aux;
	} backs[DRIVER_BACKS];
	bool verbose;
} Driver;

int driver_run(Driver *drive, int argc, const char **argv);

void driver_error(const char *message);

#endif
