#pragma once

#include <cstdint>
#include <vector>
#include <fstream>

struct NES_header
{
    uint8_t signature_[4];
};