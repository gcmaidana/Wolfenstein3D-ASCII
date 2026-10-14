# Third-party sources

- **Wolfenstein 3D** — id Software. Original release: https://github.com/id-Software/wolf3d
- **Wolf4SDL** — Moritz "Ripper" Kroll and contributors; selected repository: https://github.com/fabiangreffrath/wolf4sdl, revision `a51c229ed89cab1904d68abb8938def3cf456725`. Upstream README and both license texts are preserved in `engine/`. This project chooses the GPL option described by that README.
- **Nuked OPL3** — copyright and LGPL-2.1-or-later notice in `engine/opl3.c` / `engine/opl3.h`, preserved unchanged.
- **SDL2 / SDL2_mixer** — https://github.com/libsdl-org/SDL and https://github.com/libsdl-org/SDL_mixer, zlib licenses. Browser SDK ports used SDL 2.32.8 and SDL_mixer 2.8.0. Desktop uses the installed vcpkg libraries. Dependency source is available at those repositories; installed dependency notices are copied into the website package.
- **Emscripten** — https://github.com/emscripten-core/emscripten, SDK 4.0.15. Compiler tooling remains in `tools/emsdk/`, outside the deployment package.
- **DoomGeneric / doom-ascii** — architectural inspiration only; no code copied from those repositories.

The website includes the shareware Episode 1 data. Game data is excluded from the engine source archive. Its provenance and archive hash are in `data/README.md`.
