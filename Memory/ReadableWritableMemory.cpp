//
// Created by kvnna on 08/10/2016.
//

#include "ReadableWritableMemory.h"

using namespace std;
using namespace Gameboy::Memory;

void ReadableWritableMemory::writeExt(size_t address, uint8_t datum) {
    getMemory().at(address) = datum;
}
