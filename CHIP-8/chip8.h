#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <chrono>

static constexpr std::size_t MEM_SIZE = 4096;
static constexpr std::size_t REG_COUNT = 16;
static constexpr std::size_t SCREEN_W = 64;
static constexpr std::size_t SCREEN_H = 32;
static constexpr std::size_t STACK_DEPTH = 16;
static constexpr std::size_t KEYPAD_KEYS = 16;
static constexpr std::size_t FONTSET_SZ = 80;
static constexpr std::size_t BYTE_MAX = 0xFF;

static constexpr unsigned char chip8_fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

class CHIP8
{
private:
    using instr = void (CHIP8::*)();

    // Instruction set
    static instr opcodeTable_[16];

    void handle0xxx();
    void handle8xxx();
    void handleFxxx();

    static instr subTable8_[15];

protected:
    unsigned short opcode_;       // 2 bytes of opcode, e.g 0xA000
    unsigned char mem_[MEM_SIZE]; // 4096 bytes
    unsigned char V_[REG_COUNT];  // 16 8-bit registers
    unsigned short I_;            // Index register
    unsigned short pc_;           // Program counter

    unsigned char gfx_[SCREEN_W * SCREEN_H]; // Display screen

    unsigned char sound_timer_;
    unsigned char delay_timer_;

    unsigned short stack_[STACK_DEPTH]; // 16-level stack
    unsigned short sp_;                 // stack pointer

    unsigned char key_[KEYPAD_KEYS]; // 16 keys

    // Better random system
    unsigned seed_;
    std::mt19937 rng_;
    std::uniform_int_distribution<unsigned short> dist_;

    bool drawFlag_;

    void clearDisplay();
    void clearStack();
    void clearRegisters();
    void clearMemory();

    void sysCall(unsigned short);
    void returnFromSubroutine();

    void jumpToAddress();
    void jumpToAddressAddV0();

    void callSubroutine();

    void skipInstruction();
    void skipIfEqual(unsigned short, unsigned short);
    void skipIfNotEqual(unsigned short, unsigned short);

    void modifyRegister();
    void setRegister(unsigned short, unsigned short);
    void setRegisterRandom(unsigned short, unsigned short);
    void addToRegister(unsigned short, unsigned short);

    void loadVxFromVy();
    void orVxVy();
    void andVxVy();
    void xorVxVy();
    void addVxVy();
    void subVxVy();
    void shrVx();
    void subnVxVy();
    void shlVx();

    void modifyIndex();
    void setIndex(unsigned short);
    void addToIndex(unsigned short);
    void setIndexToSprite(unsigned short);

    void getDelayTimer(unsigned short);
    void getKeyPress(unsigned short);
    void setDelayTimer(unsigned short);
    void setSoundTimer(unsigned short);
    void setBCD(unsigned short);

    void writeToMemory(unsigned short);
    void readFromMemory(unsigned short);

    void draw();

    void cpuNULL();

    void fetch();
    void execute();

public:
    CHIP8();          // Constructor
    virtual ~CHIP8(); // Destructor

    void loadROM(const char *);

    void initialize();
    void emulateCycle();
};