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
    ppu.initialize();
    bus.setPPU(ppu);
    ppu.setBus(bus);
    ppu.setCartridge(cartridge);

    NES_cpu cpu;
    bus.setCPU(cpu);
    cpu.setBus(bus);

    cpu.initialize();

    try
    {
        while (true)
        {
            int cycle = cpu.emulateCycle();
            // sleep(1);

            // wait for 1 ms
            usleep(0);

            for (int i = 0; i < cycle * 3; i++)
            {
                ppu.tick();
                if (ppu.requestNMI())
                {
                    cpu.nmi();
                }
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}