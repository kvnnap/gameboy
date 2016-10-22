//
// Created by Kevin on 15/10/2016.
//

#ifndef GAMEBOY_JOYPAD_H
#define GAMEBOY_JOYPAD_H

#include "CPU/IInterruptible.h"
#include "General/IReadableWritable.h"

namespace Gameboy {
    namespace Input {

        enum Register : std::uint16_t {
            P1 = 0xFF00
        };

        enum Interrupt : std::uint8_t {
            JoypadIrq = 4
        };

        class Joypad
            : public General::IReadableWritable
        {
        public:

            Joypad(CPU::IInterruptible& p_interruptible);

            std::uint8_t read(std::uint16_t address) const override;
            void write(std::uint16_t address, std::uint8_t datum) override;

        private:
            CPU::IInterruptible& interruptible;
            std::uint8_t inputRegister;
        };
    }
}

#endif //GAMEBOY_JOYPAD_H
