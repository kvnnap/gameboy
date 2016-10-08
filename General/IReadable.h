//
// Created by kvnna on 08/10/2016.
//

#ifndef GAMEBOY_IREADABLE_H
#define GAMEBOY_IREADABLE_H

#include <cstdint>

namespace Gameboy {
    namespace General {
        class IReadable {
        public:
            virtual ~IReadable(){};
            virtual std::uint8_t read(std::uint16_t address) const = 0;
        };
    }
}

#endif //GAMEBOY_IREADABLE_H
