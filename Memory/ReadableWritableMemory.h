//
// Created by kvnna on 08/10/2016.
//

#ifndef GAMEBOY_READABLEWRITABLEMEMORY_H
#define GAMEBOY_READABLEWRITABLEMEMORY_H

#include "ReadableMemory.h"

namespace Gameboy {
    namespace Memory {
        class ReadableWritableMemory
            : public ReadableMemory
        {
        public:
            void writeExt(std::size_t address, std::uint8_t datum);
        };
    }
}


#endif //GAMEBOY_READABLEWRITABLEMEMORY_H
