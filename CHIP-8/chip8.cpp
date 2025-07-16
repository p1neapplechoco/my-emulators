#include "chip8.h"
#include <chrono>
#include <random>

void CHIP8::clearDisplay()
{
    for (int i = 0; i < SCREEN_W * SCREEN_H; i++)
        gfx_[i] = 0;

    drawFlag_ = false;
}
void CHIP8::clearStack()
{
    for (int i = 0; i < STACK_DEPTH; i++)
        stack_[i] = 0;
}

void CHIP8::clearRegisters()
{
    for (int i = 0; i < REG_COUNT; i++)
        V_[i] = 0;
}

void CHIP8::clearMemory()
{
    for (int i = 0; i < MEM_SIZE; i++)
        mem_[i] = 0;
}

CHIP8::CHIP8()
{
    seed_ = static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count()); // Get seed
    rng_ = std::mt19937(seed_);                                                                 // Mersenne Twister engine
    dist_ = std::uniform_int_distribution<unsigned short>(0, BYTE_MAX);
}

CHIP8::~CHIP8() = default;

void CHIP8::initialize()
{
    pc_ = 0x200;
    opcode_ = 0;
    I_ = 0;
    sp_ = 0;

    clearDisplay();
    clearStack();
    clearRegisters();
    clearMemory();

    for (int i = 0; i < FONTSET_SZ; i++)
        mem_[i] = chip8_fontset[i];

    delay_timer_ = 0;
    sound_timer_ = 0;
}

void CHIP8::fetch()
{
    unsigned char first_byte = mem_[pc_];
    unsigned char second_byte = mem_[pc_ + 1];

    opcode_ = first_byte << 8 | second_byte;
}

void CHIP8::execute()
{
    int idx = (opcode_ & 0xF000) >> 12;
    instr handler = opcodeTable_[idx];
    (this->*handler)();
}

void CHIP8::emulateCycle()
{
    fetch();
    pc_ += 2;
    execute();
}

void CHIP8::sysCall(unsigned short address)
{
    pc_ = address;
}

void CHIP8::returnFromSubroutine()
{
    sp_--;
    pc_ = stack_[sp_];
}

void CHIP8::cpuNULL()
{
    // Do nothing
}

void CHIP8::handle0xxx()
{
    unsigned char NNN = opcode_ & 0x0FFF;

    switch (opcode_)
    {
    case 0x00E0:
        clearDisplay();
        break;
    case 0x00EE:
        returnFromSubroutine();
        break;
    default:
        sysCall(NNN);
        break;
    }
}

void CHIP8::jumpToAddress()
{
    unsigned char NNN = opcode_ & 0x0FFF;
    pc_ = NNN;
}

void CHIP8::jumpToAddressAddV0()
{
    unsigned char NNN = opcode_ & 0x0FFF;
    pc_ = NNN + V_[0];
}

void CHIP8::callSubroutine()
{
    unsigned short NNN = opcode_ & 0x0FFF;
    stack_[sp_] = pc_;
    sp_++;
    pc_ = NNN;
}

void CHIP8::skipIfEqual(unsigned short lhs, unsigned short rhs)
{
    if (lhs == rhs)
        pc_ += 2;
}

void CHIP8::skipIfNotEqual(unsigned short lhs, unsigned short rhs)
{
    if (lhs != rhs)
        pc_ += 2;
}

void CHIP8::skipInstruction()
{
    unsigned short opGroup = (opcode_ & 0xF000) >> 12;
    switch (opGroup)
    {
    case 3: // 3XNN: skip if Vx == NN
    {
        unsigned short x = (opcode_ & 0x0F00) >> 8;
        unsigned short nn = opcode_ & 0x00FF;
        skipIfEqual(V_[x], nn);
        break;
    }
    case 4: // 4XNN: skip if Vx != NN
    {
        unsigned short x = (opcode_ & 0x0F00) >> 8;
        unsigned short nn = opcode_ & 0x00FF;
        skipIfNotEqual(V_[x], nn);
        break;
    }
    case 5: // 5XY0: skip if Vx == Vy, but only when low nibble == 0
    {
        if ((opcode_ & 0x000F) != 0)
            break;
        unsigned short x = (opcode_ & 0x0F00) >> 8;
        unsigned short y = (opcode_ & 0x00F0) >> 4;
        skipIfEqual(V_[x], V_[y]);
        break;
    }

    case 9: // 9XY0: skip if Vx != Vy, but only when low nibble == 0
    {
        if ((opcode_ & 0x000F) != 0)
            break;
        unsigned short x = (opcode_ & 0x0F00) >> 8;
        unsigned short y = (opcode_ & 0x00F0) >> 4;
        skipIfNotEqual(V_[x], V_[y]);
        break;
    }
    case 0xE:
    {
        unsigned short x = (opcode_ & 0x0F00) >> 8;
        if ((opcode_ & 0x00FF) == 0x009E) // EX9E: skips the next instruction if the key stored in VX is pressed
            skipIfEqual(key_[V_[x]], 1);

        else if ((opcode_ & 0x00FF) == 0x00A1) // EXA1: skips the next instruction if the key stored in VX is NOT pressed
            skipIfNotEqual(key_[V_[x]], 1);

        break;
    }
    default:
        break;
    }
}

