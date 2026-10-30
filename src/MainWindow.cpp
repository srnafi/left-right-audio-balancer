#include "MainWindow.h"
#include "GraphicsResources.h"
#include "CardRenderer.h"
#include "Widgets.h"
#include "SettingsStore.h"
#include "AvrcpHelper.h"
#include "resource.h"
#include <cmath>
#include <algorithm>

static HDC s_memDC = nullptr;
static HBITMAP s_hBmp = nullptr;
static void* s_pBits = nullptr;
static ID2D1DCRenderTarget* s_pDCRT = nullptr;

static AppState s_app;
static AudioEngine s_audio;
static const wchar_t* WINDOW_CLASS_NAME = L"LeftRightAudioBalancerNative";

AppState& MainWindow::GetAppState() { return s_app; }
AudioEngine& MainWindow::GetAudioEngine() { return s_audio; }

bool MainWindow::RegisterWindowClass(HINSTANCE hInstance) {
    WNDCLASSEXW wc = { sizeof(WNDCLASSEXW) };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = WINDOW_CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPICON));
    wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPICON));
    return RegisterClassExW(&wc) != 0;
}

HWND MainWindow::Create(HINSTANCE hInstance, AppState& app, int posX, int posY) {
    s_app = app;

    HWND hwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_APPWINDOW,
        WINDOW_CLASS_NAME,
        L"Left Right Audio Balancer",
        WS_POPUP,
        posX, posY, CARD_WIDTH, CARD_BASE_HEIGHT,
        nullptr, nullptr, hInstance, nullptr
    );

    if (hwnd) {
        InitLayeredBuffer(hwnd);
    }
    return hwnd;
}

void MainWindow::InitLayeredBuffer(HWND hwnd) {
    if (s_memDC) return;

    HDC screenDC = GetDC(nullptr);
    s_memDC = CreateCompatibleDC(screenDC);

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = CARD_WIDTH;
    bmi.bmiHeader.biHeight = -CARD_DRAWER_HEIGHT; // Top-down DIB
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    s_hBmp = CreateDIBSection(screenDC, &bmi, DIB_RGB_COLORS, &s_pBits, nullptr, 0);
    SelectObject(s_memDC, s_hBmp);
    ReleaseDC(nullptr, screenDC);

    ID2D1Factory* pFactory = GraphicsResources::GetD2DFactory();
    if (pFactory) {
        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
            0, 0,
            D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE,
            D2D1_FEATURE_LEVEL_DEFAULT
        );
        pFactory->CreateDCRenderTarget(&props, &s_pDCRT);
        if (s_pDCRT) {
            s_pDCRT->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);
            s_pDCRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
        }
    }
}

void MainWindow::PresentLayered(HWND hwnd, AppState& app, int curHeight) {
    if (!s_pDCRT || !s_memDC) return;

    CardRenderer::Render(s_pDCRT, s_memDC, app, curHeight);

    POINT ptSrc = { 0, 0 };
    SIZE sz = { CARD_WIDTH, curHeight };
    BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    UpdateLayeredWindow(hwnd, nullptr, nullptr, &sz, s_memDC, &ptSrc, 0, &blend, ULW_ALPHA);
}

