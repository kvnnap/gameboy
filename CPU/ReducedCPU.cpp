//
// Created by Kevin on 11/02/2017.
//

#include <stdexcept>

#include "ReducedCPU.h"

using namespace std;
using namespace Gameboy::CPU;
using namespace Gameboy::General;

using namespace Gameboy::CPU;

const uint8_t Gameboy::CPU::ReducedCPU::regMap[8] = {B, C, D, E, H, L, HL, A};
const uint8_t Gameboy::CPU::ReducedCPU::reg16Map[4] = {BC, DE, HL, SP};
const uint8_t Gameboy::CPU::ReducedCPU::reg16MapLdInc[4] = {BC, DE, HL, HL};
const uint8_t Gameboy::CPU::ReducedCPU::reg16MapPopPush[4] = {BC, DE, HL, AF};

const ReducedInstruction Gameboy::CPU::ReducedCPU::instructions[16] = {
        {&ReducedCPU::brancher_nop_stop_jmp_ld, nullptr},               // 0xX0
        {&ReducedCPU::load_d16_to_reg, &reg16Map},                      // 0xX1
        {&ReducedCPU::load_reg8_to_regpt_vv_inc_dec, &reg16MapLdInc},   // 0xX2
        {&ReducedCPU::inc_dec_reg16, &reg16Map},                        // 0xX3
        {&ReducedCPU::inc_dec_reg8_or_regpt, nullptr},                  // 0xX4
        {&ReducedCPU::inc_dec_reg8_or_regpt, nullptr},                  // 0xX5
        {&ReducedCPU::load_d8_to_reg8_or_regpt, nullptr},               // 0xX6
        {&ReducedCPU::brancher_r_l_r_c_cpl_c_daa_scf, nullptr},         // 0xX7
        {&ReducedCPU::brancher_nop_stop_jmp_ld, nullptr},               // 0xX8
        {&ReducedCPU::add_reg16_to_reg16_HL, &reg16Map},                // 0xX9
        {&ReducedCPU::load_reg8_to_regpt_vv_inc_dec, &reg16MapLdInc},   // 0xXA
        {&ReducedCPU::inc_dec_reg16, &reg16Map},                        // 0xXB
        {&ReducedCPU::inc_dec_reg8_or_regpt, nullptr},                  // 0xXC
        {&ReducedCPU::inc_dec_reg8_or_regpt, nullptr},                  // 0xXD
        {&ReducedCPU::load_d8_to_reg8_or_regpt, nullptr},               // 0xXE
        {&ReducedCPU::brancher_r_l_r_c_cpl_c_daa_scf, nullptr}          // 0xXF
};

const ReducedInstruction Gameboy::CPU::ReducedCPU::instructions2[16] = {
        {&ReducedCPU::brancher_ret_ld_ldh_add, nullptr},    // 0xX0
        {&ReducedCPU::push_pop, &reg16MapPopPush},          // 0xX1
        {&ReducedCPU::brancher_jmp_load, nullptr},          // 0xX2
        {nullptr, nullptr},                                 // 0xX3
        {nullptr, nullptr},                                 // 0xX4
        {&ReducedCPU::push_pop, &reg16MapPopPush},          // 0xX5
        {nullptr, nullptr},                                 // 0xX6
        {&ReducedCPU::rst, nullptr},                        // 0xX7
        {&ReducedCPU::brancher_ret_ld_ldh_add, nullptr},    // 0xX8
        {nullptr, nullptr},                                 // 0xX9
        {&ReducedCPU::brancher_jmp_load, nullptr},          // 0xXA
        {nullptr, nullptr},                                 // 0xXB
        {nullptr, nullptr},                                 // 0xXC
        {nullptr, nullptr},                                 // 0xXD
        {nullptr, nullptr},                                 // 0xXE
        {&ReducedCPU::rst, nullptr}                         // 0xXF
};

ReducedCPU::ReducedCPU(Memory::MemoryMappedIO &p_mmap)
    : mmap ( p_mmap ),
      currentPC (),
      currentInstruction (), //
      isCurrentExtended (), //
      interruptMasterEnable (),
      ticks ()
{

}

