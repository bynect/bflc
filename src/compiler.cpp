#include <string>
#include <sstream>
#include <vector>

#include "compiler.h"

std::string bf::compile_asm(const char *source, size_t size)
{
    auto buf = std::stringstream();
    uint32_t label = 0, label2 = 0;
    std::vector<uint32_t> labels;

    buf << "	# cell begin\n";
    buf << "	.data\n";
    buf << "cells:\n";
    buf << "	.space 30000, 0\n";
    buf << "cellptr:\n";
    buf << "	.quad cells\n";
    buf << "	# cell end\n";
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
                buf << "	# > begin\n";

                buf << "	movq cellptr(%rip), %rax\n";
                buf << "	addq $1, %rax\n";
                buf << "	movq %rax, cellptr(%rip)\n";

                buf << "	# > end\n";
                break;

            case '<':
                buf << "	# < begin\n";

                buf << "	movq cellptr(%rip), %rax\n";
                buf << "	subq $1, %rax\n";
                buf << "	movq %rax, cellptr(%rip)\n";

                buf << "	# < end>\n";
                break;

            case '+':
                buf << "	# + begin\n";

                buf << "	movq cellptr(%rip), %rax\n";
                buf << "	movl (%rax), %edx\n";
                buf << "	addl $1, %edx\n";
                buf << "	movl %edx, (%rax)\n";

                buf << "	# + end\n";
                break;

            case '-':
                buf << "	# - begin\n";

                buf << "	movq cellptr(%rip), %rax\n";
                buf << "	movl (%rax), %edx\n";
                buf << "	subl $1, %edx\n";
                buf << "	movl %edx, (%rax)\n";

                buf << "	# - end\n";
                break;

            case '.':
                buf << "	# . begin\n";

                buf << "	movq cellptr(%rip), %rax\n";
                buf << "	movl (%rax), %eax\n";
                buf << "	movsbl %al, %eax\n";
                buf << "	movl %eax, %edi\n";
                buf << "	call putchar@PLT\n";

                buf << "	# . end\n";
                break;

            case ',':
                buf << "	# , begin\n";

                buf << "	call getchar@PLT\n";
                buf << "	movl %eax, %edx\n";
                buf << "	movq cellptr(%rip), %rax\n";
                buf << "	movsbl %dl, %edx\n";
                buf << "	movl %edx, (%rax)\n";

                buf << "	# , end\n";
                break;

            case '[':
                buf << "	# [ begin\n";

                buf << "L" << label << ":\n";
                labels.push_back(label++);
                labels.push_back(label++);

                buf << "	movq cellptr(%rip), %rax\n";
                buf << "	movb (%rax), %al\n";
                buf << "	testb %al, %al\n";
                buf << "	je L" << (label - 1) << "\n";

                buf << "	# [ end\n";
                break;

            case ']':
                buf << "	# ] begin\n";

                label2 = labels.back();
                labels.pop_back();

                buf << "	jmp L" << labels.back() << "\n";
                buf << "L" << label2 << ":\n";
                labels.pop_back();

                buf << "	# ] end\n";
                break;
        }
    }

    buf << "\n";
    buf << "	movl $0, %eax\n";
    buf << "	popq %rbp\n";
    buf << "	ret\n";

    return buf.str();
}

std::string bf::compile_asm(std::string& source)
{
    return bf::compile_asm(source.c_str(), source.size());
}
