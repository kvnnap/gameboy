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
    if (!isCurrentExtended) {
        // TODO: Implement
        // Increment Program Counter before executing instruction
        registers.write16(PC, static_cast<uint16_t>(currentPC + 0));
        // Execute
        ticks += 0;
    } else {
        // Extended (CB) instructions - rangeNum might be more expensive
        const uint8_t rangeNum = static_cast<uint8_t>((currentInstruction >> 6) & 0x03);
        const uint8_t regCpuMapIndex = static_cast<uint8_t>(currentInstruction & 0x07);
        const uint8_t regIndex = regMap[regCpuMapIndex];
        const bool isMemoryOperation = regCpuMapIndex == 0x06;

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
