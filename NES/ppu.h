// graphical stuffs
#pragma once

#include "bus.h"
#include "cartridge.h"
#include <cstdint>
#include <vector>
#include <iostream>

// GRAPHICAL CONSTANTS
static constexpr std::size_t SCREEN_W = 256;
static constexpr std::size_t SCREEN_H = 240;

class NES_ppu
{
public:
    NES_ppu() = default;

    void setBus(NES_bus &);
    void setCartridge(NES_cartridge &);

    void initialize();
    void tick();
    void plot(uint64_t, uint64_t); // plot pixel to screen buffer

    uint8_t readCPU(uint16_t);
    void writeCPU(uint16_t, uint8_t);

    uint8_t readPPU(uint16_t);
    void writePPU(uint16_t, uint8_t);

    bool requestNMI();
    void triggerNMI();

    // REGISTER METHODS
    // PPUCTRL
    enum ctrlFlags_ : uint8_t
    {
        NAMETABL0 = 1 << 0,
        NAMETABL1 = 2 << 0,
        VRAM_ADDR_INC = 1 << 2,
        SPRITE_PATTERN_ADDR = 1 << 3,
        BG_PATTERN_ADDR = 1 << 4,
        SPRITE_SIZE = 1 << 5,
        MASTER_SLAVE = 1 << 6,
        NMI_ENABLE = 1 << 7,
    };

    bool ctrlGetFlag(ctrlFlags_);
    void ctrlSetFlag(ctrlFlags_, bool);

    // PPUMASK
    enum maskFlags_ : uint8_t
    {
        GRAYSCALE = 1 << 0,
        SHOW_BG_LEFT = 1 << 1,
        SHOW_SPRITES_LEFT = 1 << 2,
        RENDER_BG = 1 << 3,
        RENDER_SPRITES = 1 << 4,
        EMPHASIZE_RED = 1 << 5,
        EMPHASIZE_GREEN = 1 << 6,
        EMPHASIZE_BLUE = 1 << 7,
    };

    bool maskGetFlag(maskFlags_);
    void maskSetFlag(maskFlags_, bool);

    // PPUSTATUS
    enum statusFlags_ : uint8_t
    {
        /*
        this dude does not use 0-4 bits
        */
        SPRITE_OVERFLOW = 1 << 5,
        SPRITE_ZERO_HIT = 1 << 6,
        VBLANK = 1 << 7,
    };

    bool statusGetFlag(statusFlags_);
    void statusSetFlag(statusFlags_, bool);

    // OAMADDR

    // OAMDATA

    // PPUSCROLL

    // PPUADDR

    // PPUDATA

    // MISCs
    void debugGraphic();

private:
    uint8_t bgPixel(uint64_t x, uint64_t y);
    // Registers
    uint8_t ctrl_;    // $2000
    uint8_t mask_;    // $2001
    uint8_t status_;  // $2002
    uint8_t oamAddr_; // $2003
    uint8_t oamData_; // $2004
    uint8_t scroll_;  // $2005
    uint8_t addr_;    // $2006
    uint8_t data_;    // $2007
    uint8_t oamDMA_;  // $4014s

    // Internal Registers
    uint16_t v_; // current VRAM address (15 bits)
    uint16_t t_; // temporary VRAM address (15 bits)
    uint8_t x_;  // fine X scroll (3 bits)
    uint8_t w_;  // write toggle (1 bit)

    uint64_t cycle_;
    uint64_t scanline_;
    bool nmi_;

    uint8_t frameBuffer_[SCREEN_W * SCREEN_H * 4]; // RGBA buffer for the screen

    NES_bus *bus_;
    NES_cartridge *cartridge_;

    // Pattern tables
    /*
    source: nesdev.org/wiki/PPU_pattern_tables

    An NES pattern table is a 4096-byte (4 KiB) region in PPU memory address space.
    The PPU addresses two pattern tables at $0000–$0FFF and $1000–$1FFF.

    Each tile in the pattern table is 16 bytes, made of two 8×8 bitplanes:
    - The first 8 bytes represent bit 0 of each pixel in the 8×8 tile (the low bitplane).
    - The second 8 bytes represent bit 1 of each pixel in the 8×8 tile (the high bitplane).
    */

    /* WTF IS THIS
    The NES has four logical nametables, arranged in a 2x2 pattern. Each occupies a 1 KiB chunk of PPU address space, starting at $2000 at the top left, $2400 at the top right, $2800 at the bottom left, and $2C00 at the bottom right.

    But the NES system board itself has only 2 KiB of VRAM (called CIRAM, stored in a separate SRAM chip), enough for two physical nametables; hardware on the cartridge controls address bit 10 of CIRAM to map one nametable on top of another.
    */
    // uint8_t nameTbl0_[1024]; // $2000-$23FF // Temporarily not using this because im too lazy
    // uint8_t nameTbl1_[1024]; // $2400-$27FF
    // uint8_t nameTbl2_[1024]; // $2800-$2BFF
    // uint8_t nameTbl3_[1024]; // $2C00-$2FFF

    uint8_t nameTbl_[2048]; // $2000-$2FFF

    uint8_t paletteTbl_[32]; // $3F00-$3F1F
};