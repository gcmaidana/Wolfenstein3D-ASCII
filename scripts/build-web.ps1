param([string]$Emsdk = "$PSScriptRoot/../tools/emsdk")
$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
$Emsdk = (Resolve-Path $Emsdk).Path
if (!(Test-Path "$Emsdk/emsdk_env.ps1")) { throw 'Install and activate Emscripten first. See README.md.' }
. "$Emsdk/emsdk_env.ps1"
$ninja = Get-Command ninja -ErrorAction SilentlyContinue
if (!$ninja) {
    $vswhere = "${env:ProgramFiles(x86)}/Microsoft Visual Studio/Installer/vswhere.exe"
    if (Test-Path $vswhere) {
        $vs = & $vswhere -latest -property installationPath
        $ninjaDir = "$vs/Common7/IDE/CommonExtensions/Microsoft/CMake/Ninja"
        if (Test-Path "$ninjaDir/ninja.exe") { $env:PATH = "$ninjaDir;$env:PATH" }
    }
}
& "$Emsdk/upstream/emscripten/emcmake.bat" cmake -S $root -B "$root/build-web" -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF
if ($LASTEXITCODE) { throw 'Web configure failed' }
cmake --build "$root/build-web" --parallel 4
if ($LASTEXITCODE) { throw 'Web build failed' }
Write-Host "Serve $root/build-web over HTTP. Open index.html through that server."