bool MainWindow::SaveBufferToBmp(const wchar_t* filename, int width, int height) {
    if (!s_pBits) return false;

    BITMAPFILEHEADER bfh = {};
    bfh.bfType = 0x4D42; // 'BM'
    bfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (width * height * 4);
    bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    BITMAPINFOHEADER bih = {};
    bih.biSize = sizeof(BITMAPINFOHEADER);
    bih.biWidth = width;
    bih.biHeight = -height;
    bih.biPlanes = 1;
    bih.biBitCount = 32;
    bih.biCompression = BI_RGB;

    HANDLE hFile = CreateFileW(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return false;

    DWORD written = 0;
    WriteFile(hFile, &bfh, sizeof(bfh), &written, NULL);
    WriteFile(hFile, &bih, sizeof(bih), &written, NULL);
    WriteFile(hFile, s_pBits, width * height * 4, &written, NULL);
    CloseHandle(hFile);
    return true;
}

void MainWindow::OnTimer(HWND hwnd) {
    bool needRedraw = false;

    // 1. Audio Peak Meters
    float lPeak = 0.0f, rPeak = 0.0f;
    s_audio.GetPeaks(lPeak, rPeak);
    lPeak = std::min(1.0f, lPeak * 1.5f);
    rPeak = std::min(1.0f, rPeak * 1.5f);

    float prevL = s_app.leftMeter;
    float prevR = s_app.rightMeter;

    s_app.leftMeter = std::max(lPeak, s_app.leftMeter * 0.86f);
    if (s_app.leftMeter < 0.005f) s_app.leftMeter = 0.0f;

    s_app.rightMeter = std::max(rPeak, s_app.rightMeter * 0.86f);
    if (s_app.rightMeter < 0.005f) s_app.rightMeter = 0.0f;

    if (std::abs(prevL - s_app.leftMeter) > 0.005f || std::abs(prevR - s_app.rightMeter) > 0.005f) {
        needRedraw = true;
    }

    // 2. Halo animations
    float targetHaloL = (s_app.dragging == DRAG_LEFT) ? 1.0f : (s_app.hovered == HOVER_SLIDER_L ? 0.6f : 0.0f);
    float targetHaloR = (s_app.dragging == DRAG_RIGHT) ? 1.0f : (s_app.hovered == HOVER_SLIDER_R ? 0.6f : 0.0f);
    float targetHaloM = (s_app.dragging == DRAG_MASTER) ? 1.0f : (s_app.hovered == HOVER_SLIDER_M ? 0.6f : 0.0f);

    if (std::abs(s_app.haloL - targetHaloL) > 0.01f) { s_app.haloL += (targetHaloL - s_app.haloL) * 0.25f; needRedraw = true; }
    if (std::abs(s_app.haloR - targetHaloR) > 0.01f) { s_app.haloR += (targetHaloR - s_app.haloR) * 0.25f; needRedraw = true; }
    if (std::abs(s_app.haloM - targetHaloM) > 0.01f) { s_app.haloM += (targetHaloM - s_app.haloM) * 0.25f; needRedraw = true; }

    // 3. Hover fades
    float targetDev = (s_app.hovered == HOVER_DEVICE) ? 1.0f : 0.0f;
    float targetLock = (s_app.hovered == HOVER_LOCK) ? 1.0f : 0.0f;
    float targetGear = (s_app.hovered == HOVER_GEAR) ? 1.0f : 0.0f;
    float targetCentre = (s_app.hovered == HOVER_CENTRE) ? 1.0f : 0.0f;

    if (std::abs(s_app.hoverDevice - targetDev) > 0.02f) { s_app.hoverDevice += (targetDev - s_app.hoverDevice) * 0.25f; needRedraw = true; }
    if (std::abs(s_app.hoverLock - targetLock) > 0.02f) { s_app.hoverLock += (targetLock - s_app.hoverLock) * 0.25f; needRedraw = true; }
    if (std::abs(s_app.hoverGear - targetGear) > 0.02f) { s_app.hoverGear += (targetGear - s_app.hoverGear) * 0.25f; needRedraw = true; }
    if (std::abs(s_app.hoverCentre - targetCentre) > 0.02f) { s_app.hoverCentre += (targetCentre - s_app.hoverCentre) * 0.25f; needRedraw = true; }

    // 4. Smooth Drawer Height Animation
    s_app.targetHeight = s_app.drawerOpen ? (float)CARD_DRAWER_HEIGHT : (float)CARD_BASE_HEIGHT;
    if (std::abs(s_app.currentHeight - s_app.targetHeight) > 1.0f) {
        s_app.currentHeight += (s_app.targetHeight - s_app.currentHeight) * 0.28f;
        needRedraw = true;
    } else {
        s_app.currentHeight = s_app.targetHeight;
    }

    if (needRedraw) {
        PresentLayered(hwnd, s_app, (int)std::round(s_app.currentHeight));
    }
}

void MainWindow::OnMouseMove(HWND hwnd, int x, int y) {
    POINT pt = { x, y };

    // Handle Dragging
    if (s_app.dragging == DRAG_LEFT) {
        float val = Widgets::ValueFromSliderX((float)x, s_app.rectSliderLeft);
        float delta = val - s_app.leftVol;
        s_app.leftVol = val;
        s_audio.SetLeft(val);
        if (s_app.linked) {
            s_app.rightVol = std::clamp(s_app.rightVol + delta, 0.0f, 1.0f);
            s_audio.SetRight(s_app.rightVol);
        }
        s_app.masterVol = std::max(s_app.leftVol, s_app.rightVol);
        s_audio.SetMaster(s_app.masterVol);
        PresentLayered(hwnd, s_app, (int)std::round(s_app.currentHeight));
        return;
    }
    if (s_app.dragging == DRAG_RIGHT) {
        float val = Widgets::ValueFromSliderX((float)x, s_app.rectSliderRight);
        float delta = val - s_app.rightVol;
        s_app.rightVol = val;
        s_audio.SetRight(val);
        if (s_app.linked) {
            s_app.leftVol = std::clamp(s_app.leftVol + delta, 0.0f, 1.0f);
            s_audio.SetLeft(s_app.leftVol);
        }
        s_app.masterVol = std::max(s_app.leftVol, s_app.rightVol);
        s_audio.SetMaster(s_app.masterVol);
        PresentLayered(hwnd, s_app, (int)std::round(s_app.currentHeight));
        return;
    }
    if (s_app.dragging == DRAG_MASTER) {
        float val = Widgets::ValueFromSliderX((float)x, s_app.rectSliderMaster);
        if (s_app.masterVol > 0.001f) {
            float ratio = val / s_app.masterVol;
            s_app.leftVol = std::clamp(s_app.leftVol * ratio, 0.0f, 1.0f);
            s_app.rightVol = std::clamp(s_app.rightVol * ratio, 0.0f, 1.0f);
        } else {
            s_app.leftVol = val;
            s_app.rightVol = val;
        }
        s_app.masterVol = val;
        s_audio.SetMaster(val);
        s_audio.SetLeft(s_app.leftVol);
        s_audio.SetRight(s_app.rightVol);
        PresentLayered(hwnd, s_app, (int)std::round(s_app.currentHeight));
        return;
    }

    // Hover Detection
    HoverTarget prev = s_app.hovered;
    if (Widgets::PointInRect(pt, s_app.rectDevice)) s_app.hovered = HOVER_DEVICE;
    else if (Widgets::PointInRect(pt, s_app.rectBtnLock)) s_app.hovered = HOVER_LOCK;
    else if (Widgets::PointInRect(pt, s_app.rectBtnGear)) s_app.hovered = HOVER_GEAR;
    else if (Widgets::PointInRect(pt, s_app.rectSliderLeft)) s_app.hovered = HOVER_SLIDER_L;
    else if (Widgets::PointInRect(pt, s_app.rectSliderRight)) s_app.hovered = HOVER_SLIDER_R;
    else if (Widgets::PointInRect(pt, s_app.rectSliderMaster)) s_app.hovered = HOVER_SLIDER_M;
    else if (Widgets::PointInRect(pt, s_app.rectBtnLink)) s_app.hovered = HOVER_LINK;
    else if (Widgets::PointInRect(pt, s_app.rectBtnCentre)) s_app.hovered = HOVER_CENTRE;
    else if (s_app.drawerOpen && Widgets::PointInRect(pt, s_app.rectDrawerBoot)) s_app.hovered = HOVER_DRAWER_BOOT;
    else if (s_app.drawerOpen && Widgets::PointInRect(pt, s_app.rectDrawerFix)) s_app.hovered = HOVER_DRAWER_FIX;
    else if (s_app.drawerOpen && Widgets::PointInRect(pt, s_app.rectDrawerRevert)) s_app.hovered = HOVER_DRAWER_REVERT;
    else if (s_app.drawerOpen && Widgets::PointInRect(pt, s_app.rectDrawerQuit)) s_app.hovered = HOVER_DRAWER_QUIT;
    else s_app.hovered = HOVER_NONE;

    if (prev != s_app.hovered) {
        TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, hwnd, 0 };
        TrackMouseEvent(&tme);
    }
}

