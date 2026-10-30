#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <d2d1.h>
#include <string>
#include <vector>

const int CARD_WIDTH = 340;
const int CARD_BASE_HEIGHT = 286;
const int CARD_DRAWER_HEIGHT = 490;
const float CARD_RADIUS = 20.0f;

enum HoverTarget {
    HOVER_NONE,
    HOVER_DEVICE,
    HOVER_LOCK,
    HOVER_GEAR,
    HOVER_SLIDER_L,
    HOVER_SLIDER_R,
    HOVER_SLIDER_M,
    HOVER_LINK,
    HOVER_CENTRE,
    HOVER_DRAWER_BOOT,
    HOVER_DRAWER_FIX,
    HOVER_DRAWER_REVERT,
    HOVER_DRAWER_QUIT
};

enum DragTarget {
    DRAG_NONE,
    DRAG_LEFT,
    DRAG_RIGHT,
    DRAG_MASTER
};

struct DeviceItem {
    std::wstring id;
    std::wstring name;
    bool isBluetooth = false;
};

struct AppState {
    // Current levels [0.0f .. 1.0f]
    float leftVol = 0.68f;
    float rightVol = 0.61f;
    float masterVol = 0.68f;

    // Peak levels for live meters
    float leftMeter = 0.0f;
    float rightMeter = 0.0f;

    // Control flags
    bool linked = true;
    bool locked = false;
    bool drawerOpen = false;
    bool deviceDropdownOpen = false;
    bool startOnBoot = false;

    // Device information
    std::wstring currentDeviceId;
    std::wstring currentDeviceName = L"Speakers";
    bool isBluetooth = false;
    std::vector<DeviceItem> deviceList;

    // Smooth animation interpolators
    float currentHeight = (float)CARD_BASE_HEIGHT;
    float targetHeight = (float)CARD_BASE_HEIGHT;

    float haloL = 0.0f;
    float haloR = 0.0f;
    float haloM = 0.0f;

    float hoverDevice = 0.0f;
    float hoverLock = 0.0f;
    float hoverGear = 0.0f;
    float hoverCentre = 0.0f;

    HoverTarget hovered = HOVER_NONE;
    DragTarget dragging = DRAG_NONE;

    // Exact layout hit rectangles
    D2D1_RECT_F rectDevice = {};
    D2D1_RECT_F rectBtnLock = {};
    D2D1_RECT_F rectBtnGear = {};

    D2D1_RECT_F rectSliderLeft = {};
    D2D1_RECT_F rectSliderRight = {};
    D2D1_RECT_F rectSliderMaster = {};

    D2D1_RECT_F rectBtnLink = {};
    D2D1_RECT_F rectBtnCentre = {};

    D2D1_RECT_F rectDrawerBoot = {};
    D2D1_RECT_F rectDrawerFix = {};
    D2D1_RECT_F rectDrawerRevert = {};
    D2D1_RECT_F rectDrawerQuit = {};

    std::vector<D2D1_RECT_F> rectDropdownItems;
};
