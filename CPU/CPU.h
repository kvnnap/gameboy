//
// Created by kvnna on 09/10/2016.
//

#ifndef GAMEBOY_CPU_H
#define GAMEBOY_CPU_H

#include "Memory/MemoryMappedIO.h"
#include "Registers.h"
#include "Instruction.h"
#include "AbstractLR35902CPU.h"

namespace Gameboy {
    namespace CPU {

        // Gameboy CPU LR35902
        class CPU
            : public AbstractLR35902CPU
        {
        public:

            CPU(Memory::MemoryMappedIO& p_mmap);

            // Overrides
            void next() override;

            std::uint32_t getTicks() const override;
            void requestInterrupt(std::uint8_t irqLine) override;

        private:
            static const Instruction instructions[128];

            // Helper data
            uint16_t currentPC;
            uint8_t currentInstruction;
            bool isCurrentExtended;
            uint8_t splitRowSelector; // 3-bit row divided by two selector
            bool delayedIMEFlag;

            // Utils
            void incrementProgramCounterBy(std::uint16_t incrementAmount);
            std::uint8_t getImmediateValue() const;
            std::uint16_t getImmediateValue16() const;
            bool getShouldJump(bool, std::uint8_t) const;

            // Brancher
            void add_sub_and_xor_or_cp_nc_c (std::uint8_t srcValue);
            std::uint8_t brancher_add_sub_and_xor_or_cp_nc_c (const Instruction& instruction);

            // Add stuff
            void add_val8_to_reg8_nc_c(std::uint8_t srcValue, bool carry);
            void sub_val8_from_reg8_nc_c(std::uint8_t srcValue, bool carry);
            void and_val8_to_reg8(std::uint8_t srcValue);
            void xor_val8_to_reg8(std::uint8_t srcValue);
            void or_val8_to_reg8(std::uint8_t srcValue);
            void cp_val8_to_reg8(std::uint8_t srcValue);

            // Instruction implementations
            std::uint8_t load_d16_to_reg(const Instruction &instruction);

            std::uint8_t load_reg8_to_regpt(const Instruction &instruction);

            std::uint8_t inc_reg16(const Instruction &instruction);

            std::uint8_t inc_reg8(const Instruction &instruction);

            std::uint8_t dec_reg8(const Instruction &instruction);

            std::uint8_t load_d8_to_reg(const Instruction &instruction);

            std::uint8_t load_reg16_to_d16pt(const Instruction &instruction);

            std::uint8_t add_reg16_to_reg16(const Instruction &instruction);

            std::uint8_t add_r8_to_reg16(const Instruction &instruction);

            std::uint8_t load_regpt_to_reg8(const Instruction &instruction);

            std::uint8_t dec_reg16(const Instruction &instruction);

            std::uint8_t stop(const Instruction &instruction);

            std::uint8_t rel_jmp(const Instruction &instruction);

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

            uint8_t load_d8_to_regpt(const Instruction &instruction);

            uint8_t set_carryflag(const Instruction &instruction);

            uint8_t load_regptdec_to_reg8(const Instruction &instruction);

            uint8_t cpl_carryflag(const Instruction &instruction);

            uint8_t ret(const Instruction &instruction);

            uint8_t reti(const Instruction &instruction);

            uint8_t ret_cond(const Instruction &instruction, uint8_t flag, bool negative);

            uint8_t ret_nz(const Instruction &instruction);

            uint8_t ret_z(const Instruction &instruction);

            uint8_t ret_nc(const Instruction &instruction);

            uint8_t ret_c(const Instruction &instruction);

            uint8_t pop(const Instruction &instruction);

            uint8_t jmp(const Instruction &instruction);

            uint8_t jmp_cond(const Instruction &instruction, uint8_t flag, bool negative);

            uint8_t jmp_nz(const Instruction &instruction);

            uint8_t jmp_z(const Instruction &instruction);

            uint8_t jmp_nc(const Instruction &instruction);

            uint8_t jmp_c(const Instruction &instruction);

            uint8_t push(const Instruction &instruction);

            uint8_t call(const Instruction &instruction);

            uint8_t call_cond(const Instruction &instruction, uint8_t flag, bool negative);

            uint8_t call_nz(const Instruction &instruction);

            uint8_t call_z(const Instruction &instruction);

            uint8_t call_nc(const Instruction &instruction);

            uint8_t call_c(const Instruction &instruction);

            uint8_t rst(const Instruction &instruction);

            uint8_t load_reg8_to_d8pt(const Instruction &instruction);

            uint8_t load_reg8_to_reg8pt(const Instruction &instruction);

            uint8_t jmp_regpt(const Instruction &instruction);

            uint8_t load_reg8_to_d16pt(const Instruction &instruction);

            uint8_t load_d16pt_to_reg8(const Instruction &instruction);

            uint8_t load_d8pt_to_reg8(const Instruction &instruction);

            uint8_t load_reg8pt_to_reg8(const Instruction &instruction);

            uint8_t enable_interrupts(const Instruction &instruction);

            uint8_t disable_interrupts(const Instruction &instruction);

            uint8_t load_reg16_to_reg16(const Instruction &instruction);

            uint8_t ldhl_spr8(const Instruction &instruction);

            uint8_t r_l_r_c_val8(uint8_t val);

            uint8_t r_l_r_c_reg8(const Instruction &instruction);

            uint8_t s_l_r_a_l_val8(uint8_t val);
        };
    }
}

#endif //GAMEBOY_CPU_H