void MainWindow::OnLButtonDown(HWND hwnd, int x, int y) {
    SetCapture(hwnd);
    POINT pt = { x, y };

    // Device Dropdown Selection
    if (s_app.deviceDropdownOpen) {
        for (size_t i = 0; i < s_app.rectDropdownItems.size() && i < s_app.deviceList.size(); ++i) {
            if (Widgets::PointInRect(pt, s_app.rectDropdownItems[i])) {
                s_audio.BindDevice(s_app.deviceList[i].id);
                s_audio.GetLevels(s_app.leftVol, s_app.rightVol, s_app.masterVol);
                s_app.currentDeviceId = s_audio.GetCurrentDeviceId();
                s_app.currentDeviceName = s_audio.GetCurrentDeviceName();
                s_app.isBluetooth = s_audio.IsCurrentBluetooth();
                s_app.deviceDropdownOpen = false;
                PresentLayered(hwnd, s_app, (int)std::round(s_app.currentHeight));
                return;
            }
        }
        s_app.deviceDropdownOpen = false;
        PresentLayered(hwnd, s_app, (int)std::round(s_app.currentHeight));
        return;
    }

    // Toggle Dropdown
    if (Widgets::PointInRect(pt, s_app.rectDevice)) {
        s_audio.RefreshDeviceList(s_app.deviceList);
        s_app.deviceDropdownOpen = !s_app.deviceDropdownOpen;
        PresentLayered(hwnd, s_app, (int)std::round(s_app.currentHeight));
        return;
    }

    // Sliders
    if (Widgets::PointInRect(pt, s_app.rectSliderLeft)) {
        s_app.dragging = DRAG_LEFT;
        float val = Widgets::ValueFromSliderX((float)x, s_app.rectSliderLeft);
        float delta = val - s_app.leftVol;
        s_app.leftVol = val;
        s_audio.SetLeft(val);
        if (s_app.linked) {
            s_app.rightVol = std::clamp(s_app.rightVol + delta, 0.0f, 1.0f);
            s_audio.SetRight(s_app.rightVol);
        }
        s_app.masterVol = std::max(s_app.leftVol, s_app.rightVol);
        s_audio.SetMaster(s_app.masterVol);
        PresentLayered(hwnd, s_app, (int)std::round(s_app.currentHeight));
        return;
    }
    if (Widgets::PointInRect(pt, s_app.rectSliderRight)) {
        s_app.dragging = DRAG_RIGHT;
        float val = Widgets::ValueFromSliderX((float)x, s_app.rectSliderRight);
        float delta = val - s_app.rightVol;
        s_app.rightVol = val;
        s_audio.SetRight(val);
        if (s_app.linked) {
            s_app.leftVol = std::clamp(s_app.leftVol + delta, 0.0f, 1.0f);
            s_audio.SetLeft(s_app.leftVol);
        }
        s_app.masterVol = std::max(s_app.leftVol, s_app.rightVol);
        s_audio.SetMaster(s_app.masterVol);
        PresentLayered(hwnd, s_app, (int)std::round(s_app.currentHeight));
        return;
    }
    if (Widgets::PointInRect(pt, s_app.rectSliderMaster)) {
        s_app.dragging = DRAG_MASTER;
        float val = Widgets::ValueFromSliderX((float)x, s_app.rectSliderMaster);
        if (s_app.masterVol > 0.001f) {
            float ratio = val / s_app.masterVol;
            s_app.leftVol = std::clamp(s_app.leftVol * ratio, 0.0f, 1.0f);
            s_app.rightVol = std::clamp(s_app.rightVol * ratio, 0.0f, 1.0f);
        } else {
            s_app.leftVol = val;
            s_app.rightVol = val;
        }
        s_app.masterVol = val;
        s_audio.SetMaster(val);
        s_audio.SetLeft(s_app.leftVol);
        s_audio.SetRight(s_app.rightVol);
        PresentLayered(hwnd, s_app, (int)std::round(s_app.currentHeight));
        return;
    }

    // Lock Button
    if (Widgets::PointInRect(pt, s_app.rectBtnLock)) {
        s_app.locked = !s_app.locked;
        PresentLayered(hwnd, s_app, (int)std::round(s_app.currentHeight));
        return;
    }

    // Settings Gear
    if (Widgets::PointInRect(pt, s_app.rectBtnGear)) {
        s_app.drawerOpen = !s_app.drawerOpen;
        return;
    }

    // Link Toggle Pill
    if (Widgets::PointInRect(pt, s_app.rectBtnLink)) {
        s_app.linked = !s_app.linked;
        PresentLayered(hwnd, s_app, (int)std::round(s_app.currentHeight));
        return;
    }

    // Centre Button
    if (Widgets::PointInRect(pt, s_app.rectBtnCentre)) {
        float avg = (s_app.leftVol + s_app.rightVol) * 0.5f;
        s_app.leftVol = avg;
        s_app.rightVol = avg;
        s_audio.SetLeft(avg);
        s_audio.SetRight(avg);
        s_app.masterVol = avg;
        s_audio.SetMaster(avg);
        PresentLayered(hwnd, s_app, (int)std::round(s_app.currentHeight));
        return;
    }

    // Drawer Controls
    if (s_app.drawerOpen) {
        if (Widgets::PointInRect(pt, s_app.rectDrawerBoot)) {
            s_app.startOnBoot = !s_app.startOnBoot;
            SettingsStore::SetStartOnBoot(s_app.startOnBoot);
            PresentLayered(hwnd, s_app, (int)std::round(s_app.currentHeight));
            return;
        }
        if (Widgets::PointInRect(pt, s_app.rectDrawerFix)) {
            AvrcpHelper::Apply(!AvrcpHelper::IsApplied());
            PresentLayered(hwnd, s_app, (int)std::round(s_app.currentHeight));
            return;
        }
        if (Widgets::PointInRect(pt, s_app.rectDrawerRevert)) {
            AvrcpHelper::Apply(false);
            PresentLayered(hwnd, s_app, (int)std::round(s_app.currentHeight));
            return;
        }
        if (Widgets::PointInRect(pt, s_app.rectDrawerQuit)) {
            PostQuitMessage(0);
            return;
        }
    }

    // Card Dragging (when unlocked)
    if (!s_app.locked) {
        ReleaseCapture();
        SendMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
    }
}

