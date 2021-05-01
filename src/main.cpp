#include <iostream>
#include <fstream>
#include <streambuf>
#include <cstring>
#include <sstream>

#include "compiler.h"

void usage(char *prog)
{
    std::cerr << "Usage: " << prog << " input [output] [-S]" << std::endl;
}

int main(int argc, char **argv)
{
    if (argc == 1 || argc > 4)
    {
        usage(argv[0]);
        std::exit(0);
    }

    bool assembly = false;

    if (argc == 4)
    {
        if (!strcmp("-S", argv[3]))
            assembly = true;
        else
        {
            std::cerr << "Error: invalid option " << argv[3] << std::endl;
            usage(argv[0]);
            std::exit(1);
        }
    }

    char *filename = argc > 2 ? argv[2] : nullptr;
    std::ifstream input(argv[1]);

    if (!input)
    {
        std::cerr << "Error: invalid input file " << argv[1] << std::endl;
        usage(argv[0]);
        std::exit(1);
    }

    std::string source;
    input.seekg(0, std::ios::end);
    source.reserve(input.tellg());
    input.seekg(0, std::ios::beg);
    source.assign((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());

    auto compiled = bf::compile_asm(source);

    if (assembly)
    {
        if (filename)
        {
            std::ofstream output(filename);
            output << compiled << std::endl;
        }
        else
            std::cout << compiled << std::endl;
    }
    else if (filename)
    {
        auto temp1name = "/tmp/bfctemp1";
        auto temp2name = "/tmp/bfctemp2";

        std::FILE *temp1 = std::fopen(temp1name, "w");
        std::FILE *temp2 = std::fopen(temp2name, "w");

        compiled += '\n';
        std::fwrite(compiled.c_str(), sizeof(char), compiled.size(), temp1);
        std::fflush(temp1);

        auto command = std::stringstream();
        command << "as -g3 " << temp1name << " -o " << temp2name;
        command << " && gcc -g3 " << temp2name << " -o " << filename;

        auto cleanup = [&]() {
            std::fclose(temp1);
            std::fclose(temp2);
            std::remove(temp1name);
            std::remove(temp2name);
        };

        if (system(command.str().c_str()))
        {
            cleanup();
            std::cerr << "Error: assembling and linking " << argv[1] << " failed" << std::endl;
            std::exit(1);
        }
        cleanup();
    }
    else
    {
        std::cerr << "Error: expected output file" << std::endl;
        std::exit(1);
    }

    return 0;
}
