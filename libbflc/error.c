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
    if (err->pretty == NULL && err->instr == NULL)
    {
        error_init(err, pretty, instr);
    }
    else
    {
        error_t *node = malloc(sizeof(error_t));
        error_init(node, pretty, instr);

        while (err->next != NULL)
        {
            err = err->next;
        }
        err->next = node;
    }
}

void
error_free(error_t *err)
{
    err = err->next;
    while (err != NULL)
    {
        error_t *node = err;
        err = err->next;
        free(node);
    }
}

bool
error_success(const error_t *err)
{
    return err->pretty == NULL && err->instr == NULL;
}

uint32_t
error_dump(const error_t *err)
{
    uint32_t count = 0;
    if (err->pretty != NULL || err->instr != NULL)
    {
        for (error_t *node = err; node != NULL; node = node->next)
        {
            if (node->instr != NULL)
            {
                printf("%s on instr {", node->pretty);
                instr_print(node->instr);
                printf("}\n");
            }
            else
            {
                printf("%s\n", node->pretty);
            }
            ++count;
        }
    }
    return count;
}
