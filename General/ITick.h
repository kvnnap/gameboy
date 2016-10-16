//
// Created by Kevin on 16/10/2016.
//

#ifndef GAMEBOY_ITICK_H
#define GAMEBOY_ITICK_H

namespace Gameboy {
    namespace General {
        class ITick {
        public:
            virtual ~ITick(){};

            virtual void next(std::uint32_t ticks) = 0;
        };
    }
}

#endif //GAMEBOY_ITICK_H
