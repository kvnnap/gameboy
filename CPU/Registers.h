//
// Created by kvnna on 09/10/2016.
//

#ifndef GAMEBOY_REGISTERS_H
#define GAMEBOY_REGISTERS_H

#include <cstdint>
#include <string>

namespace Gameboy {
    namespace CPU {

        enum RegisterIndex : std::uint8_t {
            F = 0,
            A = 1,
            C = 2,
            B = 3,
            E = 4,
            D = 5,
            L = 6,
            H = 7
        };

        std::string RegisterIndexToString(RegisterIndex registerIndex);

        enum RegisterIndex16 : std::uint8_t {
            AF = 0,
            BC = 2,
            DE = 4,
            HL = 6,
            SP = 8,
            PC = 10
        };

        std::string RegisterIndexToString16(RegisterIndex16 registerIndex16);

        enum FlagRegister : std::uint8_t {
            CarryFlag = (1 << 4),
            HalfCarryFlag = (1 << 5),
            SubtractFlag = (1 << 6),
            ZeroFlag = (1 << 7)
        };

        class Registers {
        public:

            std::uint16_t read16 (std::uint8_t srcIndex) const;
            void write16 (std::uint8_t destIndex, std::uint16_t value);
            std::string toString() const;

            Registers();

            // The below unions and the way they will be accessed is
            // probably against the C++ standard, but GCC allows this
            // Although, thinking about this. The compiler can never
            // know with which member a value was set in separate calls.
            // So probably, unless accessing both members incorrectly in the same
            // function, this shortcut will work on all compilers
            union {
                struct {
                    union {
                        struct {
                            uint8_t f;
                            uint8_t a;
                        };
                        uint16_t af;
                    };

                    union {
                        struct {
                            uint8_t c;
                            uint8_t b;
                        };
                        uint16_t bc;
                    };

                    union {
                        struct {
                            uint8_t e;
                            uint8_t d;
                        };
                        uint16_t de;
                    };

                    union {
                        struct {
                            uint8_t l;
                            uint8_t h;
                        };
                        uint16_t hl;
                    };

                    uint16_t sp;
                    uint16_t pc;
                };
                uint8_t reg[12];
            };
        };
    }
}


#endif //GAMEBOY_REGISTERS_H
