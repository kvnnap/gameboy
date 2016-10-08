//
// Created by Kevin on 08/10/2016.
//

#ifndef GAMEBOY_MEMORYBANKCONTROLLER_H
#define GAMEBOY_MEMORYBANKCONTROLLER_H

#include "General/IReadableWritable.h"
#include "Memory/ReadableWritableMemory.h"

namespace Gameboy {
    namespace Cartridge {

        class Rom;
        using Ram = Memory::ReadableWritableMemory;

        class MemoryBankController
            : public General::IReadableWritable
        {
        public:

            MemoryBankController(const Rom& p_rom, Ram& p_ram);

            std::uint8_t read(std::uint16_t address) const override;
            void write(std::uint16_t address, std::uint8_t datum) override;

        private:
            const Rom& rom;
            Ram& ram;
        };
    }
}

#endif //GAMEBOY_MEMORYBANKCONTROLLER_H
