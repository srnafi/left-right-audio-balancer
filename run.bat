@echo off
if not exist "%~dp0bin\LeftRightAudioBalancer.exe" (
    call "%~dp0build.bat"
)
start "" "%~dp0bin\LeftRightAudioBalancer.exe" %*
