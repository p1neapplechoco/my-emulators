// graphical stuffs
#pragma once

#include <cstdint>
#include <vector>
#include <iostream>

// GRAPHICAL CONSTANTS
static constexpr std::size_t SCREEN_W = 256;
static constexpr std::size_t SCREEN_H = 240;

class NES_ppu
{
public:
    NES_ppu() = default;

    uint8_t readCPU(uint16_t);
    void writeCPU(uint16_t, uint8_t);

    uint8_t readPPU(uint16_t);
    void writePPU(uint16_t, uint8_t);

private:
    // Registers
    uint8_t ctrl_;
    uint8_t mask_;
    uint8_t status_;
    uint8_t oamAddr_;
    uint8_t oamData_;
    uint8_t scroll_;
    uint8_t addr_;
    uint8_t data_;
    uint8_t oamDMA_;
};