#pragma once

#include <iostream>
#include <cstdint>

class NES_cpu
{
public:
    // Flag based operations
    enum Flags : uint8_t
    {
        C = 1 << 0, // Carry
        Z = 1 << 1, // Zero
        I = 1 << 2, // Interrupt Disable
        D = 1 << 3, // Decimal

        B = 1 << 4, // no CPU effect, the B flag
        U = 1 << 5, // no CPU effect, always set as 1
        V = 1 << 6, // Overflow
        N = 1 << 7, // Negative
    };

    bool getFlag(Flags);
    void setFlag(Flags, bool);

    void initialize();

private:
    uint8_t A_; // Accumulator <- idek what this does
    uint8_t X_, Y_;
    uint16_t pc_;
    uint8_t sp_;
    uint8_t p_; // Status flags
};

class NES
{
public:
private:
};