void CHIP8::setRegister(unsigned short x, unsigned short nn)
{
    V_[x] = nn;
}

void CHIP8::addToRegister(unsigned short x, unsigned short nn)
{
    V_[x] += nn;
    // VF (Carry Flag) is unchanged so skip the overflowing behaviour
}

void CHIP8::setRegisterRandom(unsigned short x, unsigned short nn)
{
    V_[x] = dist_(rng_) & nn;
}

void CHIP8::modifyRegister()
{
    unsigned short opGroup = (opcode_ & 0xF000) >> 12;
    switch (opGroup)
    {
    case 6: // 6XNN: set Vx = NN
    {
        unsigned short x = (opcode_ & 0x0F00) >> 8;
        unsigned short nn = opcode_ & 0x00FF;
        setRegister(x, nn);
        break;
    }
    case 7: // 7XNN: add NN to Vx
    {
        unsigned short x = (opcode_ & 0x0F00) >> 8;
        unsigned short nn = opcode_ & 0x00FF;
        addToRegister(x, nn);
        break;
    }
    case 0xC: // CXNN: set Vx = rand() && NN
    {
        unsigned short x = (opcode_ & 0x0F00) >> 8;
        unsigned short nn = opcode_ & 0x00FF;
        setRegisterRandom(x, nn);
        break;
    }
    default:
        break;
    }
}

void CHIP8::handle8xxx()
{
    int idx = (opcode_ & 0x000F);
    instr handler = subTable8_[idx];
    (this->*handler)();
}

void CHIP8::loadVxFromVy()
{
    unsigned short x = (opcode_ & 0x0F00) >> 8;
    unsigned short y = (opcode_ & 0x00F0) >> 4;

    V_[x] = V_[y];
}

void CHIP8::orVxVy()
{
    unsigned short x = (opcode_ & 0x0F00) >> 8;
    unsigned short y = (opcode_ & 0x00F0) >> 4;

    V_[x] |= V_[y];
}

void CHIP8::andVxVy()
{
    unsigned short x = (opcode_ & 0x0F00) >> 8;
    unsigned short y = (opcode_ & 0x00F0) >> 4;

    V_[x] &= V_[y];
}

void CHIP8::xorVxVy()
{
    unsigned short x = (opcode_ & 0x0F00) >> 8;
    unsigned short y = (opcode_ & 0x00F0) >> 4;

    V_[x] ^= V_[y];
}

void CHIP8::addVxVy()
{
    unsigned short x = (opcode_ & 0x0F00) >> 8;
    unsigned short y = (opcode_ & 0x00F0) >> 4;

    uint16_t sum = V_[x] + V_[y];

    V_[0xF] = (sum > 0xFF);
    V_[x] = sum & 0xFF;
}

void CHIP8::subVxVy()
{
    unsigned short x = (opcode_ & 0x0F00) >> 8;
    unsigned short y = (opcode_ & 0x00F0) >> 4;

    V_[0xF] = (V_[x] >= V_[y]);
    V_[x] = (V_[x] - V_[y]) & 0xFF;
}

void CHIP8::shrVx()
{
    unsigned short x = (opcode_ & 0x0F00) >> 8;

    V_[0xF] = V_[x] & 0X01;
    V_[x] >>= 1;
}

void CHIP8::subnVxVy()
{
    unsigned short x = (opcode_ & 0x0F00) >> 8;
    unsigned short y = (opcode_ & 0x00F0) >> 4;

    V_[0xF] = (V_[y] >= V_[x]);
    V_[x] = (V_[y] - V_[x]) & 0xFF;
}

void CHIP8::shlVx()
{
    unsigned short x = (opcode_ & 0x0F00) >> 8;

    V_[0xF] = (V_[x] & 0x80) >> 7;
    V_[x] <<= 1;
}

CHIP8::instr CHIP8::subTable8_[15] = {
    &CHIP8::loadVxFromVy, // 8XY0
    &CHIP8::orVxVy,       // 8XY1
    &CHIP8::andVxVy,      // 8XY2
    &CHIP8::xorVxVy,      // 8XY3
    &CHIP8::addVxVy,      // 8XY4
    &CHIP8::subVxVy,      // 8XY5
    &CHIP8::shrVx,        // 8XY6
    &CHIP8::subnVxVy,     // 8XY7
    &CHIP8::cpuNULL,      // 8XY8 (unassigned)
    &CHIP8::cpuNULL,      // 8XY9 (unassigned)
    &CHIP8::cpuNULL,      // 8XYA (unassigned)
    &CHIP8::cpuNULL,      // 8XYB (unassigned)
    &CHIP8::cpuNULL,      // 8XYC (unassigned)
    &CHIP8::cpuNULL,      // 8XYD (unassigned)
    &CHIP8::shlVx         // 8XYE
};

