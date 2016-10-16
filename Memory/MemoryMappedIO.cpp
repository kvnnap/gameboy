//
// Created by kvnna on 09/10/2016.
//

#include <stdexcept>
#include "Cartridge/MBCFactory.h"
#include "MemoryMappedIO.h"
#include "General/Definitions.h"
#include "General/FileOperations.h"
#include "GPU/GPU.h"

using namespace std;
using namespace Gameboy;
using namespace Gameboy::Memory;

MemoryMappedIO::MemoryMappedIO(Cartridge::MemoryBankController& p_mbc)
    :
        mbc ( p_mbc ),
        joypad ( nullptr ),
        interruptEnableRegister (),
        lowMemoryIsCartridge ()
{
    bios.initialise(Util::FileOperations::loadBinaryFile("DMG_ROM.bin"));
    internalRam.initialise(MemoryType(8192));
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
                return gpu->read(address);
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
                    return gpu->read(address);
                } else if (address < IOPorts) {
                    // 0xFEA0 - 0xFEFF - Unusable
                    throw runtime_error("Read range error: " + to_string(address));
                } else if (address < UnusableIO2) {
                    // 0xFF00 - 0xFF4B - Usable I/O
                    if (address == 0xFF00) {
                        return joypad->read(address);
                    } else if (address >= GPU::LCDC && address <= GPU::WX) {
                        return gpu->read(address);
                    } else {
                        return ioPorts.readExt(address - IOPorts);
                    }
                    //throw runtime_error ("Cannot read from Unhandled I/O Device Register at address: " + to_string(address));
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
                return gpu->write(address, datum);
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
                    return gpu->write(address, datum);
                } else if (address < IOPorts) {
                    // 0xFEA0 - 0xFEFF - Unusable
                    throw runtime_error("Write range error: " + to_string(address));
                } else if (address < UnusableIO2) {
                    // 0xFF00 - 0xFF4B - Usable I/O
                    if (address == 0xFF00) {
                        return joypad->write(address, datum);
                    } else if (address >= GPU::LCDC && address <= GPU::WX) {
                        return gpu->write(address, datum);
                    } else {
                        // Sound, timer and serial still unhandled
                        return ioPorts.writeExt(address - IOPorts, datum);
                    }
                    //throw runtime_error ("Cannot write to Unhandled I/O Device Register at address: "
                    //                     + to_string(address) + ", datum: " + to_string(static_cast<uint32_t>(datum)));
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

void MemoryMappedIO::setInput(Input::Joypad &p_joypad) {
    joypad = &p_joypad;
}

void MemoryMappedIO::setGpu(GPU::GPU &p_gpu) {
    gpu = &p_gpu;
}






