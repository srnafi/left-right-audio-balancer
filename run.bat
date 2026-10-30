@echo off
if not exist "%~dp0src\LeftRightAudioBalancer\bin\LeftRightAudioBalancer.exe" (
    call "%~dp0build.bat"
)
start "" "%~dp0src\LeftRightAudioBalancer\bin\LeftRightAudioBalancer.exe" %*
