//
// Created by kvnna on 09/10/2016.
//

#include <stdexcept>
#include "Cartridge/MBCFactory.h"
#include "MemoryMappedIO.h"
#include "General/Definitions.h"
#include "General/FileOperations.h"

using namespace std;
using namespace Gameboy;
using namespace Gameboy::Memory;

MemoryMappedIO::MemoryMappedIO(Cartridge::MemoryBankController& p_mbc)
    :
        mbc ( p_mbc ),
        interruptEnableRegister (),
        lowMemoryIsCartridge ()
{
    bios.initialise(Util::FileOperations::loadBinaryFile("DMG_ROM.bin"));
    videoRam.initialise(MemoryType(8192));
    internalRam.initialise(MemoryType(8192));
    spriteRam.initialise(MemoryType(160));
    ioPorts.initialise(MemoryType(76));
    zeroPageRam.initialise(MemoryType(127));
}

uint8_t MemoryMappedIO::read(uint16_t address) const {
    if (address < VideoRam) {
        // Read from ROM
        if (!lowMemoryIsCartridge && address <= 0xFF) {
            return bios.readExt(address);
        } else {
            return mbc.read(address);
        }
    } else {
        // Is this operation cheaper than one comparison per if branch?
        const uint8_t rangeNum = static_cast<uint8_t>((address >> 13) & 0x03);
        switch (rangeNum) {
            case 0:
                // 0x8000 - 0x9FFF - video ram
                return videoRam.readExt(address - VideoRam);
            case 1:
                // 0xA000 - 0xBFFF -- cartridge switchable ram
                return mbc.read(address);
            case 2:
                // 0xC000 - 0xDFFF -- internal ram
                return internalRam.readExt(address - InternalRam);
            case 3:
                // 0xE000 - 0xFFFF
                if (address < SpriteRam) {
                    // 0xE000 - 0xFDFF - Echo ram
                    return internalRam.readExt(address - EchoRam);
                } else if (address < UnusableIO1) {
                    // 0xFE00 - 0xFE9f - Sprite attribute ram
                    return spriteRam.readExt(address - SpriteRam);
                } else if (address < IOPorts) {
                    // 0xFEA0 - 0xFEFF - Unusable
                    throw runtime_error("Read range error: " + to_string(address));
                } else if (address < UnusableIO2) {
                    // 0xFF00 - 0xFF4B - Usable I/O
                    return ioPorts.readExt(address - IOPorts);
                } else if (address < ZeroPageRam) {
                    // 0xFF4C - 0xFF79 - Usable I/O
                    throw runtime_error("Read range error: " + to_string(address));
                } else if (address < IntEnableReg) {
                    // 0xFF80 - 0xFFFE - Zero Page Ram
                    return zeroPageRam.readExt(address - ZeroPageRam);
                } else {
                    // 0xFFFF - 0xFFFF - Interrupt Enable Register
                    return interruptEnableRegister;
                }
            default:
                throw runtime_error("Read range error: " + to_string(address));
        }
    }
}

void MemoryMappedIO::write(uint16_t address, uint8_t datum) {
    if (address < VideoRam) {
        // Read from ROM
        if (lowMemoryIsCartridge && address <= 0xFF) {
            throw runtime_error("Write range error: " + to_string(address));
        } else {
            return mbc.write(address, datum);
        }
    } else {
        // Is this operation cheaper than one comparison per if branch?
        const uint8_t rangeNum = static_cast<uint8_t>((address >> 13) & 0x03);
        switch (rangeNum) {
            case 0:
                // 0x8000 - 0x9FFF - video ram
                return videoRam.writeExt(address - VideoRam, datum);
            case 1:
                // 0xA000 - 0xBFFF -- cartridge switchable ram
                return mbc.write(address, datum);
            case 2:
                // 0xC000 - 0xDFFF -- internal ram
                return internalRam.writeExt(address - InternalRam, datum);
            case 3:
                // 0xE000 - 0xFFFF
                if (address < SpriteRam) {
                    // 0xE000 - 0xFDFF - Echo ram
                    return internalRam.writeExt(address - EchoRam, datum);
                } else if (address < UnusableIO1) {
                    // 0xFE00 - 0xFE9f - Sprite attribute ram
                    return spriteRam.writeExt(address - SpriteRam, datum);
                } else if (address < IOPorts) {
                    // 0xFEA0 - 0xFEFF - Unusable
                    throw runtime_error("Write range error: " + to_string(address));
                } else if (address < UnusableIO2) {
                    // 0xFF00 - 0xFF4B - Usable I/O
                    return ioPorts.writeExt(address - IOPorts, datum);
                } else if (address < ZeroPageRam) {
                    // 0xFF4C - 0xFF79 - Usable I/O
                    if (address == 0xFF50) {
                        lowMemoryIsCartridge = datum;
                    }
                    throw runtime_error("Write range error: " + to_string(address));
                } else if (address < IntEnableReg) {
                    // 0xFF80 - 0xFFFE - Zero Page Ram
                    return zeroPageRam.writeExt(address - ZeroPageRam, datum);
                } else {
                    // 0xFFFF - 0xFFFF - Interrupt Enable Register
                    interruptEnableRegister = datum;
                }
            default:
                throw runtime_error("Write range error: " + to_string(address));
        }
    }
}






