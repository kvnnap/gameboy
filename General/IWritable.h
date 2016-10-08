//
// Created by kvnna on 08/10/2016.
//

#ifndef GAMEBOY_IWRITABLE_H
#define GAMEBOY_IWRITABLE_H


#include <cstdint>

namespace Gameboy {
    namespace General {
        class IWritable {
        public:
            virtual ~IWritable(){};
            virtual void write(std::uint16_t address, std::uint8_t datum) = 0;
        };
    }
}


#endif //GAMEBOY_IWRITABLE_H
