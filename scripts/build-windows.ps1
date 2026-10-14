param([string]$DependencyPrefix = 'D:/vcpkg/installed/x64-windows', [ValidateSet('SHAREWARE','FULL')][string]$Edition='SHAREWARE')
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
cmake -S $root -B "$root/build" -A x64 "-DCMAKE_PREFIX_PATH=$DependencyPrefix" "-DWOLF_EDITION=$Edition"
if($LASTEXITCODE){throw 'Configure failed'}
cmake --build "$root/build" --config Release --parallel 4 -- /nodeReuse:false
if($LASTEXITCODE){throw 'Build failed'}
# Copy the SDL runtime and codec dependencies from the selected installation.
Copy-Item "$DependencyPrefix/bin/*.dll" "$root/build/Release/"
ctest --test-dir "$root/build" -C Release --output-on-failure
if($LASTEXITCODE){throw 'Tests failed'}
