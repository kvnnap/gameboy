#include <iostream>
#include <stdexcept>

#include "Cartridge/Cartridge.h"

using namespace std;
using namespace Gameboy::Cartridge;

int main(int argc, char * argv[]) {
    try {
        if (argc != 2) {
            throw runtime_error("No file supplied");
        }
        Cartridge cartridge (argv[1]);
        cout << cartridge << endl;
        return EXIT_SUCCESS;
    } catch (const exception& ex) {
        cout << "Exception: " << ex.what() << endl;
        return EXIT_FAILURE;
    }
}