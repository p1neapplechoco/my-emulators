compile: build run

IMGUI_DIR = third_party/imgui
IMGUI_SRC = $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_draw.cpp \
    $(IMGUI_DIR)/imgui_tables.cpp $(IMGUI_DIR)/imgui_widgets.cpp \
    $(IMGUI_DIR)/backends/imgui_impl_sdl3.cpp \
    $(IMGUI_DIR)/backends/imgui_impl_sdlrenderer3.cpp

build:
	g++ main.cpp chip8.cpp emulator.cpp \
    $(IMGUI_SRC) \
    -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends \
    -lSDL3 \
    -o test.exe

run:
	./test.exe