//
// Created by kvnna on 29/07/2017.
//

#include "IReadable.h"

using namespace std;
using namespace Gameboy::General;

IReadable::~IReadable()
{}

uint16_t IReadable::read16(uint16_t address) const {
    return (read(static_cast<uint16_t>(address + 1)) << 8) | (read(address));
}
