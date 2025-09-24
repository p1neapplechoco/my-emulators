#include <SDL3/SDL.h>
#include "chip8.h"

// SCREEN DIMENSIONS

static constexpr int PIXEL_W = 10;
static constexpr int PIXEL_H = 10;

class SDLEmulator
{
public:
    CHIP8 *interpreter;

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Event event;

    bool running;

    SDLEmulator();
    ~SDLEmulator();

    void initialize();
    void updateGraphics();
    void processInput();
};