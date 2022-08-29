#ifndef SIGN_H
#define SIGN_H

#include <stdint.h>

typedef union {
	uint8_t bytes[8];
	uint64_t quad;
} Signature;

#endif
