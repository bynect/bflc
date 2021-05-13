#include "args.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void
args_parse(int argc, char **argv, args_t *args, error_t *err)
{
    args->exe = argv[0];
    args->out = NULL;
    args->in = NULL;
    args->front = NULL;
    args->back = NULL;
    args->validation = true;
    args->folding = true;
    args->func_name = NULL;
    args->flavor = FLAVOR_GNUASM;
    args->cells = 30000;
    args->flags = 0;

    for (int i = 1; i < argc; ++i)
    {
        const char *arg = argv[i];
        const size_t len = strlen(arg);

        if (arg[0] == '-')
        {
            if (arg[1] == '-')
            {
                if (len == 2)
                {
                    if (i == (argc - 2))
                    {
                        if (args->in != NULL)
                        {
                            error_node(err, "Input file alredy specified", NULL);
                        }
                        else
                        {
                            args->in = argv[i + 1];
                        }
                    }
                    goto end;
                }
                else if (!strncmp(arg + 2, "validation", 10))
                {
                    if (!strcmp(arg + 12, "=true"))
                    {
                        args->validation = true;
                    }
                    else if (!strcmp(arg + 12, "=false"))
                    {
                        args->validation = false;
                    }
                    else
                    {
                        error_node(err, "Malformed validation specifier", NULL);
                    }
                }
                else if (!strncmp(arg + 2, "folding", 8))
                {
                    if (!strcmp(arg + 10, "=true"))
                    {
                        args->folding = true;
                    }
                    else if (!strcmp(arg + 10, "=false"))
                    {
                        args->folding = false;
                    }
                    else
                    {
                        error_node(err, "Malformed folding specifier", NULL);
                    }
                }
                else if (!strncmp(arg + 2, "front", 5))
                {
                    if (args->front != NULL)
                    {
                        error_node(err, "Frontend alredy specified", NULL);
                    }
                    else if (arg[7] == '=')
                    {
                        args->front = arg + 8;
                    }
                    else
                    {
                        error_node(err, "Malformed frontend specifier", NULL);
                    }
                }
                else if (!strncmp(arg + 2, "back", 5))
                {
                    if (args->back != NULL)
                    {
                        error_node(err, "Backend alredy specified", NULL);
                    }
                    else if (arg[7] == '=')
                    {
                        args->back = arg + 8;
                    }
                    else
                    {
                        error_node(err, "Malformed frontend specifier", NULL);
                    }
                }
                else if (!strncmp(arg + 2, "asm", 3))
                {
                    if (!strcmp(arg + 5, "=gas") || !strcmp(arg + 5, "=as") ||
                        !strcmp(arg + 5, "=gnu"))
                    {
                        args->flavor = FLAVOR_GNUASM;
                    }
                    else if (!strcmp(arg + 5, "=nasm") ||
                            !strcmp(arg + 5, "=intel"))
                    {
                        args->flavor = FLAVOR_INTELASM;
                    }
                    else if (!strcmp(arg + 5, "=nasm-bin") ||
                            !strcmp(arg + 5, "=intel-bin"))
                    {
                        args->flavor = FLAVOR_INTELBIN;
                    }
                    else
                    {
                        error_node(err, "Malformed asm specifier", NULL);
                    }
                }
                else if (!strncmp(arg + 2, "cells", 5))
                {
                    if (arg[7] == '=')
                    {
                        char *ptr;
                        long long cells = strtoll(arg + 8, &ptr, 10);

                        if (*ptr == '\0' && cells >= 0)
                        {
                            args->cells = cells;
                            continue;
                        }
                    }

                    error_node(err, "Malformed cells specifier", NULL);
                }
                else if (!strncmp(arg + 2, "func-name", 9))
                {
                    if (args->front != NULL)
                    {
                        error_node(err, "Funcname alredy specified", NULL);
                    }
                    else if (arg[11] == '=')
                    {
                        args->func_name = arg + 12;
                    }
                    else
                    {
                        error_node(err, "Malformed func-name specifier", NULL);
                    }
                }
                else
                {
                    error_node(err, "Malformed option", NULL);
                }
            }
            else if (arg[1] == 'f')
            {
                switch (arg[2])
                {
                    case 'r':
                        if (!strcmp(arg + 2, "read"))
                        {
                            if (args->flags & FLAG_READ)
                            {
                                error_node(
                                    err, "Flag fread alredy specified", NULL
                                );
                            }
                            else
                            {
                                args->flags |= FLAG_READ;
                            }
                        }
                        break;

                    case 'l':
                        if (!strcmp(arg + 2, "libc"))
                        {
                            if (args->flags & FLAG_LIBC)
                            {
                                error_node(
                                    err, "Flag flibc alredy specified", NULL
                                );
                            }
                            else
                            {
                                args->flags |= FLAG_LIBC;
                            }
                        }
                        break;

                    case 'w':
                        if (!strcmp(arg + 2, "write"))
                        {
                            if (args->flags & FLAG_WRITE)
                            {
                                error_node(
                                    err, "Flag fwrite alredy specified", NULL
                                );
                            }
                            else
                            {
                                args->flags |= FLAG_WRITE;
                            }
                        }
                        else if (!strcmp(arg + 2, "wrap-cell"))
                        {
                            if (args->flags & FLAG_WRAP_CELL)
                            {
                                error_node(
                                    err, "Flag fwrap-cell alredy specified", NULL
                                );
                            }
                            else
                            {
                                args->flags |= FLAG_WRAP_CELL;
                            }
                        }
                        else if (!strcmp(arg + 2, "wrap-ptr"))
                        {
                            if (args->flags & FLAG_WRAP_PTR)
                            {
                                error_node(
                                    err, "Flag fwrap-ptr alredy specified", NULL
                                );
                            }
                            else
                            {
                                args->flags |= FLAG_WRAP_PTR;
                            }
                        }
                        break;

                    default:
                        error_node(err, "Invalid flag", NULL);
                        break;
                }
            }
            else if (arg[1] == 'o')
            {
                if (len == 2 && i != (argc - 1))
                {
                    if (args->in != NULL)
                    {
                        error_node(err, "Output file alredy specified", NULL);
                    }
                    else
                    {
                        args->out = argv[i + 1];
                    }
                }
                else
                {
                    error_node(err, "Invalid output file specifier", NULL);
                }
            }
            else
            {
                error_node(err, "Malformed option", NULL);
            }
            continue;
        }

        if (len > 0)
        {
            if (args->in != NULL)
            {
                error_node(err, "Input file alredy specified", NULL);
            }
            else
            {
                args->in = arg;
            }
        }
    }

end:
    if (args->out == NULL)
    {
        error_node(err, "Missing output file", NULL);
    }

    if (args->in == NULL)
    {
        error_node(err, "Missing input file", NULL);
    }

    if (args->front == NULL)
    {
        error_node(err, "Missing frontend name", NULL);
    }

    if (args->back == NULL)
    {
        error_node(err, "Missing backend name", NULL);
    }

    if (args->func_name == NULL)
    {
        if (args->flags & FLAVOR_INTELBIN)
        {
            args->func_name = "__prog";
        }
        else if (args->flags & FLAG_LIBC)
        {
            args->func_name = "main";
        }
        else
        {
            error_node(err, "Missing func-name", NULL);
        }
    }
}

