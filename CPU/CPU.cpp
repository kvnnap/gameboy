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
        {1,  4,  0,  0, nullptr},                     // 0x00 NOP
        {3, 12, BC,  0, &CPU::load_d16_to_reg},       // 0x01 LD  BC,  d16
        {1,  8, BC,  A, &CPU::load_reg8_to_regpt},    // 0x02 LD (BC),   A
        {1,  8, BC, BC, &CPU::inc_reg16},             // 0x03 INC BC
        {1,  4,  B,  B, &CPU::inc_reg8},              // 0x04 INC  B // Flags
        {1,  4,  B,  B, &CPU::dec_reg8},              // 0x05 DEC  B // Flags
        {2,  8,  B,  0, &CPU::load_d8_to_reg},        // 0x06 LD  B,  d8
        {1,  4,  A,  A, &CPU::rlc_reg8},              // 0x07 RLCA // Flags
        {3, 20,  0, SP, &CPU::load_reg16_to_d16pt},   // 0x08 LD (a16), SP
        {1,  8, HL, BC, &CPU::add_reg16_to_reg16},    // 0x09 ADD HL, BC // Flags
        {1,  8,  A, BC, &CPU::load_regpt_to_reg8},    // 0x0A LD A, (BC)
        {1,  8, BC, BC, &CPU::dec_reg16},             // 0x0B DEC BC
        {1,  4,  C,  C, &CPU::inc_reg8},              // 0x0C INC  C // Flags
        {1,  4,  C,  C, &CPU::dec_reg8},              // 0x0D DEC  C // Flags
        {2,  8,  C,  0, &CPU::load_d8_to_reg},        // 0x0E LD  C,  d8
        {1,  4,  A,  A, &CPU::rrc_reg8},              // 0x0F RRCA // Flags

        {2,  4,  0,  0, &CPU::stop},                  // 0x10 STOP TODO
        {3, 12, DE,  0, &CPU::load_d16_to_reg},       // 0x11 LD  DE,  d16
        {1,  8, DE,  A, &CPU::load_reg8_to_regpt},    // 0x12 LD (DE),   A
        {1,  8, DE, DE, &CPU::inc_reg16},             // 0x13 INC DE
        {1,  4,  D,  D, &CPU::inc_reg8},              // 0x14 INC  D // Flags
        {1,  4,  D,  D, &CPU::dec_reg8},              // 0x15 DEC  D // Flags
        {2,  8,  D,  0, &CPU::load_d8_to_reg},        // 0x16 LD  D,  d8
        {1,  4,  A,  A, &CPU::rl_reg8},               // 0x17 RLA // Flags
        {2, 12, PC,  0, &CPU::rel_jmp},               // 0x18 JR r8
        {1,  8, HL, DE, &CPU::add_reg16_to_reg16},    // 0x19 ADD HL, DE // Flags
        {1,  8,  A, DE, &CPU::load_regpt_to_reg8},    // 0x1A LD A, (DE)
        {1,  8, DE, DE, &CPU::dec_reg16},             // 0x1B DEC DE
        {1,  4,  E,  E, &CPU::inc_reg8},              // 0x1C INC  E // Flags
        {1,  4,  E,  E, &CPU::dec_reg8},              // 0x1D DEC  E // Flags
        {2,  8,  E,  0, &CPU::load_d8_to_reg},        // 0x1E LD  E,  d8
        {1,  4,  A,  A, &CPU::rr_reg8},               // 0x1F RRA // Flags
    }
{}

// loads
void CPU::load_d16_to_reg (const Instruction& instruction) {
    registers.write16(instruction.destRegIndex, mmap.read16(static_cast<uint16_t>(registers.pc + 1)));
}

void CPU::load_d8_to_reg (const Instruction& instruction) {
    registers.reg[instruction.destRegIndex] = mmap.read(static_cast<uint16_t>(registers.pc + 1));
}

void CPU::load_reg8_to_regpt (const Instruction& instruction) {
    mmap.write(registers.read16(instruction.destRegIndex), registers.reg[instruction.srcRegIndex]);
}

void CPU::load_regpt_to_reg8 (const Instruction& instruction) {
    registers.reg[instruction.destRegIndex] = mmap.read(registers.read16(instruction.srcRegIndex));
}

void CPU::load_reg16_to_d16pt (const Instruction& instruction) {
    mmap.write16(mmap.read16(static_cast<uint16_t>(registers.pc + 1)), registers.read16(instruction.srcRegIndex));
}

// increments / Decrements
void CPU::inc_reg16 (const Instruction& instruction) {
    registers.write16(instruction.destRegIndex, static_cast<uint16_t>(registers.read16(instruction.destRegIndex) + 1));
}

void CPU::dec_reg16 (const Instruction& instruction) {
    registers.write16(instruction.destRegIndex, static_cast<uint16_t>(registers.read16(instruction.destRegIndex) - 1));
}

void CPU::inc_reg8 (const Instruction& instruction) {
    // Affects Zero flag, Subtract flag and Half Carry flag
    uint8_t regValueAfter = ++registers.reg[instruction.destRegIndex];

    // Reset all affected flags and set as necessary ones
    registers.reg[F] &= ~(ZeroFlag | SubtractFlag | HalfCarry);
    registers.reg[F] |= (regValueAfter & 0x0F) == 0 ? HalfCarry : 0;
    registers.reg[F] |= regValueAfter == 0 ? ZeroFlag : 0;
    registers.reg[instruction.destRegIndex] = regValueAfter;
}

