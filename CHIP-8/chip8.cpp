#include "chip8.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <string>

// MASK FUNCTIONS
/*
An opcode is 2 bytes (16 bits):
High byte (first 8 bits), then low byte (last 8 bits).
Layout: 0000 0000 0000 0000
        ^^^^ ^^^^ ^^^^ ^^^^
        High Byte  Low Byte
Each byte is constructed from 2 nibbles (high, low).
*/

#define nnn(o) (o & 0x0FFF)         // Masks the lower 3 nibbles
#define n(o) (o & 0x000F)           // Masks the lower nibble
#define xh(o) ((o & 0xF000) >> 12)  // Masks the top most nibble of high byte
#define xl(o) ((o & 0x0F00) >> 8)   // Masks the lower nibble of high byte
#define yh(o) ((o & 0x00F0) >> 4)   // Masks the higher nibble of low byte
#define yl(o) (o & 0x000F)          // Masks the lower nibble of low byte
#define high(o) ((o & 0xFF00) >> 8) // Masks the high byte
#define low(o) (o & 0x00FF)         // Masks the lower byte</code></pre>

// INSTRUCTION TABLE

CHIP8::instr CHIP8::opcode_table_[16] = {
    &CHIP8::handle0xxx, // opcode 0xxx
    &CHIP8::handle1xxx, // opcode 1xxx
    &CHIP8::handle2xxx, // opcode 2xxx
    &CHIP8::handle3xxx, // opcode 3xxx
    &CHIP8::handle4xxx, // opcode 4xxx
    &CHIP8::handle5xxx, // opcode 5xxx
    &CHIP8::handle6xxx, // opcode 6xxx
    &CHIP8::handle7xxx, // opcode 7xxx
    &CHIP8::handle8xxx, // opcode 8xxx
    &CHIP8::handle9xxx, // opcode 9xxx
    &CHIP8::handleAxxx, // opcode Axxx
    &CHIP8::handleBxxx, // opcode Bxxx
    &CHIP8::handleCxxx, // opcode Cxxx
    &CHIP8::handleDxxx, // opcode Dxxx
    &CHIP8::handleExxx, // opcode Exxx
    &CHIP8::handleFxxx  // opcode Fxxx
};

// USERS METHODS

void CHIP8::loadROM(const std::string &filename)
{
    std::ifstream rom(filename, std::ios::binary | std::ios::ate);

    int fileSize = rom.tellg();
    int maxSize = MEM_SIZE - 0x200;

    if (fileSize < 0 || fileSize > maxSize)
    {
        throw std::runtime_error("ROM size is invalid or too large.");
    }

    rom.seekg(0, std::ios::beg);
    rom.read(reinterpret_cast<char *>(mem_) + 0x200, fileSize);

    if (!rom)
    {
        throw std::runtime_error("Failed to load entire ROM into memory");
        return;
    }

    pc_ = 0x200;
}

void CHIP8::initialize()
{
    pc_ = 0x0200; // Program counter starts at 0x0200 due to the first 512 bytes are used for the interpreter.
    opcode_ = 0x0000;
    I_ = 0x0000;
    sp_ = 0x0000;

    std::fill_n(gfx_, SCREEN_H * SCREEN_W, 0);
    std::fill_n(stack_, STACK_DEPTH, 0);
    std::fill_n(V_, REG_COUNT, 0);
    std::fill_n(mem_, MEM_SIZE, 0);

    // Load fontset
    for (int i = 0; i < 80; i++)
        mem_[i] = chip8_fontset[i];

    // Reset timers
    delay_timer_ = 0x00;
    sound_timer_ = 0x00;

    // Set RNG
    srand(time(NULL));
}

void CHIP8::emulateCycle()
{
    fetch();
    pc_ += 2;
    execute();

    if (delay_timer_ > 0)
        delay_timer_--;

    if (sound_timer_ > 0)
        sound_timer_--;
}

// DEBUG METHODS

void CHIP8::debugDisplay()
{
    if (drawF_ == false)
        return;

    system("cls");

    for (size_t i = 0; i < SCREEN_H; i++)
    {
        for (size_t j = 0; j < SCREEN_W; j++)
        {
            if (gfx_[i * SCREEN_W + j] == 1)
                std::cout << "0";
            else
                std::cout << " ";
        }
        std::cout << std::endl;
    }

    drawF_ = false;
}

// BEHAVIOR

void CHIP8::cpuNULL()
{
    // Do nothing
}

void CHIP8::fetch()
{
    uint8_t first_byte = mem_[pc_];
    uint8_t second_byte = mem_[pc_ + 1];

    opcode_ = (first_byte << 8) | (second_byte);
}

void CHIP8::execute()
{
    uint16_t op = xh(opcode_);
    instr handler = opcode_table_[op];
    (this->*handler)();
}

// Assignment based

void CHIP8::setVxVy(const uint8_t &x, const uint8_t &y)
{
    V_[x] = V_[y];
}

// BCD based

