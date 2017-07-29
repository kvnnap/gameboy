//
// Created by kvnna on 08/10/2016.
//

#ifndef GAMEBOY_IREADABLEWRITABLE_H
#define GAMEBOY_IREADABLEWRITABLE_H

#include "IReadable.h"

namespace Gameboy {
    namespace General {
        class IReadableWritable
            : public IReadable
        {
        public:
            ~IReadableWritable(){};

            virtual void write(std::uint16_t address, std::uint8_t datum) = 0;
            void write16 (std::uint16_t address, std::uint16_t value);
        };
    }
}

#endif //GAMEBOY_IREADABLEWRITABLE_H
