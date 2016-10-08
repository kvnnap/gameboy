//
// Created by Kevin on 08/10/2016.
//

#ifndef GAMEBOY_MBCFACTORY_H
#define GAMEBOY_MBCFACTORY_H

#include "General/IFactory.h"
#include "MemoryBankController.h"

namespace Gameboy {
    namespace Cartridge {
        class MBCFactory
            : public IFactory<MemoryBankController>
        {
        public:

            MBCFactory(const Rom& p_rom, Ram& p_ram);

            std::unique_ptr<MemoryBankController> make(const std::string& value) const override;

        private:
            const Rom& rom;
            Ram& ram;
        };
    }
}




#endif //GAMEBOY_MBCFACTORY_H