void CHIP8::bcd(const uint8_t &x)
{
    uint8_t bcd = V_[x];

    uint8_t unit = bcd % 10;

    bcd = bcd / 10;
    uint8_t tens = bcd % 10;

    bcd = bcd / 10;
    uint8_t hundreds = bcd % 10;

    mem_[I_] = hundreds;
    mem_[I_ + 1] = tens;
    mem_[I_ + 2] = unit;
}

// BitOp based

void CHIP8::orVxVy(const uint8_t &x, const uint8_t &y)
{
    V_[x] |= V_[y];
}

void CHIP8::andVxVy(const uint8_t &x, const uint8_t &y)
{
    V_[x] &= V_[y];
}

void CHIP8::xorVxVy(const uint8_t &x, const uint8_t &y)
{
    V_[x] ^= V_[y];
}

void CHIP8::shrVx(const uint8_t &x, const uint8_t &y)
{
    uint8_t src = V_[y];
    V_[x] = src >> 1;
    V_[0xF] = (src & 0x01);
}

void CHIP8::shlVx(const uint8_t &x, const uint8_t &y)
{
    uint8_t src = V_[y];
    V_[x] = src << 1;
    V_[0xF] = (src & 0x80) >> 7;
}

// Conditional branching based

void CHIP8::skipIfEqual(const uint8_t &lhs, const uint8_t &rhs)
{
    if (lhs == rhs)
        pc_ += 2;
}

void CHIP8::skipIfNotEqual(const uint8_t &lhs, const uint8_t &rhs)
{
    if (lhs != rhs)
        pc_ += 2;
}

// Const based

void CHIP8::setRegister(const uint8_t &x, const uint8_t &val)
{
    V_[x] = val;
}

void CHIP8::addToRegister(const uint8_t &x, const uint8_t &val)
{
    V_[x] += val;
}

// Display based

void CHIP8::clearDisplay()
{
    std::fill_n(gfx_, SCREEN_H * SCREEN_W, 0);
    drawF_ = true;
}

void CHIP8::drawSprite()
{
    uint8_t X = V_[xl(opcode_)];
    uint8_t Y = V_[yh(opcode_)];
    uint8_t N = yl(opcode_);

    V_[0xF] = 0;

    for (int _y = 0; _y < N; _y++)
    {
        uint8_t pixels = mem_[I_ + _y];
        for (int _x = 0; _x < 8; _x++)
        {
            if ((pixels & (0x80 >> _x)) != 0x0) // if the pixel is 1
            {
                if (gfx_[X + _x + ((Y + _y) * SCREEN_W)] == 1) // if any screen pixel is flipped
                    V_[0xF] = 1;
                gfx_[X + _x + ((Y + _y) * SCREEN_W)] ^= 1;
            }
        }
    }
    drawF_ = true;
}

// Flow based

void CHIP8::returnFromSubroutine()
{
    pc_ = stack_[sp_--];
}

void CHIP8::jumpToAddress(const uint16_t &address)
{
    pc_ = address;
}

void CHIP8::jumpToAddressV0(const uint16_t &address)
{
    pc_ = address + V_[0];
}

void CHIP8::callSubroutine(const uint16_t &address)
{
    stack_[++sp_] = pc_;
    pc_ = address;
}

// KeyOp based

void CHIP8::keyPress(const uint8_t &x)
{
    bool key_pressed = false;
    for (uint8_t i = 0; i < KEYPAD_KEYS; i++)
    {
        if (key_[i] != 0)
        {
            V_[x] = i;
            key_pressed = true;
            break;
        }
    }

    if (!key_pressed)
        pc_ -= 2; // Repeat this instruction until a key is pressed
}

// Math based

void CHIP8::addVxVy(const uint8_t &x, const uint8_t &y)
{
    uint16_t sum = V_[x] + V_[y];
    V_[x] = sum & 0xFF;
    V_[0xF] = (sum > 0xFF);
}

void CHIP8::subVxVy(const uint8_t &x, const uint8_t &y)
{
    uint8_t borrow = (V_[x] >= V_[y]);
    V_[x] = (V_[x] - V_[y]) & 0xFF;
    V_[0xF] = borrow;
}

void CHIP8::subnVxVy(const uint8_t &x, const uint8_t &y)
{
    uint8_t borrow = (V_[x] <= V_[y]);
    V_[x] = (V_[y] - V_[x]) & 0xFF;
    V_[0xF] = borrow;
}

// Memory based

void CHIP8::setIndex(const uint16_t &address)
{
    I_ = address;
}

void CHIP8::addToIndex(const uint8_t &x)
{
    I_ += V_[x];
}

void CHIP8::setSpriteToIndex(const uint8_t &x)
{
    I_ = (V_[x] & 0x0F) * 5; // This gets the starting address of a character.
}

void CHIP8::writeToMemory(const uint8_t &x)
{
    for (int i = 0; i <= x; i++)
        mem_[I_ + i] = V_[i];

    I_ = I_ + (x + 1);
}

