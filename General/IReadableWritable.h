//
// Created by kvnna on 08/10/2016.
//

#ifndef GAMEBOY_IREADABLEWRITABLE_H
#define GAMEBOY_IREADABLEWRITABLE_H

#include <cstdint>

namespace Gameboy {
    namespace General {
        class IReadableWritable {
        public:
            virtual ~IReadableWritable(){};
            virtual std::uint8_t read(std::uint16_t address) const = 0;
            virtual void write(std::uint16_t address, std::uint8_t datum) = 0;
        };
    }
}

#endif //GAMEBOY_IREADABLEWRITABLE_H
