//
// Created by kvnna on 09/10/2016.
//

#ifndef GAMEBOY_CPU_H
#define GAMEBOY_CPU_H

#include "Memory/MemoryMappedIO.h"
#include "Registers.h"
#include "Instruction.h"

namespace Gameboy {
    namespace CPU {

        // Gameboy CPU LR35902
        class CPU {
        public:

            CPU();

            void next();

        private:
            Registers registers;
            Memory::MemoryMappedIO mmap;
            Instruction instructions[512];

            void load_d16_to_reg(const Instruction &instruction);

            void load_reg8_to_regpt(const Instruction &instruction);

            void inc_reg16(const Instruction &instruction);

            void inc_reg8(const Instruction &instruction);

            void dec_reg8(const Instruction &instruction);

            void load_d8_to_reg(const Instruction &instruction);

            void rlc_reg8(const Instruction &instruction);

            void load_reg16_to_d16pt(const Instruction &instruction);

            void add_reg16_to_reg16(const Instruction &instruction);

            void load_regpt_to_reg8(const Instruction &instruction);

            void dec_reg16(const Instruction &instruction);

            void rrc_reg8(const Instruction &instruction);

            void stop(const Instruction &instruction);

            void rl_reg8(const Instruction &instruction);

            void rel_jmp(const Instruction &instruction);

            void rr_reg8(const Instruction &instruction);
        };
    }
}

#endif //GAMEBOY_CPU_H
