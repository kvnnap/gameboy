//
// Created by Kevin on 15/10/2016.
//

#include <stdexcept>
#include "Joypad.h"

using namespace std;
using namespace Gameboy::CPU;
using namespace Gameboy::Input;

Joypad::Joypad(IInterruptible& p_interruptible)
    : interruptible (p_interruptible),
      inputRegister (0x0F)
{}

uint8_t Joypad::read(uint16_t address) const {
    if (address == P1) {
        return inputRegister;
    }
    throw runtime_error("Wrong read address for Joypad: " + to_string(address));
}

void Joypad::write(uint16_t address, uint8_t datum) {
    if (address == P1) {
        if (datum & 0x10) {
            // read right, left, up, down
        } else if (datum & 0x20) {
            // read A, B, Select, Start
        }
    } else {
        throw runtime_error("Wrong write address for Joypad: " + to_string(address));
    }
}
