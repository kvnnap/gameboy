//
// Created by kvnna on 27/12/2016.
//

#ifndef GAMEBOY_IOUTPUTDEVICE_H
#define GAMEBOY_IOUTPUTDEVICE_H

#include <cstdint>

namespace Gameboy {
    namespace GPU {
        class IVideoOutputDevice {
        public:
            virtual ~IVideoOutputDevice() {}
            virtual void writeFrame(const std::uint32_t* frameBuffer) = 0;
        };
    }
}

#endif //GAMEBOY_IOUTPUTDEVICE_H
