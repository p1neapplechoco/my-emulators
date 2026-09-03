#include "bus.h"

uint8_t NES_bus::read(uint16_t addr)
{
    if (addr < 0x2000)
    {
        return ram_[addr % 2048];
    }

    return 0x00;
}

void NES_bus::write(uint16_t addr, uint8_t data)
{
    if (addr < 0x2000)
    {
        ram_[addr % 2048] = data;
    }
}