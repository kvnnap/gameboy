//
// Created by Kevin on 07/10/2016.
//

#ifndef GAMEBOY_CARTRIDGE_H
#define GAMEBOY_CARTRIDGE_H

#include <ostream>
#include <memory>

#include "Rom.h"
#include "Memory/ReadableWritableMemory.h"
#include "MemoryBankController.h"

namespace Gameboy {

    namespace Cartridge {

        using Ram = Memory::ReadableWritableMemory;

        // Cartridge contains
        // -- Rom
        // -- MBC
        // -- External Ram (backed by battery)
        class Cartridge {
        public:
            Cartridge(const std::string &romPath);

            void load(const std::string &romPath);

            const Rom& getRom() const;
            const Ram& getRam() const;
            MemoryBankController& getMBC();
        private:
            Rom rom;
            Ram ram;
            std::unique_ptr<MemoryBankController> mbc;
        };

        std::ostream &operator<<(std::ostream &strm, const Cartridge &cartridge);
    }
}

#endif //GAMEBOY_CARTRIDGE_H