void
args_dump(const args_t *args)
{
    printf("{\n");
    printf("\t\"exe\": %s,\n", args->exe);
    printf("\t\"out\": %s,\n", args->out);
    printf("\t\"in\": %s,\n", args->in);

    printf("\t\"front\": %s,\n", args->front);
    printf("\t\"back\": %s,\n", args->back);
    printf("\t\"validation\": %s,\n", args->validation ? "true" : "false");
    printf("\t\"folding\": %s,\n", args->folding ? "true" : "false");
    printf("\t\"func-name\": %s,\n", args->func_name);

    printf("\t\"asm\": ");
    switch (args->flavor)
    {
        case FLAVOR_INTELASM:
            printf("\"intel-asm\",\n");
            break;

        case FLAVOR_INTELBIN:
            printf("\"intel-bin\",\n");
            break;

        case FLAVOR_GNUASM:
            printf("\"gnu-asm\",\n");
            break;

        default:
            printf("\"unknown\",\n");
            break;
    }

    printf("\t\"cells\": %zu,\n", args->cells);

    printf("\t\"flags\": [");
    bool comma = false;

    if (args->flags & FLAG_WRITE)
    {
        printf("\"write\"");
        comma = true;
    }

    if (args->flags & FLAG_READ)
    {
        if (comma)
        {
            printf(", ");
        }

        printf("\"read\"");
        comma = true;
    }

    if (args->flags & FLAG_LIBC)
    {
        if (comma)
        {
            printf(", ");
        }

        printf("\"libc\"");
        comma = true;
    }

    if (args->flags & FLAG_WRAP_CELL)
    {
        if (comma)
        {
            printf(", ");
        }

        printf("\"wrap-cell\"");
        comma = true;
    }

    if (args->flags & FLAG_WRAP_PTR)
    {
        if (comma)
        {
            printf(", ");
        }
        printf("\"wrap-ptr\"");
    }

    printf("]\n}\n");
}