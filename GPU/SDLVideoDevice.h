//
// Created by kvnna on 27/12/2016.
//

#ifndef GAMEBOY_SDLVIDEODEVICE_H
#define GAMEBOY_SDLVIDEODEVICE_H

#include "IVideoOutputDevice.h"
#include "SDL.h"
#include <string>

namespace Gameboy {
    namespace GPU {

        class SDLVideoDevice
            : public IVideoOutputDevice
        {
        public:

            // Constructor and Destructor
            SDLVideoDevice(
                    const std::string& windowName = "Gameboy",
                    uint32_t width = 160,
                    uint32_t height = 144,
                    bool accelerated = true);
            virtual ~SDLVideoDevice();
            void destroy();
            void sdlFatalError(const std::string &errorMessage);

            // IOutputDevice
            void render(const std::uint32_t *frameBuffer) override;

            // Data
            const std::string windowName;
            const uint32_t width;
            const uint32_t height;

            bool accelerated;
            bool initialised;
            bool videoSubsystemInitialised;
            //The SDL Window we'll be rendering to
            SDL_Window* window;

            // The SDL Renderer
            SDL_Renderer* renderer;

            //The SDL Texture we will load and show on the screen
            SDL_Texture* texture;
        };

    }
}

#endif //GAMEBOY_SDLVIDEODEVICE_H
