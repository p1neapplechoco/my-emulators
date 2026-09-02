#include "chip8_gui.h"
#include <iostream>
#include <algorithm>
#include <filesystem>

// File dialog callback
static void fileDialogCallback(void *userdata, const char *const *filelist, int filter)
{
    (void)filter;
    SDLEmulator *emu = static_cast<SDLEmulator *>(userdata);
    if (filelist && filelist[0])
    {
        emu->loadROM(filelist[0]);
    }
}

// Helper for scancode name
static const char *getScancodeName(SDL_Scancode sc)
{
    SDL_Keycode kc = SDL_GetKeyFromScancode(sc, SDL_KMOD_NONE, false);
    const char *name = SDL_GetKeyName(kc);
    return (name && name[0]) ? name : "???";
}

// CONSTRUCTOR
SDLEmulator::SDLEmulator()
{
    interpreter = new CHIP8();
    window = nullptr;
    renderer = nullptr;
    running = false;
    romLoaded = false;
    paused = false;
    showControlsWindow = false;
    showGraphicsWindow = false;
    showSpeedWindow = false;
    rebindingKey = -1;
}

// DESTRUCTOR
SDLEmulator::~SDLEmulator()
{
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    delete interpreter;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

// INITIALIZE SDL + ImGui
void SDLEmulator::initialize()
{
    int winW = SCREEN_W * config.pixelScale;
    int winH = SCREEN_H * config.pixelScale + static_cast<int>(MENU_BAR_HEIGHT);

    window = SDL_CreateWindow(
        "CHIP-8 Emulator",
        winW, winH,
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

    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();

    // Make menu bar compact
    ImGuiStyle &style = ImGui::GetStyle();
    style.FramePadding = ImVec2(4, 2);
    style.WindowPadding = ImVec2(4, 4);

    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    running = true;
}

void SDLEmulator::resizeWindow()
{
    int winW = SCREEN_W * config.pixelScale;
    int winH = SCREEN_H * config.pixelScale + static_cast<int>(MENU_BAR_HEIGHT);
    SDL_SetWindowSize(window, winW, winH);
}

void SDLEmulator::loadROM(const std::string &path)
{
    interpreter->initialize();
    try
    {
        interpreter->loadROM(path);
        currentRomPath = path;
        romLoaded = true;
        paused = false;
        addRecentFile(path);

        // Update window title
        std::filesystem::path p(path);
        std::string title = "CHIP-8 - " + p.filename().string();
        SDL_SetWindowTitle(window, title.c_str());
    }
    catch (const std::exception &e)
    {
        std::cerr << "Failed to load ROM: " << e.what() << std::endl;
        romLoaded = false;
    }
}

void SDLEmulator::addRecentFile(const std::string &path)
{
    // Remove if already exists
    auto it = std::find(recentFiles.begin(), recentFiles.end(), path);
    if (it != recentFiles.end())
        recentFiles.erase(it);

    recentFiles.push_front(path);

    while (recentFiles.size() > MAX_RECENT_FILES)
        recentFiles.pop_back();
}

void SDLEmulator::updateGraphics()
{
    if (!romLoaded || !interpreter->drawFlag())
        return;

    const uint8_t *gfx = interpreter->getGfx();

    // Get current window size and compute scale dynamically
    int winW, winH;
    SDL_GetWindowSize(window, &winW, &winH);

    float availH = static_cast<float>(winH) - MENU_BAR_HEIGHT;
    float scaleX = static_cast<float>(winW) / SCREEN_W;
    float scaleY = availH / SCREEN_H;
    float scale = (scaleX < scaleY) ? scaleX : scaleY;

    // Center the display if aspect ratio doesn't match
    float totalW = scale * SCREEN_W;
    float totalH = scale * SCREEN_H;
    float offsetX = (static_cast<float>(winW) - totalW) / 2.0f;
    float offsetY = MENU_BAR_HEIGHT + (availH - totalH) / 2.0f;

    uint8_t fgR = static_cast<uint8_t>(config.fgColor.x * 255);
    uint8_t fgG = static_cast<uint8_t>(config.fgColor.y * 255);
    uint8_t fgB = static_cast<uint8_t>(config.fgColor.z * 255);
    uint8_t bgR = static_cast<uint8_t>(config.bgColor.x * 255);
    uint8_t bgG = static_cast<uint8_t>(config.bgColor.y * 255);
    uint8_t bgB = static_cast<uint8_t>(config.bgColor.z * 255);

    for (size_t y = 0; y < SCREEN_H; y++)
    {
        for (size_t x = 0; x < SCREEN_W; x++)
        {
            if (gfx[y * SCREEN_W + x] == 1)
                SDL_SetRenderDrawColor(renderer, fgR, fgG, fgB, 255);
            else
                SDL_SetRenderDrawColor(renderer, bgR, bgG, bgB, 255);

            SDL_FRect pixelRect = {
                offsetX + static_cast<float>(x) * scale,
                offsetY + static_cast<float>(y) * scale,
                scale,
                scale};
            SDL_RenderFillRect(renderer, &pixelRect);
        }
    }
}

void SDLEmulator::renderUI()
{
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    if (ImGui::BeginMainMenuBar())
    {
        // ===== FILE MENU =====
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open ROM...", "Ctrl+O"))
            {
                SDL_DialogFileFilter filters[] = {
                    {"CHIP-8 ROMs", "ch8;c8;rom;bin"},
                    {"All files", "*"}};
                SDL_ShowOpenFileDialog(fileDialogCallback, this, window, filters, 2, NULL, false);
            }

            if (ImGui::BeginMenu("Recent", !recentFiles.empty()))
            {
                for (const auto &file : recentFiles)
                {
                    std::filesystem::path p(file);
                    if (ImGui::MenuItem(p.filename().string().c_str()))
                    {
                        loadROM(file);
                    }
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Clear Recent"))
                {
                    recentFiles.clear();
                }
                ImGui::EndMenu();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Reset", "Ctrl+R", false, romLoaded))
            {
                loadROM(currentRomPath);
            }

            if (ImGui::MenuItem(paused ? "Resume" : "Pause", "Space", false, romLoaded))
            {
                paused = !paused;
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Exit", "Alt+F4"))
            {
                running = false;
            }

            ImGui::EndMenu();
        }

        // ===== CONFIG MENU =====
        if (ImGui::BeginMenu("Config"))
        {
            if (ImGui::MenuItem("Controls..."))
            {
                showControlsWindow = true;
            }

            if (ImGui::MenuItem("Speed..."))
            {
                showSpeedWindow = true;
            }

            ImGui::EndMenu();
        }

        // ===== GRAPHICS MENU =====
        if (ImGui::BeginMenu("Graphics"))
        {
            if (ImGui::MenuItem("Settings..."))
            {
                showGraphicsWindow = true;
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    // ===== CONTROLS WINDOW =====
    if (showControlsWindow)
    {
        ImGui::SetNextWindowSize(ImVec2(320, 380), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Controls", &showControlsWindow))
        {
            ImGui::Text("CHIP-8 Keypad Mapping:");
            ImGui::Separator();

            const char *chip8KeyNames[] = {
                "0", "1", "2", "3", "4", "5", "6", "7",
                "8", "9", "A", "B", "C", "D", "E", "F"};

            for (int i = 0; i < 16; i++)
            {
                ImGui::PushID(i);
                ImGui::Text("Key %s:", chip8KeyNames[i]);
                ImGui::SameLine(80);

                if (rebindingKey == i)
                {
                    ImGui::Button(">> Press a key <<", ImVec2(160, 0));
                }
                else
                {
                    const char *name = getScancodeName(config.keymap[i]);
                    if (ImGui::Button(name, ImVec2(160, 0)))
                    {
                        rebindingKey = i;
                    }
                }
                ImGui::PopID();
            }

            ImGui::Separator();
            if (ImGui::Button("Reset to Default"))
            {
                SDL_Scancode defaults[16] = {
                    SDL_SCANCODE_X, SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3,
                    SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_A,
                    SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_Z, SDL_SCANCODE_C,
                    SDL_SCANCODE_4, SDL_SCANCODE_R, SDL_SCANCODE_F, SDL_SCANCODE_V};
                std::copy(defaults, defaults + 16, config.keymap);
                rebindingKey = -1;
            }
        }
        ImGui::End();
    }

    // ===== SPEED WINDOW =====
    if (showSpeedWindow)
    {
        ImGui::SetNextWindowSize(ImVec2(300, 160), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Speed Config", &showSpeedWindow))
        {
            ImGui::SliderInt("Cycles/Frame", &config.cyclesPerFrame, 1, 100);
            ImGui::SliderInt("Frame Interval (ms)", &config.frameIntervalMs, 1, 32);

            int ips = config.cyclesPerFrame * (1000 / config.frameIntervalMs);
            ImGui::Text("~ %d instructions/sec", ips);

            ImGui::Separator();
            if (ImGui::Button("Default (30 cyc, 8ms)"))
            {
                config.cyclesPerFrame = 30;
                config.frameIntervalMs = 8;
            }
            ImGui::SameLine();
            if (ImGui::Button("Fast (60 cyc, 4ms)"))
            {
                config.cyclesPerFrame = 60;
                config.frameIntervalMs = 4;
            }
        }
        ImGui::End();
    }

    // ===== GRAPHICS WINDOW =====
    if (showGraphicsWindow)
    {
        ImGui::SetNextWindowSize(ImVec2(300, 220), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Graphics Settings", &showGraphicsWindow))
        {
            int oldScale = config.pixelScale;
            ImGui::SliderInt("Pixel Scale", &config.pixelScale, 4, 20);
            if (config.pixelScale != oldScale)
            {
                resizeWindow();
            }

            ImGui::Separator();
            ImGui::ColorEdit3("Foreground", &config.fgColor.x);
            ImGui::ColorEdit3("Background", &config.bgColor.x);

            ImGui::Separator();
            if (ImGui::Button("Classic (White/Black)"))
            {
                config.fgColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                config.bgColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
            }
            ImGui::SameLine();
            if (ImGui::Button("Green Screen"))
            {
                config.fgColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
                config.bgColor = ImVec4(0.0f, 0.1f, 0.0f, 1.0f);
            }
        }
        ImGui::End();
    }

    // ===== RENDER ImGui =====
    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
}

void SDLEmulator::handleKey(SDL_Scancode scancode, bool pressed)
{
    // If rebinding a key in controls window
    if (pressed && rebindingKey >= 0)
    {
        config.keymap[rebindingKey] = scancode;
        rebindingKey = -1;
        return;
    }

    for (uint8_t i = 0; i < KEYPAD_KEYS; i++)
    {
        if (config.keymap[i] == scancode)
        {
            interpreter->setKey(i, pressed ? 1 : 0);
            return;
        }
    }
}

void SDLEmulator::processEvents()
{
    while (SDL_PollEvent(&event))
    {
        // Let ImGui process the event first
        ImGui_ImplSDL3_ProcessEvent(&event);

        // If ImGui wants keyboard/mouse, don't forward to emulator
        ImGuiIO &io = ImGui::GetIO();

        switch (event.type)
        {
        case SDL_EVENT_QUIT:
            running = false;
            break;
        case SDL_EVENT_KEY_DOWN:
            if (!io.WantCaptureKeyboard && !event.key.repeat)
                handleKey(event.key.scancode, true);
            // Rebinding captures regardless
            else if (rebindingKey >= 0 && !event.key.repeat)
                handleKey(event.key.scancode, true);

            // Shortcut: Space to pause
            if (event.key.scancode == SDL_SCANCODE_SPACE && !event.key.repeat && !io.WantCaptureKeyboard)
                paused = !paused;
            break;
        case SDL_EVENT_KEY_UP:
            if (!io.WantCaptureKeyboard)
                handleKey(event.key.scancode, false);
            break;
        }
    }
}