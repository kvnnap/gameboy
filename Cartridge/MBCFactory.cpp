//
// Created by Kevin on 08/10/2016.
//

#include "MBCFactory.h"

#include "General/StringOperations.h"

using namespace std;
using namespace Gameboy::Cartridge;

MBCFactory::MBCFactory(const Rom &p_rom, Ram &p_ram)
        : rom ( p_rom ), ram ( p_ram )
{}

unique_ptr<MemoryBankController> MBCFactory::make(const string &value) const {
    // split name, if second element exists, it could represent and MBC
    const vector<string> splitValue = Util::StringOperations::split(value, '+');

    if (splitValue.size() >= 2) {
        const string name = splitValue[1];
        if (name == "MBC1") {
            throw runtime_error ("Memory Bank Controller 1 not yet implemented");
        } else if (name == "MBC2") {
            throw runtime_error ("Memory Bank Controller 2 not yet implemented");
        }
    }

    // Use default MBC, which signifies no MBC
    return unique_ptr<MemoryBankController>(new MemoryBankController(rom, ram));
}
