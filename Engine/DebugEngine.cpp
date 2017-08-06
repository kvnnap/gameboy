//
// Created by kvnna on 25/07/2017.
//

#include "Engine/DebugEngine.h"
#include "GPU/SDLVideoDevice.h"
#include "Cartridge/Cartridge.h"
#include "Memory/MemoryMappedIO.h"
#include "CPU/CPU.h"
#include "CPU/ReducedCPU.h"
#include "CPU/Registers.h"
#include "CPU/DebugInstruction.h"
#include "General/StringOperations.h"

#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;
using namespace Gameboy::Engine;
using namespace Util;

DebugEngine::DebugEngine()
    : step(true)
{
}

void DebugEngine::start(const string &cartridgeFileName) {
    // Load Cartridge
    Cartridge::Cartridge cartridge (cartridgeFileName);

    cout << cartridge << endl;
    // Setup Devices
    Memory::MemoryMappedIO mmap(cartridge.getMBC());
    CPU::CPU cpu(mmap);
    Input::Joypad joypad(cpu);
    GPU::SDLVideoDevice sdlVideoDevice;
    GPU::GPU gpu(cpu, mmap, sdlVideoDevice);
    Timer::Timer timer(cpu);

    // Set Memory Mapped I/O
    mmap.setInput(joypad);
    mmap.setGpu(gpu);
    mmap.setTimer(timer);

    try {
        uint32_t clock;
        uint32_t duration;
        bool running = true;
        while (running) {

            if (needToBreak(cpu)) {
                running = processInput(cpu, mmap);
                if (!running) {
                    continue;
                }
            }

            clock = cpu.getTicks();
            cpu.next();
            duration = cpu.getTicks() - clock;
            if (gpu.next(duration)) {
                running = joypad.processInput();
            }
            timer.next(duration);
        }
    } catch (...) {
        using namespace Gameboy::CPU;
        cout << cpu.getRegisters().toString() << endl;
        cout << "EI: " << (cpu.isInterruptMasterEnabled() ? "1" : "0") << endl;
        cout << "Ticks: " << cpu.getTicks() << endl;
        vector<DebugInstruction> debugInstructions = cpu.disassemble(3);
        for (const DebugInstruction& dbgInstr : debugInstructions) {
            cout << dbgInstr.toString() << endl;
        }
        throw;
    }
}

bool DebugEngine::processInput(CPU::ICPU& cpu, Memory::MemoryMappedIO& mmap) {
    using namespace Gameboy::CPU;

    Registers& registers = cpu.getRegisters();
    cout << registers.toString() << endl;
    cout << "EI: " << (cpu.isInterruptMasterEnabled() ? "1" : "0")
         << " IE: " << hex << setw(2) << static_cast<uint16_t>(mmap.read(0xFFFF))
         << " IF: " << hex << setw(2) << static_cast<uint16_t>(mmap.read(0xFF0F)) << endl;

    vector<DebugInstruction> debugInstructions = cpu.disassemble(3);
    for (const DebugInstruction& dbgInstr : debugInstructions) {
        cout << dbgInstr.toString() << endl;
    }


    string input;
    getline(cin, input);
    if (input == "q") {
        return false;
    } else if (input == "n") {
        step = true;
    } else if (input[0] == 'b') {
        vector<string> vec = Util::StringOperations::split(input, ' ');
        uint16_t addr;
        {
            stringstream ss;
            ss << hex << vec.at(1);
            ss >> addr;
        }

        breakpoints.insert(addr);
    }

    return true;
}

bool DebugEngine::needToBreak(CPU::ICPU& cpu) {
    CPU::DebugInstruction debugInstruction = cpu.disassembleNext();

    // Check breakpoints
    if (breakpoints.find(debugInstruction.Address) != breakpoints.end()) {
        return true;
    }

    // Check watchers
    if (step){
        step = false;
        return true;
    }

    return false;
}
