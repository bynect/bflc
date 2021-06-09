/*
    Copyright 2021 @bynect

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "front/brainfuck.h"
#include "mid/validation.h"
#include "mid/folding.h"
#include "back/x86_64.h"

#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

int
jit_execute(bytebuffer_t *buf)
{
    int fd = open("/dev/zero", O_RDWR);
    void *exc = mmap(
        0, buf->pos, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE, fd, 0
    );

    if (exc == MAP_FAILED)
    {
        perror("mmap");
        return 1;
    }

    printf("Before JIT execution.\n");
    printf("----------------------------------------\n");

    memcpy(exc, buf->bytes, buf->pos);
    void (*fn)() = exc;
    fn();

    printf("----------------------------------------\n");
    printf("After JIT execution.\n");

    munmap(exc, buf->pos);
    close(fd);
    return 0;
}

#define CHECK_ERR(ctx, err, ir, msg) \
    if (error_dump(&err) > 0) \
    { \
        error_free(ctx, &err); \
        ir_free(ctx, &ir); \
        context_free(ctx); \
        return 1; \
    } \
    else \
    { \
        printf(msg); \
    }

int
main(int argc, const char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "usage: %s file\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "rb");

    if (file == NULL)
    {
        perror("fopen");
        return 1;
    }

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);

    char *src = malloc(size + 1);
    src[size] = '\0';

    fread(src, 1, size, file);
    fclose(file);

    mem_t mem;
    mem_default(&mem);
    context_t *ctx = context_new(&mem);

    size_t cells = 30000;
    context_set(ctx, CTX_CELLS, &cells);

    bool syscall = true;
    context_set(ctx, CTX_FWRITE, &syscall);
    context_set(ctx, CTX_FREAD, &syscall);

    ir_t ir;
    ir_init(&ir);

    error_t err = scan_brainfuck(ctx, src, size, &ir);
    CHECK_ERR(ctx, err, ir, "Program scanned\n");

    err = pass_validation(ctx, &ir);
    CHECK_ERR(ctx, err, ir, "Program validated\n");

    err = pass_folding(ctx, &ir);
    CHECK_ERR(ctx, err, ir, "Program optimized\n");

    bytebuffer_t buf;
    bytebuffer_init(ctx, &buf, BYTEBUFFER_BLOCK);

    err = emit_mach_x86_64(ctx, &buf, &ir);

    if (!error_success(&err))
    {
        bytebuffer_free(ctx, &buf);
    }

    CHECK_ERR(ctx, err, ir, "Program compiled\n");
    int res = jit_execute(&buf);

    bytebuffer_free(ctx, &buf);
    ir_free(ctx, &ir);
    context_free(ctx);
    return res;
}
