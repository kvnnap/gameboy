//
// Created by kvnna on 25/07/2017.
//

#include "Engine/RuntimeEngine.h"

#include "GPU/SDLVideoDevice.h"

#include "Cartridge/Cartridge.h"
#include "Memory/MemoryMappedIO.h"
#include "GPU/GPU.h"
#include "Input/Joypad.h"
#include "Timer/Timer.h"
#include "CPU/CPU.h"
#include "CPU/ReducedCPU.h"

using namespace std;
using namespace Gameboy::Engine;

RuntimeEngine::RuntimeEngine() {}

void RuntimeEngine::start(const string& cartridgeFileName) {

    // Load Cartridge
    Cartridge::Cartridge cartridge (cartridgeFileName);

    // Setup Devices
    Memory::MemoryMappedIO mmap (cartridge.getMBC());
    CPU::CPU cpu (mmap);
    Input::Joypad joypad (cpu);
    GPU::SDLVideoDevice sdlVideoDevice;
    GPU::GPU gpu (cpu, sdlVideoDevice);
    Timer::Timer timer (cpu);

    // Set Memory Mapped I/O
    mmap.setInput(joypad);
    mmap.setGpu(gpu);
    mmap.setTimer(timer);

    // Print out disassembly

    /*vector<DebugInstruction> debugInstructions = cpu.disassemble(1000);
    for (const DebugInstruction& dbgInstr : debugInstructions) {
        cout << dbgInstr.toString() << endl;
    }*/

    uint32_t clock;
    uint32_t duration;
    while (true) {
        clock = cpu.getTicks();
        //cout << cpu.disassembleNext().toString() << endl;
        cpu.next();
        duration = cpu.getTicks() - clock;
        //cout << "clock: " << clock << " duration: " << duration << endl;
        gpu.next(duration);
        timer.next(duration);
    }
}


