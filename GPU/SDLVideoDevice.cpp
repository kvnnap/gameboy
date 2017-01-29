//
// Created by kvnna on 27/12/2016.
//

#include "SDLVideoDevice.h"
#include <stdexcept>

using namespace std;
using namespace Gameboy::GPU;

SDLVideoDevice::SDLVideoDevice(const string& windowName, uint32_t width, uint32_t height, bool accelerated)
    : windowName (windowName),
      width (width),
      height (height),
      accelerated (accelerated),
      initialised (false),
      videoSubsystemInitialised (false),
      window (nullptr),
      renderer (nullptr),
      texture (nullptr)
{
    //Initialize SDL
    if (SDL_Init(0) != 0) {
        sdlFatalError("SDL could not initialize");
    }
    initialised = true;

    // Init video subsystem - refcount increases if already initialised
    if (SDL_InitSubSystem(SDL_INIT_VIDEO) != 0) {
        sdlFatalError("Could not initialise SDL Video subsystem");
    }
    videoSubsystemInitialised = true;

    //Create window
    window = SDL_CreateWindow(windowName.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN /*| SDL_WINDOW_RESIZABLE*/);
    if (window == nullptr) {
        sdlFatalError("Window could not be created");
    }

    renderer = SDL_CreateRenderer(window, -1, accelerated ? SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC : SDL_RENDERER_SOFTWARE);
    if (renderer == nullptr) {
        sdlFatalError("SDL_CreateRenderer error");
    }

    // Main texture
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);
    if (texture == nullptr) {
        sdlFatalError("SDL_CreateTexture error");
    }

    // Clear Display (Optional)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

SDLVideoDevice::~SDLVideoDevice() {
    destroy();
}

void SDLVideoDevice::destroy() {
    // Destroy Texture
    if (texture != nullptr) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }

    // Destroy Renderer
    if (renderer != nullptr) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    //Destroy gWindow
    if (window != nullptr) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    // Quit SDL Video Subsystem
    if (videoSubsystemInitialised) {
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        videoSubsystemInitialised = false;
    }

    //Quit SDL subsystems
    if (initialised) {
        if (SDL_WasInit(0) == 0) {
            SDL_Quit();
        }
        initialised = false;
    }
}

void SDLVideoDevice::sdlFatalError(const std::string &errorMessage) {
    destroy();
    throw runtime_error (errorMessage + " - SDL Error: " +  SDL_GetError());
}

void SDLVideoDevice::render(const std::uint32_t *frameBuffer) {
    SDL_UpdateTexture(texture, nullptr, frameBuffer, width * sizeof(*frameBuffer));
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
    // Quick fix
    SDL_Event e;
    while( SDL_PollEvent( &e ) != 0 ) {}
}