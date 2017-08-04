//
// Created by Kevin on 15/10/2016.
//

#include <stdexcept>
#include <SDL_events.h>
#include "Joypad.h"

using namespace std;
using namespace Gameboy::CPU;
using namespace Gameboy::Input;

Joypad::Joypad(IInterruptible& p_interruptible)
    : interruptible (p_interruptible),
      inputRegister (0x3F),
      keyPressMap (0xFF)
{}

uint8_t Joypad::read(uint16_t address) const {
    if (address == P1) {
        return inputRegister;
    }
    throw runtime_error("Wrong read address for Joypad: " + to_string(address));
}

void Joypad::write(uint16_t address, uint8_t datum) {
    if (address == P1) {
        inputRegister = datum & 0x30;
        updateInputRegister();
    } else {
        throw runtime_error("Wrong write address for Joypad: " + to_string(address));
    }
}

void Joypad::processInput() {
    uint8_t tempRegister = keyPressMap;

    SDL_Event e;
    while (SDL_PollEvent( &e ) != 0) {
        if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {

            // Determine key-up or key-down
            uint8_t key = static_cast<uint8_t>((e.type == SDL_KEYUP) ? 1 : 0);

            // Determine which key was pressed
            uint8_t bitNumber;

            switch (e.key.keysym.sym)
            {
                case SDLK_s:    bitNumber = 7; break; // Start
                case SDLK_a:    bitNumber = 6; break; // Select
                case SDLK_z:    bitNumber = 5; break; //B
                case SDLK_x:    bitNumber = 4; break; //A
                case SDLK_DOWN: bitNumber = 3; break;
                case SDLK_UP:   bitNumber = 2; break;
                case SDLK_LEFT: bitNumber = 1; break;
                case SDLK_RIGHT:bitNumber = 0; break;
                default: continue; break;
            }

            // Turn off this bit
            tempRegister &= ~(1 << bitNumber);

            // Set corresponding bit
            tempRegister |= (key << bitNumber);
        }
    }



    // Check if something changed
    if ((keyPressMap ^ tempRegister) != 0) {
        // Raise interrupt
        interruptible.requestInterrupt(JoypadIrq);
    }

    // Update key values
    keyPressMap = tempRegister;
    updateInputRegister();
}

void Joypad::updateInputRegister() {

    // Reset input Register
    inputRegister &= 0x30;

    // Extract Selector
    uint8_t selector = static_cast<uint8_t >(inputRegister & 0x30);

    // Note, selector 0 is illegal
    if (selector == 0) {
        throw runtime_error ("Selecting more than 1 group of keys is illegal");
    }

    // Check whether we're selecting some keys, if so, update
    if (selector != 0x30) {
        inputRegister |= ((selector & 0x20) ? keyPressMap : keyPressMap >> 4) & 0x0F;
    } else {
        inputRegister |= 0x0F;
    }
}
