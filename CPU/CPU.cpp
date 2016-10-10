//
// Created by kvnna on 09/10/2016.
//

#include <stdexcept>

#include "CPU.h"

using namespace std;
using namespace Gameboy::CPU;
using namespace Gameboy::General;

CPU::CPU()
    : instructions {
        {1,  4,  0,  0,  0, nullptr},                     // 0x00 NOP
        {3, 12,  0, BC,  0, &CPU::load_d16_to_reg},       // 0x01 LD  BC,  d16
        {1,  8,  0, BC,  A, &CPU::load_reg8_to_regpt},    // 0x02 LD (BC),   A
        {1,  8,  0, BC, BC, &CPU::inc_reg16},             // 0x03 INC BC
        {1,  4,  0,  B,  B, &CPU::inc_reg8},              // 0x04 INC  B // Flags
        {1,  4,  0,  B,  B, &CPU::dec_reg8},              // 0x05 DEC  B // Flags
        {2,  8,  0,  B,  0, &CPU::load_d8_to_reg},        // 0x06 LD  B,  d8
        {1,  4,  0,  A,  A, &CPU::rlc_reg8},              // 0x07 RLCA // Flags
        {3, 20,  0,  0, SP, &CPU::load_reg16_to_d16pt},   // 0x08 LD (a16), SP
        {1,  8,  0, HL, BC, &CPU::add_reg16_to_reg16},    // 0x09 ADD HL, BC // Flags
        {1,  8,  0,  A, BC, &CPU::load_regpt_to_reg8},    // 0x0A LD A, (BC)
        {1,  8,  0, BC, BC, &CPU::dec_reg16},             // 0x0B DEC BC
        {1,  4,  0,  C,  C, &CPU::inc_reg8},              // 0x0C INC  C // Flags
        {1,  4,  0,  C,  C, &CPU::dec_reg8},              // 0x0D DEC  C // Flags
        {2,  8,  0,  C,  0, &CPU::load_d8_to_reg},        // 0x0E LD  C,  d8
        {1,  4,  0,  A,  A, &CPU::rrc_reg8},              // 0x0F RRCA // Flags

        {2,  4,  0,  0,  0, &CPU::stop},                  // 0x10 STOP TODO
        {3, 12,  0, DE,  0, &CPU::load_d16_to_reg},       // 0x11 LD  DE,  d16
        {1,  8,  0, DE,  A, &CPU::load_reg8_to_regpt},    // 0x12 LD (DE),   A
        {1,  8,  0, DE, DE, &CPU::inc_reg16},             // 0x13 INC DE
        {1,  4,  0,  D,  D, &CPU::inc_reg8},              // 0x14 INC  D // Flags
        {1,  4,  0,  D,  D, &CPU::dec_reg8},              // 0x15 DEC  D // Flags
        {2,  8,  0,  D,  0, &CPU::load_d8_to_reg},        // 0x16 LD  D,  d8
        {1,  4,  0,  A,  A, &CPU::rl_reg8},               // 0x17 RLA // Flags
        {2, 12,  0, PC,  0, &CPU::rel_jmp},               // 0x18 JR r8
        {1,  8,  0, HL, DE, &CPU::add_reg16_to_reg16},    // 0x19 ADD HL, DE // Flags
        {1,  8,  0,  A, DE, &CPU::load_regpt_to_reg8},    // 0x1A LD A, (DE)
        {1,  8,  0, DE, DE, &CPU::dec_reg16},             // 0x1B DEC DE
        {1,  4,  0,  E,  E, &CPU::inc_reg8},              // 0x1C INC  E // Flags
        {1,  4,  0,  E,  E, &CPU::dec_reg8},              // 0x1D DEC  E // Flags
        {2,  8,  0,  E,  0, &CPU::load_d8_to_reg},        // 0x1E LD  E,  d8
        {1,  4,  0,  A,  A, &CPU::rr_reg8},               // 0x1F RRA // Flags

        // Try to remove the need for two instruction cycle durations - 0x20 is one example
        {2, 12,  8, PC,  0, &CPU::rel_nz_jmp},            // 0x20 JR  NZ, r8
        {3, 12,  0, HL,  0, &CPU::load_d16_to_reg},       // 0x21 LD  HL, d16
        {1,  8,  0, HL,  A, &CPU::load_reg8inc_to_regpt}, // 0x22 LD (HL+), A
        {1,  8,  0, HL, HL, &CPU::inc_reg16},             // 0x23 INC HL
        {1,  4,  0,  H,  H, &CPU::inc_reg8},              // 0x24 INC  H // Flags
        {1,  4,  0,  H,  H, &CPU::dec_reg8},              // 0x25 DEC  H // Flags
        {2,  8,  0,  H,  0, &CPU::load_d8_to_reg},        // 0x26 LD  D,  d8
        {1,  4,  0,  A,  A, &CPU::daa},                   // 0x27 DAA    // Flags TODO: Check
        {2, 12,  8, PC,  0, &CPU::rel_z_jmp},             // 0x28 JR  Z, r8
        {1,  8,  0, HL, HL, &CPU::add_reg16_to_reg16},    // 0x29 ADD HL, HL // Flags
        {1,  8,  0,  A, HL, &CPU::load_regptinc_to_reg8}, // 0x2A LD A, (HL+)
        {1,  8,  0, HL, HL, &CPU::dec_reg16},             // 0x2B DEC HL
        {1,  4,  0,  L,  L, &CPU::inc_reg8},              // 0x2C INC  L // Flags
        {1,  4,  0,  L,  L, &CPU::dec_reg8},              // 0x2D DEC  L // Flags
        {2,  8,  0,  L,  0, &CPU::load_d8_to_reg},        // 0x2E LD   L,  d8
        {1,  4,  0,  A,  A, &CPU::cpl},                   // 0x2F CPL // Flags

        {2, 12,  8, PC,  0, &CPU::rel_nc_jmp},            // 0x30 JR  NC, r8
        {3, 12,  0, SP,  0, &CPU::load_d16_to_reg},       // 0x31 LD  SP, d16
        {1,  8,  0, HL,  A, &CPU::load_reg8dec_to_regpt}, // 0x32 LD (HL-), A
        {1,  8,  0, SP, SP, &CPU::inc_reg16},             // 0x33 INC SP
        {1, 12,  0, HL, HL, &CPU::inc_regpt},             // 0x34 INC (HL)
        {1, 12,  0, HL, HL, &CPU::dec_regpt},             // 0x35 DEC (HL)
    }
{}

