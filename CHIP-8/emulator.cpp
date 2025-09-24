#include "emulator.h"
#include <iostream>

// KEY MAPPING

const uint8_t key_mapping[KEYPAD_KEYS] = {
    SDL_SCANCODE_1, // 0
    SDL_SCANCODE_2, // 1
    SDL_SCANCODE_3, // 2
    SDL_SCANCODE_4, // 3
    SDL_SCANCODE_Q, // 4
    SDL_SCANCODE_W, // 5
    SDL_SCANCODE_E, // 6
    SDL_SCANCODE_R, // 7
    SDL_SCANCODE_A, // 8
    SDL_SCANCODE_S, // 9
    SDL_SCANCODE_D, // A
    SDL_SCANCODE_F, // B
    SDL_SCANCODE_Z, // C
    SDL_SCANCODE_X, // D
    SDL_SCANCODE_C, // E
    SDL_SCANCODE_V  // F
};

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

    // Clear screen

    const uint8_t *gfx = interpreter->getGfx();

    for (size_t y = 0; y < SCREEN_H; y++)
    {
        for (size_t x = 0; x < SCREEN_W; x++)
        {
            if (gfx[y * SCREEN_W + x] == 1)
            {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            }
            else
            {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            }

            SDL_FRect pixelRect = {
                static_cast<float>(x * PIXEL_W),
                static_cast<float>(y * PIXEL_H),
                static_cast<float>(PIXEL_W),
                static_cast<float>(PIXEL_H)};
            SDL_RenderFillRect(renderer, &pixelRect);
        }
    }

    SDL_RenderPresent(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void SDLEmulator::processInput()
{
    // get the current state of the keyboard
    const bool *state = SDL_GetKeyboardState(NULL);

    for (uint8_t i = 0; i < KEYPAD_KEYS; i++)
    {
        if (state[key_mapping[i]])
            interpreter->setKey(i, 1);
        else
            interpreter->setKey(i, 0);
    }
}