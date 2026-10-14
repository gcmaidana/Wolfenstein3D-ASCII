#pragma once
#include <cstdint>
#include <vector>

namespace ascii {
struct Pixel { std::uint8_t r, g, b, a; };
struct Cell { std::uint8_t glyph, r, g, b; };
// Input is RGBA bytes with explicit row pitch. Output buffers retain capacity.
void convert(const Pixel* pixels, int width, int height, int pitchBytes,
             int columns, int rows, std::vector<Cell>& output);
void rasterize(const std::vector<Cell>& cells, int columns, int rows,
               bool monochrome, std::vector<Pixel>& output);
constexpr int glyphWidth = 6, glyphHeight = 8;
const char* ramp();
}
