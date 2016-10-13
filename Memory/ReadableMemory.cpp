//
// Created by kvnna on 08/10/2016.
//

#include "ReadableMemory.h"

using namespace std;
using namespace Gameboy::Memory;

ReadableMemory::~ReadableMemory() {}

uint8_t ReadableMemory::readExt(size_t address) const {
    return memory.at(address);
}

void ReadableMemory::initialise(MemoryType&& data) {
    memory = move(data);
}

MemoryType &ReadableMemory::getMemory() {
    return memory;
}

const uint8_t &ReadableMemory::readExtRef(size_t address) const {
    return memory.at(address);
}

std::size_t ReadableMemory::getSize() const {
    return memory.size();
}

void ReadableMemory::initialise(MemoryType &data) {
    memory = move(data);
}


