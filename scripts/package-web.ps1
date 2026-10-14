param([switch]$Publish)
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$dest="$root/dist/web"
$source="$root/dist/source/WolfASCII"
New-Item -ItemType Directory -Force $dest,$source | Out-Null
foreach($file in @('index.html','wolf-ascii.js','wolf-ascii.wasm')) { Copy-Item "$root/build-web/$file" $dest }
# Bundle only the assets for the single shareware episode, never saves or config.
foreach($name in @('audiohed','audiot','gamemaps','maphead','vgadict','vgagraph','vgahead','vswap')) {
    $asset="$root/data/$name.wl1"
    if(!(Test-Path $asset)) { $asset="$root/docs/$name.wl1" }
    Copy-Item -LiteralPath $asset -Destination "$dest/$name.wl1"
}
foreach($file in @('CMakeLists.txt','README.md','LICENSE','THIRD_PARTY.md','Play.cmd','Play-browser.cmd','.gitignore')) { Copy-Item "$root/$file" $source }
foreach($dir in @('src','tests','web','scripts','licenses')) { Copy-Item "$root/$dir" $source -Recurse -Force }
New-Item -ItemType Directory -Force "$source/data" | Out-Null
Copy-Item "$root/data/README.md" "$source/data/README.md"
# Enumerate tracked engine source so the nested Git database is not packaged.
if(Test-Path "$root/engine/.git") {
    $engineFiles=& git -C "$root/engine" ls-files
    if($LASTEXITCODE){throw 'Unable to enumerate upstream engine source'}
} else {
    $engineRoot=(Resolve-Path "$root/engine").Path
    $engineFiles=Get-ChildItem $engineRoot -File -Recurse | ForEach-Object { $_.FullName.Substring($engineRoot.Length+1) }
}
foreach($file in $engineFiles) {
    $target=Join-Path "$source/engine" $file
    New-Item -ItemType Directory -Force (Split-Path $target -Parent) | Out-Null
    Copy-Item -LiteralPath "$root/engine/$file" -Destination $target
}
Compress-Archive -Path $source -DestinationPath "$dest/source.zip" -Force
Copy-Item "$root/LICENSE","$root/THIRD_PARTY.md" $dest
Copy-Item "$root/licenses" $dest -Recurse -Force
Write-Host "Website files, Episode 1 assets, and corresponding engine source are in $dest."
if($Publish) {
    New-Item -ItemType Directory -Force "$root/docs" | Out-Null
    Copy-Item "$dest/*" "$root/docs" -Recurse -Force
    Set-Content "$root/docs/.nojekyll" ''
    Write-Host 'Updated docs/ for GitHub Pages.'
}
