#pragma once
#include <SDL.h>
bool WA_Present(SDL_Renderer* renderer, SDL_Surface* frame);
bool WA_Key(SDL_Keycode key);
bool WA_NeedsRedraw();
extern bool WA_GameplayView;
void WA_Shutdown();
extern "C" void WA_SetMode(int mode);
extern "C" void WA_SetDetail(int detail);
