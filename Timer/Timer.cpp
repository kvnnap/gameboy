//
// Created by Kevin on 22/10/2016.
//

#include <stdexcept>
#include "Timer.h"

using namespace std;
using namespace Gameboy::CPU;
using namespace Gameboy::Timer;

Timer::Timer(IInterruptible &p_interruptible)
    : interruptible (p_interruptible),
      timerReg (),
      divClock (),
      timerClock ()
{}

// Cpu speed is 4,194,304 Hz
const uint16_t Timer::ticksForSpeed[4] = {
        1024, // 4096 Hz
          16, // 262144 Hz
          64, // 65536 Hz
         256  // 16384 Hz
};

uint8_t Timer::getTimerSpeed() {
    return static_cast<uint8_t>(timerReg[OffTAC] & 0x03);
}

bool Timer::isTimerEnabled() {
    return (timerReg[OffTAC] & (1 << 2)) != 0;
}

bool Timer::next(uint32_t ticks) {
    // Handle Div Register
    divClock += ticks;
    if (divClock >= ticksForSpeed[3]) {
        divClock -= ticksForSpeed[3];
        ++timerReg[OffDIV];
    }

    // Handle Timer Register
    if (isTimerEnabled()) {
        timerClock += ticks;
        uint16_t ticksFS = ticksForSpeed[getTimerSpeed()];
        while (timerClock >= ticksFS) {
            timerClock -= ticksFS;
            if (++timerReg[OffTIMA] == 0) {
                // overflow - Load Module Value
                timerReg[OffTIMA] = timerReg[OffTMA];
                // interrupt
                interruptible.requestInterrupt(TimerIrq);
            }
        }
    } /*else {
        timerClock = 0; // Not sure about this, but I don't think so
    }*/

    return true;
}

uint8_t Timer::read(uint16_t address) const {
    if (address >= DIV && address <= TAC) {
        return timerReg[address - DIV];
    }
    throw runtime_error("Wrong read address for Timer: " + to_string(address));
}

void Timer::write(uint16_t address, uint8_t datum) {
    if (address >= DIV && address <= TAC) {
        const RegisterOffset offset = static_cast<RegisterOffset>(address - DIV);
        switch (offset) {
            case OffDIV:
                timerReg[offset] = 0;
                divClock = 0; // Not sure about this, but I don't think so
                timerClock = 0; // Resetting
                break;
            case OffTIMA:
            case OffTMA:
                timerReg[offset] = datum;
                break;
            case OffTAC:
                timerReg[offset] = datum; //static_cast<uint8_t>(datum & 0x07);
                timerClock = 0;
                if (datum > 7) {
                    throw runtime_error ("Writing value: " + to_string(datum) + " to Timer Control Register");
                }
                break;
        }
    } else {
        throw runtime_error("Wrong write address for Timer: " + to_string(address));
    }
}
