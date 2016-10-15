//
// Created by kvnna on 09/10/2016.
//

#include <stdexcept>

#include "CPU.h"

using namespace std;
using namespace Gameboy::CPU;
using namespace Gameboy::General;

const Instruction Gameboy::CPU::CPU::instructions[256] = {
        {1,  4,  0,  0,  0, nullptr},                     // 0x00 NOP
        {3, 12,  0, BC,  0, &CPU::load_d16_to_reg},       // 0x01 LD  BC,  d16
        {1,  8,  0, BC,  A, &CPU::load_reg8_to_regpt},    // 0x02 LD (BC),   A
        {1,  8,  0, BC, BC, &CPU::inc_reg16},             // 0x03 INC BC
        {1,  4,  0,  B,  B, &CPU::inc_reg8},              // 0x04 INC  B // Flags
        {1,  4,  0,  B,  B, &CPU::dec_reg8},              // 0x05 DEC  B // Flags
        {2,  8,  0,  B,  0, &CPU::load_d8_to_reg},        // 0x06 LD  B,  d8
        {1,  4,  0,  A,  A, &CPU::r_l_r_c_reg8},          // 0x07 RLCA // Flags
        {3, 20,  0,  0, SP, &CPU::load_reg16_to_d16pt},   // 0x08 LD (a16), SP
        {1,  8,  0, HL, BC, &CPU::add_reg16_to_reg16},    // 0x09 ADD HL, BC // Flags
        {1,  8,  0,  A, BC, &CPU::load_regpt_to_reg8},    // 0x0A LD A, (BC)
        {1,  8,  0, BC, BC, &CPU::dec_reg16},             // 0x0B DEC BC
        {1,  4,  0,  C,  C, &CPU::inc_reg8},              // 0x0C INC  C // Flags
        {1,  4,  0,  C,  C, &CPU::dec_reg8},              // 0x0D DEC  C // Flags
        {2,  8,  0,  C,  0, &CPU::load_d8_to_reg},        // 0x0E LD  C,  d8
        {1,  4,  0,  A,  A, &CPU::r_l_r_c_reg8},          // 0x0F RRCA // Flags

        {2,  4,  0,  0,  0, &CPU::stop},                  // 0x10 STOP TODO
        {3, 12,  0, DE,  0, &CPU::load_d16_to_reg},       // 0x11 LD  DE,  d16
        {1,  8,  0, DE,  A, &CPU::load_reg8_to_regpt},    // 0x12 LD (DE),   A
        {1,  8,  0, DE, DE, &CPU::inc_reg16},             // 0x13 INC DE
        {1,  4,  0,  D,  D, &CPU::inc_reg8},              // 0x14 INC  D // Flags
        {1,  4,  0,  D,  D, &CPU::dec_reg8},              // 0x15 DEC  D // Flags
        {2,  8,  0,  D,  0, &CPU::load_d8_to_reg},        // 0x16 LD  D,  d8
        {1,  4,  0,  A,  A, &CPU::r_l_r_c_reg8},          // 0x17 RLA // Flags
        {2, 12,  0, PC,  0, &CPU::rel_jmp},               // 0x18 JR r8
        {1,  8,  0, HL, DE, &CPU::add_reg16_to_reg16},    // 0x19 ADD HL, DE // Flags
        {1,  8,  0,  A, DE, &CPU::load_regpt_to_reg8},    // 0x1A LD A, (DE)
        {1,  8,  0, DE, DE, &CPU::dec_reg16},             // 0x1B DEC DE
        {1,  4,  0,  E,  E, &CPU::inc_reg8},              // 0x1C INC  E // Flags
        {1,  4,  0,  E,  E, &CPU::dec_reg8},              // 0x1D DEC  E // Flags
        {2,  8,  0,  E,  0, &CPU::load_d8_to_reg},        // 0x1E LD  E,  d8
        {1,  4,  0,  A,  A, &CPU::r_l_r_c_reg8},          // 0x1F RRA // Flags

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
        {1, 12,  0, HL, HL, &CPU::inc_regpt},             // 0x34 INC (HL) // Flags
        {1, 12,  0, HL, HL, &CPU::dec_regpt},             // 0x35 DEC (HL) // Flags
        {2, 12,  0, HL,  0, &CPU::load_d8_to_regpt},      // 0x36 LD  (HL), d8
        {1,  4,  0,  0,  0, &CPU::set_carryflag},         // 0x37 SCF // Flags
        {2, 12,  8, PC,  0, &CPU::rel_c_jmp},             // 0x38 JR  C, r8
        {1,  8,  0, HL, SP, &CPU::add_reg16_to_reg16},    // 0x39 ADD HL, SP // Flags
        {1,  8,  0,  A, HL, &CPU::load_regptdec_to_reg8}, // 0x3A LD A, (HL-)
        {1,  8,  0, SP, SP, &CPU::dec_reg16},             // 0x3B DEC SP
        {1,  4,  0,  A,  A, &CPU::inc_reg8},              // 0x3C INC  A // Flags
        {1,  4,  0,  A,  A, &CPU::dec_reg8},              // 0x3D DEC  A // Flags
        {2,  8,  0,  A,  0, &CPU::load_d8_to_reg},        // 0x3E LD   A,  d8
        {1,  4,  0,  0,  0, &CPU::cpl_carryflag},         // 0x3F CCF // Flags

        // Huge blocks of load instructions
        {1,  4,  0,  B,  B, &CPU::load_reg8_to_reg8},     // 0x40 LD B, B
        {1,  4,  0,  B,  C, &CPU::load_reg8_to_reg8},     // 0x41 LD B, C
        {1,  4,  0,  B,  D, &CPU::load_reg8_to_reg8},     // 0x42 LD B, D
        {1,  4,  0,  B,  E, &CPU::load_reg8_to_reg8},     // 0x43 LD B, E
        {1,  4,  0,  B,  H, &CPU::load_reg8_to_reg8},     // 0x44 LD B, H
        {1,  4,  0,  B,  L, &CPU::load_reg8_to_reg8},     // 0x45 LD B, L
        {1,  8,  0,  B, HL, &CPU::load_regpt_to_reg8},    // 0x46 LD B, (HL)
        {1,  4,  0,  B,  A, &CPU::load_reg8_to_reg8},     // 0x47 LD B, A
        {1,  4,  0,  C,  B, &CPU::load_reg8_to_reg8},     // 0x48 LD C, B
        {1,  4,  0,  C,  C, &CPU::load_reg8_to_reg8},     // 0x49 LD C, C
        {1,  4,  0,  C,  D, &CPU::load_reg8_to_reg8},     // 0x4A LD C, D
        {1,  4,  0,  C,  E, &CPU::load_reg8_to_reg8},     // 0x4B LD C, E
        {1,  4,  0,  C,  H, &CPU::load_reg8_to_reg8},     // 0x4C LD C, H
        {1,  4,  0,  C,  L, &CPU::load_reg8_to_reg8},     // 0x4D LD C, L
        {1,  8,  0,  C, HL, &CPU::load_regpt_to_reg8},    // 0x4E LD C, (HL)
        {1,  4,  0,  C,  A, &CPU::load_reg8_to_reg8},     // 0x4F LD C, A

        {1,  4,  0,  D,  B, &CPU::load_reg8_to_reg8},     // 0x50 LD D, B
        {1,  4,  0,  D,  C, &CPU::load_reg8_to_reg8},     // 0x51 LD D, C
        {1,  4,  0,  D,  D, &CPU::load_reg8_to_reg8},     // 0x52 LD D, D
        {1,  4,  0,  D,  E, &CPU::load_reg8_to_reg8},     // 0x53 LD D, E
        {1,  4,  0,  D,  H, &CPU::load_reg8_to_reg8},     // 0x54 LD D, H
        {1,  4,  0,  D,  L, &CPU::load_reg8_to_reg8},     // 0x55 LD D, L
        {1,  8,  0,  D, HL, &CPU::load_regpt_to_reg8},    // 0x56 LD D, (HL)
        {1,  4,  0,  D,  A, &CPU::load_reg8_to_reg8},     // 0x57 LD D, A
        {1,  4,  0,  E,  B, &CPU::load_reg8_to_reg8},     // 0x58 LD E, B
        {1,  4,  0,  E,  C, &CPU::load_reg8_to_reg8},     // 0x59 LD E, C
        {1,  4,  0,  E,  D, &CPU::load_reg8_to_reg8},     // 0x5A LD E, D
        {1,  4,  0,  E,  E, &CPU::load_reg8_to_reg8},     // 0x5B LD E, E
        {1,  4,  0,  E,  H, &CPU::load_reg8_to_reg8},     // 0x5C LD E, H
        {1,  4,  0,  E,  L, &CPU::load_reg8_to_reg8},     // 0x5D LD E, L
        {1,  8,  0,  E, HL, &CPU::load_regpt_to_reg8},    // 0x5E LD E, (HL)
        {1,  4,  0,  E,  A, &CPU::load_reg8_to_reg8},     // 0x5F LD E, A

        {1,  4,  0,  H,  B, &CPU::load_reg8_to_reg8},     // 0x60 LD H, B
        {1,  4,  0,  H,  C, &CPU::load_reg8_to_reg8},     // 0x61 LD H, C
        {1,  4,  0,  H,  D, &CPU::load_reg8_to_reg8},     // 0x62 LD H, D
        {1,  4,  0,  H,  E, &CPU::load_reg8_to_reg8},     // 0x63 LD H, E
        {1,  4,  0,  H,  H, &CPU::load_reg8_to_reg8},     // 0x64 LD H, H
        {1,  4,  0,  H,  L, &CPU::load_reg8_to_reg8},     // 0x65 LD H, L
        {1,  8,  0,  H, HL, &CPU::load_regpt_to_reg8},    // 0x66 LD H, (HL)
        {1,  4,  0,  H,  A, &CPU::load_reg8_to_reg8},     // 0x67 LD H, A
        {1,  4,  0,  L,  B, &CPU::load_reg8_to_reg8},     // 0x68 LD L, B
        {1,  4,  0,  L,  C, &CPU::load_reg8_to_reg8},     // 0x69 LD L, C
        {1,  4,  0,  L,  D, &CPU::load_reg8_to_reg8},     // 0x6A LD L, D
        {1,  4,  0,  L,  E, &CPU::load_reg8_to_reg8},     // 0x6B LD L, E
        {1,  4,  0,  L,  H, &CPU::load_reg8_to_reg8},     // 0x6C LD L, H
        {1,  4,  0,  L,  L, &CPU::load_reg8_to_reg8},     // 0x6D LD L, L
        {1,  8,  0,  L, HL, &CPU::load_regpt_to_reg8},    // 0x6E LD L, (HL)
        {1,  4,  0,  L,  A, &CPU::load_reg8_to_reg8},     // 0x6F LD L, A

        {1,  8,  0, HL,  B, &CPU::load_reg8_to_regpt},    // 0x70 LD (HL), B
        {1,  8,  0, HL,  C, &CPU::load_reg8_to_regpt},    // 0x71 LD (HL), C
        {1,  8,  0, HL,  D, &CPU::load_reg8_to_regpt},    // 0x72 LD (HL), D
        {1,  8,  0, HL,  E, &CPU::load_reg8_to_regpt},    // 0x73 LD (HL), E
        {1,  8,  0, HL,  H, &CPU::load_reg8_to_regpt},    // 0x74 LD (HL), H
        {1,  8,  0, HL,  L, &CPU::load_reg8_to_regpt},    // 0x75 LD (HL), L
        {1,  4,  0,  0,  0, &CPU::halt},                  // 0x76 HALT
        {1,  8,  0, HL,  A, &CPU::load_reg8_to_regpt},    // 0x77 LD (HL), A
        {1,  4,  0,  A,  B, &CPU::load_reg8_to_reg8},     // 0x78 LD A, B
        {1,  4,  0,  A,  C, &CPU::load_reg8_to_reg8},     // 0x79 LD A, C
        {1,  4,  0,  A,  D, &CPU::load_reg8_to_reg8},     // 0x7A LD A, D
        {1,  4,  0,  A,  E, &CPU::load_reg8_to_reg8},     // 0x7B LD A, E
        {1,  4,  0,  A,  H, &CPU::load_reg8_to_reg8},     // 0x7C LD A, H
        {1,  4,  0,  A,  L, &CPU::load_reg8_to_reg8},     // 0x7D LD A, L
        {1,  8,  0,  A, HL, &CPU::load_regpt_to_reg8},    // 0x7E LD A, (HL)
        {1,  4,  0,  A,  A, &CPU::load_reg8_to_reg8},     // 0x7F LD A, A

        // ALU ---
        // ADD instructions
        {1,  4,  0,  A,  B, &CPU::add_reg8_to_reg8},      // 0x80 ADD A, B
        {1,  4,  0,  A,  C, &CPU::add_reg8_to_reg8},      // 0x81 ADD A, C
        {1,  4,  0,  A,  D, &CPU::add_reg8_to_reg8},      // 0x82 ADD A, D
        {1,  4,  0,  A,  E, &CPU::add_reg8_to_reg8},      // 0x83 ADD A, E
        {1,  4,  0,  A,  H, &CPU::add_reg8_to_reg8},      // 0x84 ADD A, H
        {1,  4,  0,  A,  L, &CPU::add_reg8_to_reg8},      // 0x85 ADD A, L
        {1,  8,  0,  A, HL, &CPU::add_regpt_to_reg8},     // 0x86 ADD A, (HL)
        {1,  4,  0,  A,  A, &CPU::add_reg8_to_reg8},      // 0x87 ADD A, A
        {1,  4,  0,  A,  B, &CPU::add_reg8_to_reg8_c},    // 0x88 ADC A, B
        {1,  4,  0,  A,  C, &CPU::add_reg8_to_reg8_c},    // 0x89 ADC A, C
        {1,  4,  0,  A,  D, &CPU::add_reg8_to_reg8_c},    // 0x8A ADC A, D
        {1,  4,  0,  A,  E, &CPU::add_reg8_to_reg8_c},    // 0x8B ADC A, E
        {1,  4,  0,  A,  H, &CPU::add_reg8_to_reg8_c},    // 0x8C ADC A, H
        {1,  4,  0,  A,  L, &CPU::add_reg8_to_reg8_c},    // 0x8D ADC A, L
        {1,  8,  0,  A, HL, &CPU::add_regpt_to_reg8_c},   // 0x8E ADC A, (HL)
        {1,  4,  0,  A,  A, &CPU::add_reg8_to_reg8_c},    // 0x8F ADC A, A

        // SUB instructions
        {1,  4,  0,  A,  B, &CPU::sub_reg8_from_reg8},    // 0x90 SUB A, B
        {1,  4,  0,  A,  C, &CPU::sub_reg8_from_reg8},    // 0x91 SUB A, C
        {1,  4,  0,  A,  D, &CPU::sub_reg8_from_reg8},    // 0x92 SUB A, D
        {1,  4,  0,  A,  E, &CPU::sub_reg8_from_reg8},    // 0x93 SUB A, E
        {1,  4,  0,  A,  H, &CPU::sub_reg8_from_reg8},    // 0x94 SUB A, H
        {1,  4,  0,  A,  L, &CPU::sub_reg8_from_reg8},    // 0x95 SUB A, L
        {1,  8,  0,  A, HL, &CPU::sub_regpt_from_reg8},   // 0x96 SUB A, (HL)
        {1,  4,  0,  A,  A, &CPU::sub_reg8_from_reg8},    // 0x97 SUB A, A
        {1,  4,  0,  A,  B, &CPU::sub_reg8_from_reg8_c},  // 0x98 SBC A, B
        {1,  4,  0,  A,  C, &CPU::sub_reg8_from_reg8_c},  // 0x99 SBC A, C
        {1,  4,  0,  A,  D, &CPU::sub_reg8_from_reg8_c},  // 0x9A SBC A, D
        {1,  4,  0,  A,  E, &CPU::sub_reg8_from_reg8_c},  // 0x9B SBC A, E
        {1,  4,  0,  A,  H, &CPU::sub_reg8_from_reg8_c},  // 0x9C SBC A, H
        {1,  4,  0,  A,  L, &CPU::sub_reg8_from_reg8_c},  // 0x9D SBC A, L
        {1,  8,  0,  A, HL, &CPU::sub_regpt_from_reg8_c}, // 0x9E SBC A, (HL)
        {1,  4,  0,  A,  A, &CPU::sub_reg8_from_reg8_c},  // 0x9F SBC A, A

        // AND and XOR
        {1,  4,  0,  A,  B, &CPU::and_reg8_to_reg8},      // 0xA0 AND A, B
        {1,  4,  0,  A,  C, &CPU::and_reg8_to_reg8},      // 0xA1 AND A, C
        {1,  4,  0,  A,  D, &CPU::and_reg8_to_reg8},      // 0xA2 AND A, D
        {1,  4,  0,  A,  E, &CPU::and_reg8_to_reg8},      // 0xA3 AND A, E
        {1,  4,  0,  A,  H, &CPU::and_reg8_to_reg8},      // 0xA4 AND A, H
        {1,  4,  0,  A,  L, &CPU::and_reg8_to_reg8},      // 0xA5 AND A, L
        {1,  8,  0,  A, HL, &CPU::and_regpt_to_reg8},     // 0xA6 AND A, (HL)
        {1,  4,  0,  A,  A, &CPU::and_reg8_to_reg8},      // 0xA7 AND A, A
        {1,  4,  0,  A,  B, &CPU::xor_reg8_to_reg8},      // 0xA8 XOR A, B
        {1,  4,  0,  A,  C, &CPU::xor_reg8_to_reg8},      // 0xA9 XOR A, C
        {1,  4,  0,  A,  D, &CPU::xor_reg8_to_reg8},      // 0xAA XOR A, D
        {1,  4,  0,  A,  E, &CPU::xor_reg8_to_reg8},      // 0xAB XOR A, E
        {1,  4,  0,  A,  H, &CPU::xor_reg8_to_reg8},      // 0xAC XOR A, H
        {1,  4,  0,  A,  L, &CPU::xor_reg8_to_reg8},      // 0xAD XOR A, L
        {1,  8,  0,  A, HL, &CPU::xor_regpt_to_reg8},     // 0xAE XOR A, (HL)
        {1,  4,  0,  A,  A, &CPU::xor_reg8_to_reg8},      // 0xAF XOR A, A

        // OR and CP
        {1,  4,  0,  A,  B, &CPU::or_reg8_to_reg8},       // 0xB0 OR  A, B
        {1,  4,  0,  A,  C, &CPU::or_reg8_to_reg8},       // 0xB1 OR  A, C
        {1,  4,  0,  A,  D, &CPU::or_reg8_to_reg8},       // 0xB2 OR  A, D
        {1,  4,  0,  A,  E, &CPU::or_reg8_to_reg8},       // 0xB3 OR  A, E
        {1,  4,  0,  A,  H, &CPU::or_reg8_to_reg8},       // 0xB4 OR  A, H
        {1,  4,  0,  A,  L, &CPU::or_reg8_to_reg8},       // 0xB5 OR  A, L
        {1,  8,  0,  A, HL, &CPU::or_regpt_to_reg8},      // 0xB6 OR  A, (HL)
        {1,  4,  0,  A,  A, &CPU::or_reg8_to_reg8},       // 0xB7 OR  A, A
        {1,  4,  0,  A,  B, &CPU::cp_reg8_to_reg8},       // 0xB8 CP  A, B
        {1,  4,  0,  A,  C, &CPU::cp_reg8_to_reg8},       // 0xB9 CP  A, C
        {1,  4,  0,  A,  D, &CPU::cp_reg8_to_reg8},       // 0xBA CP  A, D
        {1,  4,  0,  A,  E, &CPU::cp_reg8_to_reg8},       // 0xBB CP  A, E
        {1,  4,  0,  A,  H, &CPU::cp_reg8_to_reg8},       // 0xBC CP  A, H
        {1,  4,  0,  A,  L, &CPU::cp_reg8_to_reg8},       // 0xBD CP  A, L
        {1,  8,  0,  A, HL, &CPU::cp_regpt_to_reg8},      // 0xBE CP  A, (HL)
        {1,  4,  0,  A,  A, &CPU::cp_reg8_to_reg8},       // 0xBF CP  A, A

        // C BLOCK
        {1,  20,  8, PC, SP, &CPU::ret_nz},               // 0xC0 RET NZ
        {1,  12,  0, BC, SP, &CPU::pop},                  // 0xC1 POP BC
        {3,  16, 12, PC,  0, &CPU::jmp_nz},               // 0xC2 JP NZ, a16
        {3,  16,  0, PC,  0, &CPU::jmp},                  // 0xC3 JP a16
        {3,  24, 12, SP, PC, &CPU::call_nz},              // 0xC4 CALL NZ, a16
        {1,  16,  0, SP, BC, &CPU::push},                 // 0xC5 PUSH BC
        {2,   8,  0,  A,  0, &CPU::add_d8_to_reg8},       // 0xC6 ADD A, d8
        {1,  16,  0, SP, PC, &CPU::rst},                  // 0xC7 RST $00
        {1,  20,  8, PC, SP, &CPU::ret_z},                // 0xC8 RET Z
        {1,  16,  0, PC, SP, &CPU::ret},                  // 0xC9 RET
        {3,  16, 12, PC,  0, &CPU::jmp_z},                // 0xCA JP Z, a16
        {1,   4,  0,  0,  0, nullptr},                    // 0xCB PREFIX CB
        {3,  24, 12, SP, PC, &CPU::call_z},               // 0xCC CALL Z, a16
        {3,  24,  0, SP, PC, &CPU::call},                 // 0xCD CALL a16
        {2,   8,  0,  A,  0, &CPU::add_d8_to_reg8_c},     // 0xCE ADC A, d8
        {1,  16,  0, SP, PC, &CPU::rst},                  // 0xCF RST $08

        // D BLOCK
        {1,  20,  8, PC, SP, &CPU::ret_nc},               // 0xD0 RET NC
        {1,  12,  0, DE, SP, &CPU::pop},                  // 0xD1 POP DE
        {3,  16, 12, PC,  0, &CPU::jmp_nc},               // 0xD2 JP NC, a16
        {1,   0,  0,  0,  0, nullptr},                    // 0xD3 NO INSTRUCTION
        {3,  24, 12, SP, PC, &CPU::call_nc},              // 0xD4 CALL NC, a16
        {1,  16,  0, SP, DE, &CPU::push},                 // 0xD5 PUSH DE
        {2,   8,  0,  A,  0, &CPU::sub_d8_to_reg8},       // 0xD6 SUB A, d8
        {1,  16,  0, SP, PC, &CPU::rst},                  // 0xD7 RST $10
        {1,  20,  8, PC, SP, &CPU::ret_c},                // 0xD8 RET C
        {1,  16,  0, PC, SP, &CPU::reti},                 // 0xD9 RETI
        {3,  16, 12, PC,  0, &CPU::jmp_c},                // 0xDA JP C, a16
        {1,   0,  0,  0,  0, nullptr},                    // 0xDB NO INSTRUCTION
        {3,  24, 12, SP, PC, &CPU::call_c},               // 0xDC CALL C, a16
        {1,   0,  0,  0,  0, nullptr},                    // 0xDD NO INSTRUCTION
        {2,   8,  0,  A,  0, &CPU::sub_d8_to_reg8_c},     // 0xDE SBC A, d8
        {1,  16,  0, SP, PC, &CPU::rst},                  // 0xDF RST $18

        // E Block
        {2,  12,  0,  0,  A, &CPU::load_reg8_to_d8pt},    // 0xE0 LDH (a8), A
        {1,  12,  0, HL, SP, &CPU::pop},                  // 0xE1 POP HL
        {1,   8,  0,  C,  A, &CPU::load_reg8_to_reg8pt},  // 0xE2 LD (C), A //TODO: make sure length is 1
        {1,   0,  0,  0,  0, nullptr},                    // 0xE3 NO INSTRUCTION
        {1,   0,  0,  0,  0, nullptr},                    // 0xE4 NO INSTRUCTION
        {1,  16,  0, SP, HL, &CPU::push},                 // 0xE5 PUSH HL
        {2,   8,  0,  A,  0, &CPU::and_d8_to_reg8},       // 0xE6 AND A, d8
        {1,  16,  0, SP, PC, &CPU::rst},                  // 0xE7 RST $20
        {2,  16,  0, SP,  0, &CPU::add_r8_to_reg16},      // 0xE8 ADD SP, r8 // TODO: Check flags if correct
        {1,   4,  0, PC, HL, &CPU::jmp_regpt},            // 0xE9 JP (HL)
        {3,  16,  0,  0,  A, &CPU::load_reg8_to_d16pt},   // 0xEA LD (a16), A
        {1,   0,  0,  0,  0, nullptr},                    // 0xEB NO INSTRUCTION
        {1,   0,  0,  0,  0, nullptr},                    // 0xEC NO INSTRUCTION
        {1,   0,  0,  0,  0, nullptr},                    // 0xED NO INSTRUCTION
        {2,   8,  0,  A,  0, &CPU::xor_d8_to_reg8},       // 0xEE XOR A, d8
        {1,  16,  0, SP, PC, &CPU::rst},                  // 0xEF RST $28

        // F BLOCK
        {2,  12,  0,  A,  0, &CPU::load_d8pt_to_reg8},    // 0xF0 LDH A, (a8)
        {1,  12,  0, AF, SP, &CPU::pop},                  // 0xF1 POP AF
        {1,   8,  0,  A,  C, &CPU::load_reg8pt_to_reg8},  // 0xF2 LD A, (C) //TODO: make sure length is 1
        {1,   4,  0,  0,  0, &CPU::disable_interrupts},   // 0xF3 DI
        {1,   0,  0,  0,  0, nullptr},                    // 0xF4 NO INSTRUCTION
        {1,  16,  0, SP, AF, &CPU::push},                 // 0xF5 PUSH AF
        {2,   8,  0,  A,  0, &CPU::or_d8_to_reg8},        // 0xF6 OR A, d8
        {1,  16,  0, SP, PC, &CPU::rst},                  // 0xF7 RST $30
        {2,  12,  0, HL, SP, &CPU::ldhl_spr8},            // 0xF8 LD HL, SP+r8 // TODO: Check flags if correct
        {1,   8,  0, SP, HL, &CPU::load_reg16_to_reg16},  // 0xF9 LD SP, HL
        {3,  16,  0,  A,  0, &CPU::load_d16pt_to_reg8},   // 0xFA LD A, (a16)
        {1,   4,  0,  0,  0, &CPU::enable_interrupts},    // 0xFB EI
        {1,   0,  0,  0,  0, nullptr},                    // 0xFC NO INSTRUCTION
        {1,   0,  0,  0,  0, nullptr},                    // 0xFD NO INSTRUCTION
        {2,   8,  0,  A,  0, &CPU::cp_d8_to_reg8},        // 0xFE CP  A, d8
        {1,  16,  0, SP, PC, &CPU::rst}                   // 0xFF RST $38
};

