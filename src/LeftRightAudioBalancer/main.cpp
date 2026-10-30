#include <windows.h>
#include <shellapi.h>
#include <fstream>
#include "AppState.h"
#include "GraphicsResources.h"
#include "MainWindow.h"
#include "SettingsStore.h"
#include "AvrcpHelper.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, int nCmdShow) {
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);

    // 1. Elevated AVRCP Absolute Volume Handler
    for (int i = 1; i < argc; ++i) {
        if (_wcsicmp(argv[i], L"--set-absolute-volume") == 0 && i + 1 < argc) {
            bool enable = (_wcsicmp(argv[i + 1], L"1") == 0);
            int res = AvrcpHelper::WriteRegistry(enable);
            LocalFree(argv);
            CoUninitialize();
            return res;
        }
    }

    // 2. CI Diagnostic: Shortcut Check
    for (int i = 1; i < argc; ++i) {
        if (_wcsicmp(argv[i], L"--shortcut-check") == 0 && i + 1 < argc) {
            std::wstring reportPath = argv[i + 1];
            std::wstring report = SettingsStore::PerformShortcutCheck();
            std::wofstream f(reportPath);
            if (f.is_open()) {
                f << report;
                f.close();
            }
            LocalFree(argv);
            CoUninitialize();
            return 0;
        }
    }

    // Parse test / capture flags
    std::wstring shotPath = L"";
    std::wstring mockDevice = L"";
    bool shotMode = false;
    bool drawerMode = false;

    if (argv) {
        for (int i = 1; i < argc; ++i) {
            if (_wcsicmp(argv[i], L"--shot") == 0 && i + 1 < argc) {
                shotMode = true;
                shotPath = argv[++i];
            } else if (_wcsicmp(argv[i], L"--drawer") == 0) {
                drawerMode = true;
            } else if (_wcsicmp(argv[i], L"--device") == 0 && i + 1 < argc) {
                mockDevice = argv[++i];
            }
        }
        LocalFree(argv);
    }

    // Initialize Direct2D & DirectWrite typography
    if (!GraphicsResources::Initialize()) {
        CoUninitialize();
        return 1;
    }

    // Load persistent settings
    PersistentSettings saved = SettingsStore::Load();

    AppState& app = MainWindow::GetAppState();
    AudioEngine& audio = MainWindow::GetAudioEngine();

    app.locked = saved.locked;
    app.linked = saved.linked;
    app.startOnBoot = SettingsStore::IsStartOnBootEnabled();

    // Initialize Audio Engine
    if (audio.Initialize()) {
        if (!saved.deviceId.empty()) {
            audio.BindDevice(saved.deviceId);
        }
        audio.GetLevels(app.leftVol, app.rightVol, app.masterVol);
        app.currentDeviceId = audio.GetCurrentDeviceId();
        app.currentDeviceName = audio.GetCurrentDeviceName();
        app.isBluetooth = audio.IsCurrentBluetooth();
    }

    if (!mockDevice.empty()) {
        app.currentDeviceName = mockDevice;
        app.isBluetooth = true;
    }

    // Window positioning
    int posX = 0;
    int posY = 0;
    if (saved.hasPosition && saved.windowLeft >= 0 && saved.windowTop >= 0) {
        posX = saved.windowLeft;
        posY = saved.windowTop;
    } else {
        RECT workArea;
        SystemParametersInfoW(SPI_GETWORKAREA, 0, &workArea, 0);
        posX = workArea.right - CARD_WIDTH - 24;
        posY = workArea.bottom - CARD_BASE_HEIGHT - 24;
    }

    // Register class and create layered window
    MainWindow::RegisterWindowClass(hInstance);
    HWND hwnd = MainWindow::Create(hInstance, app, posX, posY);
    if (!hwnd) {
        GraphicsResources::Release();
        CoUninitialize();
        return 1;
    }

    // Headless Screenshot Mode for Automated Verification & Testing
    if (shotMode && !shotPath.empty()) {
        int h = drawerMode ? CARD_DRAWER_HEIGHT : CARD_BASE_HEIGHT;
        app.drawerOpen = drawerMode;
        MainWindow::PresentLayered(hwnd, app, h);
        MainWindow::SaveBufferToBmp(shotPath.c_str(), CARD_WIDTH, h);
        DestroyWindow(hwnd);
        GraphicsResources::Release();
        CoUninitialize();
        return 0;
    }

    // Initial Presentation & Working Set Trim
    MainWindow::PresentLayered(hwnd, app, CARD_BASE_HEIGHT);
    ShowWindow(hwnd, nCmdShow);
    SetProcessWorkingSetSize(GetCurrentProcess(), (SIZE_T)-1, (SIZE_T)-1);

    // Run Message Loop
    int result = MainWindow::RunMessageLoop(hwnd);

    // Final Cleanup
    GraphicsResources::Release();
    CoUninitialize();
    return result;
}
