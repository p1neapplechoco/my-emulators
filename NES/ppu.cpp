#include "ppu.h"

void NES_ppu::initialize()
{
    ctrl_ = 0;
    mask_ = 0;
    status_ = 0;
    oamAddr_ = 0;
    oamData_ = 0;
    scroll_ = 0;
    addr_ = 0;
    data_ = 0;
    oamDMA_ = 0;

    cycle_ = 0;
    scanline_ = 0;
}

void NES_ppu::tick()
{
    cycle_++;

    if (cycle_ == 341)
    {
        cycle_ = 0;
        scanline_++;
    }

    if (scanline_ == 241 && cycle_ == 1)
    {
        statusSetFlag(VBLANK, true);
    }

    if (scanline_ == 261 && cycle_ == 1)
    {
        statusSetFlag(VBLANK, false);
    }

    if (scanline_ == 262)
    {
        scanline_ = 0;
    }
}

uint8_t NES_ppu::readCPU(uint16_t addr)
{
    uint8_t data = 0x00;

    if (addr >= 0x2000 && addr <= 0x3FFF)
    {
        // Handle reads from PPU registers here
        uint16_t reg = addr & 0x2007;

        if (reg == 0x2002)
        {
            data = status_;
            statusSetFlag(VBLANK, false);
        }
        else if (reg == 0x2004)
            data = oamData_;
        else if (reg == 0x2007)
            data = data_;
    }
    else if (addr == 0x4014)
    {
        // Handle OAM DMA read here
    }
    else
    {
        // Handle other CPU reads from PPU memory here
    }

    return data;
}

void NES_ppu::writeCPU(uint16_t addr, uint8_t data)
{
    if (addr >= 0x2000 && addr <= 0x3FFF)
    {
        // Handle writes to PPU registers here
        uint16_t reg = addr & 0x2007;

        if (reg == 0x2000)
            ctrl_ = data;
        else if (reg == 0x2001)
            mask_ = data;
        else if (reg == 0x2003)
            oamAddr_ = data;
        else if (reg == 0x2004)
            oamData_ = data;
        else if (reg == 0x2005)
            scroll_ = data;
        else if (reg == 0x2006)
            addr_ = data;
        else if (reg == 0x2007)
            data_ = data;
    }
    else if (addr == 0x4014)
    {
        // Handle OAM DMA write here
    }
    else
    {
        // Handle other CPU writes to PPU memory here
    }
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

bool NES_ppu::requestNMI()
{
    return scanline_ == 241 && cycle_ == 1 && ctrlGetFlag(NMI_ENABLE);
}

// REGISTER METHODS
// PPUCTRL
bool NES_ppu::ctrlGetFlag(ctrlFlags_ flag) { return (ctrl_ & flag) != 0; }
void NES_ppu::ctrlSetFlag(ctrlFlags_ flag, bool condition) { ctrl_ = (ctrl_ & ~flag) | (-(uint8_t)condition & flag); }

// PPUMASK
bool NES_ppu::maskGetFlag(maskFlags_ flag) { return (mask_ & flag) != 0; }
void NES_ppu::maskSetFlag(maskFlags_ flag, bool condition) { mask_ = (mask_ & ~flag) | (-(uint8_t)condition & flag); }

// PPUSTATUS
bool NES_ppu::statusGetFlag(statusFlags_ flag) { return (status_ & flag) != 0; }
void NES_ppu::statusSetFlag(statusFlags_ flag, bool condition) { status_ = (status_ & ~flag) | (-(uint8_t)condition & flag); }
