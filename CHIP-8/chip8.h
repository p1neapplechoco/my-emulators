#pragma once

#ifndef _CHIP8H_
#define _CHIP8H_

#define MEM_SIZE 4096
#define REG_SIZE 16

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

#define STACK_SIZE 16
#define KEYPAD 16

#define FONTSET_SIZE 80

const unsigned char chip8_fontset[80] = {
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
protected:
    unsigned short opcode;       // 2 bytes of opcode, e.g 0xA000
    unsigned char mem[MEM_SIZE]; // 4096 bytes
    unsigned char V[REG_SIZE];   // 16 8-bit registers
    unsigned short I;            // Index register
    unsigned short pc;           // Program counter

    unsigned char gfx[SCREEN_WIDTH * SCREEN_HEIGHT]; // Display screen

    unsigned char sound_timer;
    unsigned char delay_timer;

    unsigned short stack[STACK_SIZE]; // 16-level stack
    unsigned short sp;                // stack pointer

    unsigned char key[KEYPAD]; // 16 keys

    bool drawFlag;

    void clearDisplay();
    void clearStack();
    void clearRegisters();
    void clearMemory();

    void (*Table[16])();
    void (*Arithmetic[16])();

    void __fetch();
    void __execute();

public:
    CHIP8();          // Constructor
    virtual ~CHIP8(); // Destructor

    void loadROM(const char *);

    void initialize();
    void emulateCycle();
};

#endif