const uint8_t Gameboy::CPU::CPU::regMap[8] = {B, C, D, E, H, L, HL, A};

CPU::CPU(Memory::MemoryMappedIO& p_mmap)
    : mmap ( p_mmap ),
      currentPC (),
      interruptMasterEnable (),
      ticks ()
{}

//interrupts
std::uint8_t CPU::enable_interrupts (const Instruction& instruction) {
    interruptMasterEnable = true;
    return instruction.cycles;
}

std::uint8_t CPU::disable_interrupts (const Instruction& instruction) {
    interruptMasterEnable = false;
    return instruction.cycles;
}

// RST
std::uint8_t CPU::rst (const Instruction& instruction) {
    // A call is the same as pushing from the program counter
    // dest has SP, src = PC
    push(instruction);
    // dynamically calculate from opcode
    registers.write16(instruction.srcRegIndex, static_cast<uint16_t>(currentInstruction & 0x38));
    return instruction.cycles;
}

// flags
std::uint8_t CPU::set_carryflag (const Instruction& instruction) {
    registers.reg[F] &= ~(SubtractFlag | HalfCarryFlag);
    registers.reg[F] |= CarryFlag;
    return instruction.cycles;
}

std::uint8_t CPU::cpl_carryflag (const Instruction& instruction) {
    registers.reg[F] &= ~(SubtractFlag | HalfCarryFlag);
    registers.reg[F] ^= CarryFlag;
    return instruction.cycles;
}

