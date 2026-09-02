# Self Implemented Emulators

This repo is organized as a collection of emulator projects, each with its own folder and its own runtime. The root launcher at [launcher.py](launcher.py) picks which emulator to start.

## Project layout

- [CHIP-8/](CHIP-8/) — the current working emulator
- [NES/](NES/) — reserved for a future Nintendo emulator
- [launcher.py](launcher.py) — root-level launcher menu

## How to add a new emulator

Use this pattern for every new system:

1. Create a new folder, e.g. `GameBoy/`
2. Put a dedicated implementation in that folder
   - `main.cpp` for the entry point
   - `gameboy.cpp` / `gameboy.h` for the core logic
   - `gameboy_gui.cpp` for SDL + UI glue
3. Add a per-emulator `Makefile` if needed
4. Register it in [launcher.py](launcher.py) in the `EMULATORS` list
5. Optionally add a root target in the project `Makefile`

A minimal emulator entry should follow this shape:

```cpp
int main() {
    Emulator emu;
    emu.initialize();

    while (emu.running) {
        emu.processEvents();
        emu.update();
        emu.render();
    }

    return 0;
}
```

The important idea is separation of concerns:

- core emulator logic: CPU, memory, instruction handling
- renderer / window code: SDL, ImGui, input mapping
- launcher entry: chooses which emulator to run

## Keep the UI isolated

The CHIP-8 implementation is already organized around that idea:

- [CHIP-8/chip8.cpp](CHIP-8/chip8.cpp) contains the Chip-8 core machine logic
- [CHIP-8/chip8_gui.cpp](CHIP-8/chip8_gui.cpp) contains the SDL/ImGui window and controls
- [CHIP-8/chip8_gui.h](CHIP-8/chip8_gui.h) defines the shared state used by the GUI layer

This is the pattern to copy for your next emulator: keep the cartridge/system logic separate from the UI layer.

## Common commands

```bash
python3 launcher.py
python3 launcher.py chip8
python3 launcher.py --list
make chip8
```

## Adding a future emulator

When you make a new emulator, add a block like this to [launcher.py](launcher.py):

```python
{
    "id": "gameboy",
    "label": "Game Boy",
    "dir": "GameBoy",
    "build": ["make", "-C", "GameBoy", "build"],
    "run": ["./gameboy.exe"],
}
```

Then make sure the corresponding folder contains a `Makefile` and a runnable binary name that matches the script.
