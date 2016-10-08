//
// Created by Kevin on 07/10/2016.
//

#ifndef GAMEBOY_CARTRIDGE_H
#define GAMEBOY_CARTRIDGE_H

#include <ostream>
#include "Rom.h"

namespace Gameboy {

    namespace Cartridge {



        // Cartridge contains
        // -- Rom
        // -- MBC
        // -- External Ram (backed by battery)
        class Cartridge {
        public:
            Cartridge(const std::string &romPath);

            void load(const std::string &romPath);

            const Rom& getRom() const;
        private:
            Rom rom;
        };

        std::ostream &operator<<(std::ostream &strm, const Cartridge &cartridge);
    }
}

#endif //GAMEBOY_CARTRIDGE_H
