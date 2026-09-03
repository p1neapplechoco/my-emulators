#include "bus.h"

uint8_t NES_bus::readCPU(uint16_t addr)
{
    uint8_t data = 0x00;

    if (addr < 0x2000)
        data = ram_[addr & 0x07FF];
    else if (addr >= 0x2000 && addr < 0x4000)
        data = ppu_->readCPU(addr & 0x0007); // PPU registers are mirrored every 8 bytes

    return data;
}

void NES_bus::writeCPU(uint16_t addr, uint8_t data)
{
    if (addr < 0x2000)
    {
        ram_[addr & 0x07FF] = data;
    }
}