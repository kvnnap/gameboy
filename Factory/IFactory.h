//
// Created by kvnna on 25/07/2017.
//

#ifndef GAMEBOY_IFACTORY_H
#define GAMEBOY_IFACTORY_H

#include <string>
#include <memory>

namespace Gameboy {
    namespace Factory{
        template <class T>
        class IFactory {
        public:
            virtual ~IFactory() {}
            virtual std::unique_ptr<T> make(const std::string& name) const = 0;
        private:

        };
    }
}

#endif //GAMEBOY_IFACTORY_H
