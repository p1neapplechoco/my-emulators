#pragma once

#include <cstdint>
#include <vector>
#include <string>

// NES CARTRIDGE CONSTANTS

struct NES_header
{
    uint8_t signature_[4];
    uint8_t prgRomSize_; // Size of PRG ROM in 16KB units
    uint8_t chrRomSize_; // Size of CHR ROM in 8KB units
    uint8_t flags6_;     // Flags 6
    uint8_t flags7_;     // Flags 7
    uint8_t flags8_;     // Flags 8
    uint8_t flags9_;     // Flags 9
    uint8_t flags10_;    // Flags 10
    uint8_t padding_[5]; // Padding to make the header 16 bytes
};

class NES_cartridge // this thing connects to both buses
{
public:
    NES_cartridge() = default;

    // methods to interact with CPU and PPU
    /*
    according to nesdev.org/wiki/NROM, im a fucking idiot

    CPU $6000-$7FFF: Unbanked PRG-RAM, mirrored as necessary to fill entire 8 KiB window, write protectable with an external switch. (Family BASIC only)
    --> this is the area the thing can perform writing.

    CPU $8000-$BFFF: First 16 KiB of PRG-ROM.
    CPU $C000-$FFFF: Last 16 KiB of PRG-ROM (NROM-256) or mirror owf $8000-$BFFF (NROM-128).
    --> reading

    PPU $0000-$1FFF: 8 KiB CHR-ROM.
    --> self-explanatory
    */
    uint8_t readCPU(uint16_t);
    void writeCPU(uint16_t, uint8_t);
    uint8_t readPPU(uint16_t);
    void writePPU(uint16_t, uint8_t);

    // rom
    int loadROM(const std::string &);

private:
    NES_header header_;
    std::vector<uint8_t> prgRom_; // prg is program rom which is solely processed by the cpu
    std::vector<uint8_t> chrRom_; // chr is character/graphics rom --> ppu
    std::vector<uint8_t> sRam_;   // 8KB of  RAM
};