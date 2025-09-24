#include <iostream>
#include "chip8.h"
#include "emulator.h"
#include <string>
#include <SDL3/SDL.h>

const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 320;

using namespace std;

// int main()
// {
//     CHIP8 chip8;
//     chip8.initialize();
//     string rom_folder = "./test-suite/";
//     string rom = "2";
//     chip8.loadROM(rom_folder + rom + ".ch8");
//     for (;;)
//     {
//         chip8.emulateCycle();
//         chip8.debugDisplay();
//     }
// }

int main()
{
    SDLEmulator emulator;
    emulator.initialize();
    emulator.interpreter->initialize();

    string rom_folder = "./test-suite/";
    string rom = "6";

    emulator.interpreter->loadROM(rom_folder + rom + ".ch8");

    while (emulator.running)
    {
        while (SDL_PollEvent(&emulator.event))
        {
            if (emulator.event.type == SDL_EVENT_QUIT)
            {
                emulator.running = false;
            }
        }

        emulator.processInput();
        emulator.interpreter->emulateCycle();
        emulator.updateGraphics();
        SDL_Delay(2); // Adjust delay for speed control
    }

    return 0;
}