void CHIP8::modifyIndex()
{
    unsigned short opGroup = (opcode_ & 0xF000) >> 12;
    switch (opGroup)
    {
    case 0xA: // ANNN: set I = NNN
    {
        unsigned short NNN = opcode_ & 0x0FFF;
        setIndex(NNN);
        break;
    }
    case 0xF: // FXNN: add NN to Vx
    {
        unsigned short x = (opcode_ & 0x0F00) >> 8;

        if ((opcode_ & 0x00FF) == 0x1E)
            addToIndex(x);

        else if ((opcode_ & 0x00FF) == 0x29)
            setIndexToSprite(x);

        break;
    }
    default:
        break;
    }
}

void CHIP8::setIndex(unsigned short address)
{
    I_ = address;
}

void CHIP8::addToIndex(unsigned short x)
{
    I_ += V_[x];
}

void CHIP8::setIndexToSprite(unsigned short x)
{
    I_ = (V_[x] & 0x0F) * 5; // This gets the starting address of a character.
}

void CHIP8::draw()
{
    unsigned short x = V_[(opcode_ & 0x0F00) >> 8];
    unsigned short y = V_[(opcode_ & 0x00F0) >> 4];
    unsigned short height = (opcode_ & 0x000F);
    uint8_t pixels_row;

    V_[0xF] = 0;
    for (int _y = 0; _y < height; _y++)
    {
        pixels_row = mem_[I_ + _y];
        for (int _x = 0; _x < 8; _x++)
        {
            if ((pixels_row & (0x80 >> _x)) != 0x0) // if the pixel is 1
            {
                if (gfx_[x + _x + ((y + _y) * SCREEN_W)] == 1) // if any screen pixel is flipped
                    V_[0xF] = 1;
                gfx_[x + _x + ((y + _y) * SCREEN_W)] ^= 1;
            }
        }
    }

    drawFlag_ = true;
}

void CHIP8::getDelayTimer(unsigned short x)
{
    V_[x] = delay_timer_;
}

void CHIP8::getKeyPress(unsigned short x)
{
    for (int i = 0; i < KEYPAD_KEYS; ++i)
        if (key_[i] != 0)
            V_[(opcode_ & 0x0F00) >> 8] = i;
}

void CHIP8::setDelayTimer(unsigned short x)
{
    delay_timer_ = V_[x];
}

void CHIP8::setSoundTimer(unsigned short x)
{
    sound_timer_ = V_[x];
}

void CHIP8::setBCD(unsigned short x)
{
    mem_[I_] = V_[(opcode_ & 0x0F00) >> 8] / 100;
    mem_[I_ + 1] = (V_[(opcode_ & 0x0F00) >> 8] / 10) % 10;
    mem_[I_ + 2] = (V_[(opcode_ & 0x0F00) >> 8] % 100) % 10;
}

void CHIP8::writeToMemory(unsigned short x)
{
    for (int i = 0; i <= x; i++)
        mem_[I_ + i] = V_[i];

    I_ = I_ + (x + 1);
}

void CHIP8::readFromMemory(unsigned short x)
{
    for (int i = 0; i <= x; i++)
        V_[i] = mem_[I_ + i];

    I_ = I_ + (x + 1);
}

void CHIP8::handleFxxx()
{
    uint8_t operands = (opcode_ & 0x00FF);
    unsigned short x = (opcode_ & 0x0F00) >> 8;
    int idx;

    switch (operands)
    {
    case 0x07:
    {
        getDelayTimer(x);
        break;
    }
    case 0x0A:
    {
        getKeyPress(x);
        break;
    }
    case 0x15:
    {
        setDelayTimer(x);
        break;
    }
    case 0x18:
    {
        setSoundTimer(x);
        break;
    }
    case 0x1E:
    case 0x29:
    {
        modifyIndex();
        break;
    }
    case 0x33:
    {
        setBCD(x);
        break;
    }
    case 0x55:
    {
        writeToMemory(x);
        break;
    }
    case 0x65:
    {
        readFromMemory(x);
        break;
    }
    default:
        return;
    }
}

CHIP8::instr CHIP8::opcodeTable_[16] = {
    &CHIP8::handle0xxx,         // opcode 0xxx
    &CHIP8::jumpToAddress,      // opcode 1xxx
    &CHIP8::callSubroutine,     // opcode 2xxx
    &CHIP8::skipInstruction,    // opcode 3xxx
    &CHIP8::skipInstruction,    // opcode 4xxx
    &CHIP8::skipInstruction,    // opcode 5xxx
    &CHIP8::modifyRegister,     // opcode 6xxx
    &CHIP8::modifyRegister,     // opcode 7xxx
    &CHIP8::handle8xxx,         // opcode 8xxx
    &CHIP8::skipInstruction,    // opcode 9xxx
    &CHIP8::modifyIndex,        // opcode Axxx
    &CHIP8::jumpToAddressAddV0, // opcode Bxxx
    &CHIP8::modifyRegister,     // opcode Cxxx
    &CHIP8::draw,               // opcode Dxxx
    &CHIP8::skipInstruction,    // opcode Exxx
    &CHIP8::handleFxxx          // opcode Fxxx
};