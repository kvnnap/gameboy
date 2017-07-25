//
// Created by kvnna on 23/07/2017.
//

#include "AbstractLR35902CPU.h"
#include "DebugInstruction.h"
#include <cstdio>

using namespace std;
using namespace Gameboy::CPU;

const DebugInstruction Gameboy::CPU::AbstractLR35902CPU::debugInstructions[128] = {

        DebugInstruction("NOP", 1),                 // 0x00 NOP
        DebugInstruction("LD BC, %04X", 3),         // 0x01 LD  BC,  d16
        DebugInstruction("LD (BC), A", 1),          // 0x02 LD (BC),   A
        DebugInstruction("INC BC", 1),              // 0x03 INC BC
        DebugInstruction("INC B", 1),               // 0x04 INC  B // Flags
        DebugInstruction("DEC B", 1),               // 0x05 DEC  B // Flags
        DebugInstruction("LD B, %02X", 2),          // 0x06 LD  B,  d8
        DebugInstruction("RLCA", 1),                // 0x07 RLCA // Flags
        DebugInstruction("LD (%04X), SP", 3, true), // 0x08 LD (a16), SP
        DebugInstruction("ADD HL, BC", 1),          // 0x09 ADD HL, BC // Flags
        DebugInstruction("LD A, (BC)", 1),          // 0x0A LD A, (BC)
        DebugInstruction("DEC BC", 1),              // 0x0B DEC BC
        DebugInstruction("INC C", 1),               // 0x0C INC  C // Flags
        DebugInstruction("DEC C", 1),               // 0x0D DEC  C // Flags
        DebugInstruction("LD C, %02X", 2),          // 0x0E LD  C,  d8
        DebugInstruction("RRCA", 1),                // 0x0F RRCA // Flags

        DebugInstruction("STOP 0", 2),              // 0x10 STOP TODO
        DebugInstruction("LD DE, %04X", 3),         // 0x11 LD  DE,  d16
        DebugInstruction("LD (DE), A", 1),          // 0x12 LD (DE),   A
        DebugInstruction("INC DE", 1),              // 0x13 INC DE
        DebugInstruction("INC D", 1),               // 0x14 INC  D // Flags
        DebugInstruction("DEC D", 1),               // 0x15 DEC  D // Flags
        DebugInstruction("LD D, %02X", 2),          // 0x16 LD  D,  d8
        DebugInstruction("RLA", 1),                 // 0x17 RLA // Flags
        DebugInstruction("JR %02X", 2),             // 0x18 JR r8
        DebugInstruction("ADD HL, DE", 1),          // 0x19 ADD HL, DE // Flags
        DebugInstruction("LD A, (DE)", 1),          // 0x1A LD A, (DE)
        DebugInstruction("DEC DE", 1),              // 0x1B DEC DE
        DebugInstruction("INC E", 1),               // 0x1C INC  E // Flags
        DebugInstruction("DEC E", 1),               // 0x1D DEC  E // Flags
        DebugInstruction("LD E, %02X", 2),          // 0x1E LD  E,  d8
        DebugInstruction("RRA", 1),                 // 0x1F RRA // Flags

        DebugInstruction("JR NZ, %02X", 2),         // 0x20 JR  NZ, r8
        DebugInstruction("LD HL, %04X", 3),         // 0x21 LD  HL, d16
        DebugInstruction("LD (HL+), A", 1),         // 0x22 LD (HL+), A
        DebugInstruction("INC HL", 1),              // 0x23 INC HL
        DebugInstruction("INC H", 1),               // 0x24 INC  H // Flags
        DebugInstruction("DEC H", 1),               // 0x25 DEC  H // Flags
        DebugInstruction("LD H, %02X", 2),          // 0x26 LD  H,  d8
        DebugInstruction("DAA", 1),                 // 0x27 DAA    // Flags TODO: Check
        DebugInstruction("JR Z, %02X", 2),          // 0x28 JR  Z, r8
        DebugInstruction("ADD HL, HL", 1),          // 0x29 ADD HL, HL // Flags
        DebugInstruction("LD A, (HL+)", 1),         // 0x2A LD A, (HL+)
        DebugInstruction("DEC HL", 1),              // 0x2B DEC HL
        DebugInstruction("INC L", 1),               // 0x2C INC  L // Flags
        DebugInstruction("DEC L", 1),               // 0x2D DEC  L // Flags
        DebugInstruction("LD L, %02X", 2),          // 0x2E LD   L,  d8
        DebugInstruction("CPL", 1),                 // 0x2F CPL // Flags

        DebugInstruction("JR NC, %02X", 2),          // 0x30 JR  NC, r8
        DebugInstruction("LD SP, %04X", 3),          // 0x31 LD  SP, d16
        DebugInstruction("LD (HL-), A", 1),          // 0x32 LD (HL-), A
        DebugInstruction("INC SP", 1),               // 0x33 INC SP
        DebugInstruction("INC (HL)", 1),                // 0x34 INC (HL) // Flags
        DebugInstruction("DEC (HL)", 1),                // 0x35 DEC (HL) // Flags
        DebugInstruction("LD (HL), %02X", 2),           // 0x36 LD  (HL), d8
        DebugInstruction("SCF", 1),                  // 0x37 SCF // Flags
        DebugInstruction("JR C, %02X", 2),           // 0x38 JR  C, r8
        DebugInstruction("ADD HL, SP", 1),           // 0x39 ADD HL, SP // Flags
        DebugInstruction("LD A, (HL-)", 1),          // 0x3A LD A, (HL-)
        DebugInstruction("DEC SP", 1),               // 0x3B DEC SP
        DebugInstruction("INC A", 1),                // 0x3C INC  A // Flags
        DebugInstruction("DEC A", 1),                // 0x3D DEC  A // Flags
        DebugInstruction("LD A, %02X", 2),           // 0x3E LD   A,  d8
        DebugInstruction("CCF", 1),                  // 0x3F CCF // Flags

        // C BLOCK
        DebugInstruction("RET NZ", 1),               // 0xC0 RET NZ
        DebugInstruction("POP BC", 1),               // 0xC1 POP BC
        DebugInstruction("JP NZ, %04X", 3, true),    // 0xC2 JP NZ, a16
        DebugInstruction("JP %04X", 3, true),        // 0xC3 JP a16
        DebugInstruction("CALL NZ, %04X", 3, true),  // 0xC4 CALL NZ, a16
        DebugInstruction("PUSH BC", 1),              // 0xC5 PUSH BC
        DebugInstruction("ADD A, %02X", 2),          // 0xC6 ADD A, d8
        DebugInstruction("RST $00", 1),              // 0xC7 RST $00
        DebugInstruction("RET Z", 1),                // 0xC8 RET Z
        DebugInstruction("RET", 1),                  // 0xC9 RET
        DebugInstruction("JP Z, %04X", 3, true),     // 0xCA JP Z, a16
        DebugInstruction("PREFIX CB", 1),            // 0xCB PREFIX CB
        DebugInstruction("CALL Z, %04X", 3, true),   // 0xCC CALL Z, a16
        DebugInstruction("CALL %04X", 3, true),      // 0xCD CALL a16
        DebugInstruction("ADC A, %02X", 2),          // 0xCE ADC A, d8
        DebugInstruction("RST $08", 1),              // 0xCF RST $08

        // D BLOCK
        DebugInstruction("RET NC", 1),               // 0xD0 RET NC
        DebugInstruction("POP DE", 1),               // 0xD1 POP DE
        DebugInstruction("JP NC, %04X", 3, true),    // 0xD2 JP NC, a16
        DebugInstruction("EMPTY", 1),                // 0xD3 NO INSTRUCTION
        DebugInstruction("CALL NC, %04X", 3, true),  // 0xD4 CALL NC, a16
        DebugInstruction("PUSH DE", 1),              // 0xD5 PUSH DE
        DebugInstruction("SUB A, %02X", 2),          // 0xD6 SUB A, d8
        DebugInstruction("RST $10", 1),              // 0xD7 RST $10
        DebugInstruction("RET C", 1),                // 0xD8 RET C
        DebugInstruction("RETI", 1),                 // 0xD9 RETI
        DebugInstruction("JP C, %04X", 3, true),     // 0xDA JP C, a16
        DebugInstruction("EMPTY", 1),                // 0xDB NO INSTRUCTION
        DebugInstruction("CALL C, %04X", 3, true),   // 0xDC CALL C, a16
        DebugInstruction("EMPTY", 1),                // 0xDD NO INSTRUCTION
        DebugInstruction("SBC A, %02X", 2),          // 0xDE SBC A, d8
        DebugInstruction("RST $18", 1),              // 0xDF RST $18

        // E Block
        DebugInstruction("LDH (%02X), A", 2, true),  // 0xE0 LDH (a8), A
        DebugInstruction("POP HL", 1),               // 0xE1 POP HL
        DebugInstruction("LD (C), A", 1),            // 0xE2 LD (C), A //TODO: make sure length is 1
        DebugInstruction("EMPTY", 1),                // 0xE3 NO INSTRUCTION
        DebugInstruction("EMPTY", 1),                // 0xE4 NO INSTRUCTION
        DebugInstruction("PUSH HL", 1),              // 0xE5 PUSH HL
        DebugInstruction("AND A, %02X", 2),          // 0xE6 AND A, d8
        DebugInstruction("RST $20", 1),              // 0xE7 RST $20
        DebugInstruction("ADD SP, %02X", 2),         // 0xE8 ADD SP, r8 // TODO: Check flags if correct
        DebugInstruction("JP (HL)", 1),              // 0xE9 JP (HL)
        DebugInstruction("LD (%04X), A", 3, true),   // 0xEA LD (a16), A
        DebugInstruction("EMPTY", 1),                // 0xEB NO INSTRUCTION
        DebugInstruction("EMPTY", 1),                // 0xEC NO INSTRUCTION
        DebugInstruction("EMPTY", 1),                // 0xED NO INSTRUCTION
        DebugInstruction("XOR %02X", 2),          // 0xEE XOR A, d8
        DebugInstruction("RST $28", 1),              // 0xEF RST $28

        // F BLOCK
        DebugInstruction("LDH A, (%02X)", 2, true),  // 0xF0 LDH A, (a8)
        DebugInstruction("POP AF", 1),               // 0xF1 POP AF
        DebugInstruction("LD (A), C", 1),            // 0xF2 LD A, (C) //TODO: make sure length is 1
        DebugInstruction("DI", 1),                   // 0xF3 DI
        DebugInstruction("EMPTY", 1),                // 0xF4 NO INSTRUCTION
        DebugInstruction("PUSH AF", 1),              // 0xF5 PUSH AF
        DebugInstruction("OR A, %02X", 2),           // 0xF6 OR A, d8
        DebugInstruction("RST $30", 1),              // 0xF7 RST $30
        DebugInstruction("LD HL, SP+%02X", 2),       // 0xF8 LD HL, SP+r8 // TODO: Check flags if correct
        DebugInstruction("LD SP, HL", 1),            // 0xF9 LD SP, HL
        DebugInstruction("LD A, (%04X)", 3, true),   // 0xFA LD A, (a16)
        DebugInstruction("EI", 1),                   // 0xFB EI
        DebugInstruction("EMPTY", 1),                // 0xFC NO INSTRUCTION
        DebugInstruction("EMPTY", 1),                // 0xFD NO INSTRUCTION
        DebugInstruction("CP %02X", 2),              // 0xFE CP  A, d8
        DebugInstruction("RST $38", 1)               // 0xFF RST $38
};

