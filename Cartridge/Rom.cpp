//
// Created by kvnna on 08/10/2016.
//

#include <ostream>
#include <stdexcept>

#include "Rom.h"

using namespace std;
using namespace Gameboy::Cartridge;

string Rom::getTitle() const {
    return string(reinterpret_cast<const char *>(&readExtRef(Title)), ColorGB - Title);
}

bool Rom::isColorGB() const {
    return readExt(ColorGB) == 0x80;
}

bool Rom::isSuperGB() const {
    return readExt(GBOrSGB) == 0x03;
}

CartridgeType Rom::getCartridgeType() const {
    return static_cast<CartridgeType>(readExt(CartridgeTypeAddr));
}

std::string Rom::getCartridgeTypeAsString() const {
    switch(getCartridgeType()){
        case RomOnly:
            return "ROM ONLY";
        case RomMBC1:
            return "ROM+MBC1";
        case RomMBC1Ram:
            return "ROM+MBC1+RAM";
        case RomMBC1RamBatt:
            return "ROM+MBC1+RAM+BATT";
        case RomMBC2:
            return "ROM+MBC2";
        case RomMBC2Batt:
            return "ROM+MBC2+BATTERY";
        case RomRam:
            return "ROM+RAM";
        case RomRamBatt:
            return "ROM+RAM+BATTERY";
        case RomMMM01:
            return "ROM+MMM01";
        case RomMMM01SRam:
            return "ROM+MMM01+SRAM";
        case RomMMM01SRamBatt:
            return "ROM+MMM01+SRAM+BATT";
        case RomMBC3TimerBatt:
            return "ROM+MBC3+TIMER+BATT";
        case RomMBC3TimerRamBatt:
            return "ROM+MBC3+TIMER+RAM+BATT";
        case RomMBC3:
            return "ROM+MBC3";
        case RomMBC3Ram:
            return "ROM+MBC3+RAM";
        case RomMBC3RamBatt:
            return "ROM+MBC3+RAM+BATT";
        case RomMBC5:
            return "ROM+MBC5";
        case RomMBC5Ram:
            return "ROM+MBC5+RAM";
        case RomMBC5RamBatt:
            return "ROM+MBC5+RAM+BATT";
        case RomMBC5Rumble:
            return "ROM+MBC5+RUMBLE";
        case RomMBC5RamRumbleSRam:
            return "ROM+MBC5+RUMBLE+SRAM";
        case RomMBC5RamRumbleSRamBatt:
            return "ROM+MBC5+RUMBLE+SRAM+BATT";
        case PocketCamera:
            return "POCKET CAMERA";
        case BandaiTama5:
            return "BANDAI TAMA5";
        case HudsonHuc3:
            return "HUDSON HUC-3";
        case HudsonHuc1:
            return "HUDSON HUC-1";
        default:
            return "UNDEFINED";
    }
}

uint8_t Rom::getRomSizeValue() const {
    return readExt(RomSize);
}

uint8_t Rom::getRamSizeValue() const {
    return readExt(RamSize);
}

// Each bank is 16 KB
uint8_t Rom::getNumRomBanks() const {
    const uint8_t romSizeValue = getRomSizeValue();
    if (romSizeValue >= 0x00 && romSizeValue <= 0x06) {
        return static_cast<uint8_t>((2 << romSizeValue));
    } else {
        switch (romSizeValue) {
            case 0x52:
                return 72;
            case 0x53:
                return 80;
            case 0x54:
                return 96;
            default:
                throw runtime_error ("Rom size value unknown: " + to_string(romSizeValue));
        }
    }
}

uint8_t Rom::getNumRamBanks() const {
    const uint8_t ramSizeValue = getRamSizeValue();
    switch (ramSizeValue) {
        case 0x00:
            return 0;
        case 0x01:
            return 1;
        case 0x02:
            return 1;
        case 0x03:
            return 4;
        case 0x04:
            return 16;
        default:
            throw runtime_error ("Ram size value unknown: " + to_string(ramSizeValue));
    }
}

bool Rom::isJapanese() const {
    return readExt(DestinationCode) == 0;
}

// Super gameboy does not work if not 0x33
uint8_t Rom::getLicenseeCode() const {
    return readExt(LicenseeCode);
}

uint8_t Rom::getMaskRomVerNum() const {
    return readExt(MaskRomVerNum);
}

bool Rom::validChecksum() const {
    uint16_t sum ( 0 );
    for (size_t i = 0; i < getSize(); ++i) {
        if (i != ChecksumHigh && i != ChecksumLow) {
            sum += readExt(i);
        }
    }
    return ((readExt(ChecksumHigh) << 8) | readExt(ChecksumLow)) == sum;
}

uint16_t Rom::getExtendedLicenseeCode() const {
    return ((readExt(LicenseeHigh) << 8) | readExt(LicenseeLow));
}

ostream& ::Gameboy::Cartridge::operator<<(std::ostream &strm, const Rom &rom) {
    if (rom.validChecksum()) {
        strm << "Rom:" << endl
             << "\tTitle: " << rom.getTitle() << endl
             << "\tType: " << rom.getCartridgeTypeAsString() << endl
             << "\tRom Banks: " << static_cast<uint32_t>(rom.getNumRomBanks()) << endl
             << "\tRam Banks: " << static_cast<uint32_t>(rom.getNumRamBanks()) << endl
             << "\tColor GB: " << (rom.isColorGB() ? "Yes" : "No") << endl
             << "\tSuper GB: " << (rom.isSuperGB() ? "Yes" : "No") << endl
             << "\tJapanese: " << (rom.isJapanese() ? "Yes" : "No") << endl
             << "\tLicense Code: 0x" << hex << static_cast<uint32_t>(rom.getLicenseeCode()) << dec << endl
             << "\tExtended License Code: 0x" << hex << static_cast<uint32_t>(rom.getExtendedLicenseeCode()) << dec << endl
             << "\tMask Rom Version Number: 0x" << hex << static_cast<uint32_t>(rom.getMaskRomVerNum()) << dec;
    } else {
        strm << "Rom is Invalid!";
    }
    return strm;
}
