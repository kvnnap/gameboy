//
// Created by kvnna on 09/10/2016.
//

#ifndef GAMEBOY_CPU_H
#define GAMEBOY_CPU_H

#include "Memory/MemoryMappedIO.h"
#include "Registers.h"
#include "Instruction.h"
#include "IInterruptible.h"

namespace Gameboy {
    namespace CPU {

        // Gameboy CPU LR35902
        class CPU
            : public IInterruptible
        {
        public:

            CPU(Memory::MemoryMappedIO& p_mmap);

            std::uint32_t getTicks() const;
            void next();

            void interrupt(std::uint8_t irqLine) override;

        private:
            static const Instruction instructions[256];
            static const uint8_t regMap[8];

            Registers registers;
            Memory::MemoryMappedIO& mmap;
            uint16_t currentPC;
            uint8_t currentInstruction;
            bool isCurrentExtended;
            bool interruptMasterEnable;

            uint32_t ticks;

            // Instruction implementations
            std::uint8_t load_d16_to_reg(const Instruction &instruction);

            std::uint8_t load_reg8_to_regpt(const Instruction &instruction);

            std::uint8_t inc_reg16(const Instruction &instruction);

            std::uint8_t inc_reg8(const Instruction &instruction);

            std::uint8_t dec_reg8(const Instruction &instruction);

            std::uint8_t load_d8_to_reg(const Instruction &instruction);

            std::uint8_t load_reg16_to_d16pt(const Instruction &instruction);

            std::uint8_t add_reg16_to_reg16(const Instruction &instruction);

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

            uint8_t load_reg8_to_reg8(const Instruction &instruction);

            uint8_t halt(const Instruction &instruction);

            void add_val8_to_reg8_nc_c(std::uint8_t destRegIndex, std::uint8_t srcValue, bool carry);

            uint8_t add_reg8_to_reg8(const Instruction &instruction);

            uint8_t add_reg8_to_reg8_c(const Instruction &instruction);

            uint8_t add_regpt_to_reg8(const Instruction &instruction);

            uint8_t add_regpt_to_reg8_c(const Instruction &instruction);

            void sub_val8_from_reg8_nc_c(std::uint8_t destRegIndex, std::uint8_t srcValue, bool carry);

            uint8_t sub_reg8_from_reg8(const Instruction &instruction);

            uint8_t sub_reg8_from_reg8_c(const Instruction &instruction);

            uint8_t sub_regpt_from_reg8(const Instruction &instruction);

            uint8_t sub_regpt_from_reg8_c(const Instruction &instruction);

            uint8_t and_reg8_to_reg8(const Instruction &instruction);

            uint8_t and_regpt_to_reg8(const Instruction &instruction);

            void and_val8_to_reg8(uint8_t destRegIndex, uint8_t srcValue);

            void xor_val8_to_reg8(uint8_t destRegIndex, uint8_t srcValue);

            uint8_t xor_reg8_to_reg8(const Instruction &instruction);

            uint8_t xor_regpt_to_reg8(const Instruction &instruction);

            void or_val8_to_reg8(uint8_t destRegIndex, uint8_t srcValue);

            uint8_t or_reg8_to_reg8(const Instruction &instruction);

            uint8_t or_regpt_to_reg8(const Instruction &instruction);

            void cp_val8_to_reg8(uint8_t destRegIndex, uint8_t srcValue);

            uint8_t cp_reg8_to_reg8(const Instruction &instruction);

            uint8_t cp_regpt_to_reg8(const Instruction &instruction);

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

            uint8_t add_d8_to_reg8(const Instruction &instruction);

            uint8_t add_d8_to_reg8_c(const Instruction &instruction);

            uint8_t sub_d8_to_reg8(const Instruction &instruction);

            uint8_t sub_d8_to_reg8_c(const Instruction &instruction);

            uint8_t rst(const Instruction &instruction);


            uint8_t load_reg8_to_d8pt(const Instruction &instruction);

            uint8_t load_reg8_to_reg8pt(const Instruction &instruction);

            uint8_t and_d8_to_reg8(const Instruction &instruction);

            uint8_t xor_d8_to_reg8(const Instruction &instruction);

            uint8_t or_d8_to_reg8(const Instruction &instruction);

            uint8_t cp_d8_to_reg8(const Instruction &instruction);

            uint8_t add_r8_to_reg16(const Instruction &instruction);

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
