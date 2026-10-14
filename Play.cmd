@echo off
setlocal
cd /d "%~dp0data"
if not exist "..\build\Release\wolf-ascii.exe" (
  echo Build the project first using scripts\build-windows.ps1.
  pause
  exit /b 1
)
if not exist "vswap.wl1" if exist "..\docs\vswap.wl1" copy /y "..\docs\*.wl1" . >nul
if not exist "vswap.wl1" if not exist "vswap.wl6" (
  echo Game data is missing. See data\README.md.
  pause
  exit /b 1
)
"..\build\Release\wolf-ascii.exe" --nowait %*
if errorlevel 1 pause
