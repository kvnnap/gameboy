//
// Created by Kevin on 16/10/2016.
//

#ifndef GAMEBOY_IINTERRUPTIBLE_H
#define GAMEBOY_IINTERRUPTIBLE_H

#include <cstdint>

namespace Gameboy {
    namespace CPU {
        class IInterruptible {
        public:
            virtual ~IInterruptible() {};

            virtual void interrupt(std::uint8_t irqLine) = 0;
        };
    }
}

#endif //GAMEBOY_IINTERRUPTIBLE_H
