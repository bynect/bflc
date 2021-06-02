#include "reloc.h"

#include <malloc.h>

void
reloc_init(reloc_t *reloc, size_t min)
{
    labelstack_init(&reloc->labelstack, LABELSTACK_BLOCK);
    min *= sizeof(*(reloc->offs));
    reloc->offs = malloc(min);
    reloc->off_len = min;
    reloc->off_pos = 0;
}

void
reloc_reset(reloc_t *reloc, size_t min)
{
    labelstack_reset(&reloc->labelstack, LABELSTACK_BLOCK);
    min *= sizeof(*reloc->offs);
    if (reloc->offs != NULL)
    {
        if (reloc->off_len < min)
        {
            reloc->offs = realloc(reloc->offs, min);
            reloc->off_len = min;
        }
        reloc->off_pos = 0;
        return;
    }

    reloc->offs = malloc(min);
    reloc->off_len = min;
    reloc->off_pos = 0;
}

void
reloc_write(reloc_t *reloc, uint8_t type, uint32_t off)
{
    size_t off_pos = (reloc->off_pos + 1) * sizeof(*reloc->offs);
    if (off_pos + 1 >= reloc->off_len)
    {
        reloc->off_len += RELOC_BLOCK * sizeof(*reloc->offs);
        reloc->offs = realloc(reloc->offs, reloc->off_len);
    }

    reloc->offs[reloc->off_pos].type = type;
    reloc->offs[reloc->off_pos].off = off;
    ++reloc->off_pos;
}

void
reloc_patch(reloc_t *reloc, bytebuffer_t *buf, uint32_t cellmem,
            uint32_t cellptr, addr_patch_fn addr_patch, void *extra)
{
    for (size_t i = 0; i < reloc->off_pos; ++i)
    {
        addr_patch(
            buf, reloc->offs[i].type == RELOC_PATCH_MEM ? cellmem : cellptr,
            reloc->offs[i].type, reloc->offs[i].off, extra
        );
    }
}

void
reloc_free(reloc_t *reloc)
{
    labelstack_free(&reloc->labelstack);
    free(reloc->offs);
    reloc->offs = NULL;
    reloc->off_len = 0;
    reloc->off_pos = 0;
}
