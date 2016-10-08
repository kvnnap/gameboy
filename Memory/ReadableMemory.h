//
// Created by kvnna on 08/10/2016.
//

#ifndef GAMEBOY_READABLEMEMORY_H
#define GAMEBOY_READABLEMEMORY_H

#include <cstdint>
#include <vector>

namespace Gameboy {
    namespace Memory {

        using MemoryType = std::vector<std::uint8_t>;

        class ReadableMemory {
        public:
            virtual ~ReadableMemory();
            virtual void initialise(MemoryType& data);

            std::uint8_t readExt(std::size_t address) const;
            const std::uint8_t& readExtRef(std::size_t address = 0) const;
            std::size_t getSize() const;
        protected:
            MemoryType& getMemory();

        private:
            MemoryType memory;
        };

    }
}




#endif //GAMEBOY_READABLEMEMORY_H
