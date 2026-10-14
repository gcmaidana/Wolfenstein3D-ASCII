// SPDX-License-Identifier: GPL-2.0-or-later
#include "ascii_core.h"
#include <algorithm>
#include <stdexcept>

namespace ascii {
// Original 5x7 bitmaps, increasing ink coverage: space . : - = + * # % @
static constexpr std::uint8_t font[][7] = {
 {0,0,0,0,0,0,0}, {0,0,0,0,0,4,4}, {0,4,4,0,4,4,0},
 {0,0,0,31,0,0,0}, {0,0,31,0,31,0,0}, {0,4,4,31,4,4,0},
 {0,21,14,31,14,21,0}, {10,31,10,10,31,10,0},
 {25,27,2,4,8,27,19}, {14,17,23,21,23,16,15}
};
const char* ramp() { return " .:-=+*#%@"; }

void convert(const Pixel* pixels, int width, int height, int pitchBytes,
             int columns, int rows, std::vector<Cell>& output) {
 if (!pixels || width < 1 || height < 1 || width > 16384 || height > 16384 ||
     pitchBytes < width * int(sizeof(Pixel)) || columns < 1 || rows < 1 ||
     columns > width || rows > height)
   throw std::invalid_argument("Invalid ASCII framebuffer dimensions or pitch");
 output.resize(std::size_t(columns) * rows);
 for (int cy=0; cy<rows; ++cy) for (int cx=0; cx<columns; ++cx) {
   const int x0=cx*width/columns, x1=(cx+1)*width/columns;
   const int y0=cy*height/rows, y1=(cy+1)*height/rows;
   std::uint64_t r=0,g=0,b=0;
   for(int y=y0; y<y1; ++y) {
     auto line=reinterpret_cast<const Pixel*>(reinterpret_cast<const std::uint8_t*>(pixels)+std::size_t(y)*pitchBytes);
     for(int x=x0; x<x1; ++x) { r+=line[x].r; g+=line[x].g; b+=line[x].b; }
   }
   const auto n=std::uint64_t(x1-x0)*(y1-y0);
   r/=n; g/=n; b/=n;
   const unsigned luminance=unsigned((54*r+183*g+19*b)/256);
   // Lift shadows for readable walls without turning black into visible ink.
   const unsigned lifted=std::min(255u, luminance*3/2);
   auto& cell=output[std::size_t(cy)*columns+cx];
   cell={std::uint8_t((lifted*9+127)/255),std::uint8_t(r),std::uint8_t(g),std::uint8_t(b)};
 }
}
void rasterize(const std::vector<Cell>& cells, int columns, int rows,
               bool monochrome, std::vector<Pixel>& output) {
 if(columns<1 || rows<1 || columns>1024 || rows>1024 || cells.size()!=std::size_t(columns)*rows)
   throw std::invalid_argument("Invalid ASCII grid");
 const int w=columns*glyphWidth, h=rows*glyphHeight;
 output.assign(std::size_t(w)*h, Pixel{0,0,0,255});
 for(int cy=0;cy<rows;++cy) for(int cx=0;cx<columns;++cx) {
   const auto& c=cells[std::size_t(cy)*columns+cx];
   const unsigned peak=std::max({unsigned(c.r),unsigned(c.g),unsigned(c.b),1u});
   const unsigned gain=std::max(180u,peak);
   const Pixel ink=monochrome ? Pixel{190,235,195,255} :
     Pixel{std::uint8_t(c.r*gain/peak),std::uint8_t(c.g*gain/peak),std::uint8_t(c.b*gain/peak),255};
   for(int y=0;y<7;++y) for(int x=0;x<5;++x)
     if(font[std::min<unsigned>(c.glyph,9)][y] & (1<<(4-x)))
       output[std::size_t(cy*glyphHeight+y)*w+cx*glyphWidth+x]=ink;
 }
}
}