uint32_t ReducedCPU::getTicks() const {
    return ticks;
}

void ReducedCPU::requestInterrupt(uint8_t irqLine) {
    mmap.write(0xFF0F, static_cast<uint8_t>(mmap.read(0xFF0F) | (1 << irqLine)));
}

void ReducedCPU::next() {

    // Reset vars
    isCurrentExtended = false;

    // Fetch
    currentPC = registers.read16(PC);
    currentInstruction = mmap.read(currentPC);

    // Decode Extended
    if (currentInstruction == 0xCB) {
        isCurrentExtended = true;
        registers.write16(PC, static_cast<uint16_t>(currentPC + 1));
        currentPC = registers.read16(PC);
        currentInstruction = mmap.read(currentPC);
        registers.write16(PC, static_cast<uint16_t>(currentPC + 1));
        // read next instruction
    }

    // Decode and Execute
    const uint8_t rangeNum = static_cast<uint8_t>((currentInstruction >> 6) & 0x03);
    const uint8_t regCpuMapIndex = static_cast<uint8_t>(currentInstruction & 0x07);
    const uint8_t regIndex = regMap[regCpuMapIndex];
    const bool isMemoryOperation = regCpuMapIndex == 0x06;

    if (!isCurrentExtended) {
        // TODO: Implement

        splitRowSelector = static_cast<uint8_t>((currentInstruction >> 3) & 0x07);

        switch (rangeNum) {
            case 0:
            {
                rowSelector = splitRowSelector >> 1;
                const ReducedInstruction * instruction = &instructions[currentInstruction & 0x0F];
                ticks += (this->*instruction->execfn)(*instruction);
            }
                break;
            // Loads
            case 1:
            {
                // Decode
                const uint8_t destRegIndex = regMap[splitRowSelector];
                const bool isDestMemoryOperation = splitRowSelector == 0x06;

                // Increment Program Counter by 1 (Loads are byte long)
                registers.write16(PC, static_cast<uint16_t>(currentPC + 1));

                // Further decode and execute Load
                if (isDestMemoryOperation) {
                    if (isMemoryOperation) {
                        // HALT - 0x76
                        ticks += 4;
                        throw runtime_error("HALT instruction not yet implemented");
                    } else {
                        mmap.write(registers.read16(destRegIndex), registers.reg[regIndex]);
                    }
                } else {
                    registers.reg[destRegIndex] = isMemoryOperation ? mmap.read(registers.read16(regIndex))
                                                                    : registers.reg[regIndex];
                }

                // ticks for loads
                ticks += isMemoryOperation || isDestMemoryOperation ? 8 : 4;
            }

                break;
            case 2:
            {
                // Increment Program Counter by 1
                registers.write16(PC, static_cast<uint16_t>(currentPC + 1));

                // ADD, ADC, SUB, SBC, AND, XOR, OR, CP
                rowSelector = splitRowSelector >> 1;
                const bool carryOrDualOperation = (splitRowSelector & 0x01) == 0x01;
                const uint8_t readValue = isMemoryOperation ? mmap.read(registers.read16(regIndex))
                                                            : registers.reg[regIndex];

                // In this block, rowSelector only used in switch
                switch (rowSelector) {
                    case 0: // ADD/ADC
                        add_val8_to_reg8_nc_c(readValue, carryOrDualOperation);
                        break;
                    case 1: // SUB/SBC
                        sub_val8_from_reg8_nc_c(readValue, carryOrDualOperation);
                        break;
                    case 2: // AND/XOR
                        if (carryOrDualOperation) {
                            // XOR
                            xor_val8_to_reg8(readValue);
                        } else {
                            // AND
                            and_val8_to_reg8(readValue);
                        }
                        break;
                    case 3: // OR/CP
                        if (carryOrDualOperation) {
                            // CP
                            cp_val8_to_reg8(readValue);
                        } else {
                            // OR
                            or_val8_to_reg8(readValue);
                        }
                        break;
                    default:
                        throw runtime_error("Code that should never be reached was reached");
                }

                // ticks for loads
                ticks += isMemoryOperation ? 8 : 4;
            }
                break;
            case 3:
            {
                rowSelector = splitRowSelector >> 1;
                const ReducedInstruction * instruction = &instructions2[currentInstruction & 0x0F];
                ticks += (this->*instruction->execfn)(*instruction);
            }
                break;
            default:
                throw runtime_error ("instruction not yet implemented");
                break;
        }

        // Increment Program Counter before executing instruction
        //registers.write16(PC, static_cast<uint16_t>(currentPC + 0));
        // Execute
        //ticks += 0;
    } else {
        // Extended (CB) instructions - rangeNum might be more expensive

        // Prepare variables for memory operation, read data and address
        uint16_t address;
        //union { // TODO: add union when not debugging
        uint8_t readValue;
        uint8_t writeValue;
        //};

        if (isMemoryOperation) {
            address = registers.read16(regIndex);
            readValue = mmap.read(address);
        } else {
            readValue = registers.reg[regIndex];
        }

        // Calculate operation
        if (rangeNum == 0) {
            if (currentInstruction & 0x20) {
                // 0x20 - 0x3F - shift left, right, right logical, swap
                writeValue = s_l_r_a_l_val8(readValue);
            } else {
                // 0x00 - 0x1F - Rotates
                writeValue = r_l_r_c_val8(readValue);
            }
        } else {
            const uint8_t bitNumFlag = static_cast<uint8_t>(1 << ((currentInstruction >> 3) & 0x07));
            if (rangeNum == 1) { // BIT instructions
                // Reset flags
                registers.reg[F] &= ~(ZeroFlag | SubtractFlag);
                registers.reg[F] |= HalfCarryFlag;
                if ((readValue & bitNumFlag) == 0) {
                    registers.reg[F] |= ZeroFlag;
                }
            } else { // RES and SET instructions
                writeValue = rangeNum == 2 ? readValue & ~bitNumFlag : readValue | bitNumFlag;
            }
        }

        // Write value if necessary
        if (rangeNum != 1) {
            if (isMemoryOperation) {
                mmap.write(address, writeValue);
            } else {
                registers.reg[regIndex] = writeValue;
            }
        }

        ticks += isMemoryOperation ? 16 : 8;
    }

    // Check interrupts
    if (interruptMasterEnable) {
        // Read interrupt enable register
        uint8_t ier = mmap.read(0xFFFF);
        uint8_t ifr;
        if (ier != 0) {
            ifr = mmap.read(0xFF0F);
            if (ifr != 0) {
                // Some interrupt was raised, handle the interrupt
                for (size_t i = 0; i < 5; ++i) {
                    const std::uint8_t bitFlag = static_cast<uint8_t>(1 << i);
                    if ((ier & bitFlag) && (ifr & bitFlag)) {
                        // handle interrupt i, reset IF flag
                        mmap.write(0xFF0F, static_cast<uint8_t>(ifr & ~bitFlag));
                        interruptMasterEnable = false;
                        // Read stack pointer
                        uint16_t stackPointer = registers.read16(SP);
                        // Copy to stack
                        mmap.write16(stackPointer -= 2, registers.read16(PC));
                        // decrement stack pointer
                        registers.write16(SP, stackPointer);
                        // jump to address
                        registers.write16(PC, static_cast<uint16_t>(0x0040 | (i << 3)));
                        // Some time must have passed - undocumented
                        // https://github.com/sinamas/gambatte/blob/master/libgambatte/src/interrupter.cpp
                        // probably 20 cycles or 24?
                        ticks += 20;
                        break;
                    }
                }
            }
        }
    }
}

