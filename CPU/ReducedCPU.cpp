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

        const uint8_t destCpuMapIndex = static_cast<uint8_t>((currentInstruction >> 3) & 0x07);

        // Loads
        if (rangeNum == 1) {

            // Decode
            const uint8_t destRegIndex = regMap[destCpuMapIndex];
            const bool isDestMemoryOperation = destCpuMapIndex == 0x06;

            // Increment Program Counter by 1 (Loads are byte long)
            registers.write16(PC, static_cast<uint16_t>(currentPC + 1));

            // Further decode and execute Load
            if (isDestMemoryOperation) {
                if (isMemoryOperation) {
                    // HALT - 0x76
                    ticks += 4;
                    throw runtime_error ("HALT instruction not yet implemented");
                } else {
                    mmap.write(registers.read16(destRegIndex), registers.reg[regIndex]);
                }
            } else {
                registers.reg[destRegIndex] = isMemoryOperation ? mmap.read(registers.read16(regIndex))
                                                                : registers.reg[regIndex];
            }

            // ticks for loads
            ticks += isMemoryOperation || isDestMemoryOperation ? 8 : 4;

        } else if (rangeNum == 2) {

            // Increment Program Counter by 1
            registers.write16(PC, static_cast<uint16_t>(currentPC + 1));

            // ADD, ADC, SUB, SBC, AND, XOR, OR, CP
            const uint8_t rowSelector = destCpuMapIndex >> 1;
            const bool carryOrDualOperation = (destCpuMapIndex & 0x01) == 0x01;
            const uint8_t readValue = isMemoryOperation ? mmap.read(registers.read16(regIndex)) : registers.reg[regIndex];

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
                    throw runtime_error ("Code that should never be reached was reached");
            }

            // ticks for loads
            ticks += isMemoryOperation ? 8 : 4;

        } else {
            throw runtime_error ("instruction not yet implemented");
        }

        // Increment Program Counter before executing instruction
        registers.write16(PC, static_cast<uint16_t>(currentPC + 0));
        // Execute
        ticks += 0;
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
