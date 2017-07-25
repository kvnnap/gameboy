//
// Created by kvnna on 25/07/2017.
//

#ifndef GAMEBOY_RUNTIMEENGINE_H
#define GAMEBOY_RUNTIMEENGINE_H

#include "Engine/IEngine.h"

namespace Gameboy {
    namespace Engine {
        class RuntimeEngine
            : public IEngine
        {
        public:

            RuntimeEngine();

            void start(const std::string& cartridgeFileName) override;
        };
    }
}

#endif //GAMEBOY_RUNTIMEENGINE_H
