param (
    [string]$BuildDir = "",
    [string]$OutDir = ""
)

$ErrorActionPreference = "Stop"

Add-Type -AssemblyName "System.IO.Compression.FileSystem"

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$RootDir = Split-Path -Parent (Split-Path -Parent $ScriptDir)

if ([string]::IsNullOrEmpty($BuildDir)) {
    $BuildDir = [System.IO.Path]::Combine($RootDir, "build")
}
if ([string]::IsNullOrEmpty($OutDir)) {
    $OutDir = [System.IO.Path]::Combine($RootDir, "dist", "NodeForge-Portable-win64")
}

Write-Host "===> Packaging NodeForge Portable Distribution..." -ForegroundColor Cyan
Write-Host "Root Directory: $RootDir"
Write-Host "Build Directory: $BuildDir"
Write-Host "Output Directory: $OutDir"

# 1. Prepare output folders
if (Test-Path -LiteralPath $OutDir) {
    Remove-Item -Recurse -Force -LiteralPath $OutDir
}
New-Item -ItemType Directory -Force -Path "$OutDir/bin" | Out-Null
New-Item -ItemType Directory -Force -Path "$OutDir/plugins" | Out-Null
New-Item -ItemType Directory -Force -Path "$OutDir/sdk/include" | Out-Null
New-Item -ItemType Directory -Force -Path "$OutDir/sdk/samples" | Out-Null
New-Item -ItemType Directory -Force -Path "$OutDir/shaders" | Out-Null

# 2. Copy Executables and DLLs
$BuildBinDir = [System.IO.Path]::Combine($BuildDir, "bin")
Write-Host "Copying binaries from $BuildBinDir..."
if (Test-Path -LiteralPath $BuildBinDir) {
    Get-ChildItem -LiteralPath $BuildBinDir -Filter "*.exe" | ForEach-Object {
        Copy-Item -LiteralPath $_.FullName -Destination "$OutDir/bin/" -Force
    }
    Get-ChildItem -LiteralPath $BuildBinDir -Filter "*.dll" | ForEach-Object {
        if ($_.Name -like "*Plugin.dll") {
            Copy-Item -LiteralPath $_.FullName -Destination "$OutDir/plugins/" -Force
        } else {
            Copy-Item -LiteralPath $_.FullName -Destination "$OutDir/bin/" -Force
        }
    }
}

# 3. Copy Plugins from lib if any
$BuildLibDir = [System.IO.Path]::Combine($BuildDir, "lib")
if (Test-Path -LiteralPath $BuildLibDir) {
    Get-ChildItem -LiteralPath $BuildLibDir -Filter "*Plugin.dll" | ForEach-Object {
        Copy-Item -LiteralPath $_.FullName -Destination "$OutDir/plugins/" -Force
    }
}

# 4. Copy SDK Headers and README
Write-Host "Copying SDK headers..."
$SdkIncludeDir = [System.IO.Path]::Combine($RootDir, "sdk", "include")
$SdkSamplesDir = [System.IO.Path]::Combine($RootDir, "sdk", "samples")
$SdkReadmeFile = [System.IO.Path]::Combine($RootDir, "sdk", "README.md")

if (Test-Path -LiteralPath $SdkIncludeDir) {
    Copy-Item -Recurse -LiteralPath "$SdkIncludeDir/*" -Destination "$OutDir/sdk/include/" -Force
}
if (Test-Path -LiteralPath $SdkSamplesDir) {
    Copy-Item -Recurse -LiteralPath "$SdkSamplesDir/*" -Destination "$OutDir/sdk/samples/" -Force
}
if (Test-Path -LiteralPath $SdkReadmeFile) {
    Copy-Item -LiteralPath $SdkReadmeFile -Destination "$OutDir/sdk/" -Force
}

# 5. Copy Shaders and Assets
$ShadersDir = [System.IO.Path]::Combine($RootDir, "shaders")
if (Test-Path -LiteralPath $ShadersDir) {
    Copy-Item -Recurse -LiteralPath "$ShadersDir/*" -Destination "$OutDir/shaders/" -Force -ErrorAction SilentlyContinue
}

# 6. Create Zip Archive (.NET System.IO.Compression.ZipFile for reliable cross-platform zip)
$DistFolder = [System.IO.Path]::GetFullPath([System.IO.Path]::GetDirectoryName($OutDir))
if (-not (Test-Path -LiteralPath $DistFolder)) {
    New-Item -ItemType Directory -Force -Path $DistFolder | Out-Null
}
$ZipPath = [System.IO.Path]::Combine($DistFolder, "NodeForge-Portable-v0.1.0-win64.zip")
Write-Host "Creating archive $ZipPath..."
if (Test-Path -LiteralPath $ZipPath) {
    Remove-Item -Force -LiteralPath $ZipPath
}

$ResolvedOutDir = [System.IO.Path]::GetFullPath($OutDir)
[System.IO.Compression.ZipFile]::CreateFromDirectory($ResolvedOutDir, $ZipPath, [System.IO.Compression.CompressionLevel]::Optimal, $false)

# 7. Create Standalone SDK Zip Archive
$SdkZipPath = [System.IO.Path]::Combine($DistFolder, "NodeForge-SDK-v0.1.0-win64.zip")
Write-Host "Creating SDK archive $SdkZipPath..."
if (Test-Path -LiteralPath $SdkZipPath) {
    Remove-Item -Force -LiteralPath $SdkZipPath
}
$ResolvedSdkDir = [System.IO.Path]::Combine($OutDir, "sdk")
[System.IO.Compression.ZipFile]::CreateFromDirectory($ResolvedSdkDir, $SdkZipPath, [System.IO.Compression.CompressionLevel]::Optimal, $false)

Write-Host "===> Portable distribution successfully created at: $ZipPath" -ForegroundColor Green
Write-Host "===> Standalone SDK archive created at: $SdkZipPath" -ForegroundColor Green
