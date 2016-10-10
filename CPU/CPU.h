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

            std::uint8_t next();

        private:
            Registers registers;
            Memory::MemoryMappedIO mmap;
            Instruction instructions[512];
            uint16_t currentPC;

            // Instruction implementations
            std::uint8_t load_d16_to_reg(const Instruction &instruction);

            std::uint8_t load_reg8_to_regpt(const Instruction &instruction);

            std::uint8_t inc_reg16(const Instruction &instruction);

            std::uint8_t inc_reg8(const Instruction &instruction);

            std::uint8_t dec_reg8(const Instruction &instruction);

            std::uint8_t load_d8_to_reg(const Instruction &instruction);

            std::uint8_t rlc_reg8(const Instruction &instruction);

            std::uint8_t load_reg16_to_d16pt(const Instruction &instruction);

            std::uint8_t add_reg16_to_reg16(const Instruction &instruction);

            std::uint8_t load_regpt_to_reg8(const Instruction &instruction);

            std::uint8_t dec_reg16(const Instruction &instruction);

            std::uint8_t rrc_reg8(const Instruction &instruction);

            std::uint8_t stop(const Instruction &instruction);

            std::uint8_t rl_reg8(const Instruction &instruction);

            std::uint8_t rel_jmp(const Instruction &instruction);

            std::uint8_t rr_reg8(const Instruction &instruction);

            uint8_t rel_nz_jmp(const Instruction &instruction);

            uint8_t rel_cond_jmp(const Instruction &instruction, uint8_t flag, bool negative);

            uint8_t load_reg8inc_to_regpt(const Instruction &instruction);

            uint8_t daa(const Instruction &instruction);

            uint8_t rel_z_jmp(const Instruction &instruction);

            uint8_t load_regptinc_to_reg8(const Instruction &instruction);

            uint8_t cpl(const Instruction &instruction);

            uint8_t rel_nc_jmp(const Instruction &instruction);

            uint8_t rel_c_jmp(const Instruction &instruction);

            uint8_t load_reg8dec_to_regpt(const Instruction &instruction);

            uint8_t inc_regpt(const Instruction &instruction);

            uint8_t dec_regpt(const Instruction &instruction);

            void incdec8_flags(uint8_t valueAfter, bool isSubtract);
        };
    }
}

#endif //GAMEBOY_CPU_H
