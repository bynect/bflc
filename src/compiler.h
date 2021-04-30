#ifndef BF_COMP_H
#define BF_COMP_H

#include <string>
#include <vector>

namespace bf {

    std::string compile_asm(const char *source, size_t size);

    std::string compile_asm(std::string& source);

    std::string compile_asm(std::vector<uint8_t> source);

    std::string compile_asm(std::vector<char> source);

}

#endif
