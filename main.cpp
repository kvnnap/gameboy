#include <iostream>

#include "Cartridge.h"

using namespace std;
using namespace Gameboy;

int main() {
    Cartridge cartridge;
    try {
        cartridge.load("game.gb");
        cout << cartridge << endl;
    } catch (const exception& ex) {
        cout << "Exception: " << ex.what() << endl;
    }
    return 0;
}