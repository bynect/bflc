#include <string>
#include <sstream>

std::string compile_asm(const char *source, size_t size)
{
    auto buf = std::stringstream();

    buf << "	.data\n";
    buf << "cells:\n";
    buf << "	.space 3000, 0\n";
    buf << "cellptr:\n";
    buf << "	.quad cells\n";
    buf << "\n";

    buf << "	.text\n";
    buf << "	.globl main\n";
    buf << "	.type main, @function\n";
    buf << "main:\n";
    buf << "	pushq %rbp\n";
    buf << "	movq %rsp, %rbp\n";
    buf << "\n";

    for (size_t i = 0; i < size; ++i)
    {
        switch (source[i])
        {
            case '>':
                buf << "	addq $1, cellptr(%rip)\n";
                break;

            case '<':
                buf << "	subq $-1, cellptr(%rip)\n";
                break;

            case '+':
                buf << "	movq cellptr(%rip), %r11\n";
                buf << "	addb $1, (%r11)\n";
                break;

            case '-':
                buf << "	movq cellptr(%rip), %r11\n";
                buf << "	subb $1, (%r11)\n";
                break;

            case '.':
                buf << "	movq cellptr(%rip), %rdi\n";
                buf << "	call putchar@PLT\n";
                break;

            case ',':
                buf << "	call getchar@PLT\n";
                buf << "	movq %rax, %r10\n";
                buf << "	movb %r10b, cellptr(%rip)\n";
                break;

        }
    }

    buf << "\n";
    buf << "	popq %rbp\n";
    buf << "	ret\n";

    return buf.str();
}

#include <iostream>

int main()
{
    std::cout << compile_asm(",.", 2) << std::endl;
}
