#include <string>
#include <sstream>
#include <vector>

#include "compiler.h"

static std::string
compile_asm_internal(std::stringstream& buffer, const char *source, size_t size)
{
    uint32_t label = 0, label2 = 0;
    std::vector<uint32_t> labels;

    buffer << "	# cell begin\n";
    buffer << "	.data\n";
    buffer << "cells:\n";
    buffer << "	.space 30000, 0\n";
    buffer << "cellptr:\n";
    buffer << "	.quad cells\n";
    buffer << "	# cell end\n";
    buffer << "\n";

    buffer << "	.text\n";
    buffer << "	.globl main\n";
    buffer << "	.type main, @function\n";
    buffer << "main:\n";
    buffer << "	pushq %rbp\n";
    buffer << "	movq %rsp, %rbp\n";
    buffer << "\n";

    for (size_t i = 0; i < size; ++i)
    {
        switch (source[i])
        {
            case '>':
                buffer << "	# > begin\n";

                buffer << "	movq cellptr(%rip), %rax\n";
                buffer << "	addq $1, %rax\n";
                buffer << "	movq %rax, cellptr(%rip)\n";

                buffer << "	# > end\n";
                break;

            case '<':
                buffer << "	# < begin\n";

                buffer << "	movq cellptr(%rip), %rax\n";
                buffer << "	subq $1, %rax\n";
                buffer << "	movq %rax, cellptr(%rip)\n";

                buffer << "	# < end>\n";
                break;

            case '+':
                buffer << "	# + begin\n";

                buffer << "	movq cellptr(%rip), %rax\n";
                buffer << "	movl (%rax), %edx\n";
                buffer << "	addl $1, %edx\n";
                buffer << "	movl %edx, (%rax)\n";

                buffer << "	# + end\n";
                break;

            case '-':
                buffer << "	# - begin\n";

                buffer << "	movq cellptr(%rip), %rax\n";
                buffer << "	movl (%rax), %edx\n";
                buffer << "	subl $1, %edx\n";
                buffer << "	movl %edx, (%rax)\n";

                buffer << "	# - end\n";
                break;

            case '.':
                buffer << "	# . begin\n";

                buffer << "	movq cellptr(%rip), %rax\n";
                buffer << "	movl (%rax), %eax\n";
                buffer << "	movsbl %al, %eax\n";
                buffer << "	movl %eax, %edi\n";
                buffer << "	call putchar@PLT\n";

                buffer << "	# . end\n";
                break;

            case ',':
                buffer << "	# , begin\n";

                buffer << "	call getchar@PLT\n";
                buffer << "	movl %eax, %edx\n";
                buffer << "	movq cellptr(%rip), %rax\n";
                buffer << "	movsbl %dl, %edx\n";
                buffer << "	movl %edx, (%rax)\n";

                buffer << "	# , end\n";
                break;

            case '[':
                buffer << "	# [ begin\n";

                buffer << "L" << label << ":\n";
                labels.push_back(label++);
                labels.push_back(label++);

                buffer << "	movq cellptr(%rip), %rax\n";
                buffer << "	movb (%rax), %al\n";
                buffer << "	testb %al, %al\n";
                buffer << "	je L" << (label - 1) << "\n";

                buffer << "	# [ end\n";
                break;

            case ']':
                buffer << "	# ] begin\n";

                label2 = labels.back();
                labels.pop_back();

                buffer << "	jmp L" << labels.back() << "\n";
                buffer << "L" << label2 << ":\n";
                labels.pop_back();

                buffer << "	# ] end\n";
                break;
        }
    }

    buffer << "\n";
    buffer << "	movl $0, %eax\n";
    buffer << "	popq %rbp\n";
    buffer << "	ret\n";

    return buffer.str();
}

std::string bf::compile_asm(const char *source, size_t size)
{
    auto buffer = std::stringstream();
    return compile_asm_internal(buffer, source, size);
}

std::string bf::compile_asm(std::string& source)
{
    return bf::compile_asm(source.c_str(), source.size());
}


std::string bf::compile_asm(std::vector<uint8_t> source)
{
    return bf::compile_asm((char *)source.data(), source.size());
}

std::string bf::compile_asm(std::vector<char> source)
{
    return bf::compile_asm(source.data(), source.size());
}
