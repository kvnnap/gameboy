//
// Created by kvnna on 09/10/2016.
//

#include "Registers.h"
#include <sstream>
#include <iomanip>

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
        default: return "";
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
        default: return "";
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
    // lower 4 bits are unused in F register
    reg[destIndex] = static_cast<uint8_t>(destIndex == AF ? (value & 0xF0) : value);
    reg[destIndex + 1] = static_cast<uint8_t>(value >> 8);
}

uint16_t Registers::read16(uint8_t srcIndex) const {
    return (reg[srcIndex + 1] << 8) | reg[srcIndex];
}

std::string Registers::toString() const {
    stringstream ss;
    ss << hex
       << "A -> " << setw(2) << static_cast<uint16_t>(reg[A]) << " | " << "F -> "
       << ((reg[F] & ZeroFlag) ? 'Z' : '0')
            << ((reg[F] & SubtractFlag) ? 'N' : '0')
            << ((reg[F] & HalfCarryFlag) ? 'H' : '0')
            << ((reg[F] & CarryFlag) ? 'C' : '0')
       << " (" << setw(2) << static_cast<uint16_t>(reg[F]) << ")" << endl
       << "B -> " << setw(2) << static_cast<uint16_t>(reg[B]) << " | " << "C -> " << setw(2) << static_cast<uint16_t>(reg[C]) << endl
       << "D -> " << setw(2) << static_cast<uint16_t>(reg[D]) << " | " << "E -> " << setw(2) << static_cast<uint16_t>(reg[E]) << endl
       << "H -> " << setw(2) << static_cast<uint16_t>(reg[H]) << " | " << "L -> " << setw(2) << static_cast<uint16_t>(reg[L]) << endl
       << "SP-> " << setw(4) << read16(SP) << endl
       << "PC-> " << setw(4) << read16(PC);
    return ss.str();
}
