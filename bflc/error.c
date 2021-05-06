#include "error.h"

#include <malloc.h>

void
error_init(error_t *err, const char *pretty, const instr_t *instr)
{
    err->pretty = pretty;
    err->instr = instr;
    err->next = NULL;
}

void
error_node(error_t *err, const char *pretty, const instr_t *instr)
{
    error_t *node = malloc(sizeof(error_t));
    node->pretty = pretty;
    node->instr = instr;
    node->next = NULL;

    while (err->next != NULL)
    {
        err = err->next;
    }
    err->next = node;
}

void
error_free(error_t *err)
{
    while (err != NULL)
    {
        error_t *node = err;
        err = err->next;
        free(node);
    }
}
