//
// Created by kvnna on 23/07/2017.
//

#ifndef GAMEBOY_DEBUGINSTRUCTION_H
#define GAMEBOY_DEBUGINSTRUCTION_H

#include <string>

namespace Gameboy {
    namespace CPU {
        class DebugInstruction {
        public:


            DebugInstruction();
            DebugInstruction(
                    const std::string& strInstruction,
                    std::uint8_t length,
                    bool isUnsigned = false);

            std::string toString() const;

            std::string StringInstruction;
            std::uint16_t Address;
            std::uint16_t Operand16;
            std::uint8_t Operand;
            std::uint8_t Opcode;
            std::uint8_t Length;
            bool IsExtended;
            bool IsUnsigned;
        };
    }
}

#endif //GAMEBOY_DEBUGINSTRUCTION_H
