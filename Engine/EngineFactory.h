//
// Created by kvnna on 25/07/2017.
//

#ifndef GAMEBOY_ENGINEFACTORY_H
#define GAMEBOY_ENGINEFACTORY_H

#include "Factory/IFactory.h"
#include "Engine/IEngine.h"
#include "Engine/RuntimeEngine.h"

namespace Gameboy
{
    namespace Engine
    {
        class EngineFactory
            : public Factory::IFactory<IEngine>
        {
        public:
            std::unique_ptr<IEngine> make(const std::string& name) const override;
        };
    }
}




#endif //GAMEBOY_ENGINEFACTORY_H
