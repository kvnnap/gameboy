#include <iostream>
#include <stdexcept>
#include <GPU/SDLVideoDevice.h>

#include "Cartridge/Cartridge.h"
#include "Memory/MemoryMappedIO.h"
#include "GPU/GPU.h"
#include "Input/Joypad.h"
#include "Timer/Timer.h"
#include "CPU/CPU.h"
#include "CPU/ReducedCPU.h"

using namespace std;
using namespace Gameboy::Cartridge;
using namespace Gameboy::Memory;
using namespace Gameboy::Input;
using namespace Gameboy::GPU;
using namespace Gameboy::Timer;
using namespace Gameboy::CPU;

int main(int argc, char * argv[]) {
    try {
        if (argc != 2) {
            throw runtime_error("No file supplied");
        }

        // Load Cartridge
        Cartridge cartridge (argv[1]);

        // Setup Devices
        MemoryMappedIO mmap (cartridge.getMBC());
        ReducedCPU cpu (mmap);
        Joypad joypad (cpu);
        SDLVideoDevice sdlVideoDevice;
        GPU gpu (cpu, sdlVideoDevice);
        Timer timer (cpu);

        // Set Memory Mapped I/O
        mmap.setInput(joypad);
        mmap.setGpu(gpu);
        mmap.setTimer(timer);

        cout << cartridge << endl;
        cout << "Starting Execution" << endl;
        std::uint32_t clock;
        std::uint32_t duration;
        while(true) {
            clock = cpu.getTicks();
            cpu.next();
            duration = cpu.getTicks() - clock;
            //cout << "clock: " << clock << " duration: " << duration << endl;
            gpu.next(duration);
            timer.next(duration);
        }
        return EXIT_SUCCESS;
    } catch (const exception& ex) {
        cout << "Exception: " << ex.what() << endl;
        return EXIT_FAILURE;
    }
}