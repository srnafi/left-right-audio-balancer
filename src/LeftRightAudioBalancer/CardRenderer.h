#pragma once
#include <windows.h>
#include <d2d1.h>
#include "AppState.h"

class CardRenderer {
public:
    static void Render(
        ID2D1DCRenderTarget* rt,
        HDC memDC,
        AppState& app,
        int targetHeight
    );
};