void CPU::dec_reg8 (const Instruction& instruction) {
    // Affects Zero flag, Subtract flag and Half Carry flag
    uint8_t regValueAfter = --registers.reg[instruction.destRegIndex];

    // Reset all affected flags and set as necessary ones
    registers.reg[F] &= ~(ZeroFlag | SubtractFlag | HalfCarry);
    registers.reg[F] |= SubtractFlag;
    registers.reg[F] |= (regValueAfter & 0x0F) == 0x0F ? HalfCarry : 0;
    registers.reg[F] |= regValueAfter == 0 ? ZeroFlag : 0;
    registers.reg[instruction.destRegIndex] = regValueAfter;
}

// Adds
void CPU::add_reg16_to_reg16 (const Instruction& instruction) {
    uint16_t srcReg  = registers.read16(instruction.srcRegIndex);
    uint16_t destReg = registers.read16(instruction.destRegIndex);
    registers.write16(instruction.destRegIndex, srcReg + destReg);

    // Affected flags - Zero and Carry flag
    registers.reg[F] &= ~(SubtractFlag | HalfCarry | Carry);
    // set if carry from bit 11 0x0FFF
    registers.reg[F] |= 0x1000 & ((destReg & 0x0FFF) + (srcReg & 0x0FFF)) ? HalfCarry : 0;
    // set if carry from bit 15
    registers.reg[F] |= 0x10000 & (static_cast<uint32_t>(destReg) + srcReg) ? Carry : 0;
}

// rotates
void CPU::rlc_reg8 (const Instruction& instruction) {
    uint8_t regValue = registers.reg[instruction.destRegIndex];
    uint8_t bit7 = static_cast<uint8_t>(regValue & 0x80 ? 1 : 0);
    regValue <<= 1;
    regValue |= bit7;
    registers.reg[instruction.destRegIndex] = regValue;

    // Affected flags - Zero and Carry flag
    registers.reg[F] &= ~(ZeroFlag | SubtractFlag | HalfCarry | Carry);
    registers.reg[F] |= regValue == 0 ? ZeroFlag : 0;
    registers.reg[F] |= bit7 ? Carry : 0;
}

void CPU::rrc_reg8 (const Instruction& instruction) {
    uint8_t regValue = registers.reg[instruction.destRegIndex];
    uint8_t bit0 = static_cast<uint8_t>(regValue & 0x01);
    regValue >>= 1; // right shifting on unsigned should not preserve sign bit - exactly what we want
    regValue |= bit0 ? 0x80 : 0;
    registers.reg[instruction.destRegIndex] = regValue;

    // Affected flags - Zero and Carry flag
    registers.reg[F] &= ~(ZeroFlag | SubtractFlag | HalfCarry | Carry);
    registers.reg[F] |= regValue == 0 ? ZeroFlag : 0;
    registers.reg[F] |= bit0 ? Carry : 0;
}

void CPU::rl_reg8 (const Instruction& instruction) {
    uint8_t regValue = registers.reg[instruction.destRegIndex];
    uint8_t bit7 = static_cast<uint8_t>(regValue & 0x80);
    regValue <<= 1;
    regValue |= registers.reg[F] & Carry ? 1 : 0;
    registers.reg[instruction.destRegIndex] = regValue;

    // Affected flags - Zero and Carry flag
    registers.reg[F] &= ~(ZeroFlag | SubtractFlag | HalfCarry | Carry);
    registers.reg[F] |= regValue == 0 ? ZeroFlag : 0;
    registers.reg[F] |= bit7 ? Carry : 0;
}

void CPU::rr_reg8 (const Instruction& instruction) {
    uint8_t regValue = registers.reg[instruction.destRegIndex];
    uint8_t bit0 = static_cast<uint8_t>(regValue & 0x01);
    regValue >>= 1; // right shifting on unsigned should not preserve sign bit - exactly what we want
    regValue |= registers.reg[F] & Carry ? 0x80 : 0;
    registers.reg[instruction.destRegIndex] = regValue;

    // Affected flags - Zero and Carry flag
    registers.reg[F] &= ~(ZeroFlag | SubtractFlag | HalfCarry | Carry);
    registers.reg[F] |= regValue == 0 ? ZeroFlag : 0;
    registers.reg[F] |= bit0 ? Carry : 0;
}

void CPU::stop (const Instruction& instruction) {
    throw runtime_error ("STOP instruction not yet implemented");
}

void CPU::rel_jmp (const Instruction& instruction) {
    // destRegIndex must be PC - TODO: check that global PC increment is ok
    registers.write16(instruction.destRegIndex,
                      (registers.read16(instruction.destRegIndex) +
                      static_cast<int8_t>(mmap.read(static_cast<uint16_t>(registers.pc + 1))))
                      - instruction.cycles // compensate for increment later
    );
}

void CPU::next() {

    // Check interrupts

    // fetch and decode
    const Instruction& instruction = instructions[mmap.read(registers.pc)];

    // Execute
    if (instruction.execfn != nullptr) {
        (this->*instruction.execfn)(instruction);
    }

    // Increment Program counter
    registers.pc += instruction.cycles;

}