// Instructions

// 8-bit ADD/ADC
void ReducedCPU::add_val8_to_reg8_nc_c(uint8_t srcValue, bool carry) {
    uint8_t destRegValue = registers.reg[A];
    uint8_t carryValue = 0;
    uint8_t result = srcValue + destRegValue;
    if (carry && (registers.reg[F] & CarryFlag)) {
        result += ++carryValue;
    }
    registers.reg[A] = result;

    // update flags
    registers.reg[F] &= ~(ZeroFlag | SubtractFlag | HalfCarryFlag | CarryFlag);
    if (result == 0) {
        registers.reg[F] |= ZeroFlag;
    }
    if (((srcValue & 0x0F) + (destRegValue & 0x0F) + carryValue) & 0x10) {
        registers.reg[F] |= HalfCarryFlag;
    }
    if ((static_cast<uint16_t>(srcValue) + destRegValue + carryValue) & 0x100) {
        registers.reg[F] |= CarryFlag;
    }
}

// 8-bit SUB/SBC
void ReducedCPU::sub_val8_from_reg8_nc_c(uint8_t srcValue, bool carry) {
    uint8_t destRegValue = registers.reg[A];
    uint8_t carryValue = 0;
    uint8_t result = destRegValue - srcValue;
    if (carry && (registers.reg[F] & CarryFlag)) {
        result -= ++carryValue;
    }
    registers.reg[A] = result;

    // update flags
    registers.reg[F] &= ~(ZeroFlag | HalfCarryFlag | CarryFlag);
    registers.reg[F] |= SubtractFlag;
    if (result == 0) {
        registers.reg[F] |= ZeroFlag;
    }
    if (((destRegValue & 0x0F) < ((srcValue & 0x0F) + carryValue))) {
        registers.reg[F] |= HalfCarryFlag;
    }
    if (destRegValue < (static_cast<uint16_t>(srcValue) + carryValue)) {
        registers.reg[F] |= CarryFlag;
    }
}

