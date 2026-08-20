@echo off
setlocal enabledelayedexpansion

if exist "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat" (
    call "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" (
    call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" (
    call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
)

echo Visual Studio environment loaded.
cd /d "%~dp0"
cmake --build "build" --config Debug --target nodeforge_tests
if %ERRORLEVEL% NEQ 0 (
    echo Build failed with error %ERRORLEVEL%
    exit /b %ERRORLEVEL%
)

echo Build succeeded. Running tests...
"%~dp0build\bin\nodeforge_tests.exe"
exit /b %ERRORLEVEL%
