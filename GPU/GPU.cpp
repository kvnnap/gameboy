//
// Created by Kevin on 15/10/2016.
//

#include <stdexcept>
#include "GPU.h"

using namespace std;
using namespace Gameboy::GPU;

GPU::GPU(Gameboy::CPU::IInterruptible &p_interruptible)
    : interruptible (p_interruptible),
      frameBuffer (160 * 144),
      clock (),
      gpuReg ()
{
    gpuReg[OffSTAT] = OAMUsed;
    videoRam.initialise(Memory::MemoryType(8192));
    spriteRam.initialise(Memory::MemoryType(160));
}

void GPU::next(uint32_t ticks) {
    if (!isLcdOn()) {
        return;
    }
    clock += ticks;
    Mode mode = getMode();
    switch (mode) {
        case HBlank:
            if (clock >= 204) {
                // next mode is either mode 1 - VBlank or mode 2 - OAMUsed
                clock -= 204;
                incrementLY();
                if (gpuReg[OffLY] == 144) {
                    setMode(VBlank);
                    // Frame is ready, commit it
                } else {
                    setMode(OAMUsed);
                }
            }
            break;
        case VBlank:
            if (clock >= 456) {
                clock -= 456;
                incrementLY();
                if (gpuReg[OffLY] == 0) {
                    setMode(OAMUsed);
                }
            }
            break;
        case OAMUsed:
            if (clock >= 80) {
                // next mode is mode 3 - no interrupts to raise
                clock -= 80;
                setMode(OAMVRamUsed);
            }
            break;
        case OAMVRamUsed:
            if (clock >= 172) {
                // next mode is mode 0 - HBlank
                clock -= 172;
                setMode(HBlank);
            }
            break;
    }
}

// LCDC
bool GPU::isLcdOn() const {
    return (gpuReg[OffLCDC] & (1 << 7)) != 0;
}

uint16_t GPU::getWindowTileMapOffset() const {
    return static_cast<uint16_t>((gpuReg[OffLCDC] & (1 << 6)) ? 0x9C00 : 0x9800);
}

bool GPU::isWindowDisplayOn() const {
    return (gpuReg[OffLCDC] & (1 << 5)) != 0;
}

uint16_t GPU::getBgWindowTileDataOffset() const {
    return static_cast<uint16_t>((gpuReg[OffLCDC] & (1 << 4)) ? 0x8800 : 0x8000);
}

uint16_t GPU::getBgWindowTileMapOffset() const {
    return static_cast<uint16_t>((gpuReg[OffLCDC] & (1 << 3)) ? 0x9C00 : 0x9800);
}

uint8_t GPU::getSpriteHeight() const {
    return static_cast<uint8_t>((gpuReg[OffLCDC] & (1 << 2)) ? 16 : 8);
}

bool GPU::isSpriteDisplayOn() const {
    return (gpuReg[OffLCDC] & (1 << 1)) != 0;
}

bool GPU::isBgWindowDisplayOn() const {
    return (gpuReg[OffLCDC] & (1 << 0)) != 0;
}

// STAT
void GPU::setMode(Mode mode) {
    gpuReg[OffSTAT] &= 0xFC;
    gpuReg[OffSTAT] |= mode;
    if ((mode != OAMVRamUsed) && (gpuReg[OffSTAT] & (1 << (3 + mode)))) {
        interruptible.requestInterrupt(StatIrq);
    }
    if (mode == VBlank) {
        interruptible.requestInterrupt(VBlankIrq);
    }
}

void GPU::incrementLY() {
    if (++gpuReg[OffLY] == 154) {
        gpuReg[OffLY] = 0;
    }
    // check coincidence flag
    if (gpuReg[OffLY] == gpuReg[OffLYC]) {
        gpuReg[OffSTAT] |= (1 << 2);
        // Check and raise coincidence interrupt
        if (gpuReg[OffSTAT] & (1 << 6)) {
            interruptible.requestInterrupt(StatIrq);
        }
    } else {
        gpuReg[OffSTAT] &= ~(1 << 2);
    }
}

Mode GPU::getMode() const {
    return static_cast<Mode>(gpuReg[OffSTAT] & 0x03);
}

uint8_t GPU::read(uint16_t address) const {
    if (address >= VideoRam && address < RamBankN) {
        // VRam
        if (isLcdOn() && (getMode() == Mode::OAMVRamUsed)) {
            throw runtime_error ("Cannot read from VRAM, it is in use by the GPU");
        }
        return videoRam.readExt(address - VideoRam);
    } else if (address >= SpriteRam && address < UnusableIO1) {
        // Sprite Ram
        if (isLcdOn() && (getMode() & 0x02)) {
            throw runtime_error ("Cannot read from Sprite Ram, it is in use by the GPU");
        }
        return spriteRam.readExt(address - SpriteRam);
    } else if (address >= LCDC && address <= WX) {
        // Registers
        return gpuReg[address - LCDC];
    }
    throw runtime_error ("GPU read out of range: " + to_string(address));
}

void GPU::write(uint16_t address, uint8_t datum) {
    if (address >= VideoRam && address < RamBankN) {
        // VRam
        if (isLcdOn() && (getMode() == Mode::OAMVRamUsed)) {
            throw runtime_error ("Cannot write to VRAM, it is in use by the GPU");
        }
        return videoRam.writeExt(address - VideoRam, datum);
    } else if (address >= SpriteRam && address < UnusableIO1) {
        // Sprite Ram
        if (isLcdOn() && (getMode() & 0x02)) {
            throw runtime_error ("Cannot write to Sprite Ram, it is in use by the GPU");
        }
        return spriteRam.writeExt(address - SpriteRam, datum);
    } else if (address >= LCDC && address <= WX) {
        // Registers
        const RegisterOffset offset = static_cast<RegisterOffset>(address - LCDC);
        switch (offset) {
            case OffLCDC: // 0xFF40
            case OffSTAT: // 0xFF41
            case OffSCY:  // 0xFF42
            case OffSCX:  // 0xFF43
            case OffLY:   // 0xFF44
            case OffLYC:  // 0xFF45
            case OffDMA:  // 0xFF46
            case OffBGP:  // 0xFF47
            case OffOBP0: // 0xFF48
            case OffOBP1: // 0xFF49
            case OffWY:   // 0xFF4A
            case OffWX:   // 0xFF4B
                gpuReg[offset] = datum;
            break;
        }
        return;
    }
    throw runtime_error ("GPU write out of range: " + to_string(address) + ", datum: " + to_string(static_cast<uint32_t>(datum)));
}
