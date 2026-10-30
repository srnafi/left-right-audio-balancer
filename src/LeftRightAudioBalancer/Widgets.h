#pragma once
#include <windows.h>
#include <d2d1.h>

class Widgets {
public:
    static void DrawSlider(
        ID2D1RenderTarget* rt,
        const D2D1_RECT_F& trackRect,
        float value,
        float meter,
        bool isMaster,
        float haloAnim,
        ID2D1Brush* pTrackBg,
        ID2D1Brush* pActiveGradient,
        ID2D1Brush* pThumbBrush
    );

    static bool PointInRect(POINT pt, const D2D1_RECT_F& rect);
    static float ValueFromSliderX(float x, const D2D1_RECT_F& rect);
};
