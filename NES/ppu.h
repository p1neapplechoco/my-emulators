// graphical stuffs
#pragma once

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
    void initialize();
    void tick();

    uint8_t readCPU(uint16_t);
    void writeCPU(uint16_t, uint8_t);

    uint8_t readPPU(uint16_t);
    void writePPU(uint16_t, uint8_t);

    bool requestNMI();

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

private:
    // Registers
    uint8_t ctrl_;
    uint8_t mask_;
    uint8_t status_;
    uint8_t oamAddr_;
    uint8_t oamData_;
    uint8_t scroll_;
    uint8_t addr_;
    uint8_t data_;
    uint8_t oamDMA_;

    uint64_t cycle_;
    uint64_t scanline_;

    // Pattern tables
    /*
    source: nesdev.org/wiki/PPU_pattern_tables

    An NES pattern table is a 4096-byte (4 KiB) region in PPU memory address space.
    The PPU addresses two pattern tables at $0000–$0FFF and $1000–$1FFF.

    Each tile in the pattern table is 16 bytes, made of two 8×8 bitplanes:
    - The first 8 bytes represent bit 0 of each pixel in the 8×8 tile (the low bitplane).
    - The second 8 bytes represent bit 1 of each pixel in the 8×8 tile (the high bitplane).
    */
    std::vector<uint8_t> patternTbl0_ = std::vector<uint8_t>(4096, 0x00);
    std::vector<uint8_t> patternTbl1_ = std::vector<uint8_t>(4096, 0x00);
};