// loads
std::uint8_t CPU::load_d16_to_reg (const Instruction& instruction) {
    registers.write16(instruction.destRegIndex, mmap.read16(static_cast<uint16_t>(currentPC + 1)));
    return instruction.cycles;
}

std::uint8_t CPU::load_d8_to_reg (const Instruction& instruction) {
    registers.reg[instruction.destRegIndex] = mmap.read(static_cast<uint16_t>(currentPC + 1));
    return instruction.cycles;
}

std::uint8_t CPU::load_reg8_to_regpt (const Instruction& instruction) {
    mmap.write(registers.read16(instruction.destRegIndex), registers.reg[instruction.srcRegIndex]);
    return instruction.cycles;
}

std::uint8_t CPU::load_reg8inc_to_regpt (const Instruction& instruction) {
    // Put srcReg data into memory address pointed to by destReg
    uint16_t destRegValue = registers.read16(instruction.destRegIndex);
    mmap.write(destRegValue, registers.reg[instruction.srcRegIndex]);
    // Increment HL
    //inc_reg16(instruction); // this would work too since it is destReg to be incremented
    registers.write16(instruction.destRegIndex, static_cast<uint16_t>(destRegValue + 1));
    return instruction.cycles;
}

std::uint8_t CPU::load_reg8dec_to_regpt (const Instruction& instruction) {
    // Put srcReg data into memory address pointed to by destReg
    uint16_t destRegValue = registers.read16(instruction.destRegIndex);
    mmap.write(destRegValue, registers.reg[instruction.srcRegIndex]);
    // Decrement HL
    registers.write16(instruction.destRegIndex, static_cast<uint16_t>(destRegValue - 1));
    return instruction.cycles;
}

std::uint8_t CPU::load_regpt_to_reg8 (const Instruction& instruction) {
    registers.reg[instruction.destRegIndex] = mmap.read(registers.read16(instruction.srcRegIndex));
    return instruction.cycles;
}

std::uint8_t CPU::load_regptinc_to_reg8 (const Instruction& instruction) {
    uint16_t srcRegValue = registers.read16(instruction.srcRegIndex);
    registers.reg[instruction.destRegIndex] = mmap.read(srcRegValue);
    registers.write16(instruction.srcRegIndex, static_cast<uint16_t>(srcRegValue + 1));
    return instruction.cycles;
}

std::uint8_t CPU::load_reg16_to_d16pt (const Instruction& instruction) {
    mmap.write16(mmap.read16(static_cast<uint16_t>(currentPC + 1)), registers.read16(instruction.srcRegIndex));
    return instruction.cycles;
}

// increments / Decrements
std::uint8_t CPU::inc_reg16 (const Instruction& instruction) {
    registers.write16(instruction.destRegIndex, static_cast<uint16_t>(registers.read16(instruction.destRegIndex) + 1));
    return instruction.cycles;
}

std::uint8_t CPU::dec_reg16 (const Instruction& instruction) {
    registers.write16(instruction.destRegIndex, static_cast<uint16_t>(registers.read16(instruction.destRegIndex) - 1));
    return instruction.cycles;
}

