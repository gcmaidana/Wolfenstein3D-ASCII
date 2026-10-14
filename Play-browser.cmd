@echo off
cd /d "%~dp0"
node scripts\serve-web.cjs
if errorlevel 1 pause
