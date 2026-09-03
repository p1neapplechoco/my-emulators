#pragma once

#include "cpu.h"
#include "ppu.h"
#include "cartridge.h"
#include <cstdint>

class NES_bus
{
public:
    NES_bus() = default;

    uint8_t readCPU(uint16_t);
    void writeCPU(uint16_t, uint8_t);

private:
    // Devices on bus
    NES_cpu *cpu_;
    NES_ppu *ppu_;
    NES_cartridge *cartridge_;

    uint8_t ram_[2048]; // 2KB of RAMs
};