std::uint8_t CPU::inc_reg8 (const Instruction& instruction) {
    // Affects Zero flag, Subtract flag and Half Carry flag
    uint8_t regValueAfter = ++registers.reg[instruction.destRegIndex];
    registers.reg[instruction.destRegIndex] = regValueAfter;

    // Reset all affected flags and set necessary ones
    incdec8_flags(regValueAfter, false);
    return instruction.cycles;
}

std::uint8_t CPU::inc_regpt (const Instruction& instruction) {
    // Affects Zero flag, Subtract flag and Half Carry flag
    uint16_t memLocation = registers.read16(instruction.srcRegIndex);
    uint8_t memValueAfter = mmap.read(memLocation);
    mmap.write16(memLocation, ++memValueAfter);

    // Reset all affected flags and set necessary ones
    incdec8_flags(memValueAfter, false);
    return instruction.cycles;
}

std::uint8_t CPU::dec_reg8 (const Instruction& instruction) {
    // Affects Zero flag, Subtract flag and Half Carry flag
    uint8_t regValueAfter = --registers.reg[instruction.destRegIndex];
    registers.reg[instruction.destRegIndex] = regValueAfter;

    // Reset all affected flags and set as necessary ones
    incdec8_flags(regValueAfter, true);
    return instruction.cycles;
}

std::uint8_t CPU::dec_regpt (const Instruction& instruction) {
    // Affects Zero flag, Subtract flag and Half Carry flag
    uint16_t memLocation = registers.read16(instruction.srcRegIndex);
    uint8_t memValueAfter = mmap.read(memLocation);
    mmap.write16(memLocation, --memValueAfter);

    // Reset all affected flags and set as necessary ones
    incdec8_flags(memValueAfter, true);
    return instruction.cycles;
}

void CPU::incdec8_flags(std::uint8_t valueAfter, bool isSubtract) {
    registers.reg[F] &= ~(ZeroFlag | SubtractFlag | HalfCarry);
    registers.reg[F] |= valueAfter == 0 ? ZeroFlag : 0;
    if (isSubtract) {
        registers.reg[F] |= SubtractFlag;
        registers.reg[F] |= (valueAfter & 0x0F) == 0x0F ? HalfCarry : 0;
    } else {
        registers.reg[F] |= (valueAfter & 0x0F) == 0 ? HalfCarry : 0;
    }
}

// Complement
std::uint8_t CPU::cpl (const Instruction& instruction) {
    registers.reg[instruction.destRegIndex] = ~registers.reg[instruction.destRegIndex];
    // Set necessary flags
    registers.reg[F] |= (SubtractFlag | HalfCarry);
    return instruction.cycles;
}

// Adds
std::uint8_t CPU::add_reg16_to_reg16 (const Instruction& instruction) {
    uint16_t srcReg  = registers.read16(instruction.srcRegIndex);
    uint16_t destReg = registers.read16(instruction.destRegIndex);
    registers.write16(instruction.destRegIndex, srcReg + destReg);

    // Affected flags - Half Carry flag and Carry flag
    registers.reg[F] &= ~(SubtractFlag | HalfCarry | Carry);
    // set if carry from bit 11 0x0FFF
    registers.reg[F] |= 0x1000 & ((destReg & 0x0FFF) + (srcReg & 0x0FFF)) ? HalfCarry : 0;
    // set if carry from bit 15
    registers.reg[F] |= 0x10000 & (static_cast<uint32_t>(destReg) + srcReg) ? Carry : 0;
    return instruction.cycles;
}

// rotates
std::uint8_t CPU::rlc_reg8 (const Instruction& instruction) {
    uint8_t regValue = registers.reg[instruction.destRegIndex];
    uint8_t bit7 = static_cast<uint8_t>(regValue & 0x80 ? 1 : 0);
    regValue <<= 1;
    regValue |= bit7;
    registers.reg[instruction.destRegIndex] = regValue;

    // Affected flags - Zero and Carry flag
    registers.reg[F] &= ~(ZeroFlag | SubtractFlag | HalfCarry | Carry);
    registers.reg[F] |= regValue == 0 ? ZeroFlag : 0;
    registers.reg[F] |= bit7 ? Carry : 0;
    return instruction.cycles;
}

std::uint8_t CPU::rrc_reg8 (const Instruction& instruction) {
    uint8_t regValue = registers.reg[instruction.destRegIndex];
    uint8_t bit0 = static_cast<uint8_t>(regValue & 0x01);
    regValue >>= 1; // right shifting on unsigned should not preserve sign bit - exactly what we want
    regValue |= bit0 ? 0x80 : 0;
    registers.reg[instruction.destRegIndex] = regValue;

    // Affected flags - Zero and Carry flag
    registers.reg[F] &= ~(ZeroFlag | SubtractFlag | HalfCarry | Carry);
    registers.reg[F] |= regValue == 0 ? ZeroFlag : 0;
    registers.reg[F] |= bit0 ? Carry : 0;
    return instruction.cycles;
}

