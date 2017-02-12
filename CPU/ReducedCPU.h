//
// Created by Kevin on 11/02/2017.
//

#ifndef GAMEBOY_REDUCEDCPU_H
#define GAMEBOY_REDUCEDCPU_H

#include "Memory/MemoryMappedIO.h"
#include "Registers.h"
#include "IInterruptible.h"

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
            static const uint8_t regMap[8];

            Registers registers;
            Memory::MemoryMappedIO& mmap;
            uint16_t currentPC;
            uint8_t currentInstruction;
            bool isCurrentExtended;
            bool interruptMasterEnable;

            uint32_t ticks;

            // Implementations
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
