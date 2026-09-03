#pragma once

#include "ppu.h"
#include "cartridge.h"
#include <cstdint>

class NES_cpu; // forward declaration: cpu.h uses NES_bus

class NES_bus
{
public:
    NES_bus() = default;

    // MISCs
    void setCPU(NES_cpu &);
    void setPPU(NES_ppu &);
    void setCartridge(NES_cartridge &);

    uint8_t readCPU(uint16_t);
    void writeCPU(uint16_t, uint8_t);

private:
    // Devices on bus
    NES_cpu *cpu_;
    NES_ppu *ppu_;
    NES_cartridge *cartridge_;

    uint8_t ram_[2048]; // 2KB of RAMs
};