// special loads
std::uint8_t CPU::ldhl_spr8 (const Instruction& instruction) {
    uint16_t srcVal = registers.read16(instruction.srcRegIndex);
    int8_t srcValToAdd = static_cast<int8_t>(mmap.read(static_cast<uint16_t>(currentPC + 1)));

    // Affected flags - Half Carry flag and Carry flag
    registers.reg[F] &= ~(ZeroFlag | SubtractFlag | HalfCarryFlag | CarryFlag);
    // set if carry from bit 11 0x0FFF
    if (0x1000 & ((srcVal & 0x0FFF) + srcValToAdd)) {
        registers.reg[F] |=  HalfCarryFlag;
    }
    // set if carry from bit 15
    if (0x10000 & (static_cast<uint32_t>(srcVal) + srcValToAdd)) {
        registers.reg[F] |=  CarryFlag;
    }
    return instruction.cycles;
}

// loads
std::uint8_t CPU::load_reg16_to_reg16 (const Instruction& instruction) {
    registers.write16(instruction.destRegIndex, registers.read16(instruction.srcRegIndex));
    return instruction.cycles;
}

std::uint8_t CPU::load_reg16_to_d16pt (const Instruction& instruction) {
    mmap.write16(mmap.read16(static_cast<uint16_t>(currentPC + 1)), registers.read16(instruction.srcRegIndex));
    return instruction.cycles;
}

