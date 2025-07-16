#include "chip8.h"

void CHIP8::clearDisplay()
{
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
        this->gfx[i] = 0;
}
void CHIP8::clearStack()
{
    for (int i = 0; i < STACK_SIZE; i++)
        this->stack[i] = 0;
}

void CHIP8::clearRegisters()
{
    for (int i = 0; i < REG_SIZE; i++)
        this->V[i] = 0;
}

void CHIP8::clearMemory()
{
    for (int i = 0; i < MEM_SIZE; i++)
        this->mem[i] = 0;
}

CHIP8::CHIP8() = default;
CHIP8::~CHIP8() = delete;

void CHIP8::initialize()
{
    this->pc = 0x200;
    this->opcode = 0;
    this->I = 0;
    this->sp = 0;

    this->clearDisplay();
    this->clearStack();
    this->clearRegisters();
    this->clearMemory();

    for (int i = 0; i < FONTSET_SIZE; i++)
        this->mem[i] = chip8_fontset[i];

    this->delay_timer = 0;
    this->sound_timer = 0;
}

void CHIP8::__fetch()
{
    unsigned char first_byte = this->mem[this->pc];
    unsigned char second_byte = this->mem[this->pc + 1];

    this->opcode = first_byte << 8 | second_byte;
}

void CHIP8::__execute()
{
}

void CHIP8::emulateCycle()
{
    __fetch();
    this->pc += 2;
    __execute();
}
