//
// Created by kvnna on 23/07/2017.
//

#ifndef GAMEBOY_ICPU_H
#define GAMEBOY_ICPU_H

#include "DebugInstruction.h"
#include "Registers.h"
#include <vector>

namespace Gameboy {
    namespace CPU {
        class ICPU {
        public:
            virtual ~ICPU() {};
            virtual void next() = 0;
            virtual std::uint32_t getTicks() const = 0;
            virtual DebugInstruction disassembleNext() const = 0;
            virtual std::vector<DebugInstruction> disassemble(std::size_t num) const = 0;

            virtual Registers& getRegisters() = 0;
            virtual bool isInterruptMasterEnabled() const = 0;
        };
    }
}

#endif //GAMEBOY_ICPU_H
