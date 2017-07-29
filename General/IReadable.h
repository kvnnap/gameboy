//
// Created by kvnna on 29/07/2017.
//

#ifndef GAMEBOY_IREADABLE_H
#define GAMEBOY_IREADABLE_H

#include <cstdint>

namespace Gameboy
{
    namespace General
    {
        class IReadable
        {
        public:
            virtual ~IReadable();

            virtual std::uint8_t read(std::uint16_t address) const = 0;
            std::uint16_t read16 (std::uint16_t address) const;
        };
    }
}




#endif //GAMEBOY_IREADABLE_H
