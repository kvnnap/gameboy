//
// Created by kvnna on 09/10/2016.
//

#ifndef GAMEBOY_INSTRUCTION_H
#define GAMEBOY_INSTRUCTION_H

#include <cstdint>

namespace Gameboy {
    namespace CPU {
        class CPU;
        class Instruction {
        public:
            //Instruction();
            //std::uint8_t opcode;
            std::uint8_t length;
            std::uint8_t cycles;
            std::uint8_t destRegIndex;
            std::uint8_t srcRegIndex;
            void (CPU::*execfn)(const Instruction& instruction);
        };
    }
}


#endif //GAMEBOY_INSTRUCTION_H