// 8-bit AND
void ReducedCPU::and_val8_to_reg8(uint8_t srcValue) {
    registers.reg[F] &= ~(ZeroFlag | SubtractFlag | CarryFlag);
    registers.reg[F] |= HalfCarryFlag;
    if ((registers.reg[A] &= srcValue) == 0) {
        registers.reg[F] |= ZeroFlag;
    }
}

// 8-bit XOR
void ReducedCPU::xor_val8_to_reg8(uint8_t srcValue) {
    registers.reg[F] &= ~(ZeroFlag | SubtractFlag | HalfCarryFlag | CarryFlag);
    if ((registers.reg[A] ^= srcValue) == 0) {
        registers.reg[F] |= ZeroFlag;
    }
}

// 8-bit OR
void ReducedCPU::or_val8_to_reg8(uint8_t srcValue) {
    registers.reg[F] &= ~(ZeroFlag | SubtractFlag | HalfCarryFlag | CarryFlag);
    if ((registers.reg[A] |= srcValue) == 0) {
        registers.reg[F] |= ZeroFlag;
    }
}

// 8-bit CP
void ReducedCPU::cp_val8_to_reg8(uint8_t srcValue) {
    const uint8_t destRegValue = registers.reg[A];

    // update flags
    registers.reg[F] &= ~(ZeroFlag | HalfCarryFlag | CarryFlag);
    registers.reg[F] |= SubtractFlag;
    if (destRegValue == srcValue) {
        registers.reg[F] |= ZeroFlag;
    }
    if (((destRegValue & 0x0F) < ((srcValue & 0x0F)))) {
        registers.reg[F] |= HalfCarryFlag;
    }
    if (destRegValue < (static_cast<uint16_t>(srcValue))) {
        registers.reg[F] |= CarryFlag;
    }
}

