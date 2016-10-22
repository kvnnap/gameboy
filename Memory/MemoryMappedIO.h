//
// Created by kvnna on 09/10/2016.
//

#ifndef GAMEBOY_MEMORYMAPPEDIO_H
#define GAMEBOY_MEMORYMAPPEDIO_H

#include <string>
#include "Timer/Timer.h"
#include "GPU/GPU.h"
#include "Input/Joypad.h"
#include "Cartridge/Cartridge.h"
#include "General/IReadableWritable.h"

namespace Gameboy {
    namespace Memory {
        class MemoryMappedIO
            : public General::IReadableWritable
        {
        public:

            MemoryMappedIO(Cartridge::MemoryBankController& p_mbc);

            std::uint8_t read(std::uint16_t address) const override;
            void write(std::uint16_t address, std::uint8_t datum) override;

            void setInput(Input::Joypad &p_joypad);

            void setGpu(GPU::GPU &p_gpu);
            void setTimer(Timer::Timer &p_timer);

        private:
            // has rom and its own ram
            Memory::ReadableMemory bios;
            Cartridge::MemoryBankController& mbc;
            Memory::ReadableWritableMemory internalRam;

            Input::Joypad * joypad;
            GPU::GPU * gpu;
            Timer::Timer * timer;
            Memory::ReadableWritableMemory ioPorts;
            Memory::ReadableWritableMemory zeroPageRam;
            uint8_t interruptEnableRegister;
            bool lowMemoryIsCartridge;
            // Contiguous Physical Addressing
            // 0x0000 - 0x1FFF = Video RAM (8KB)
            // 0x2000 - 0x3FFF = Internal RAM (8KB)
            // 0x4000 - 0x409F = Sprite RAM (160 bytes)
            // 0x40A0 - 0x40EB = I/O (Usable) (76 bytes)
            // 0x40EC - 0x416A = Zero Page (127 bytes)
            // 0x416B - 0x416B = Interrupt Enable Register (1 byte)
        };
    }
}



#endif //GAMEBOY_MEMORYMAPPEDIO_H
