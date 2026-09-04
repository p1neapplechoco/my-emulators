#include "bus.h"

void NES_bus::setCPU(NES_cpu &cpu) { cpu_ = &cpu; }
void NES_bus::setPPU(NES_ppu &ppu) { ppu_ = &ppu; }
void NES_bus::setCartridge(NES_cartridge &cartridge) { cartridge_ = &cartridge; }

uint8_t NES_bus::readCPU(uint16_t addr)
{
    uint8_t data = 0x00;

    if (addr < 0x2000)
        data = ram_[addr & 0x07FF];

    else if (addr >= 0x2000 && addr <= 0x3FFF)
        data = ppu_->readCPU(addr);

    else if (addr >= 0x6000)
        data = cartridge_->readCPU(addr);

    return data;
}

void NES_bus::writeCPU(uint16_t addr, uint8_t data)
{
    if (addr < 0x2000)
        ram_[addr & 0x07FF] = data;

    else if (addr >= 0x2000 && addr <= 0x3FFF)
        ppu_->writeCPU(addr, data); // PPU

    else if (addr >= 0x6000 && addr <= 0x7FFF)
        cartridge_->writeCPU(addr, data);

    else if (addr >= 0x8000)
        cartridge_->writeCPU(addr, data);
}