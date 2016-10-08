//
// Created by Kevin on 08/10/2016.
//

#ifndef GAMEBOY_STRINGOPERATIONS_H
#define GAMEBOY_STRINGOPERATIONS_H

#include <string>
#include <vector>

namespace Util
{
    namespace StringOperations {
        std::vector<std::string>& split(const std::string &s, char delim, std::vector<std::string> &elems);
        std::vector<std::string> split(const std::string &s, char delim);
    }
}

#endif //GAMEBOY_STRINGOPERATIONS_H