// Extended - Shifts
uint8_t ReducedCPU::s_l_r_a_l_val8(uint8_t val) {
    // Derive whether to carry from currentInstruction
    const bool up = !(currentInstruction & 0x10); // arithmetic are up/ logical are down with swap
    const bool left = !(currentInstruction & 0x08); //right are all shifts/ left could be swap

    // Affected flags
    registers.reg[F] &= ~(ZeroFlag | SubtractFlag | HalfCarryFlag | CarryFlag);

    if (left) {
        // Left arithmetic shift + Swap
        if (up) {
            // left arithmetic shift
            const uint8_t msbSet = static_cast<uint8_t>(val & 0x80);
            if (msbSet) {
                registers.reg[F] = CarryFlag;
            }
            val <<= 1;
        } else {
            // swap
            val = static_cast<uint8_t>((val << 4) | (val >> 4));
        }

    } else {
        // right logical
        const uint8_t msbSet = static_cast<uint8_t>(val & 0x80);
        if (val & 0x01) {
            registers.reg[F] = CarryFlag;
        }
        val >>= 1;
        // right arithmetic is same as logical but preserves MSB
        if (!up && msbSet) {
            val |= msbSet;
        }
    }

    if (val == 0) { // only executed by the extended instructions
        registers.reg[F] |= ZeroFlag;
    }

    return val;
}

// Extended - Rotates
uint8_t ReducedCPU::r_l_r_c_val8(uint8_t val) {
    // Derive whether to carry from currentInstruction
    const bool carry = !(currentInstruction & 0x10); // same as (currentInstruction & 0x10) == 0
    const bool left = !(currentInstruction & 0x08);
    bool lsbMsbSet; //lsb or msb bit set
    if (left) {
        lsbMsbSet = (val & 0x80) != 0; //MSB
        val <<= 1;
        val |= carry ? (lsbMsbSet ? 0x01 : 0) : (registers.reg[F] & CarryFlag ? 0x01 : 0);
    } else {
        lsbMsbSet = (val & 0x01) != 0; //LSB
        val >>= 1;
        val |= carry ? (lsbMsbSet ? 0x80 : 0) : (registers.reg[F] & CarryFlag ? 0x80 : 0);
    }
    // Affected flags - Zero and Carry flag
    registers.reg[F] &= ~(ZeroFlag | SubtractFlag | HalfCarryFlag | CarryFlag);
    if (lsbMsbSet) {
        registers.reg[F] |= CarryFlag;
    }
    if (isCurrentExtended && val == 0) { // only executed by the extended instructions
        registers.reg[F] |= ZeroFlag;
    }
    return val;
}

void ReducedCPU::incrementProgramCounterBy(uint16_t incrementAmount) {
    registers.write16(PC, currentPC + incrementAmount);
}

// Load 16-bit immediate to 16-bit Reg
uint8_t ReducedCPU::load_d16_to_reg(const ReducedInstruction &instruction) {
    incrementProgramCounterBy(3);
    registers.write16(instruction.getRegisterIndex(rowSelector), mmap.read16(static_cast<uint16_t>(currentPC + 1)));
    return 12;
}

uint8_t ReducedCPU::load_reg8_to_regpt_vv_inc_dec(const ReducedInstruction &instruction) {
    incrementProgramCounterBy(1);

    // determine register containing destination pointer - BC, DE, HL+, HL-
    const uint8_t reg16Index = instruction.getRegisterIndex(rowSelector);
    uint16_t regAddress = registers.read16(reg16Index);

    if (currentInstruction & 0x08) { // LD A, (xx)
        registers.reg[A] = mmap.read(regAddress);
    } else { // LD (xx), A
        mmap.write(regAddress, registers.reg[A]);
    }

    // With Increment/Dec
    if (rowSelector & 0x02) {
        registers.write16(reg16Index, rowSelector == 2 ? ++regAddress : --regAddress);
    }

    return 8;
}

uint8_t ReducedCPU::inc_dec_reg16(const ReducedInstruction &instruction) {
    incrementProgramCounterBy(1);
    const uint8_t regIndex = instruction.getRegisterIndex(rowSelector);
    uint16_t regValue = registers.read16(regIndex);
    registers.write16(regIndex, currentInstruction & 0x08 ? --regValue : ++regValue);
    return 8;
}

