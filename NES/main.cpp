#include "bus.h"
#include "cpu.h"
#include "cartridge.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>

// for sleep
#include <unistd.h>

using namespace std;

int main()
{
    NES_cartridge cartridge;
    cartridge.loadROM("tests/nestest.nes");

    NES_bus bus;
    bus.setCartridge(cartridge);

    NES_ppu ppu;
    bus.setPPU(ppu);

    NES_cpu cpu;
    bus.setCPU(cpu);
    cpu.setBus(bus);

    cpu.initialize();

    try
    {
        while (true)
        {
            cpu.emulateCycle();
            sleep(1);
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}