#pragma once

#include <cstdint>

class NES_bus
{
public:
    NES_bus() = default;

    uint8_t read(uint16_t);
    void write(uint16_t, uint8_t);

private:
    uint8_t ram_[2048]; // 2KB of RAMs
};