void MainWindow::OnLButtonUp(HWND hwnd) {
    if (s_app.dragging != DRAG_NONE) {
        s_app.dragging = DRAG_NONE;
        ReleaseCapture();
        PresentLayered(hwnd, s_app, (int)std::round(s_app.currentHeight));
    }
}

LRESULT CALLBACK MainWindow::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        SetTimer(hwnd, 1, 16, nullptr); // 60 FPS animation timer
        break;
    }
    case WM_TIMER: {
        if (wParam == 1) {
            OnTimer(hwnd);
        }
        break;
    }
    case WM_MOUSEMOVE: {
        int x = (int)(short)LOWORD(lParam);
        int y = (int)(short)HIWORD(lParam);
        OnMouseMove(hwnd, x, y);
        break;
    }
    case WM_MOUSELEAVE: {
        s_app.hovered = HOVER_NONE;
        break;
    }
    case WM_LBUTTONDOWN: {
        int x = (int)(short)LOWORD(lParam);
        int y = (int)(short)HIWORD(lParam);
        OnLButtonDown(hwnd, x, y);
        break;
    }
    case WM_LBUTTONUP: {
        OnLButtonUp(hwnd);
        break;
    }
    case WM_RBUTTONUP: {
        // Quick quit on right click (matches original app behavior)
        PostQuitMessage(0);
        return 0;
    }
    case WM_EXITSIZEMOVE: {
        // Save window position
        RECT rc;
        if (GetWindowRect(hwnd, &rc)) {
            PersistentSettings s = SettingsStore::Load();
            s.windowLeft = rc.left;
            s.windowTop = rc.top;
            s.hasPosition = true;
            SettingsStore::Save(s);
        }
        break;
    }
    case WM_DESTROY: {
        KillTimer(hwnd, 1);

        // Save persistent settings on close
        PersistentSettings s;
        s.deviceId = s_app.currentDeviceId;
        s.locked = s_app.locked;
        s.linked = s_app.linked;
        s.startOnBoot = s_app.startOnBoot;
        RECT rc;
        if (GetWindowRect(hwnd, &rc)) {
            s.windowLeft = rc.left;
            s.windowTop = rc.top;
            s.hasPosition = true;
        }
        SettingsStore::Save(s);

        PostQuitMessage(0);
        return 0;
    }
    default:
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int MainWindow::RunMessageLoop(HWND hwnd) {
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    // Cleanup resources
    s_audio.Release();
    if (s_pDCRT) { s_pDCRT->Release(); s_pDCRT = nullptr; }
    if (s_hBmp) { DeleteObject(s_hBmp); s_hBmp = nullptr; }
    if (s_memDC) { DeleteDC(s_memDC); s_memDC = nullptr; }

    return (int)msg.wParam;
}
