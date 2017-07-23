//
// Created by Kevin on 11/02/2017.
//

#ifndef GAMEBOY_REDUCEDCPU_H
#define GAMEBOY_REDUCEDCPU_H

#include "Memory/MemoryMappedIO.h"
#include "Registers.h"
#include "AbstractLR35902CPU.h"
#include "ReducedInstruction.h"

namespace Gameboy {
    namespace CPU {

        // Gameboy CPU LR35902
        class ReducedCPU
            : public AbstractLR35902CPU
        {
        public:

            ReducedCPU(Memory::MemoryMappedIO& p_mmap);

            std::uint32_t getTicks() const override;
            void next() override;

            void requestInterrupt(std::uint8_t irqLine) override;

        private:
            static const ReducedInstruction instructions[16];
            static const ReducedInstruction instructions2[16];
            static const uint8_t reg16Map[4];
            static const uint8_t reg16MapLdInc[4];
            static const uint8_t reg16MapPopPush[4];

            uint16_t currentPC;
            uint8_t currentInstruction;
            bool isCurrentExtended;


            uint8_t rowSelector; // 2-bit row selector
            uint8_t splitRowSelector; // 3-bit row divided by two selector

            // Useful methods
            void incrementProgramCounterBy(std::uint16_t incrementAmount);
            std::uint8_t getImmediateValue() const;
            std::uint16_t getImmediateValue16() const;
            bool getShouldJump(FlagRegister flag) const;

            // Branchers
            std::uint8_t brancher_nop_stop_jmp_ld(const ReducedInstruction& instruction);
            std::uint8_t brancher_r_l_r_c_cpl_c_daa_scf(const ReducedInstruction& instruction);
            std::uint8_t brancher_jmp_load(const ReducedInstruction& instruction);
            std::uint8_t brancher_ret_ld_ldh_add(const ReducedInstruction& instruction);
            std::uint8_t brancher_jmp_ei_di(const ReducedInstruction &instruction);
            std::uint8_t brancher_ret_jmp_ld(const ReducedInstruction &instruction);

            // Less-compact implementations (Part 1)
            std::uint8_t load_d16_to_reg(const ReducedInstruction& instruction);
            std::uint8_t load_reg8_to_regpt_vv_inc_dec(const ReducedInstruction &instruction);
            std::uint8_t inc_dec_reg16(const ReducedInstruction& instruction);
            std::uint8_t inc_dec_reg8_or_regpt(const ReducedInstruction &instruction);
            std::uint8_t load_d8_to_reg8_or_regpt(const ReducedInstruction& instruction);
            std::uint8_t add_reg16_to_reg16_HL(const ReducedInstruction &instruction); // TO HL
            std::uint8_t load_reg8_to_d8_pt_vv();

            // Implementations used by branchers
            std::uint8_t rel_cond_jmp();
            std::uint8_t load_reg16_to_d16pt();
            std::uint8_t cond_jmp();
            std::uint8_t load_reg8_to_reg8pt_vv(); // These add to
            std::uint8_t cond_ret();
            std::uint8_t jmp(bool incrementPC = true);
            std::uint8_t jmp_from_HL_pt();
            std::uint8_t load_HL_to_SP();
            std::uint8_t b_call(bool incrementPC = true);
            std::uint8_t ret(bool incrementPC = true, bool enableInterrupts = false);

            // Less-compact implementations (Part 2)
            std::uint8_t push_pop(const ReducedInstruction &instruction);
            std::uint8_t rst(const ReducedInstruction &instruction);
            std::uint8_t call(const ReducedInstruction &instruction);
            std::uint8_t rel_cond_call(const ReducedInstruction &instruction);
            std::uint8_t dispatch_add_sub_and_xor_or_cp_nc_c(const ReducedInstruction &instruction);


            void add_val8_to_reg8_nc_c(std::uint8_t srcValue, bool carry);
            void sub_val8_from_reg8_nc_c(std::uint8_t srcValue, bool carry);
            void and_val8_to_reg8(std::uint8_t srcValue);
            void xor_val8_to_reg8(std::uint8_t srcValue);
            void or_val8_to_reg8(std::uint8_t srcValue);
            void cp_val8_to_reg8(std::uint8_t srcValue);
            void add_sub_and_xor_or_cp_nc_c (std::uint8_t srcValue);

            // Prefix CB Implementations
            std::uint8_t s_l_r_a_l_val8(std::uint8_t val);
            std::uint8_t r_l_r_c_val8(std::uint8_t val);
        };
    }
}


#endif //GAMEBOY_REDUCEDCPU_H
