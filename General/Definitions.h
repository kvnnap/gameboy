//
// Created by Kevin on 08/10/2016.
//

#ifndef GAMEBOY_DEFINITIONS_H
#define GAMEBOY_DEFINITIONS_H

#include <cstdint>

namespace Gameboy {

    enum MemoryMap : std::uint16_t {
        RomBank0 =      0x0000, // 16KB
        RomBankN =      0x4000, // 16KB
        VideoRam =      0x8000, //  8KB
        RamBankN =      0xA000, //  8KB (External)
        InternalRam =   0xC000, //  8KB
        EchoRam =       0xE000, //  7.5KB (7680 bytes)
        SpriteRam =     0xFE00, // 160 bytes
        UnusableIO1 =   0xFEA0, //  96 bytes
        IOPorts =       0xFF00, //  76 bytes
        UnusableIO2 =   0xFF4C, //  52 bytes
        ZeroPageRam =   0xFF80, // 127 bytes
        IntEnableReg =  0xFFFF  //   1 byte
    };
}


#endif //GAMEBOY_DEFINITIONS_H
