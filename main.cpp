#include <iostream>
#include <stdexcept>

#include "Cartridge/Cartridge.h"
#include "Memory/MemoryMappedIO.h"
#include "GPU/GPU.h"
#include "Input/Joypad.h"
#include "CPU/CPU.h"

using namespace std;
using namespace Gameboy::Cartridge;
using namespace Gameboy::Memory;
using namespace Gameboy::Input;
using namespace Gameboy::GPU;
using namespace Gameboy::CPU;

int main(int argc, char * argv[]) {
    try {
        if (argc != 2) {
            throw runtime_error("No file supplied");
        }
        Cartridge cartridge (argv[1]);
        MemoryMappedIO mmap (cartridge.getMBC());
        CPU cpu (mmap);
        Joypad joypad (cpu);
        GPU gpu (cpu);
        mmap.setInput(joypad);
        mmap.setGpu(gpu);

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
        }
        return EXIT_SUCCESS;
    } catch (const exception& ex) {
        cout << "Exception: " << ex.what() << endl;
        return EXIT_FAILURE;
    }
}