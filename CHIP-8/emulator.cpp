#include "emulator.h"
#include <iostream>

// CONSTRUCTOR
SDLEmulator::SDLEmulator()
{
    interpreter = new CHIP8();
    window = nullptr;
    renderer = nullptr;
    running = false;
}

// DESTRUCTOR
SDLEmulator::~SDLEmulator()
{
    delete interpreter;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

// INITIALIZE SDL
void SDLEmulator::initialize()
{
    window = SDL_CreateWindow(
        "CHIP-8 Emulator",
        SCREEN_W * PIXEL_W, SCREEN_H * PIXEL_H,
        SDL_WINDOW_RESIZABLE);

    if (!window)
    {
        std::cerr << "Error creating window: " << SDL_GetError() << std::endl;
        return;
    }

    renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer)
    {
        std::cerr << "Error creating renderer: " << SDL_GetError() << std::endl;
        return;
    }

    running = true;
}

void SDLEmulator::updateGraphics()
{
    if (!interpreter->drawFlag())
        return;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    const uint8_t *gfx = interpreter->getGfx();

    for (size_t y = 0; y < SCREEN_H; y++)
    {
        for (size_t x = 0; x < SCREEN_W; x++)
        {
            if (gfx[y * SCREEN_W + x] == 1)
            {
                SDL_FRect pixelRect = {
                    static_cast<float>(x * PIXEL_W),
                    static_cast<float>(y * PIXEL_H),
                    static_cast<float>(PIXEL_W),
                    static_cast<float>(PIXEL_H)};
                SDL_RenderFillRect(renderer, &pixelRect);
            }
        }
    }

    SDL_RenderPresent(renderer);
}