//
// Created by Kevin on 11/02/2017.
//

#ifndef GAMEBOY_REDUCEDCPU_H
#define GAMEBOY_REDUCEDCPU_H

#include "Memory/MemoryMappedIO.h"
#include "Registers.h"
#include "IInterruptible.h"
#include "ReducedInstruction.h"

namespace Gameboy {
    namespace CPU {

        // Gameboy CPU LR35902
        class ReducedCPU
            : public IInterruptible
        {
        public:

            ReducedCPU(Memory::MemoryMappedIO& p_mmap);

            std::uint32_t getTicks() const;
            void next();

            void requestInterrupt(std::uint8_t irqLine) override;

        private:
            static const ReducedInstruction instructions[16];
            static const uint8_t regMap[8];
            static const uint8_t reg16Map[4];
            static const uint8_t reg16MapLdInc[4];

            Registers registers;
            Memory::MemoryMappedIO& mmap;
            uint16_t currentPC;
            uint8_t currentInstruction;
            bool isCurrentExtended;
            bool interruptMasterEnable;

            uint32_t ticks;

            uint8_t rowSelector; // 2-bit row selector
            uint8_t splitRowSelector; // 3-bit row divided by two selector

            // Useful methods
            void incrementProgramCounterBy(std::uint16_t incrementAmount);
            std::uint8_t getImmediateValue() const;

            // Branchers
            std::uint8_t brancher_nop_stop_jmp_ld(const ReducedInstruction& instruction);
            std::uint8_t brancher_r_l_r_c_cpl_c_daa_scf(const ReducedInstruction& instruction);

            // Less-compact implementations
            std::uint8_t load_d16_to_reg(const ReducedInstruction& instruction);
            std::uint8_t load_reg8_to_regpt_vv_inc_dec(const ReducedInstruction &instruction);
            std::uint8_t inc_dec_reg16(const ReducedInstruction& instruction);
            std::uint8_t inc_dec_reg8_or_regpt(const ReducedInstruction &instruction);
            std::uint8_t load_d8_to_reg8_or_regpt(const ReducedInstruction& instruction);
            std::uint8_t load_reg16_to_d16pt();
            std::uint8_t add_reg16_to_reg16_HL(const ReducedInstruction &instruction); // TO HL

            // Implementations
            std::uint8_t rel_cond_jmp();

            void add_val8_to_reg8_nc_c(std::uint8_t srcValue, bool carry);
            void sub_val8_from_reg8_nc_c(std::uint8_t srcValue, bool carry);
            void and_val8_to_reg8(std::uint8_t srcValue);
            void xor_val8_to_reg8(std::uint8_t srcValue);
            void or_val8_to_reg8(std::uint8_t srcValue);
            void cp_val8_to_reg8(std::uint8_t srcValue);

            // Prefix CB Implementations
            std::uint8_t s_l_r_a_l_val8(std::uint8_t val);
            std::uint8_t r_l_r_c_val8(std::uint8_t val);
        };
    }
}


#endif //GAMEBOY_REDUCEDCPU_H
