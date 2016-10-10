//
// Created by kvnna on 09/10/2016.
//

#ifndef GAMEBOY_MEMORYMAPPEDIO_H
#define GAMEBOY_MEMORYMAPPEDIO_H

#include "General/IReadableWritable.h"

namespace Gameboy {
    namespace Memory {
        class MemoryMappedIO
            : public General::IReadableWritable
        {
        public:
            std::uint8_t read(std::uint16_t address) const override;
            void write(std::uint16_t address, std::uint8_t datum) override;
        };
    }
}



#endif //GAMEBOY_MEMORYMAPPEDIO_H
