# Game data

The engine source does not include the maps, textures, or audio.

The default build requires **Wolfenstein 3D shareware v1.4**. Place these eight files here:

`audiohed.wl1`, `audiot.wl1`, `gamemaps.wl1`, `maphead.wl1`,
`vgadict.wl1`, `vgagraph.wl1`, `vgahead.wl1`, `vswap.wl1`.

Local testing data was obtained from the [Wolf3D.net shareware package](https://beta.wolf3d.net/games/29/downloads/4477).
Only these eight data files were copied; none of the package's executables or DLLs are used.
The downloaded archive is retained under `downloads/` and excluded from source control.
SHA256: `3ff0798f7d912dcd03f78419b55d63dd1934a93b6b3be1fab425dc2b148612cc`.

The website package copies these files and loads Episode 1 automatically. Players do not select or upload files.
For the full GT/ID v1.4 edition, rebuild with `-DWOLF_EDITION=FULL` and use the equivalent `.wl6` files on desktop.
Other releases require different engine version definitions and are not supported by these presets.
Game assets retain their original terms; they are not covered by the engine's GPL license.
