//
// Created by Kevin on 07/10/2016.
//

#include <fstream>

#include "Cartridge.h"

using namespace std;
using namespace Gameboy;

void Cartridge::load(const std::string &romPath) {
    ifstream file (romPath, ios::binary | ios::ate);

    if (!file) {
        throw runtime_error("Cannot open file: " + romPath);
    }

    //file.seekg(0, ios::end);
    streampos length = file.tellg();
    file.seekg(0, ios::beg);

    cartridgeData.clear();
    cartridgeData.resize(length);
    if (!file.read(reinterpret_cast<char *>(cartridgeData.data()), length)) {
        throw runtime_error("Cannot read data from file: " + romPath);
    }
}

string Cartridge::getTitle() const {
    return string(reinterpret_cast<const char *>(&cartridgeData.at(Title)), ColorGB - Title);
}

bool Cartridge::isColorGB() const {
    return cartridgeData.at(ColorGB) == 0x80;
}

bool Cartridge::isSuperGB() const {
    return cartridgeData.at(GBOrSGB) == 0x03;
}

CartridgeType Cartridge::getCartridgeType() const {
    return static_cast<CartridgeType>(cartridgeData.at(CartridgeTypeAddr));
}

std::string Cartridge::getCartridgeTypeAsString() const {
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

uint8_t Cartridge::getRomSizeValue() const {
    return cartridgeData.at(RomSize);
}

uint8_t Cartridge::getRamSizeValue() const {
    return cartridgeData.at(RamSize);
}

// Each bank is 16 KB
uint8_t Cartridge::getNumRomBanks() const {
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

uint8_t Cartridge::getNumRamBanks() const {
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

bool Cartridge::isJapanese() const {
    return cartridgeData.at(DestinationCode) == 0;
}

// Super gamboy does not work if not 0x33
std::uint8_t Cartridge::getLicenseeCode() const {
    return cartridgeData.at(LicenseeCode);
}

std::uint8_t Cartridge::getMaskRomVerNum() const {
    return cartridgeData.at(MaskRomVerNum);
}

bool Cartridge::validChecksum() const {
    uint16_t sum ( 0 );
    for (size_t i = 0; i < cartridgeData.size(); ++i) {
        if (i != ChecksumHigh && i != ChecksumLow) {
            sum += cartridgeData[i];
        }
    }
    return ((cartridgeData.at(ChecksumHigh) << 8) | cartridgeData.at(ChecksumLow)) == sum;
}

std::uint16_t Cartridge::getExtendedLicenseeCode() const {
    return ((cartridgeData.at(LicenseeHigh) << 8) | cartridgeData.at(LicenseeLow));
}

std::ostream& ::Gameboy::operator<<(std::ostream &strm, const Cartridge &cartridge) {
    if (cartridge.validChecksum()) {
        strm << "Cartridge:" << endl
             << "\tTitle: " << cartridge.getTitle() << endl
             << "\tType: " << cartridge.getCartridgeTypeAsString() << endl
             << "\tRom Banks: " << static_cast<uint32_t>(cartridge.getNumRomBanks()) << endl
             << "\tRam Banks: " << static_cast<uint32_t>(cartridge.getNumRamBanks()) << endl
             << "\tColor GB: " << (cartridge.isColorGB() ? "Yes" : "No") << endl
             << "\tSuper GB: " << (cartridge.isSuperGB() ? "Yes" : "No") << endl
             << "\tJapanese: " << (cartridge.isJapanese() ? "Yes" : "No") << endl
             << "\tLicense Code: 0x" << hex << static_cast<uint32_t>(cartridge.getLicenseeCode()) << dec << endl
             << "\tExtended License Code: 0x" << hex << static_cast<uint32_t>(cartridge.getExtendedLicenseeCode()) << dec << endl
             << "\tMask Rom Version Number: 0x" << hex << static_cast<uint32_t>(cartridge.getMaskRomVerNum()) << dec;
    } else {
        strm << "Cartridge: Invalid!";
    }
    return strm;
}
