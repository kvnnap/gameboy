//
// Created by Kevin on 07/10/2016.
//

#include <fstream>
#include <stdexcept>

#include "Cartridge.h"

using namespace std;
using namespace Gameboy::Cartridge;


Cartridge::Cartridge(const string& romPath) {
    load(romPath);
}

void Cartridge::load(const std::string &romPath) {
    ifstream file (romPath, ios::binary | ios::ate);

    if (!file) {
        throw runtime_error("Cannot open file: " + romPath);
    }

    //file.seekg(0, ios::end);
    streampos length = file.tellg();
    file.seekg(0, ios::beg);

    Memory::MemoryType memory;

    memory.resize(length);
    if (!file.read(reinterpret_cast<char *>(memory.data()), length)) {
        throw runtime_error("Cannot read data from file: " + romPath);
    }

    // Initialise this Rom with the loaded data from file
    rom.initialise(memory);
}

const Rom &Cartridge::getRom() const {
    return rom;
}

ostream& ::Gameboy::Cartridge::operator<<(std::ostream &strm, const Cartridge &cartridge) {
    strm << "Cartridge: " << endl << " Rom: " << cartridge.getRom();
    return strm;
}