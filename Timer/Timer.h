//
// Created by Kevin on 22/10/2016.
//

#ifndef GAMEBOY_TIMER_H
#define GAMEBOY_TIMER_H

#include <cstdint>

#include "CPU/IInterruptible.h"
#include "General/IReadableWritable.h"
#include "General/ITick.h"

namespace Gameboy {
    namespace Timer {

        enum Register : std::uint16_t {
            DIV  = 0xFF04, // Divider Register
            TIMA = 0xFF05, // Timer Counter
            TMA  = 0xFF06, // Timer Modulo
            TAC  = 0xFF07  // Timer Control
        };

        enum RegisterOffset : std::uint8_t {
            OffDIV  = 0x00, // Divider Register
            OffTIMA = 0x01, // Timer Counter
            OffTMA  = 0x02, // Timer Modulo
            OffTAC  = 0x03  // Timer Control
        };

        enum Interrupt : std::uint8_t {
            TimerIrq = 2
        };

        class Timer
            : public General::IReadableWritable, public General::ITick
        {
        public:
            Timer (CPU::IInterruptible &p_interruptible);

            std::uint8_t read(std::uint16_t address) const override;
            void write(std::uint16_t address, std::uint8_t datum) override;

            bool next(std::uint32_t ticks) override;
        private:
            static const uint16_t ticksForSpeed[4];
            CPU::IInterruptible & interruptible;
            // Timer Registers
            std::uint8_t timerReg[4];

            // Clocks
            std::uint32_t divClock;
            std::uint32_t timerClock;

            uint8_t getTimerSpeed();

            bool isTimerEnabled();
        };
    }
}

#endif //GAMEBOY_TIMER_H
