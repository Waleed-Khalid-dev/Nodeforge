Set-Location -LiteralPath 'D:\[Project]\Touch Designer'
$env:PATH += ";C:\Program Files;C:\dev\vcpkg"
$env:VCPKG_ROOT = "C:\dev\vcpkg"
$env:VCPKG_FORCE_SYSTEM_BINARIES = "1"

for ($i = 0; $i -lt 150; $i++) {
    Write-Host "\n--- Iteration $i ---"
    Write-Host "Running patch_cmake.py..."
    python.exe patch_cmake.py
    
    Write-Host "Running CMake Configure..."
    $cmakeOutput = cmake -B build -G Ninja -DCMAKE_MAKE_PROGRAM="C:\Program Files\ninja.exe" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" 2>&1
    $outputString = $cmakeOutput | Out-String
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host "SUCCESS! CMake finished cleanly!"
        break
    }
    
    Write-Host "CMake/vcpkg failed. Running patch_cmake.py again and retrying..."
    # Optional: we could extract the download hack here, but curl already downloaded 7zip, cmake, powershell.
}
