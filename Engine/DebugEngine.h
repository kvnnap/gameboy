//
// Created by kvnna on 25/07/2017.
//

#ifndef GAMEBOY_DEBUGENGINE_H
#define GAMEBOY_DEBUGENGINE_H

#include <set>
#include "Engine/IEngine.h"
#include "CPU/ICPU.h"

namespace Gameboy {
    namespace Engine {
        class DebugEngine
                : public IEngine
        {
        public:

            DebugEngine();

            void start(const std::string& cartridgeFileName) override;

        private:
            bool needToBreak(CPU::ICPU& cpu);

            bool processInput(CPU::ICPU& cpu);

            std::set<uint16_t> breakpoints;
            bool step;
        };
    }
}

#endif //GAMEBOY_DEBUGENGINE_H
