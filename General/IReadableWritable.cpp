//
// Created by kvnna on 09/10/2016.
//

#include "General/IReadableWritable.h"

using namespace std;
using namespace Gameboy::General;

void IReadableWritable::write16(std::uint16_t address, std::uint16_t value) {
    write(address, static_cast<uint8_t>(value));
    write(static_cast<uint16_t>(address + 1), static_cast<uint8_t>(value >> 8));
}
