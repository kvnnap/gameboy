//
// Created by Kevin on 13/10/2016.
//

#ifndef GAMEBOY_FILEOPERATIONS_H
#define GAMEBOY_FILEOPERATIONS_H

#include <cstdint>
#include <vector>
#include <string>

namespace Util
{
    namespace FileOperations {
        std::vector<std::uint8_t> loadBinaryFile(const std::string& filePath);
    }
}


#endif //GAMEBOY_FILEOPERATIONS_H
