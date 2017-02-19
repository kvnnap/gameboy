//
// Created by Kevin on 19/02/2017.
//

#include "ReducedInstruction.h"

using namespace Gameboy::CPU;

std::uint8_t ReducedInstruction::getRegisterIndex(std::uint8_t index) const {
    return (*relatedRegMap)[index];
}
