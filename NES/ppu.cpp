#include "ppu.h"

uint8_t NES_ppu::readCPU(uint16_t addr)
{
    uint8_t data = 0x00;

    return data;
}

void NES_ppu::writeCPU(uint16_t addr, uint8_t data)
{
    // Handle writes to PPU registers here
}

uint8_t NES_ppu::readPPU(uint16_t addr)
{
    uint8_t data = 0x00;

    return data;
}

void NES_ppu::writePPU(uint16_t addr, uint8_t data)
{
    // Handle writes to PPU memory here
}