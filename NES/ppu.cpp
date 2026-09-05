#include "ppu.h"
#include <iomanip>
#include <cstdlib>
#include <unistd.h> // for usleep

void NES_ppu::setBus(NES_bus &bus) { bus_ = &bus; }
void NES_ppu::setCartridge(NES_cartridge &cartridge) { cartridge_ = &cartridge; }

void NES_ppu::initialize()
{
    ctrl_ = 0;
    mask_ = 0;
    status_ = 0;
    oamAddr_ = 0;
    oamData_ = 0;
    scroll_ = 0;
    addr_ = 0;
    data_ = 0;
    oamDMA_ = 0;

    cycle_ = 0;
    scanline_ = 0;

    v_ = 0;
    t_ = 0;
    x_ = 0;
    w_ = 0;

    nmi_ = false;
}

void NES_ppu::tick()
{
    cycle_++;

    if (cycle_ == 341)
    {
        cycle_ = 0;
        scanline_++;
    }

    if (scanline_ <= 239 && cycle_ >= 1 && cycle_ <= 256)
    {
        plot(cycle_ - 1, scanline_); // plot pixel to screen buffer
    }

    if (scanline_ == 241 && cycle_ == 1)
    {
        statusSetFlag(VBLANK, true);
        if (ctrlGetFlag(NMI_ENABLE))
            triggerNMI();

        debugGraphic(); // for debugging purposes, prints the screen buffer to console
    }

    if (scanline_ == 261 && cycle_ == 1)
    {
        statusSetFlag(VBLANK, false);
    }

    if (scanline_ == 262)
    {
        scanline_ = 0;
    }
}

uint8_t NES_ppu::bgPixel(uint64_t x, uint64_t y)
{
    uint8_t tile = nameTbl_[(y / 8) * 32 + (x / 8)];
    uint16_t base = ctrlGetFlag(BG_PATTERN_ADDR) ? 0x1000 : 0x0000;

    uint8_t lowByte = cartridge_->readPPU(base + tile * 16 + (y % 8));
    uint8_t highByte = cartridge_->readPPU(base + tile * 16 + (y % 8) + 8);

    int bit = 7 - (x % 8);
    return ((highByte >> bit) & 1) << 1 | ((lowByte >> bit) & 1);
}

void NES_ppu::plot(uint64_t x, uint64_t y)
{
    uint8_t pixel = bgPixel(x, y);
    uint8_t color = paletteTbl_[pixel];

    frameBuffer_[y * SCREEN_W * 4 + x * 4 + 0] = color;
    frameBuffer_[y * SCREEN_W * 4 + x * 4 + 1] = color;
    frameBuffer_[y * SCREEN_W * 4 + x * 4 + 2] = color;
    frameBuffer_[y * SCREEN_W * 4 + x * 4 + 3] = 0xFF;
}

uint8_t NES_ppu::readCPU(uint16_t addr)
{
    uint8_t data = 0x00;

    if (addr >= 0x2000 && addr <= 0x3FFF)
    {
        // Handle reads from PPU registers here
        uint16_t reg = addr & 0x2007;

        if (reg == 0x2002) // "Reading this register has the side effect of clearing the PPU's internal w register." Source: nesdev.org/wiki/PPU_registers
        {
            data = status_;
            statusSetFlag(VBLANK, false);

            w_ = 0;
        }
        else if (reg == 0x2004)
            data = oamData_;
        else if (reg == 0x2007)
        {
            data = data_;
            data_ = readPPU(v_ & 0x3FFF);
            v_ += ctrlGetFlag(VRAM_ADDR_INC) ? 32 : 1;
        }
    }
    else if (addr == 0x4014)
    {
        // Handle OAM DMA read here
    }
    else
    {
        // Handle other CPU reads from PPU memory here
    }

    return data;
}

