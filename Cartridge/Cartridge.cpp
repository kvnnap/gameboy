//
// Created by Kevin on 07/10/2016.
//

#include <fstream>
#include <stdexcept>

#include "Cartridge.h"
#include "MBCFactory.h"
#include "General/FileOperations.h"

using namespace std;
using namespace Gameboy::Cartridge;


Cartridge::Cartridge(const string& romPath) {
    load(romPath);
}

void Cartridge::load(const std::string &romPath) {
    Memory::MemoryType memory = Util::FileOperations::loadBinaryFile(romPath);

    // Initialise this Rom with the loaded data from file
    rom.initialise(memory);

    // Initialise ram
    memory.resize(rom.getNumRamBanks() * 8192);
    ram.initialise(memory);

    // Initialise MBC
    mbc = MBCFactory(rom, ram).make(rom.getCartridgeTypeAsString());
}

const Rom &Cartridge::getRom() const {
    return rom;
}

const Ram &Cartridge::getRam() const {
    return ram;
}

MemoryBankController &Cartridge::getMBC() {
    return *mbc;
}

ostream& ::Gameboy::Cartridge::operator<<(std::ostream &strm, const Cartridge &cartridge) {
    strm << "Cartridge: " << endl << cartridge.getRom() << endl << "Ram size: " << cartridge.getRam().getSize() << endl;
    return strm;
}