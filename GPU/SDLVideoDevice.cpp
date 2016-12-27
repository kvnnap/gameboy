//
// Created by kvnna on 27/12/2016.
//

#include "SDLVideoDevice.h"
#include <stdexcept>

using namespace std;
using namespace Gameboy::GPU;

SDLVideoDevice::SDLVideoDevice(const string& windowName, uint32_t width, uint32_t height, bool accelerated)
    : windowName (windowName), width (width), height (height), window (nullptr), renderer (nullptr), texture (nullptr)
{
    //Initialize SDL
    if( SDL_Init( 0 ) != 0 ) {
        throw runtime_error (string("SDL could not initialize! SDL_Error: ") + SDL_GetError());
    }

    // Init video subsystem
    if (SDL_WasInit(SDL_INIT_VIDEO) == 0)
    {
        if (SDL_InitSubSystem(SDL_INIT_VIDEO) != 0) {
            throw runtime_error (string("Could not initialise SDL Video subsystem! SDL_Error: ") + SDL_GetError());
        }
    }

    //Create wndow
    window = SDL_CreateWindow(windowName.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN /*| SDL_WINDOW_RESIZABLE*/);
    if(window == nullptr)
    {
        throw runtime_error (string("Window could not be created! SDL_Error: ") + SDL_GetError());
    }

    renderer = SDL_CreateRenderer(window, -1, accelerated ? SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC : SDL_RENDERER_SOFTWARE);
    if(renderer == nullptr)
    {
        throw runtime_error (string("SDL_CreateRenderer error! SDL_Error: ") + SDL_GetError());
    }

    // Main texture
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);
    if (texture == nullptr) {
        throw runtime_error (string("SDL_CreateTexture error! SDL_Error: ") + SDL_GetError());
    }
}

SDLVideoDevice::~SDLVideoDevice() {
    // Destroy Texture
    SDL_DestroyTexture(texture);

    // Destroy Renderer
    SDL_DestroyRenderer(renderer);

    //Destroy gWindow
    SDL_DestroyWindow(window);

    // Quit SDL Video Subsystem
    SDL_QuitSubSystem(SDL_INIT_VIDEO);

    //Quit SDL subsystems
    SDL_Quit();
}

void SDLVideoDevice::writeFrame(const std::uint32_t *frameBuffer) {
    SDL_UpdateTexture(texture, nullptr, frameBuffer, width * sizeof(*frameBuffer));
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}
