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
        CPU cpu (mmap);
        Joypad joypad (cpu);
        SDLVideoDevice sdlVideoDevice;
        GPU gpu (cpu, sdlVideoDevice);
        Timer timer (cpu);

        // Set Memory Mapped I/O
        mmap.setInput(joypad);
        mmap.setGpu(gpu);
        mmap.setTimer(timer);

        cout << cartridge << endl;

        // Print out disassembly

        /*vector<DebugInstruction> debugInstructions = cpu.disassemble(1000);
        for (const DebugInstruction& dbgInstr : debugInstructions) {
            cout << dbgInstr.toString() << endl;
        }*/

        cout << "Starting Execution" << endl;
        std::uint32_t clock;
        std::uint32_t duration;
        while(true) {
            clock = cpu.getTicks();
            cout << cpu.disassembleNext().toString() << endl;
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