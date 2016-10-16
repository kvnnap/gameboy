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
      lcdcReg (),
      statReg (OAMUsed),
      scyReg (),
      scxReg (),
      lyReg (),
      lycReg ()
{
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
                if (lyReg == 144) {
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
                if (lyReg == 0) {
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
    return (lcdcReg & (1 << 7)) != 0;
}

uint16_t GPU::getWindowTileMapOffset() const {
    return static_cast<uint16_t>((lcdcReg & (1 << 6)) ? 0x9C00 : 0x9800);
}

bool GPU::isWindowDisplayOn() const {
    return (lcdcReg & (1 << 5)) != 0;
}

uint16_t GPU::getBgWindowTileDataOffset() const {
    return static_cast<uint16_t>((lcdcReg & (1 << 4)) ? 0x8800 : 0x8000);
}

uint16_t GPU::getBgWindowTileMapOffset() const {
    return static_cast<uint16_t>((lcdcReg & (1 << 3)) ? 0x9C00 : 0x9800);
}

uint8_t GPU::getSpriteHeight() const {
    return static_cast<uint8_t>((lcdcReg & (1 << 2)) ? 16 : 8);
}

bool GPU::isSpriteDisplayOn() const {
    return (lcdcReg & (1 << 1)) != 0;
}

bool GPU::isBgWindowDisplayOn() const {
    return (lcdcReg & (1 << 0)) != 0;
}

// STAT
void GPU::setMode(Mode mode) {
    statReg &= 0xFC;
    statReg |= mode;
    if ((mode != OAMVRamUsed) && (statReg & (1 << (3 + mode)))) {
        interruptible.interrupt(StatIrq);
    }
    if (mode == VBlank) {
        interruptible.interrupt(VBlankIrq);
    }
}

void GPU::incrementLY() {
    if (++lyReg == 154) {
        lyReg = 0;
    }
    // check coincidence flag
    if (lyReg == lycReg) {
        statReg |= (1 << 2);
        // Check and raise coincidence interrupt
        if (statReg & (1 << 6)) {
            interruptible.interrupt(StatIrq);
        }
    } else {
        statReg &= ~(1 << 2);
    }
}

Mode GPU::getMode() const {
    return static_cast<Mode>(statReg & 0x03);
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
        return 144;
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
        return;
    }
    throw runtime_error ("GPU write out of range: " + to_string(address) + ", datum: " + to_string(static_cast<uint32_t>(datum)));
}
