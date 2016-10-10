//
// Created by kvnna on 09/10/2016.
//

#include "Registers.h"

using namespace std;
using namespace Gameboy::CPU;

Registers::Registers()
    : f (), a (),
      c (), b (),
      e (), d (),
      l (), h (),
      sp (),
      pc ()
{}

void Registers::write16(uint8_t destIndex, uint16_t value) {
    reg[destIndex] = static_cast<uint8_t>(value);
    reg[destIndex + 1] = static_cast<uint8_t>(value >> 8);
}

uint16_t Registers::read16(uint8_t srcIndex) const {
    return (reg[srcIndex + 1] << 8) | reg[srcIndex];
}
