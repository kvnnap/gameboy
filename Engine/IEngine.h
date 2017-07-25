//
// Created by kvnna on 25/07/2017.
//

#ifndef GAMEBOY_IENGINE_H
#define GAMEBOY_IENGINE_H

#include <string>

namespace Gameboy {
    namespace Engine {
        class IEngine {
        public:
            virtual ~IEngine();

            // Start
            virtual void start(const std::string& cartridgeFileName) = 0;
        };
    }
}



#endif //GAMEBOY_IENGINE_H
