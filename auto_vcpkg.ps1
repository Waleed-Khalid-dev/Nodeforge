Set-Location -LiteralPath 'D:\[Project]\Touch Designer'
$env:PATH += ";C:\Program Files;C:\dev\vcpkg"
$env:VCPKG_ROOT = "C:\dev\vcpkg"
$env:VCPKG_FORCE_SYSTEM_BINARIES = "1"

for ($i = 0; $i -lt 150; $i++) {
    Write-Host "
--- Iteration $i ---"
    Write-Host "Running CMake Configure..."
    
    $cmakeOutput = cmake -B build -G Ninja -DCMAKE_MAKE_PROGRAM="C:\Program Files\ninja.exe" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" 2>&1
    $outputString = $cmakeOutput | Out-String
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host "SUCCESS! CMake finished cleanly!"
        break
    }
    
    if ($outputString -match "Downloading (https?://\S+)\s+->\s+(\S+)") {
        $url = $matches[1]
        $file = $matches[2]
        $dest = "C:\dev\vcpkg\downloads\$file"
        
        Write-Host "CMake failed on $file"
        Write-Host "Auto-Hacking: Manually downloading $file..."
        
        & C:\Windows\System32\curl.exe --insecure -L $url -o $dest
        
        if ($LASTEXITCODE -eq 0) {
            Write-Host "Download successful. Restarting CMake..."
        } else {
            Write-Host "Manual download failed! Exiting."
            break
        }
    } else {
        Write-Host "CMake failed for a non-download reason or URL not found. Exiting."
        Write-Host $outputString
        break
    }
}
