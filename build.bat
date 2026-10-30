@echo off
setlocal enabledelayedexpansion
cd /d "%~dp0"

echo =======================================================
echo Building LeftRightAudioBalancer Native C++ (Direct2D)...
echo =======================================================

where cl.exe >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo Setting up MSVC environment...
    if exist "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" (
        call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
    ) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" (
        call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
    ) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" (
        call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" >nul
    ) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat" (
        call "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat" >nul
    ) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" (
        call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" >nul
    )
)

if not exist bin mkdir bin
if not exist obj mkdir obj

echo Compiling resource file...
rc /nologo /I include /fo obj\resource.res res\resource.rc
if %ERRORLEVEL% neq 0 (
    echo Resource compilation failed.
    exit /b %ERRORLEVEL%
)

echo Compiling modular C++ source files...
cl.exe /nologo /O2 /std:c++17 /EHsc /W3 /DUNICODE /D_UNICODE /DNOMINMAX /DNDEBUG /I include ^
    /Fo"obj\\" ^
    /c ^
    src\main.cpp ^
    src\MainWindow.cpp ^
    src\CardRenderer.cpp ^
    src\Widgets.cpp ^
    src\VectorIcons.cpp ^
    src\GraphicsResources.cpp ^
    src\AudioEngine.cpp ^
    src\AvrcpHelper.cpp ^
    src\SettingsStore.cpp

if %ERRORLEVEL% neq 0 (
    echo Compilation failed.
    exit /b %ERRORLEVEL%
)

echo Linking executable...
link.exe /nologo /SUBSYSTEM:WINDOWS ^
    /OUT:bin\LeftRightAudioBalancer.exe ^
    obj\main.obj ^
    obj\MainWindow.obj ^
    obj\CardRenderer.obj ^
    obj\Widgets.obj ^
    obj\VectorIcons.obj ^
    obj\GraphicsResources.obj ^
    obj\AudioEngine.obj ^
    obj\AvrcpHelper.obj ^
    obj\SettingsStore.obj ^
    obj\resource.res ^
    user32.lib gdi32.lib ole32.lib shell32.lib shlwapi.lib advapi32.lib d2d1.lib dwrite.lib dwmapi.lib

if %ERRORLEVEL% neq 0 (
    echo Linking failed.
    exit /b %ERRORLEVEL%
)

echo.
echo ========================================================
echo Build Succeeded! Executable: bin\LeftRightAudioBalancer.exe
echo ========================================================
