#include <iostream>
#include <stdexcept>

#include "Engine/IEngine.h"
#include "Engine/EngineFactory.h"


using namespace std;
using namespace Gameboy::Engine;

int main(int argc, char * argv[]) {
    try {
        if (argc != 2) {
            throw runtime_error("No file supplied");
        }

        unique_ptr<IEngine> engine = EngineFactory().make("RuntimeEngine");

        engine->start(argv[1]);

        return EXIT_SUCCESS;
    } catch (const exception& ex) {
        cout << "Exception: " << ex.what() << endl;
        return EXIT_FAILURE;
    }
}