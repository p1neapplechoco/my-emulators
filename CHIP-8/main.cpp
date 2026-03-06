#include <iostream>
#include "chip8.h"
#include "emulator.h"
#include <string>
#include <SDL3/SDL.h>

using namespace std;

int main()
{
    SDLEmulator emulator;
    emulator.initialize();
    emulator.interpreter->initialize();

    Uint64 lastTick = SDL_GetTicks();

    while (emulator.running)
    {
        emulator.processEvents();

        Uint64 now = SDL_GetTicks();
        Uint64 interval = static_cast<Uint64>(emulator.config.frameIntervalMs);

        if (now - lastTick >= interval)
        {
            lastTick = now;

            // Clear the entire screen
            SDL_SetRenderDrawColor(emulator.renderer, 0, 0, 0, 255);
            SDL_RenderClear(emulator.renderer);

            if (emulator.romLoaded && !emulator.paused)
            {
                for (int i = 0; i < emulator.config.cyclesPerFrame; i++)
                {
                    emulator.processEvents();
                    emulator.interpreter->emulateCycle();
                }

                emulator.interpreter->updateTimers();
            }

            // Draw CHIP-8 display
            emulator.updateGraphics();

            // Draw ImGui UI on top
            emulator.renderUI();

            // Present everything
            SDL_RenderPresent(emulator.renderer);
        }
        else
        {
            SDL_Delay(1);
        }
    }

    return 0;
}