void NES_ppu::writeCPU(uint16_t addr, uint8_t data)
{
    if (addr >= 0x2000 && addr <= 0x3FFF)
    {
        // Handle writes to PPU registers here
        uint16_t reg = addr & 0x2007;

        if (reg == 0x2000)
            ctrl_ = data;
        else if (reg == 0x2001)
            mask_ = data;
        else if (reg == 0x2003)
            oamAddr_ = data;
        else if (reg == 0x2004)
            oamData_ = data;
        else if (reg == 0x2005)
        {
            scroll_ = data;
            w_ = 1 - w_; // Toggle w_ between 0 and 1
        }
        else if (reg == 0x2006)
        {
            t_ = (w_ == 0) ? (t_ & 0x00FF) | ((data & 0x3F) << 8) : (t_ & 0xFF00) | data; // if w_ is 0, set the high byte of t_, else set the low byte of t_
            v_ = (w_ == 0) ? v_ : t_;                                                     // if w_ is 0, v_ remains unchanged, else v_ is set to t_
            w_ = 1 - w_;
        }
        else if (reg == 0x2007)
        {
            writePPU(v_ & 0x3FFF, data);
            v_ += ctrlGetFlag(VRAM_ADDR_INC) ? 32 : 1; // if VRAM_ADDR_INC is set, increment by 32, else increment by 1
        }
    }
    else if (addr == 0x4014)
    {
        // Handle OAM DMA write here
    }
    else
    {
        // Handle other CPU writes to PPU memory here
    }
}

uint8_t NES_ppu::readPPU(uint16_t addr)
{
    uint8_t data = 0x00;

    if (addr >= 0x0000 && addr <= 0x1FFF)
    {
        data = cartridge_->readPPU(addr);
    }
    else if (addr >= 0x2000 && addr <= 0x3EFF)
    {
        data = nameTbl_[addr & 0x07FF];
    }
    else if (addr >= 0x3F00 && addr <= 0x3FFF)
    {
        data = paletteTbl_[addr & 0x001F];
    }

    return data;
}

void NES_ppu::writePPU(uint16_t addr, uint8_t data)
{
    if (addr >= 0x0000 && addr <= 0x1FFF)
    {
    }
    else if (addr >= 0x2000 && addr <= 0x3EFF)
    {
        nameTbl_[addr & 0x07FF] = data;
    }
    else if (addr >= 0x3F00 && addr <= 0x3FFF)
    {
        paletteTbl_[addr & 0x001F] = data;
    }
}

bool NES_ppu::requestNMI()
{
    if (!nmi_)
        return false;

    nmi_ = false;
    return true;
}

void NES_ppu::triggerNMI()
{
    nmi_ = true;
}

void NES_ppu::debugGraphic()
{
    system("clear"); // clear the console
    for (uint64_t y = 0; y < SCREEN_H; y += 1)
    {
        for (uint64_t x = 0; x < SCREEN_W; x += 1)
        {
            uint8_t r = frameBuffer_[y * SCREEN_W * 4 + x * 4 + 0];
            uint8_t g = frameBuffer_[y * SCREEN_W * 4 + x * 4 + 1];
            uint8_t b = frameBuffer_[y * SCREEN_W * 4 + x * 4 + 2];

            std::cout << "\033[48;2;" << (int)r << ";" << (int)g << ";" << (int)b << "m  \033[0m";
        }
        std::cout << std::endl;
    }

    usleep(100000); // sleep for 100ms
}

// REGISTER METHODS
// PPUCTRL
bool NES_ppu::ctrlGetFlag(ctrlFlags_ flag) { return (ctrl_ & flag) != 0; }
void NES_ppu::ctrlSetFlag(ctrlFlags_ flag, bool condition) { ctrl_ = (ctrl_ & ~flag) | (-(uint8_t)condition & flag); }

// PPUMASK
bool NES_ppu::maskGetFlag(maskFlags_ flag) { return (mask_ & flag) != 0; }
void NES_ppu::maskSetFlag(maskFlags_ flag, bool condition) { mask_ = (mask_ & ~flag) | (-(uint8_t)condition & flag); }

// PPUSTATUS
bool NES_ppu::statusGetFlag(statusFlags_ flag) { return (status_ & flag) != 0; }
void NES_ppu::statusSetFlag(statusFlags_ flag, bool condition) { status_ = (status_ & ~flag) | (-(uint8_t)condition & flag); }
