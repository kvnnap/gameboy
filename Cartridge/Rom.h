//
// Created by kvnna on 08/10/2016.
//

#ifndef GAMEBOY_ROM_H
#define GAMEBOY_ROM_H

#include <string>
#include <vector>
#include <cstdint>

#include "Memory/ReadableMemory.h"

namespace Gameboy {

    namespace Cartridge {

        enum RomAddress : std::uint16_t {
            BeginInstruction = 0x0100,
            ScrollingGraphic = 0x0104,
            Title = 0x0134, //null terminated if < 15
            ColorGB = 0x0143,
            LicenseeHigh = 0x0144,
            LicenseeLow = 0x0145,
            GBOrSGB = 0x0146,
            CartridgeTypeAddr = 0x0147,
            RomSize = 0x0148,
            RamSize = 0x0149,
            DestinationCode = 0x014A,
            LicenseeCode = 0x014B,
            MaskRomVerNum = 0x014C,
            ComplementCheck = 0x014D,
            ChecksumHigh = 0x014E,
            ChecksumLow = 0x014F
        };

        enum CartridgeType : std::uint8_t {
            RomOnly = 0x00,
            RomMBC1 = 0x01,
            RomMBC1Ram = 0x02,
            RomMBC1RamBatt = 0x03,

            RomMBC2 = 0x05,
            RomMBC2Batt = 0x06,

            RomRam = 0x08,
            RomRamBatt = 0x09,

            RomMMM01 = 0x0B,
            RomMMM01SRam = 0x0C,
            RomMMM01SRamBatt = 0x0D,

            RomMBC3TimerBatt = 0x0F,
            RomMBC3TimerRamBatt = 0x10,
            RomMBC3 = 0x11,
            RomMBC3Ram = 0x12,
            RomMBC3RamBatt = 0x13,

            RomMBC5 = 0x19,
            RomMBC5Ram = 0x1A,
            RomMBC5RamBatt = 0x1B,
            RomMBC5Rumble = 0x1C,
            RomMBC5RamRumbleSRam = 0x1D,
            RomMBC5RamRumbleSRamBatt = 0x1E,

            PocketCamera = 0x1F,
            BandaiTama5 = 0xFD,
            HudsonHuc3 = 0xFE,
            HudsonHuc1 = 0xFF
        };

        class Rom
            : public Memory::ReadableMemory
        {

        public:
            std::string getTitle() const;

            bool isColorGB() const;

            bool isSuperGB() const;

            CartridgeType getCartridgeType() const;

            std::string getCartridgeTypeAsString() const;

            std::uint8_t getRomSizeValue() const;

            std::uint8_t getRamSizeValue() const;

            std::uint8_t getNumRomBanks() const;

            std::uint8_t getNumRamBanks() const;

            bool isJapanese() const;

            std::uint8_t getLicenseeCode() const;

            std::uint16_t getExtendedLicenseeCode() const;

            std::uint8_t getMaskRomVerNum() const;

            bool validChecksum() const;

        private:
            //std::vector<std::uint8_t> cartridgeData;

        };

        std::ostream &operator<<(std::ostream &strm, const Rom &rom);
    }
}
#endif //GAMEBOY_ROM_H
