// For loading ROM

#include "cartridge.h"
#include <cassert>
#include <iostream>
#include <fstream>

uint8_t NES_cartridge::readCPU(uint16_t addr)
{
    uint8_t data = 0x00;

    if (addr >= 0x6000 && addr <= 0x7FFF)
    {
        uint16_t sRamAddr = addr - 0x6000;
        data = sRam_[sRamAddr];
    }
    else if (addr >= 0x8000 && addr <= 0xFFFF)
    {
        uint16_t prgRomAddr = addr - 0x8000;
        data = prgRom_[prgRomAddr % prgRom_.size()];
    }

    return data;
}

void NES_cartridge::writeCPU(uint16_t addr, uint8_t data)
{
    if (addr >= 0x6000 && addr <= 0x7FFF)
    {
        uint16_t sRamAddr = addr - 0x6000;
        sRam_[sRamAddr] = data;
    }
}

uint8_t NES_cartridge::readPPU(uint16_t addr)
{
    uint8_t data = 0x00;

    if (addr >= 0x0000 && addr <= 0x1FFF)
        data = chrRom_[addr % chrRom_.size()];

    return data;
}

void NES_cartridge::writePPU(uint16_t addr, uint8_t data)
{
    if (addr >= 0x0000 && addr <= 0x1FFF)
        chrRom_[addr % chrRom_.size()] = data;
}

// rom
int NES_cartridge::loadROM(const std::string &filename)
{

    std::ifstream file(filename, std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file: " + filename);
        return 0;
    }

    // header first
    file.read((char *)&header_, sizeof(NES_header));

    assert(header_.signature_[0] == 'N' && header_.signature_[1] == 'E' && header_.signature_[2] == 'S' && header_.signature_[3] == 0x1A);

    // prgRom next
    prgRom_.resize(header_.prgRomSize_ * 16 * 1024);
    file.read((char *)prgRom_.data(), prgRom_.size());

    // chrRom last
    chrRom_.resize(header_.chrRomSize_ * 8 * 1024);
    file.read((char *)chrRom_.data(), chrRom_.size());

    return 1;
}