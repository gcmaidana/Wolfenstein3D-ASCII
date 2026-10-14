#include "ascii_core.h"
#include <iostream>
#include <stdexcept>
#include <vector>
static void require(bool value,const char* why) { if(!value) throw std::runtime_error(why); }
int main() {
 try {
  std::vector<ascii::Cell> cells;
  // Padded rows and odd dimensions catch stride errors and omitted edge pixels.
  std::vector<ascii::Pixel> input(8*5, {255,0,0,255});
  for(int y=0;y<5;++y) for(int x=0;x<7;++x) input[y*8+x]={255,255,255,255};
  ascii::convert(input.data(),7,5,8*4,3,2,cells);
  require(cells.size()==6,"grid size");
  for(auto c:cells) require(c.glyph==9 && c.g==255 && c.b==255,"stride and area coverage");
  input.assign(40,{0,0,0,255});
  ascii::convert(input.data(),7,5,32,3,2,cells);
  for(auto c:cells) require(c.glyph==0,"black must remain empty");
  std::vector<ascii::Pixel> output;
  ascii::rasterize(cells,3,2,false,output);
  require(output.size()==3*6*2*8,"raster dimensions");
  for(auto p:output) require(p.r==0 && p.g==0 && p.b==0 && p.a==255,"black raster");
  cells[0]={9,255,30,5};
  ascii::rasterize(cells,3,2,false,output);
  bool found=false; for(auto p:output) found |= p.r==255 && p.g==30 && p.b==5;
  require(found,"colored glyph");
  ascii::rasterize(cells,3,2,true,output);
  found=false; for(auto p:output) found |= p.r==190 && p.g==235 && p.b==195;
  require(found,"monochrome glyph");
  bool rejected=false;
  try { ascii::convert(input.data(),7,5,4,3,2,cells); } catch(const std::invalid_argument&) { rejected=true; }
  require(rejected,"reject short pitch");
  rejected=false;
  try { ascii::convert(input.data(),7,5,32,8,2,cells); } catch(const std::invalid_argument&) { rejected=true; }
  require(rejected,"reject oversized grid");
  std::cout<<"ASCII conversion, stride, edge coverage, raster and validation tests passed\n";
 } catch(const std::exception& error) { std::cerr<<error.what()<<'\n'; return 1; }
}
