//
// Created by kvnna on 25/07/2017.
//

#include <stdexcept>
#include "EngineFactory.h"

using namespace std;
using namespace Gameboy::Engine;

unique_ptr<IEngine> EngineFactory::make(const string &name) const {
    if (name == "RuntimeEngine")
    {
        return unique_ptr<IEngine>(new RuntimeEngine());
    }

    throw runtime_error ("Unknown Engine Name: " + name);
}
