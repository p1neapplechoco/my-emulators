#include <SDL3/SDL.h>
#include "third_party/imgui/imgui.h"
#include "third_party/imgui/backends/imgui_impl_sdl3.h"
#include "third_party/imgui/backends/imgui_impl_sdlrenderer3.h"
#include "chip8.h"

#include <string>
#include <vector>
#include <deque>

// SCREEN DIMENSIONS

static constexpr int PIXEL_W = 10;
static constexpr int PIXEL_H = 10;

// Max recent files to remember
static constexpr int MAX_RECENT_FILES = 8;

// Menu bar height (pixels)
static constexpr float MENU_BAR_HEIGHT = 20.0f;

struct EmulatorConfig
{
    // Speed
    int cyclesPerFrame = 30;
    int frameIntervalMs = 8;

    // Graphics
    int pixelScale = 10;
    ImVec4 fgColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // white
    ImVec4 bgColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f); // black

    // Controls (scancode for each CHIP-8 key 0-F)
    SDL_Scancode keymap[16] = {
        SDL_SCANCODE_X, // 0
        SDL_SCANCODE_1, // 1
        SDL_SCANCODE_2, // 2
        SDL_SCANCODE_3, // 3
        SDL_SCANCODE_Q, // 4
        SDL_SCANCODE_W, // 5
        SDL_SCANCODE_E, // 6
        SDL_SCANCODE_A, // 7
        SDL_SCANCODE_S, // 8
        SDL_SCANCODE_D, // 9
        SDL_SCANCODE_Z, // A
        SDL_SCANCODE_C, // B
        SDL_SCANCODE_4, // C
        SDL_SCANCODE_R, // D
        SDL_SCANCODE_F, // E
        SDL_SCANCODE_V  // F
    };
};

class SDLEmulator
{
public:
    CHIP8 *interpreter;

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Event event;

    bool running;
    bool romLoaded;
    bool paused;

    EmulatorConfig config;
    std::string currentRomPath;
    std::deque<std::string> recentFiles;

    // UI state
    bool showControlsWindow;
    bool showGraphicsWindow;
    bool showSpeedWindow;
    int rebindingKey; // -1 = not rebinding, 0-15 = which key

    SDLEmulator();
    ~SDLEmulator();

    void initialize();
    void updateGraphics();
    void renderUI();

    void handleKey(SDL_Scancode scancode, bool pressed);
    void processEvents();

    void loadROM(const std::string &path);
    void addRecentFile(const std::string &path);
    void resizeWindow();
};