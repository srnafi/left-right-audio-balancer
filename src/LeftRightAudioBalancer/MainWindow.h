#pragma once
#include <windows.h>
#include <d2d1.h>
#include "AppState.h"
#include "AudioEngine.h"

class MainWindow {
public:
    static bool RegisterWindowClass(HINSTANCE hInstance);
    static HWND Create(HINSTANCE hInstance, AppState& app, int posX, int posY);

    static void InitLayeredBuffer(HWND hwnd);
    static void PresentLayered(HWND hwnd, AppState& app, int curHeight);
    static bool SaveBufferToBmp(const wchar_t* filename, int width, int height);

    static int RunMessageLoop(HWND hwnd);
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    static AppState& GetAppState();
    static AudioEngine& GetAudioEngine();

private:
    static void OnTimer(HWND hwnd);
    static void OnMouseMove(HWND hwnd, int x, int y);
    static void OnLButtonDown(HWND hwnd, int x, int y);
    static void OnLButtonUp(HWND hwnd);
};
