#include <iostream>
#include <stdexcept>

#include "Cartridge/Cartridge.h"
#include "Memory/MemoryMappedIO.h"
#include "CPU/CPU.h"

using namespace std;
using namespace Gameboy::Cartridge;
using namespace Gameboy::Memory;
using namespace Gameboy::CPU;

int main(int argc, char * argv[]) {
    try {
        if (argc != 2) {
            throw runtime_error("No file supplied");
        }
        Cartridge cartridge (argv[1]);
        MemoryMappedIO mmap (cartridge.getMBC());
        CPU cpu (mmap);

        cout << cartridge << endl;
        cout << "Starting Execution" << endl;
        while(true) {
            cpu.next();
        }
        return EXIT_SUCCESS;
    } catch (const exception& ex) {
        cout << "Exception: " << ex.what() << endl;
        return EXIT_FAILURE;
    }
}