uint8_t ReducedCPU::inc_dec_reg8_or_regpt(const ReducedInstruction &instruction) {
    incrementProgramCounterBy(1);
    const uint8_t regIndex = regMap[splitRowSelector];
    const bool isSubtract = (currentInstruction & 0x01) != 0;

    uint8_t valueAfter, ticks;
    if (splitRowSelector == 0x06) { // 0x34 or 0x35
        const uint16_t memLocation = registers.read16(regIndex);
        valueAfter = mmap.read(memLocation);
        if (isSubtract) {
            --valueAfter;
        } else {
            ++valueAfter;
        }
        mmap.write16(memLocation, valueAfter);
        ticks = 12;
    } else {
        valueAfter = isSubtract ? --registers.reg[regIndex] : ++registers.reg[regIndex];
        ticks = 4;
    }

    // Process flags - Affects Zero flag, Subtract flag and Half Carry flag
    registers.reg[F] &= ~(ZeroFlag | SubtractFlag | HalfCarryFlag);
    if (valueAfter == 0) {
        registers.reg[F] |= ZeroFlag;
    }
    const uint8_t lowValueNibble = valueAfter & static_cast<uint8_t>(0x0F);
    if (isSubtract) {
        registers.reg[F] |= SubtractFlag;
        if (lowValueNibble == 0x0F) {
            registers.reg[F] |= HalfCarryFlag;
        }
    } else if (lowValueNibble == 0) {
        registers.reg[F] |= HalfCarryFlag;
    }

    return ticks;
}

std::uint8_t ReducedCPU::load_d8_to_reg8_or_regpt(const ReducedInstruction &instruction) {
    incrementProgramCounterBy(2);
    const uint8_t regIndex = regMap[splitRowSelector];
    const uint8_t immediateValue = getImmediateValue();
    // LD X, d8 - 8-bit immediate
    if (splitRowSelector == 0x06) {
        mmap.write(registers.read16(regIndex), immediateValue);
        return 12;
    } else {
        registers.reg[regIndex] = immediateValue;
        return 8;
    }
}

// JR X,r8
uint8_t ReducedCPU::rel_cond_jmp() {
    incrementProgramCounterBy(2);
    bool shouldJump = true;

    // Check if conditional jump
    if (rowSelector & 0x02) {
        shouldJump = getShouldJump(rowSelector == 2 ? ZeroFlag : CarryFlag);
    }

    if (shouldJump) {
        registers.write16(PC, registers.read16(PC) + static_cast<int8_t>(getImmediateValue()));
        return 12;
    } else {
        return 8;
    }
}

std::uint8_t ReducedCPU::cond_jmp() {
    incrementProgramCounterBy(3);
    bool shouldJump = true;

    // Check if conditional jump
    if ((currentInstruction & 0x01) == 0) {
        shouldJump = getShouldJump(rowSelector == 0 ? ZeroFlag : CarryFlag);
    }

    if (shouldJump) {
        registers.write16(PC, mmap.read16(static_cast<uint16_t>(currentPC + 1)));
        return 16;
    } else {
        return 12;
    }
}

std::uint8_t ReducedCPU::getImmediateValue() const {
    return mmap.read(static_cast<uint16_t>(currentPC + 1));
}

std::uint8_t ReducedCPU::brancher_nop_stop_jmp_ld(const ReducedInstruction &instruction) {
    if (rowSelector & 0x02) {
        return rel_cond_jmp(); // JR conditional
    } else {
        const bool firstRow = rowSelector == 0;
        if ((currentInstruction & 0x08) == 0) { // NOP, STOP
            if (firstRow) {
                incrementProgramCounterBy(1);
            } else {
                incrementProgramCounterBy(2);
                throw runtime_error ("STOP instruction not yet implemented");
            }
            return 4;
        } else { // LD, Jmp
            if (firstRow) { // LD
                return load_reg16_to_d16pt();
            } else {
                return rel_cond_jmp(); // JR Unconditional
            }
        }
    }
}

// LD (a16),SP
std::uint8_t ReducedCPU::load_reg16_to_d16pt() {
    incrementProgramCounterBy(3);
    mmap.write16(mmap.read16(static_cast<uint16_t>(currentPC + 1)), registers.read16(SP));
    return 20;
}

