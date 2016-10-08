//
// Created by Kevin on 08/10/2016.
//

#include "General/Definitions.h"
#include "Rom.h"
#include "Cartridge.h"

#include "MemoryBankController.h"


using namespace std;
using namespace Gameboy;
using namespace Gameboy::Cartridge;

MemoryBankController::MemoryBankController(const Rom &p_rom, Ram &p_ram)
    : rom ( p_rom ), ram ( p_ram )
{}

uint8_t MemoryBankController::read(uint16_t address) const {
    if (address >= MemoryMap::RomBank0 && address < MemoryMap::VideoRam) {
        return rom.readExt(address - MemoryMap::RomBank0);
    } else if (address >= MemoryMap::RamBankN && address < MemoryMap::InternalRam) {
        return ram.readExt(address - MemoryMap::RamBankN);
    }

    throw runtime_error ("MBC: Invalid Read Address - " + to_string(address));
}

void MemoryBankController::write(uint16_t address, uint8_t datum) {
    if (address >= MemoryMap::RamBankN && address < MemoryMap::InternalRam) {
        ram.writeExt(address - MemoryMap::RamBankN, datum);
    } else {
        throw runtime_error("MBC: Invalid Write Address - " + to_string(address));
    }
}

