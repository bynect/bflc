#ifndef BF_COMP_H
#define BF_COMP_H

#include <string>

namespace bf {

    std::string compile_asm(const char *source, size_t size);

    std::string compile_asm(std::string& source);

}

#endif