std::uint8_t ReducedCPU::add_reg16_to_reg16_HL(const ReducedInstruction &instruction) {
    incrementProgramCounterBy(1);
    const uint16_t srcReg  = registers.read16(instruction.getRegisterIndex(rowSelector));
    const uint16_t destReg = registers.read16(HL);
    registers.write16(HL, srcReg + destReg);

    // Affected flags - Half Carry flag and Carry flag
    registers.reg[F] &= ~(SubtractFlag | HalfCarryFlag | CarryFlag);
    // set if carry from bit 11 0x0FFF
    if (0x1000 & ((destReg & 0x0FFF) + (srcReg & 0x0FFF))) {
        registers.reg[F] |= HalfCarryFlag;
    }
    // set if carry from bit 15
    if (0x10000 & (static_cast<uint32_t>(destReg) + srcReg)) {
        registers.reg[F] |= CarryFlag;
    }
    return 8;
}

std::uint8_t ReducedCPU::brancher_r_l_r_c_cpl_c_daa_scf(const ReducedInstruction &instruction) {
    incrementProgramCounterBy(1);
    if (rowSelector & 0x02) {
        if ((currentInstruction & 0x08) == 0) { // DAA, SCF
            if (rowSelector == 2) { // DAA
                // Reset all affected flags and set as necessary ones
                registers.reg[F] &= ~(ZeroFlag | HalfCarryFlag | CarryFlag);

                uint8_t regValue = registers.reg[A];
                uint8_t low = static_cast<uint8_t>(regValue & 0x0F);
                if ((registers.reg[F] & HalfCarryFlag) || low > 9) {
                    if (registers.reg[F] & SubtractFlag) {
                        regValue -= 0x06;
                    } else {
                        regValue += 0x06;
                    }
                }
                uint8_t high = static_cast<uint8_t>((regValue >> 4) & 0x0F);
                if ((registers.reg[F] & CarryFlag) || high > 9) {
                    if (registers.reg[F] & SubtractFlag) {
                        regValue -= 0x60;
                    } else {
                        regValue += 0x60;
                    }
                    registers.reg[F] |= CarryFlag; // Set carry flag
                }

                if (regValue == 0) {
                    registers.reg[F] |=  ZeroFlag;
                }
            } else { // SCF
                registers.reg[F] &= ~(SubtractFlag | HalfCarryFlag);
                registers.reg[F] |= CarryFlag;
            }
        } else { // CPL, CCF
            if (rowSelector == 2) { // CPL
                registers.reg[A] = ~registers.reg[A];
                registers.reg[F] |= (SubtractFlag | HalfCarryFlag);
            } else { // CCF
                registers.reg[F] &= ~(SubtractFlag | HalfCarryFlag);
                registers.reg[F] ^= CarryFlag;
            }
        }
    } else {
        registers.reg[A] = r_l_r_c_val8(registers.reg[A]);
    }
    return 4;
}

std::uint8_t ReducedCPU::push_pop(const ReducedInstruction &instruction) {
    incrementProgramCounterBy(1);
    // Read stack pointer - src has SP
    uint16_t stackPointer = registers.read16(SP);
    const uint8_t registerIndex = instruction.getRegisterIndex(rowSelector);

    if (currentInstruction & 0x04) { // Push
        // Copy to stack
        mmap.write16(stackPointer -= 2, registers.read16(registerIndex));
    } else { // Pop
        // Copy to register - read two bytes from stack and assign them to destination register
        registers.write16(registerIndex, mmap.read16(stackPointer));
        // Increment stack pointer
        stackPointer += 2;
    }

    registers.write16(SP, stackPointer);
    return 12;
}

std::uint8_t ReducedCPU::rst(const ReducedInstruction &instruction) {
    incrementProgramCounterBy(1);
    // Copy to stack
    uint16_t stackPointer = registers.read16(SP);
    mmap.write16(stackPointer -= 2, registers.read16(PC));
    registers.write16(SP, stackPointer);
    // dynamically calculate from opcode
    registers.write16(PC, static_cast<uint16_t>(currentInstruction & 0x38));
    return 16;
}

