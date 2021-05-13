#include "args.h"
#include "driver.h"

int
main(int argc, char **argv)
{
    args_t args;
    error_t err = {NULL, NULL};

    args_parse(argc, argv, &args, &err);
    args_dump(&args);
    if (!error_success(&err))
    {
        error_dump(&err);
        error_free(&err);
        return 1;
    }

    context_t *ctx = context_new();
    int res = driver_main(ctx, &args);
    context_free(ctx);
    return res;
}
