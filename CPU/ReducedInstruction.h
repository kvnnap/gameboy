//
// Created by Kevin on 19/02/2017.
//

#ifndef GAMEBOY_REDUCEDINSTRUCTION_H
#define GAMEBOY_REDUCEDINSTRUCTION_H

#include <cstdint>

namespace Gameboy {
    namespace CPU {
        class ReducedCPU;
        class ReducedInstruction {
        public:
            std::uint8_t (ReducedCPU::* const execfn)(const ReducedInstruction& reducedInstruction);
            const std::uint8_t (*relatedRegMap)[4];

            // Methods
            std::uint8_t getRegisterIndex(std::uint8_t index) const;
        };
    }
}

#endif //GAMEBOY_REDUCEDINSTRUCTION_H
