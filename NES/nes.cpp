#include "nes.h"

bool NES_cpu::getFlag(Flags F) { return (p_ & F) != 0; }

void NES_cpu::setFlag(Flags F, bool condition)
{
    p_ = (p_ & ~F) | (-(uint8_t)condition & F);
}

void NES_cpu::initialize()
{
    A_ = 0;
    X_ = 0;
    Y_ = 0;

    pc_ = 0xFFFC;

    sp_ = 0xFD;

    // setting le flags
    setFlag(C, 0);
    setFlag(Z, 0);
    setFlag(I, 1);
    setFlag(D, 0);

    setFlag(B, 1);
    setFlag(U, 0);
    setFlag(V, 0);
    setFlag(N, 0);
}