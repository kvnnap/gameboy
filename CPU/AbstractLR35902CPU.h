//
// Created by kvnna on 23/07/2017.
//

#ifndef GAMEBOY_ABSTRACTLR35902CPU_H
#define GAMEBOY_ABSTRACTLR35902CPU_H

#include "Memory/MemoryMappedIO.h"
#include "Registers.h"
#include "ICPU.h"
#include "IInterruptible.h"
#include "DebugInstruction.h"

namespace Gameboy {

    namespace CPU {

        class AbstractLR35902CPU
            : public ICPU, public IInterruptible
        {
        public:
            virtual ~AbstractLR35902CPU();
            AbstractLR35902CPU(Memory::MemoryMappedIO& p_mmap);

            DebugInstruction disassembleNext() const override;
            std::vector<DebugInstruction> disassemble(std::size_t num) const override;

        protected:
            // Maps
            static const uint8_t regMap[8];

            // Reference to memory map
            Memory::MemoryMappedIO& mmap;

            // data
            Registers registers;
            uint32_t ticks;
            bool interruptMasterEnable;

        private:
            // Map
            static const DebugInstruction debugInstructions[128];

            // Utils
            std::uint8_t getImmediateValue(std::uint16_t pc) const;
            std::uint16_t getImmediateValue16(std::uint16_t pc) const;
            DebugInstruction fetchAndDecode(std::uint16_t pc) const;
        };
    }
}
#endif //GAMEBOY_ABSTRACTLR35902CPU_H
