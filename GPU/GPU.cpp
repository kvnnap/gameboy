//
// Created by Kevin on 15/10/2016.
//

#include <stdexcept>
#include "GPU/GPU.h"

using namespace std;
using namespace Gameboy::GPU;
using namespace Gameboy::General;

GPU::GPU(Gameboy::CPU::IInterruptible &p_interruptible, IReadable& p_readableMemoryMappedIO, IVideoOutputDevice& p_outputDevice)
    : interruptible (p_interruptible),
      readableMemoryMappedIO (p_readableMemoryMappedIO),
      outputDevice (p_outputDevice),
      frameBuffer (160 * 144),
      clock (),
      gpuReg (),
      colors {0xFFFFFFFF, 0xFFC0C0C0, 0xFF606060, 0xFF000000},
      lastWindowYPosition ()
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
                    outputDevice.render(frameBuffer.data());
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
                    lastWindowYPosition = gpuReg[OffWY];
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
                // render scanLine
                renderScanLine();
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
    return static_cast<uint16_t>((gpuReg[OffLCDC] & (1 << 6)) ? 0x9C00 : 0x9800) - VideoRam;
}

bool GPU::isWindowDisplayOn() const {
    return (gpuReg[OffLCDC] & (1 << 5)) != 0;
}

uint16_t GPU::getBgWindowTileDataOffset() const {
    // When 9000, signed addressing is used
    return static_cast<uint16_t>((gpuReg[OffLCDC] & (1 << 4)) ? 0x8000 : 0x9000) - VideoRam;
}

uint16_t GPU::getBgTileMapOffset() const {
    return static_cast<uint16_t>((gpuReg[OffLCDC] & (1 << 3)) ? 0x9C00 : 0x9800) - VideoRam;
}

uint8_t GPU::getSpriteHeight() const {
    return static_cast<uint8_t>((gpuReg[OffLCDC] & (1 << 2)) ? 16 : 8);
}

bool GPU::isSpriteDisplayOn() const {
    return (gpuReg[OffLCDC] & (1 << 1)) != 0;
}

bool GPU::isBgDisplayOn() const {
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
                gpuReg[offset] = datum;
                break;
            case OffSTAT: // 0xFF41
                gpuReg[offset] &= 0x03;
                gpuReg[offset] |= datum & 0xFC;
                break;
            case OffSCY:  // 0xFF42
            case OffSCX:  // 0xFF43
                gpuReg[offset] = datum;
                break;
            case OffLY:   // 0xFF44
                throw runtime_error("Attempted write to LY Register, datum:" + to_string(static_cast<uint32_t>(datum)));
            case OffLYC:  // 0xFF45
                gpuReg[offset] = datum;
                break;
            case OffDMA:  // 0xFF46
                // Perform DMA transfer
            {
                const uint16_t baseAddress = static_cast<uint16_t>(datum) << 8;
                for (uint8_t i = 0; i < (UnusableIO1 - SpriteRam); ++i) {
                    //write(SpriteRam + i, readableMemoryMappedIO.read(baseAddress + i));
                    // Rough implementation, need to emulate more
                    // TODO: This takes 671 ticks, data wriging should be probably emulated
                    // during this course of time. Or at the very least, lock the whole memory map
                    // except High Ram.
                    spriteRam.writeExt(i, readableMemoryMappedIO.read(baseAddress + i));
                }
            }
                break;
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

void GPU::renderScanLine() {

    const bool bgEnabled = isBgDisplayOn();
    const bool windowEnabled = isWindowDisplayOn()
                               && (/*gpuReg[OffWY]*/ lastWindowYPosition <= gpuReg[OffLY])
                               && (gpuReg[OffWX] <= 166);
    const uint8_t alteredWX = static_cast<uint8_t>(gpuReg[OffWX] >= 7 ? gpuReg[OffWX] - 7 : 0);

    // Data offset for both background and window
    const uint16_t dataOffset = getBgWindowTileDataOffset();
    const bool negativeAddressing = dataOffset != 0;

    // Get the background and Window Map
    const uint16_t backgroundMapOffset = getBgTileMapOffset();
    const uint16_t windowMapOffset = getWindowTileMapOffset();

    for (uint8_t x = 0; (bgEnabled || windowEnabled) && (x < 160); ++x) {

        // Determine whether to draw using window or background
        uint16_t mapOffset;

        // Pixel offsets
        uint8_t xOffset;
        uint8_t yOffset;

        if (windowEnabled && x >= alteredWX && x <= alteredWX) {
            // Use window
            mapOffset = windowMapOffset;
            xOffset = 0;
            // Handle case where window is interrupted and resumed at a later line
            // This value is updated to WY upon exiting VBLANK
            yOffset = lastWindowYPosition++;
        } else {
            // Use background
            mapOffset = backgroundMapOffset;
            xOffset = gpuReg[OffSCX];
            yOffset = gpuReg[OffSCY] + gpuReg[OffLY];
        }

        // Tile offsets - Divide by 8 since tiles are 8x8 and division will transform
        // view from pixel coordinates to tile coordinates
        const uint8_t yTileOffset = yOffset >> 3; // Divide by 8

        // Select one of the 8 rows found in the tile
        const uint8_t tileLineIndex = yOffset & static_cast<uint8_t>(0x07);

        const uint8_t xVal = xOffset + x;

        // Divide by 8 to transform from pixel coordinates to tile coordinates
        const uint8_t xTileOffset = xVal >> 3;

        // Get tile number from tile map - multiply yTileOffset by 32 as there are 32 tiles within one row
        const uint8_t tileNumber = videoRam.readExt(mapOffset +
                                          // & ~0x400 is the same as % 1024
                                          static_cast<size_t>(((yTileOffset << 5) + xTileOffset) & ~0x400));

        // Get tile address from the tile index, this address is used to compose the final address
        // to read data from the tile data region
        const uint16_t tileAddress =
                dataOffset + (negativeAddressing ? (static_cast<int8_t>(tileNumber) << 4) : tileNumber << 4);

        // Tile x-Pixel Index
        const uint8_t tileXPixelIndex = xVal & static_cast<uint8_t>(0x07);

        // Get 2-bit value - multiply tileLineIndex by two as each line is composed of 2-bytes
        const uint16_t finalAddress = tileAddress + (tileLineIndex << 1);
        uint8_t pixelValue = static_cast<uint8_t>((videoRam.readExt(finalAddress + 1) &
                                                    (1 << (7 - tileXPixelIndex))) ? 2 : 0);
        pixelValue |= (videoRam.readExt(finalAddress) >> (7 - tileXPixelIndex)) & 1;

        // Get palette colour value
        const uint8_t colorIndexValue = static_cast<uint8_t>((gpuReg[OffBGP] >> (pixelValue << 1)) & 0x03);

        // Commit to frame buffer
        frameBuffer[gpuReg[OffLY] * 160 + x] = colors[colorIndexValue];
    }
}


