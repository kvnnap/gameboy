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
#include <iostream>

using namespace std;
using namespace Gameboy::Engine;

RuntimeEngine::RuntimeEngine() {}

void RuntimeEngine::start(const string& cartridgeFileName) {

    // Load Cartridge
    Cartridge::Cartridge cartridge (cartridgeFileName);
    cout << cartridge << endl;

    // Setup Devices
    Memory::MemoryMappedIO mmap (cartridge.getMBC());
    CPU::CPU cpu (mmap);
    Input::Joypad joypad (cpu);
    GPU::SDLVideoDevice sdlVideoDevice;
    GPU::GPU gpu (cpu, mmap, sdlVideoDevice);
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
        cpu.next();
        duration = cpu.getTicks() - clock;
        if (gpu.next(duration)) {
            joypad.processInput();
        }
        timer.next(duration);
    }
}


