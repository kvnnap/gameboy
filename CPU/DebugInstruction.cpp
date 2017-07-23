//
// Created by kvnna on 23/07/2017.
//

#include "DebugInstruction.h"

using namespace std;
using namespace Gameboy::CPU;

DebugInstruction::DebugInstruction()
    : Address (), Operand16 (), Operand (), Opcode(), Length (), IsExtended(), IsUnsigned()
{}

DebugInstruction::DebugInstruction(const std::string& strInstruction, std::uint8_t length,
                                   bool isUnisgned)
    : StringInstruction(strInstruction),
      Address (),
      Operand16 (),
      Operand (),
      Opcode(),
      Length (length),
      IsExtended(),
      IsUnsigned(isUnisgned)
{}

std::string DebugInstruction::toString() const {
    return to_string(Address) + ": " + to_string(Opcode) + ": " + StringInstruction;
}
