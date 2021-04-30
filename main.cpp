#include <iostream>
#include <fstream>
#include <streambuf>
#include <cstring>

#include "src/compiler.h"

void usage(char *prog)
{
    std::cout << "Usage: " << prog << " input [output]" << std::endl;
}

int main(int argc, char **argv)
{
    if (argc != 2 && argc != 3)
    {
        usage(argv[0]);
        std::exit(0);
    }

    char *filename = argc > 2 ? argv[2] : nullptr;
    std::ifstream input(argv[1]);

    if (!input)
    {
        std::cout << "Error: invalid input file " << argv[1] << std::endl;
        usage(argv[0]);
        std::exit(1);
    }

    std::string source;
    input.seekg(0, std::ios::end);
    source.reserve(input.tellg());
    input.seekg(0, std::ios::beg);
    source.assign((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());

    auto compiled = bf::compile_asm(source);

    if (filename)
    {
        std::ofstream output(filename);
        output << compiled << std::endl;
    }
    else
        std::cout << compiled << std::endl;

    return 0;
}