AbstractLR35902CPU::~AbstractLR35902CPU()
{}

AbstractLR35902CPU::AbstractLR35902CPU(Memory::MemoryMappedIO& p_mmap)
    : mmap (p_mmap),
      ticks (),
      interruptMasterEnable ()
{}

const uint8_t Gameboy::CPU::AbstractLR35902CPU::regMap[8] = {B, C, D, E, H, L, HL, A};

// Utils
uint8_t AbstractLR35902CPU::getImmediateValue(uint16_t pc) const {
    return mmap.read(static_cast<uint16_t>(pc + 1));
}

uint16_t AbstractLR35902CPU::getImmediateValue16(uint16_t pc) const {
    return mmap.read16(static_cast<uint16_t>(pc + 1));
}

DebugInstruction AbstractLR35902CPU::fetchAndDecode(uint16_t pc) const {
    // Reset vars
    DebugInstruction debugInstruction;
    bool isCurrentExtended = false;
    char buff[24];

    // Fetch
    debugInstruction.Address = pc;
    uint8_t currentInstruction = mmap.read(pc);

    // Decode
    if (currentInstruction == 0xCB) {
        isCurrentExtended = true;
        currentInstruction = mmap.read(++pc);
        ++pc;
    }

    // Second stage decoding
    const uint8_t rangeNum = static_cast<uint8_t>((currentInstruction >> 6) & 0x03);
    const uint8_t splitRowSelector = static_cast<uint8_t>((currentInstruction >> 3) & 0x07);
    const uint8_t regCpuMapIndex = static_cast<uint8_t>(currentInstruction & 0x07);
    const uint8_t regIndex = regMap[regCpuMapIndex];
    const bool isMemoryOperation = regCpuMapIndex == 0x06;

    string strInstruction;

    debugInstruction.IsExtended = isCurrentExtended;
    debugInstruction.Opcode = currentInstruction;

    if (!isCurrentExtended) {
        if (rangeNum == 1) {
            // Loads
            const uint8_t destRegIndex = regMap[splitRowSelector];
            const bool isDestMemoryOperation = splitRowSelector == 0x06;
            debugInstruction.Length = 1;
            if (isMemoryOperation && isDestMemoryOperation) {
                // HALT
                // debugInstruction.Ticks = 4;
                strInstruction = "HALT";

            } else {
                // debugInstruction.Ticks = static_cast<uint8_t>(isMemoryOperation ? 8 : 4);
                strInstruction = "LD";
                if (isDestMemoryOperation) {
                    strInstruction += " (HL), %s";
                    std::snprintf(buff, sizeof(buff), strInstruction.c_str(),
                                  RegisterIndexToString(static_cast<RegisterIndex>(regIndex)).c_str());
                } else {
                    strInstruction += isMemoryOperation ? "%s, (HL)" : " %s, %s";
                    std::snprintf(buff, sizeof(buff), strInstruction.c_str(),
                                  RegisterIndexToString(static_cast<RegisterIndex>(destRegIndex)).c_str(),
                                  RegisterIndexToString(static_cast<RegisterIndex>(regIndex)).c_str());
                }
            }
        } else if (rangeNum == 2) {
            // Add, ADC, SUB, SBC, AND, XOR, OR, CP
            debugInstruction.Length = 1;
            switch (splitRowSelector) {
                case 0: strInstruction = "ADD A,"; break;
                case 1: strInstruction = "ADC A,"; break;
                case 2: strInstruction = "SUB"; break;
                case 3: strInstruction = "SBC A,"; break;
                case 4: strInstruction = "AND"; break;
                case 5: strInstruction = "XOR"; break;
                case 6: strInstruction = "OR"; break;
                case 7: strInstruction = "CP"; break;
                default: break;
            }
            strInstruction += isMemoryOperation ? " (HL)" : " %s";
            std::snprintf(buff, sizeof(buff), strInstruction.c_str(),
                          RegisterIndexToString(static_cast<RegisterIndex>(regIndex)).c_str());
        } else {
            // Use Map
            const DebugInstruction * const debugInstructionMapElement = &debugInstructions[currentInstruction & 0x7F];
            debugInstruction.Length = debugInstructionMapElement->Length;
            strInstruction = debugInstructionMapElement->StringInstruction;

            switch (debugInstruction.Length) {
                case 0: case 1:
                    snprintf(buff, sizeof(buff), strInstruction.c_str());
                    break;
                case 2:
                    debugInstruction.Operand = getImmediateValue(pc);
                    snprintf(buff, sizeof(buff), strInstruction.c_str(), debugInstruction.Operand);
                    break;
                case 3:
                    debugInstruction.Operand16 = getImmediateValue16(pc);
                    snprintf(buff, sizeof(buff), strInstruction.c_str(), debugInstruction.Operand16);
                    break;
                default: break;
            }
        }
    } else {
        // Extended
        debugInstruction.Length = 2;
        // debugInstruction.Ticks = static_cast<uint8_t>(isMemoryOperation ? 16 : 8);
        if (rangeNum == 0) {
            switch (splitRowSelector) {
                case 0: strInstruction = "RLC"; break;
                case 1: strInstruction = "RRC"; break;
                case 2: strInstruction = "RL"; break;
                case 3: strInstruction = "RR"; break;
                case 4: strInstruction = "SLA"; break;
                case 5: strInstruction = "SRA"; break;
                case 6: strInstruction = "SWAP"; break;
                case 7: strInstruction = "SRL"; break;
                default: break;
            }
            strInstruction += isMemoryOperation ? " (HL)" : " %s";
            std::snprintf(buff, sizeof(buff), strInstruction.c_str(),
                    RegisterIndexToString(static_cast<RegisterIndex>(regIndex)).c_str());
        } else {
            switch(rangeNum) {
                case 1: strInstruction = "BIT"; break;
                case 2: strInstruction = "RES"; break;
                case 3: strInstruction = "SET"; break;
                default: break;
            }
            strInstruction += isMemoryOperation ? " %u, (HL)" : " %u, %s";
            std::snprintf(buff, sizeof(buff), strInstruction.c_str(), splitRowSelector,
                    RegisterIndexToString(static_cast<RegisterIndex>(regIndex)).c_str());
        }
    }

    debugInstruction.StringInstruction = string(buff);

    return debugInstruction;
}

vector<DebugInstruction> AbstractLR35902CPU::disassemble(std::size_t num) const {
    //
    uint16_t currentPC = registers.read16(PC);

    vector<DebugInstruction> debugInstructions;
    debugInstructions.reserve(num);

    for (size_t i = 0; i < num; ++i) {
        DebugInstruction debugInstruction = fetchAndDecode(currentPC);
        if (((uint32_t)currentPC + debugInstruction.Length) > 0xFFFF) {
            break;
        }
        debugInstructions.push_back(debugInstruction);
        currentPC += debugInstruction.Length;
    }

    return debugInstructions;
}

DebugInstruction AbstractLR35902CPU::disassembleNext() const {
    return fetchAndDecode(registers.read16(PC));
}

Registers &AbstractLR35902CPU::getRegisters() {
    return registers;
}

bool AbstractLR35902CPU::isInterruptMasterEnabled() const {
    return interruptMasterEnable;
}