void CHIP8::loadFromMemory(const uint8_t &x)
{
    for (int i = 0; i <= x; i++)
        V_[i] = mem_[I_ + i];

    I_ = I_ + (x + 1);
}

// Rand based

void CHIP8::setVxRandom(const uint8_t &x, const uint8_t &val)
{
    V_[x] = (std::rand() % 256) & val;
}

// Sound timer based

void CHIP8::setST(const uint8_t &x)
{
    sound_timer_ = V_[x];
}

// Delay timer based

void CHIP8::getDT(const uint8_t &x)
{
    V_[x] = delay_timer_;
}

void CHIP8::setDT(const uint8_t &x)
{
    delay_timer_ = V_[x];
}

// INSTRUCTION HANDLING

void CHIP8::handle0xxx()
{
    switch (opcode_)
    {
    case 0x00E0:
    {
        clearDisplay();
        break;
    }
    case 0x00EE:
    {
        returnFromSubroutine();
        break;
    }
    default:
        break;
    }
}

void CHIP8::handle1xxx()
{
    uint16_t address = nnn(opcode_);
    jumpToAddress(address);
}

void CHIP8::handle2xxx()
{
    uint16_t address = nnn(opcode_);
    callSubroutine(address);
}

void CHIP8::handle3xxx()
{
    uint8_t nn = low(opcode_);
    uint8_t x = xl(opcode_);

    skipIfEqual(V_[x], nn);
}

void CHIP8::handle4xxx()
{
    uint8_t nn = low(opcode_);
    uint8_t x = xl(opcode_);

    skipIfNotEqual(V_[x], nn);
}

void CHIP8::handle5xxx()
{
    if (yl(opcode_) != 0x00)
        return;
    uint8_t x = xl(opcode_);
    uint8_t y = yh(opcode_);

    skipIfEqual(V_[x], V_[y]);
}

void CHIP8::handle6xxx()
{
    uint8_t nn = low(opcode_);
    uint8_t x = xl(opcode_);

    setRegister(x, nn);
}

void CHIP8::handle7xxx()
{
    uint8_t nn = low(opcode_);
    uint8_t x = xl(opcode_);

    addToRegister(x, nn);
}

void CHIP8::handle8xxx()
{
    uint8_t x = xl(opcode_);
    uint8_t y = yh(opcode_);

    switch (yl(opcode_))
    {
    case 0x0000:
    {
        setVxVy(x, y);
        break;
    }
    case 0x0001:
    {
        orVxVy(x, y);
        break;
    }
    case 0x0002:
    {
        andVxVy(x, y);
        break;
    }
    case 0x0003:
    {
        xorVxVy(x, y);
        break;
    }
    case 0x0004:
    {
        addVxVy(x, y);
        break;
    }
    case 0x0005:
    {
        subVxVy(x, y);
        break;
    }
    case 0x0006:
    {
        shrVx(x, y);
        break;
    }
    case 0x0007:
    {
        subnVxVy(x, y);
        break;
    }
    case 0x000E:
    {
        shlVx(x, y);
        break;
    }
    default:
        break;
    }
}

void CHIP8::handle9xxx()
{
    if (yl(opcode_) != 0x00)
        return;
    uint8_t x = xl(opcode_);
    uint8_t y = yh(opcode_);

    skipIfNotEqual(V_[x], V_[y]);
}

void CHIP8::handleAxxx()
{
    uint16_t address = nnn(opcode_);
    setIndex(address);
}

void CHIP8::handleBxxx()
{
    uint16_t address = nnn(opcode_);
    jumpToAddressV0(address);
}

void CHIP8::handleCxxx()
{
    uint8_t x = xl(opcode_);
    uint8_t nn = low(opcode_);

    setVxRandom(x, nn);
}

void CHIP8::handleDxxx()
{
    drawSprite();
}

void CHIP8::handleExxx()
{
    if (low(opcode_) != 0x009E && low(opcode_) != 0x00A1)
        return;

    uint8_t x = xl(opcode_);
    uint8_t key = V_[x];

    if (low(opcode_) == 0x009E)
    {
        if (key_[key] != 0)
            pc_ += 2;
    }
    else if (low(opcode_) == 0x00A1)
    {
        if (key_[key] == 0)
            pc_ += 2;
    }
}

void CHIP8::handleFxxx()
{
    uint8_t x = xl(opcode_);

    switch (low(opcode_))
    {
    case 0x000A:
    {
        keyPress(x);
        break;
    }
    case 0x001E:
    {
        addToIndex(x);
        break;
    }
    case 0x0007:
    {
        getDT(x);
        break;
    }
    case 0x15:
    {
        setDT(x);
        break;
    }
    case 0x18:
    {
        setST(x);
        break;
    }
    case 0x29:
    {
        setSpriteToIndex(x);
        break;
    }
    case 0x33:
    {
        bcd(x);
        break;
    }
    case 0x55:
    {
        writeToMemory(x);
        break;
    }
    case 0x65:
    {
        loadFromMemory(x);
        break;
    }
    default:
        break;
    }
}