std::uint8_t CPU::load_d16_to_reg (const Instruction& instruction) {
    registers.write16(instruction.destRegIndex, mmap.read16(static_cast<uint16_t>(currentPC + 1)));
    return instruction.cycles;
}

std::uint8_t CPU::load_d8pt_to_reg8 (const Instruction& instruction) {
    registers.reg[instruction.destRegIndex] =
            mmap.read(static_cast<uint16_t>(0xFF00 | mmap.read(static_cast<uint16_t>(currentPC + 1))));
    return instruction.cycles;
}

std::uint8_t CPU::load_reg8pt_to_reg8 (const Instruction& instruction) {
    registers.reg[instruction.destRegIndex] =
            mmap.read(static_cast<uint16_t>(0xFF00 | registers.reg[instruction.srcRegIndex]));
    return instruction.cycles;
}

std::uint8_t CPU::load_d8_to_reg (const Instruction& instruction) {
    registers.reg[instruction.destRegIndex] = mmap.read(static_cast<uint16_t>(currentPC + 1));
    return instruction.cycles;
}

std::uint8_t CPU::load_d8_to_regpt (const Instruction& instruction) {
    mmap.write(registers.read16(instruction.destRegIndex), mmap.read(static_cast<uint16_t>(currentPC + 1)));
    return instruction.cycles;
}