// could separate these in two, but want to keep compact for debugging
std::uint8_t ReducedCPU::load_reg8_to_reg8pt_vv() {
    if ((currentInstruction & 0x08) == 0) { // LD A,(C) has alternative mnemonic LD A,($FF00+C), LD C,(A) has alternative mnemonic LD ($FF00+C),A
        incrementProgramCounterBy(2);
        if (rowSelector == 2) {
            mmap.write(static_cast<uint16_t>(0xFF00 | mmap.read(registers.reg[C])), registers.reg[A]);
        } else {
            registers.reg[A] = mmap.read(static_cast<uint16_t>(0xFF00 | registers.reg[C]));
        }
        return 8;
    } else {
        incrementProgramCounterBy(3);
        const uint16_t address = mmap.read16(static_cast<uint16_t>(currentPC + 1));
        if (rowSelector == 2) {
            mmap.write(address, registers.reg[A]);
        } else {
            registers.reg[A] = mmap.read(address);
        }
        return 16;
    }
}

std::uint8_t ReducedCPU::brancher_jmp_load(const ReducedInstruction &instruction) {
    return rowSelector & 0x02 ? load_reg8_to_reg8pt_vv() : cond_jmp();
}

std::uint8_t ReducedCPU::brancher_ret_ld_ldh_add(const ReducedInstruction &instruction) {
    return rowSelector & 0x02 ? load_reg8_to_d8_pt_vv() : cond_ret();
}

std::uint8_t ReducedCPU::cond_ret() {
    // jump
    incrementProgramCounterBy(1);
    bool shouldJump = true;

    // Check if conditional jump
    if ((currentInstruction & 0x01) == 0) {
        shouldJump = getShouldJump(rowSelector == 0 ? ZeroFlag : CarryFlag);
    }

    if (shouldJump) {
        // Basically pop
        // Read stack pointer - src has SP
        uint16_t stackPointer = registers.read16(SP);
        // Copy to register - read two bytes from stack and assign them to destination register
        registers.write16(PC, mmap.read16(stackPointer));
        // Increment stack pointer
        stackPointer += 2;
        registers.write16(SP, stackPointer);

        return 20;
    } else {
        return 8;
    }
}

bool ReducedCPU::getShouldJump(FlagRegister flag) const {
    const bool notZeroOrNotCarry = (currentInstruction & 0x08) == 0;
    return notZeroOrNotCarry != ((registers.reg[F] & flag) != 0);
}

std::uint8_t ReducedCPU::load_reg8_to_d8_pt_vv() {
    incrementProgramCounterBy(2);
    uint8_t count = 12;
    if (splitRowSelector & 0x01) { // LD HL, SP+r8
        uint16_t temp = registers.read16(SP);
        int8_t srcValToAdd = static_cast<int8_t>(mmap.read(static_cast<uint16_t>(currentPC + 1)));
        if (rowSelector & 0x01) {
            registers.write16(HL, temp + srcValToAdd);
        } else {
            // AKA ADD SP, r8
            registers.write16(SP, temp + srcValToAdd);
            count = 16;
        }

        // Affected flags - Half Carry flag and Carry flag
        registers.reg[F] &= ~(ZeroFlag | SubtractFlag | HalfCarryFlag | CarryFlag);
        // set if carry from bit 11 0x0FFF
        if (0x1000 & ((temp & 0x0FFF) + srcValToAdd)) {
            registers.reg[F] |= HalfCarryFlag;
        }
        // set if carry from bit 15
        if (0x10000 & (static_cast<uint32_t>(temp) + srcValToAdd)) {
            registers.reg[F] |= CarryFlag;
        }
    } else {
        if (rowSelector & 0x01) {
            registers.reg[A] =
                    mmap.read(static_cast<uint16_t>(0xFF00 | mmap.read(static_cast<uint16_t>(currentPC + 1))));
        } else {
            mmap.write(static_cast<uint16_t>(0xFF00 | mmap.read(static_cast<uint16_t>(currentPC + 1))),
                       registers.reg[A]);
        }
    }
    return count;
}

