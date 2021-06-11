#include "mem.h"

#include <stdlib.h>

static void *
mem_alloc(size_t size, void *extra)
{
    return malloc(size);
}

static void *
mem_realloc(void *ptr, size_t prev, size_t size, void *extra)
{
    return realloc(ptr, size);
}

static void
mem_free(void *ptr, size_t prev, void *extra)
{
    free(ptr);
}

void
mem_default(mem_t *mem)
{
    mem->alloc_fn = mem_alloc;
    mem->realloc_fn = mem_realloc;
    mem->free_fn = mem_free;
    mem->extra = NULL;
}
