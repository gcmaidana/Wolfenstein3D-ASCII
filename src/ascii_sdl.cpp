// SPDX-License-Identifier: GPL-2.0-or-later
#include "ascii_sdl.h"
#include "ascii_core.h"
#include <vector>
#include <cstdio>
#include <cstdlib>
bool WA_GameplayView=false;

namespace {
int mode=1, detail=0;
bool redraw=false;
SDL_Texture* texture=nullptr;
int textureWidth=0, textureHeight=0;
std::vector<ascii::Pixel> source, raster;
std::vector<ascii::Cell> cells;
}
extern "C" void WA_SetMode(int value) { if(value>=0 && value<=2) { mode=value; redraw=true; } }
extern "C" void WA_SetDetail(int value) { detail=value ? 1 : 0; redraw=true; }
bool WA_NeedsRedraw() { const bool value=redraw; redraw=false; return value; }
bool WA_Key(SDL_Keycode key) {
 if(key==SDLK_F6) { WA_SetMode((mode+1)%3); return true; }
 if(key==SDLK_F7) { WA_SetDetail(1-detail); return true; }
 return false;
}
void WA_Shutdown() { SDL_DestroyTexture(texture); texture=nullptr; textureWidth=textureHeight=0; }
bool WA_Present(SDL_Renderer* renderer, SDL_Surface* frame) {
 if(mode==0) return false;
 source.resize(std::size_t(frame->w)*frame->h);
 if(SDL_ConvertPixels(frame->w,frame->h,frame->format->format,frame->pixels,frame->pitch,
                      SDL_PIXELFORMAT_RGBA32,source.data(),frame->w*4)<0) {
   std::fprintf(stderr,"ASCII conversion failed: %s\n",SDL_GetError()); return false;
 }
 const int cols=detail ? 160:80, rows=detail ? 100:50;
 ascii::convert(source.data(),frame->w,frame->h,frame->w*4,cols,rows,cells);
 ascii::rasterize(cells,cols,rows,mode==2,raster);
 const int w=cols*ascii::glyphWidth,h=rows*ascii::glyphHeight;
#ifndef __EMSCRIPTEN__
 // Opt-in headless integration capture; never enabled during ordinary play.
 static int frames=0;
 const char* capture=std::getenv("WOLF_ASCII_CAPTURE");
 const char* frameNumber=std::getenv("WOLF_ASCII_CAPTURE_FRAME");
 const int captureFrame=frameNumber ? std::atoi(frameNumber) : 180;
 if(capture && ++frames==captureFrame) {
   SDL_Surface* shot=SDL_CreateRGBSurfaceWithFormatFrom(raster.data(),w,h,32,w*4,SDL_PIXELFORMAT_RGBA32);
   if(!shot || SDL_SaveBMP(shot,capture)<0) {
     std::fprintf(stderr,"Capture failed: %s\n",SDL_GetError()); std::exit(2);
   }
   SDL_FreeSurface(shot);
   std::fprintf(stdout,"ASCII frame captured: %s\n",capture);
   if(std::getenv("WOLF_ASCII_QUIT_AFTER_CAPTURE")) {
     SDL_Event event{}; event.type=SDL_QUIT; SDL_PushEvent(&event);
   }
 }
#endif
 if(!texture || w!=textureWidth || h!=textureHeight) {
   WA_Shutdown();
   texture=SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA32,SDL_TEXTUREACCESS_STREAMING,w,h);
   if(!texture) { std::fprintf(stderr,"ASCII texture failed: %s\n",SDL_GetError()); return false; }
   textureWidth=w; textureHeight=h;
 }
 SDL_UpdateTexture(texture,nullptr,raster.data(),w*4);
 SDL_RenderClear(renderer);
 SDL_RenderCopy(renderer,texture,nullptr,nullptr);
 SDL_RenderPresent(renderer);
 return true;
}
