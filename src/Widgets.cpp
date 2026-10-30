#include "Widgets.h"
#include <algorithm>

bool Widgets::PointInRect(POINT pt, const D2D1_RECT_F& rect) {
    return (pt.x >= rect.left && pt.x <= rect.right && pt.y >= rect.top && pt.y <= rect.bottom);
}

float Widgets::ValueFromSliderX(float x, const D2D1_RECT_F& rect) {
    return std::clamp((x - rect.left) / (rect.right - rect.left), 0.0f, 1.0f);
}

void Widgets::DrawSlider(
    ID2D1RenderTarget* rt,
    const D2D1_RECT_F& trackRect,
    float value,
    float meter,
    bool isMaster,
    float haloAnim,
    ID2D1Brush* pTrackBg,
    ID2D1Brush* pActiveGradient,
    ID2D1Brush* pThumbBrush
) {
    float cy = (trackRect.top + trackRect.bottom) * 0.5f;
    float trackHeight = isMaster ? 5.5f : 6.5f;
    float trackRadius = trackHeight * 0.5f;
    float trackWidth = trackRect.right - trackRect.left;

    // Background track
    D2D1_RECT_F bgRect = D2D1::RectF(trackRect.left, cy - trackRadius, trackRect.right, cy + trackRadius);
    rt->FillRoundedRectangle(D2D1::RoundedRect(bgRect, trackRadius, trackRadius), pTrackBg);

    // Active gradient track fill
    float fillRight = trackRect.left + (trackWidth * std::clamp(value, 0.0f, 1.0f));
    if (fillRight > trackRect.left + 1.0f) {
        D2D1_RECT_F activeRect = D2D1::RectF(trackRect.left, cy - trackRadius, fillRight, cy + trackRadius);
        rt->FillRoundedRectangle(D2D1::RoundedRect(activeRect, trackRadius, trackRadius), pActiveGradient);
    }

    // Live Peak Meter Bar inside the track
    if (!isMaster && meter > 0.01f) {
        float meterRight = trackRect.left + (trackWidth * std::clamp(meter, 0.0f, 1.0f));
        if (meterRight > trackRect.left + 2.0f) {
            D2D1_RECT_F meterRect = D2D1::RectF(trackRect.left, cy - trackRadius, meterRight, cy + trackRadius);
            ID2D1SolidColorBrush* pMeterGlow = nullptr;
            rt->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.38f), &pMeterGlow);
            if (pMeterGlow) {
                rt->FillRoundedRectangle(D2D1::RoundedRect(meterRect, trackRadius, trackRadius), pMeterGlow);
                pMeterGlow->Release();
            }
        }
    }

    // Smooth Glowing Outer Halo
    float thumbRadius = isMaster ? 7.0f : 8.5f;
    float haloExpansion = 3.0f + (haloAnim * 5.0f);
    float haloRadius = thumbRadius + haloExpansion;
    ID2D1SolidColorBrush* pHaloBrush = nullptr;
    rt->CreateSolidColorBrush(D2D1::ColorF(0x253B5E, 0.85f), &pHaloBrush);
    if (pHaloBrush) {
        rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(fillRight, cy), haloRadius, haloRadius), pHaloBrush);
        pHaloBrush->Release();
    }

    // Solid Crisp White Knob
    rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(fillRight, cy), thumbRadius, thumbRadius), pThumbBrush);
}
