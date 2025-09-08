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
//     SDL_Window *window = SDL_CreateWindow(
//         "CHIP-8 Emulator",
//         WINDOW_WIDTH, WINDOW_HEIGHT,
//         SDL_WINDOW_RESIZABLE);

//     SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);

//     SDL_Event event;
//     bool running = true;

//     while (running)
//     {
//         while (SDL_PollEvent(&event))
//         {
//             if (event.type == SDL_EVENT_QUIT)
//             {
//                 running = false;
//             }
//         }

//         SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
//         SDL_RenderClear(renderer);

//         SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

//         SDL_FRect square = {200, 150, 100, 100};

//         SDL_RenderFillRect(renderer, &square);

//         SDL_RenderPresent(renderer);
//         SDL_Delay(16); // Roughly 60 FPS
//     }

//     SDL_DestroyRenderer(renderer);
//     SDL_DestroyWindow(window);
//     SDL_Quit();

//     return 0;
// }

int main()
{
    SDLEmulator emulator;
    emulator.initialize();
    emulator.interpreter->initialize();

    string rom_folder = "./test-suite/";
    string rom = "3";

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

        emulator.interpreter->emulateCycle();
        emulator.updateGraphics();
        SDL_Delay(2); // Adjust delay for speed control
    }

    return 0;
}