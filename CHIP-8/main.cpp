#include <iostream>
#include "chip8.h"
#include <string>

using namespace std;

int main()
{
    string rom_folder = "./test-suite/";
    string rom = "3";

    CHIP8 emu;
    emu.initialize();
    emu.loadROM(rom_folder + rom + ".ch8");

    for (;;)
    {
        emu.emulateCycle();
        emu.debugDisplay();
    }
}