std::uint8_t CPU::load_reg8_to_regpt (const Instruction& instruction) {
    mmap.write(registers.read16(instruction.destRegIndex), registers.reg[instruction.srcRegIndex]);
    return instruction.cycles;
}

std::uint8_t CPU::load_reg8_to_d8pt(const Instruction &instruction) {
    mmap.write(static_cast<uint16_t>(0xFF00 | mmap.read(static_cast<uint16_t>(currentPC + 1))),
               registers.reg[instruction.srcRegIndex]);
    return instruction.cycles;
}

std::uint8_t CPU::load_reg8_to_reg8pt(const Instruction &instruction) {
    mmap.write(static_cast<uint16_t>(0xFF00 | mmap.read(registers.reg[instruction.destRegIndex])),
               registers.reg[instruction.srcRegIndex]);
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

std::uint8_t CPU::load_regptdec_to_reg8 (const Instruction& instruction) {
    uint16_t srcRegValue = registers.read16(instruction.srcRegIndex);
    registers.reg[instruction.destRegIndex] = mmap.read(srcRegValue);
    registers.write16(instruction.srcRegIndex, static_cast<uint16_t>(srcRegValue - 1));
    return instruction.cycles;
}

std::uint8_t CPU::load_reg8_to_d16pt(const Instruction &instruction) {
    mmap.write(mmap.read16(static_cast<uint16_t>(currentPC + 1)), registers.reg[instruction.srcRegIndex]);
    return instruction.cycles;
}

std::uint8_t CPU::load_d16pt_to_reg8 (const Instruction& instruction) {
    registers.reg[instruction.destRegIndex] =
            mmap.read(mmap.read16(static_cast<uint16_t>(currentPC + 1)));
    return instruction.cycles;
}

std::uint8_t CPU::load_reg8_to_reg8 (const Instruction& instruction) {
    registers.reg[instruction.destRegIndex] = registers.reg[instruction.srcRegIndex];
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
    registers.reg[F] &= ~(ZeroFlag | SubtractFlag | HalfCarryFlag);
    if (valueAfter == 0) {
        registers.reg[F] |= ZeroFlag;
    }
    if (isSubtract) {
        registers.reg[F] |= SubtractFlag;
        if ((valueAfter & 0x0F) == 0x0F) {
            registers.reg[F] |= HalfCarryFlag;
        }
    } else if ((valueAfter & 0x0F) == 0) {
        registers.reg[F] |= HalfCarryFlag;
    }
}

// Complement
std::uint8_t CPU::cpl (const Instruction& instruction) {
    registers.reg[instruction.destRegIndex] = ~registers.reg[instruction.destRegIndex];
    // Set necessary flags
    registers.reg[F] |= (SubtractFlag | HalfCarryFlag);
    return instruction.cycles;
}

// Adds
std::uint8_t CPU::add_reg16_to_reg16 (const Instruction& instruction) {
    uint16_t srcReg  = registers.read16(instruction.srcRegIndex);
    uint16_t destReg = registers.read16(instruction.destRegIndex);
    registers.write16(instruction.destRegIndex, srcReg + destReg);

    // Affected flags - Half Carry flag and Carry flag
    registers.reg[F] &= ~(SubtractFlag | HalfCarryFlag | CarryFlag);
    // set if carry from bit 11 0x0FFF
    registers.reg[F] |= 0x1000 & ((destReg & 0x0FFF) + (srcReg & 0x0FFF)) ? HalfCarryFlag : 0;
    // set if carry from bit 15
    registers.reg[F] |= 0x10000 & (static_cast<uint32_t>(destReg) + srcReg) ? CarryFlag : 0;
    return instruction.cycles;
}

// SP add
std::uint8_t CPU::add_r8_to_reg16 (const Instruction& instruction) {
    int8_t srcVal  = static_cast<int8_t>(mmap.read(static_cast<uint16_t>(currentPC + 1)));
    uint16_t destReg = registers.read16(instruction.destRegIndex);
    registers.write16(instruction.destRegIndex, destReg + srcVal);

    // Affected flags - Half Carry flag and Carry flag
    registers.reg[F] &= ~(ZeroFlag | SubtractFlag | HalfCarryFlag | CarryFlag);
    // set if carry from bit 11 0x0FFF
    registers.reg[F] |= 0x1000 & ((destReg & 0x0FFF) + srcVal) ? HalfCarryFlag : 0;
    // set if carry from bit 15
    registers.reg[F] |= 0x10000 & (static_cast<uint32_t>(destReg) + srcVal) ? CarryFlag : 0;
    return instruction.cycles;
}

// dest should always be the accumulator
void CPU::add_val8_to_reg8_nc_c(std::uint8_t destRegIndex, std::uint8_t srcValue, bool carry) {
    uint8_t destRegValue = registers.reg[destRegIndex];
    uint8_t carryValue = 0;
    uint8_t result = srcValue + destRegValue;
    if (carry && (registers.reg[F] & CarryFlag)) {
        result += ++carryValue;
    }
    registers.reg[destRegIndex] = result;

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

std::uint8_t CPU::add_d8_to_reg8 (const Instruction& instruction) {
    add_val8_to_reg8_nc_c(instruction.destRegIndex, mmap.read(static_cast<uint16_t>(currentPC + 1)), false);
    return instruction.cycles;
}

std::uint8_t CPU::add_d8_to_reg8_c (const Instruction& instruction) {
    add_val8_to_reg8_nc_c(instruction.destRegIndex, mmap.read(static_cast<uint16_t>(currentPC + 1)), true);
    return instruction.cycles;
}

std::uint8_t CPU::add_reg8_to_reg8 (const Instruction& instruction) {
    add_val8_to_reg8_nc_c(instruction.destRegIndex, registers.reg[instruction.srcRegIndex], false);
    return instruction.cycles;
}

std::uint8_t CPU::add_reg8_to_reg8_c (const Instruction& instruction) {
    add_val8_to_reg8_nc_c(instruction.destRegIndex, registers.reg[instruction.srcRegIndex], true);
    return instruction.cycles;
}

std::uint8_t CPU::add_regpt_to_reg8 (const Instruction& instruction) {
    add_val8_to_reg8_nc_c(instruction.destRegIndex, mmap.read(registers.read16(instruction.srcRegIndex)), false);
    return instruction.cycles;
}

std::uint8_t CPU::add_regpt_to_reg8_c (const Instruction& instruction) {
    add_val8_to_reg8_nc_c(instruction.destRegIndex, mmap.read(registers.read16(instruction.srcRegIndex)), true);
    return instruction.cycles;
}

// decrements
void CPU::sub_val8_from_reg8_nc_c(std::uint8_t destRegIndex, std::uint8_t srcValue, bool carry) {
    uint8_t destRegValue = registers.reg[destRegIndex];
    uint8_t carryValue = 0;
    uint8_t result = destRegValue - srcValue;
    if (carry && (registers.reg[F] & CarryFlag)) {
        result -= ++carryValue;
    }
    registers.reg[destRegIndex] = result;

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

std::uint8_t CPU::sub_d8_to_reg8 (const Instruction& instruction) {
    sub_val8_from_reg8_nc_c(instruction.destRegIndex, mmap.read(static_cast<uint16_t>(currentPC + 1)), false);
    return instruction.cycles;
}

std::uint8_t CPU::sub_d8_to_reg8_c (const Instruction& instruction) {
    sub_val8_from_reg8_nc_c(instruction.destRegIndex, mmap.read(static_cast<uint16_t>(currentPC + 1)), true);
    return instruction.cycles;
}

std::uint8_t CPU::sub_reg8_from_reg8(const Instruction &instruction) {
    sub_val8_from_reg8_nc_c(instruction.destRegIndex, registers.reg[instruction.srcRegIndex], false);
    return instruction.cycles;
}

std::uint8_t CPU::sub_reg8_from_reg8_c(const Instruction &instruction) {
    sub_val8_from_reg8_nc_c(instruction.destRegIndex, registers.reg[instruction.srcRegIndex], true);
    return instruction.cycles;
}

std::uint8_t CPU::sub_regpt_from_reg8(const Instruction &instruction) {
    sub_val8_from_reg8_nc_c(instruction.destRegIndex, mmap.read(registers.read16(instruction.srcRegIndex)), false);
    return instruction.cycles;
}

std::uint8_t CPU::sub_regpt_from_reg8_c(const Instruction &instruction) {
    sub_val8_from_reg8_nc_c(instruction.destRegIndex, mmap.read(registers.read16(instruction.srcRegIndex)), true);
    return instruction.cycles;
}

// AND
void CPU::and_val8_to_reg8(std::uint8_t destRegIndex, std::uint8_t srcValue) {
    registers.reg[F] &= ~(ZeroFlag | SubtractFlag | CarryFlag);
    registers.reg[F] |= HalfCarryFlag;
    if ((registers.reg[destRegIndex] &= srcValue) == 0) {
        registers.reg[F] |= ZeroFlag;
    }
}

std::uint8_t CPU::and_reg8_to_reg8 (const Instruction& instruction) {
    and_val8_to_reg8(instruction.destRegIndex, registers.reg[instruction.srcRegIndex]);
    return instruction.cycles;
}

std::uint8_t CPU::and_regpt_to_reg8 (const Instruction& instruction) {
    and_val8_to_reg8(instruction.destRegIndex, mmap.read(registers.read16(instruction.srcRegIndex)));
    return instruction.cycles;
}

std::uint8_t CPU::and_d8_to_reg8 (const Instruction& instruction) {
    and_val8_to_reg8(instruction.destRegIndex, mmap.read(static_cast<uint16_t>(currentPC + 1)));
    return instruction.cycles;
}

// XOR
void CPU::xor_val8_to_reg8(std::uint8_t destRegIndex, std::uint8_t srcValue) {
    registers.reg[F] &= ~(ZeroFlag | SubtractFlag | HalfCarryFlag | CarryFlag);
    if ((registers.reg[destRegIndex] ^= srcValue) == 0) {
        registers.reg[F] |= ZeroFlag;
    }
}

std::uint8_t CPU::xor_reg8_to_reg8 (const Instruction& instruction) {
    xor_val8_to_reg8(instruction.destRegIndex, registers.reg[instruction.srcRegIndex]);
    return instruction.cycles;
}

std::uint8_t CPU::xor_regpt_to_reg8 (const Instruction& instruction) {
    xor_val8_to_reg8(instruction.destRegIndex, mmap.read(registers.read16(instruction.srcRegIndex)));
    return instruction.cycles;
}

std::uint8_t CPU::xor_d8_to_reg8 (const Instruction& instruction) {
    xor_val8_to_reg8(instruction.destRegIndex, mmap.read(static_cast<uint16_t>(currentPC + 1)));
    return instruction.cycles;
}

// OR
void CPU::or_val8_to_reg8(std::uint8_t destRegIndex, std::uint8_t srcValue) {
    registers.reg[F] &= ~(ZeroFlag | SubtractFlag | HalfCarryFlag | CarryFlag);
    if ((registers.reg[destRegIndex] |= srcValue) == 0) {
        registers.reg[F] |= ZeroFlag;
    }
}

std::uint8_t CPU::or_reg8_to_reg8 (const Instruction& instruction) {
    or_val8_to_reg8(instruction.destRegIndex, registers.reg[instruction.srcRegIndex]);
    return instruction.cycles;
}

std::uint8_t CPU::or_regpt_to_reg8 (const Instruction& instruction) {
    or_val8_to_reg8(instruction.destRegIndex, mmap.read(registers.read16(instruction.srcRegIndex)));
    return instruction.cycles;
}

std::uint8_t CPU::or_d8_to_reg8 (const Instruction& instruction) {
    or_val8_to_reg8(instruction.destRegIndex, mmap.read(static_cast<uint16_t>(currentPC + 1)));
    return instruction.cycles;
}

// CP
void CPU::cp_val8_to_reg8(std::uint8_t destRegIndex, std::uint8_t srcValue) {
    const uint8_t destRegValue = registers.reg[destRegIndex];

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

std::uint8_t CPU::cp_reg8_to_reg8 (const Instruction& instruction) {
    cp_val8_to_reg8(instruction.destRegIndex, registers.reg[instruction.srcRegIndex]);
    return instruction.cycles;
}

std::uint8_t CPU::cp_regpt_to_reg8 (const Instruction& instruction) {
    cp_val8_to_reg8(instruction.destRegIndex, mmap.read(registers.read16(instruction.srcRegIndex)));
    return instruction.cycles;
}

std::uint8_t CPU::cp_d8_to_reg8 (const Instruction& instruction) {
    cp_val8_to_reg8(instruction.destRegIndex, mmap.read(static_cast<uint16_t>(currentPC + 1)));
    return instruction.cycles;
}

// Rotates
std::uint8_t CPU::r_l_r_c_val8(std::uint8_t val) {
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

std::uint8_t CPU::r_l_r_c_reg8 (const Instruction& instruction) {
    registers.reg[instruction.destRegIndex] = r_l_r_c_val8(registers.reg[instruction.destRegIndex]);
    return instruction.cycles;
}

// Shifts
std::uint8_t CPU::s_l_r_a_l_val8(std::uint8_t val) {
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

std::uint8_t CPU::stop (const Instruction& instruction) {
    throw runtime_error ("STOP instruction not yet implemented");
}

std::uint8_t CPU::halt (const Instruction& instruction) {
    throw runtime_error ("HALT instruction not yet implemented");
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
    // if ((negative && (registers.reg[F] & flag == 0)) || (!negative && (registers.reg[F] & flag != 0)))
    if (negative != ((registers.reg[F] & flag) != 0)) {
        rel_jmp(instruction);
        return instruction.cycles;
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
    return rel_cond_jmp(instruction, CarryFlag, true);
}
uint8_t CPU::rel_c_jmp(const Instruction &instruction) {
    return rel_cond_jmp(instruction, CarryFlag, false);
}

// Jumps - absolute
std::uint8_t CPU::jmp (const Instruction& instruction) {
    // destRegIndex must be PC
    registers.write16(instruction.destRegIndex, mmap.read16(static_cast<uint16_t>(currentPC + 1)));
    return instruction.cycles;
}

std::uint8_t CPU::jmp_regpt (const Instruction& instruction) {
    // destRegIndex must be PC
    registers.write16(instruction.destRegIndex, mmap.read16(registers.read16(instruction.srcRegIndex)));
    return instruction.cycles;
}

std::uint8_t CPU::jmp_cond (const Instruction& instruction, uint8_t flag, bool negative) {
    // destRegIndex must be PC
    if (negative != ((registers.reg[F] & flag) != 0)) {
        jmp(instruction);
        return instruction.cycles;
    }
    return instruction.cyclesNoAction;
}

uint8_t CPU::jmp_nz(const Instruction &instruction) {
    return jmp_cond(instruction, ZeroFlag, true);
}
uint8_t CPU::jmp_z(const Instruction &instruction) {
    return jmp_cond(instruction, ZeroFlag, false);
}
uint8_t CPU::jmp_nc(const Instruction &instruction) {
    return jmp_cond(instruction, CarryFlag, true);
}
uint8_t CPU::jmp_c(const Instruction &instruction) {
    return jmp_cond(instruction, CarryFlag, false);
}

// Ret
std::uint8_t CPU::ret (const Instruction& instruction) {
    // A jump is the same as popping to the program counter
    pop(instruction);
    return instruction.cycles;
}

std::uint8_t CPU::ret_cond (const Instruction& instruction, uint8_t flag, bool negative) {
    if (negative != ((registers.reg[F] & flag) != 0)) {
        ret(instruction);
        return instruction.cycles;
    }
    return instruction.cyclesNoAction;
}

uint8_t CPU::ret_nz(const Instruction &instruction) {
    return ret_cond(instruction, ZeroFlag, true);
}
uint8_t CPU::ret_z(const Instruction &instruction) {
    return ret_cond(instruction, ZeroFlag, false);
}
uint8_t CPU::ret_nc(const Instruction &instruction) {
    return ret_cond(instruction, CarryFlag, true);
}
uint8_t CPU::ret_c(const Instruction &instruction) {
    return ret_cond(instruction, CarryFlag, false);
}

// Reti
std::uint8_t CPU::reti (const Instruction& instruction) {
    ret(instruction);
    interruptMasterEnable = true;
    return instruction.cycles;
}

// Pop
std::uint8_t CPU::pop (const Instruction& instruction) {
    // Read stack pointer - src has SP
    uint16_t stackPointer = registers.read16(instruction.srcRegIndex);
    // Copy to register - read two bytes from stack and assign them to destination register
    registers.write16(instruction.destRegIndex, mmap.read16(stackPointer));
    // Increment stack pointer
    stackPointer += 2;
    registers.write16(instruction.srcRegIndex, stackPointer);
    return instruction.cycles;
}

// Push
std::uint8_t CPU::push (const Instruction& instruction) {
    // Read stack pointer
    uint16_t stackPointer = registers.read16(instruction.destRegIndex);
    // Copy to stack
    mmap.write16(stackPointer -= 2, registers.read16(instruction.srcRegIndex));
    // decrement stack pointer
    registers.write16(instruction.destRegIndex, stackPointer);
    return instruction.cycles;
}

std::uint8_t CPU::call (const Instruction& instruction) {
    // A call is the same as pushing from the program counter
    // dest has SP, src = PC
    push(instruction);
    // jump to address
    registers.write16(instruction.srcRegIndex, mmap.read16(static_cast<uint16_t>(currentPC + 1)));
    return instruction.cycles;
}

std::uint8_t CPU::call_cond (const Instruction& instruction, uint8_t flag, bool negative) {
    if (negative != ((registers.reg[F] & flag) != 0)) {
        call(instruction);
        return instruction.cycles;
    }
    return instruction.cyclesNoAction;
}

uint8_t CPU::call_nz(const Instruction &instruction) {
    return call_cond(instruction, ZeroFlag, true);
}
uint8_t CPU::call_z(const Instruction &instruction) {
    return call_cond(instruction, ZeroFlag, false);
}
uint8_t CPU::call_nc(const Instruction &instruction) {
    return call_cond(instruction, CarryFlag, true);
}
uint8_t CPU::call_c(const Instruction &instruction) {
    return call_cond(instruction, CarryFlag, false);
}

// DAA - TODO: Use Z80 table instead, or check with it. This is probably buggy
// http://www.z80.info/z80syntx.htm#DAA
uint8_t CPU::daa(const Instruction &instruction) {

    // Reset all affected flags and set as necessary ones
    registers.reg[F] &= ~(ZeroFlag | HalfCarryFlag | CarryFlag);

    uint8_t regValue = registers.reg[instruction.destRegIndex];
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
    return instruction.cycles;
}

void CPU::next() {

    // fetch and decode
    isCurrentExtended = false;
    const Instruction * instruction = nullptr;

    currentPC = registers.read16(PC);
    instruction = &instructions[currentInstruction = mmap.read(currentPC)];
    registers.write16(PC, currentPC + instruction->length);
    if (currentInstruction == 0xCB) {
        isCurrentExtended = true;
        currentPC = registers.read16(PC);
        currentInstruction = mmap.read(currentPC);
        instruction = nullptr;
        registers.write16(PC, static_cast<uint16_t>(currentPC + 1));
        // read next instruction
    }

    if (!isCurrentExtended) {
        // Execute
        uint8_t cycles = instruction->cycles;
        if (instruction->execfn != nullptr) {
            cycles = (this->*instruction->execfn)(*instruction);
        }
        ticks += cycles;
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
                    if (ifr & (1 << i)) {
                        // handle interrupt i, reset IF flag
                        mmap.write(0xFF0F, static_cast<uint8_t>(ifr & ~(1 << i)));
                        interruptMasterEnable = false;
                        // Read stack pointer
                        uint16_t stackPointer = registers.read16(SP);
                        // Copy to stack
                        mmap.write16(stackPointer -= 2, registers.read16(PC));
                        // decrement stack pointer
                        registers.write16(SP, stackPointer);
                        // jump to address
                        registers.write16(PC, static_cast<uint16_t>(0x0040 | (i << 3)));
                        break;
                    }
                }
            }
        }
    }
}