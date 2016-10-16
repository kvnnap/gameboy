//
// Created by Kevin on 15/10/2016.
//

#ifndef GAMEBOY_GPU_H
#define GAMEBOY_GPU_H

#include <vector>
#include "CPU/IInterruptible.h"
#include "General/IReadableWritable.h"
#include "General/ITick.h"
#include "General/Definitions.h"
#include "Memory/ReadableWritableMemory.h"

namespace Gameboy {
    namespace GPU {

        enum Register : std::uint16_t {
            LCDC = 0xFF40,  // LCD Control Operation
            STAT = 0xFF41,  // LCDC Status - Flip modes on this register
            SCY  = 0xFF42,  // 8 Bit value $00-$FF to scroll BG Y screen position
            SCX  = 0xFF43,  // 8 Bit value $00-$FF to scroll BG X screen position
            LY   = 0xFF44,  // Read only, indicates the vertical line to which
                            // the present data is transferred to the LCD
                            // Driver. The LY can take on any value
                            // between 0 through 153. The values between
                            // 144 and 153 indicate the V-Blank period.
                            // Writing will reset the counter.
            LYC  = 0xFF45,  // The LYC compares itself with the LY. If the
                            // values are the same it causes the STAT to
                            // set the coincident flag.
            DMA  = 0xFF46,  // Write only -  DMA Transfer (40*28 bit) from internal ROM or
                            // RAM ($0000-$F19F) to the OAM (address $FE00-$FE9F)
            BGP  = 0xFF47,  // BG & Window Palette Data
            OBP0 = 0xFF48,  // Object Palette 0 Data
            OBP1 = 0xFF49,  // Object Palette 1 Data
            WY   = 0xFF4A,  // Window Y Position
            WX   = 0xFF4B,  // Window X Position
        };

        enum Mode : std::uint8_t {
            HBlank      = 0x00,
            VBlank      = 0x01,
            OAMUsed     = 0x02,
            OAMVRamUsed = 0x03
        };

        enum Interrupt : std::uint8_t {
            VBlankIrq = 0,
            StatIrq = 1
        };

        class GPU
            : public General::IReadableWritable, public General::ITick
        {
        public:

            GPU(CPU::IInterruptible& p_interruptible);

            void next(std::uint32_t ticks) override;

            std::uint8_t read(std::uint16_t address) const override;
            virtual void write(std::uint16_t address, std::uint8_t datum) override;

            // LCDC
            bool isLcdOn() const;
            std::uint16_t getWindowTileMapOffset() const;
            bool isWindowDisplayOn() const;
            std::uint16_t getBgWindowTileDataOffset() const;
            std::uint16_t getBgWindowTileMapOffset() const;
            std::uint8_t getSpriteHeight() const;
            bool isSpriteDisplayOn() const;
            bool isBgWindowDisplayOn() const;

            // STAT
            void setMode(Mode mode);
            Mode getMode() const;
            void incrementLY();

        private:
            CPU::IInterruptible& interruptible;

            Memory::ReadableWritableMemory videoRam;  // 0x8000 - 0x9FFF (8KB)
            Memory::ReadableWritableMemory spriteRam; // 0xFE00 - 0xFE9F (160 bytes)

            std::vector<std::uint32_t> frameBuffer;
            // Clock
            std::uint32_t clock;

            // Registers
            std::uint8_t lcdcReg;
            std::uint8_t statReg;
            std::uint8_t scyReg;
            std::uint8_t scxReg;
            std::uint8_t lyReg;
            std::uint8_t lycReg;

        };
    }
}

#endif //GAMEBOY_GPU_H
