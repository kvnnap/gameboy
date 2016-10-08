//
// Created by Kevin on 08/10/2016.
//

#ifndef GAMEBOY_IFACTORY_H
#define GAMEBOY_IFACTORY_H

#include <string>
#include <memory>

template <class T>
class IFactory {
public:
    virtual ~IFactory() {}
    virtual std::unique_ptr<T> make(const std::string& name) const = 0;
private:

};

#endif //GAMEBOY_IFACTORY_H
