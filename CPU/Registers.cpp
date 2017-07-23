//
// Created by kvnna on 09/10/2016.
//

#include "Registers.h"

using namespace std;
using namespace Gameboy::CPU;

string Gameboy::CPU::RegisterIndexToString(RegisterIndex registerIndex) {
    switch (registerIndex) {
        case F: return "F";
        case A: return "A";
        case C: return "C";
        case B: return "B";
        case E: return "E";
        case D: return "D";
        case L: return "L";
        case H: return "H";
    }
}

string Gameboy::CPU::RegisterIndexToString16(RegisterIndex16 registerIndex16) {
    switch (registerIndex16) {
        case AF: return "AF";
        case BC: return "BC";
        case DE: return "DE";
        case HL: return "HL";
        case SP: return "SP";
        case PC: return "PC";
    }
}

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