std::uint8_t CPU::rl_reg8 (const Instruction& instruction) {
    uint8_t regValue = registers.reg[instruction.destRegIndex];
    uint8_t bit7 = static_cast<uint8_t>(regValue & 0x80);
    regValue <<= 1;
    regValue |= registers.reg[F] & Carry ? 1 : 0;
    registers.reg[instruction.destRegIndex] = regValue;

    // Affected flags - Zero and Carry flag
    registers.reg[F] &= ~(ZeroFlag | SubtractFlag | HalfCarry | Carry);
    registers.reg[F] |= regValue == 0 ? ZeroFlag : 0;
    registers.reg[F] |= bit7 ? Carry : 0;
    return instruction.cycles;
}

std::uint8_t CPU::rr_reg8 (const Instruction& instruction) {
    uint8_t regValue = registers.reg[instruction.destRegIndex];
    uint8_t bit0 = static_cast<uint8_t>(regValue & 0x01);
    regValue >>= 1; // right shifting on unsigned should not preserve sign bit - exactly what we want
    regValue |= registers.reg[F] & Carry ? 0x80 : 0;
    registers.reg[instruction.destRegIndex] = regValue;

    // Affected flags - Zero and Carry flag
    registers.reg[F] &= ~(ZeroFlag | SubtractFlag | HalfCarry | Carry);
    registers.reg[F] |= regValue == 0 ? ZeroFlag : 0;
    registers.reg[F] |= bit0 ? Carry : 0;
    return instruction.cycles;
}

std::uint8_t CPU::stop (const Instruction& instruction) {
    throw runtime_error ("STOP instruction not yet implemented");
}

// Jumps - relative
std::uint8_t CPU::rel_jmp (const Instruction& instruction) {
    // destRegIndex must be PC
    registers.write16(instruction.destRegIndex,
                      registers.read16(instruction.destRegIndex) +
                      static_cast<int8_t>(mmap.read(static_cast<uint16_t>(currentPC + 1)))
    );
    return instruction.cycles;
}

std::uint8_t CPU::rel_cond_jmp (const Instruction& instruction, uint8_t flag, bool negative) {
    // destRegIndex must be PC
    // if ((negative && (registers.reg[F] & flag != 0)) || (!negative && (registers.reg[F] & flag == 0)))
    if (negative != ((registers.reg[F] & flag) == 0)) {
        return rel_jmp(instruction);
        //return instruction.cycles;
    }
    return instruction.cyclesNoAction;
}

uint8_t CPU::rel_nz_jmp(const Instruction &instruction) {
    return rel_cond_jmp(instruction, ZeroFlag, true);
}
uint8_t CPU::rel_z_jmp(const Instruction &instruction) {
    return rel_cond_jmp(instruction, ZeroFlag, false);
}
uint8_t CPU::rel_nc_jmp(const Instruction &instruction) {
    return rel_cond_jmp(instruction, Carry, true);
}
uint8_t CPU::rel_c_jmp(const Instruction &instruction) {
    return rel_cond_jmp(instruction, Carry, false);
}

// DAA - TODO: Use Z80 table instead, or check with it. This is probably buggy
// http://www.z80.info/z80syntx.htm#DAA
uint8_t CPU::daa(const Instruction &instruction) {

    // Reset all affected flags and set as necessary ones
    registers.reg[F] &= ~(ZeroFlag | HalfCarry | Carry);

    uint8_t regValue = registers.reg[instruction.destRegIndex];
    uint8_t low = static_cast<uint8_t>(regValue & 0x0F);
    if ((registers.reg[F] & HalfCarry) || low > 9) {
        if (registers.reg[F] & SubtractFlag) {
            regValue -= 0x06;
        } else {
            regValue += 0x06;
        }
    }
    uint8_t high = static_cast<uint8_t>((regValue >> 4) & 0x0F);
    if ((registers.reg[F] & Carry) || high > 9) {
        if (registers.reg[F] & SubtractFlag) {
            regValue -= 0x60;
        } else {
            regValue += 0x60;
        }
        registers.reg[F] |= Carry; // Set carry flag
    }

    if (regValue == 0) {
        registers.reg[F] |=  ZeroFlag;
    }
    return instruction.cycles;
}

std::uint8_t CPU::next() {

    // Check interrupts

    // fetch and decode
    currentPC = registers.read16(PC);
    const Instruction& instruction = instructions[currentPC];
    registers.write16(PC, currentPC + instruction.length);

    // Execute
    uint8_t cycles = instruction.cycles;
    if (instruction.execfn != nullptr) {
        cycles = (this->*instruction.execfn)(instruction);
    }

    return cycles;
}
