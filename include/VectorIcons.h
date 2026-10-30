#pragma once
#include <windows.h>
#include <d2d1.h>

class VectorIcons {
public:
    static void DrawHeadphone(ID2D1RenderTarget* rt, float cx, float cy, ID2D1Brush* brush);
    static void DrawBluetooth(ID2D1RenderTarget* rt, float cx, float cy, ID2D1Brush* brush);
    static void DrawPadlock(ID2D1RenderTarget* rt, float cx, float cy, bool locked, ID2D1Brush* brush);
    static void DrawSliders(ID2D1RenderTarget* rt, float cx, float cy, ID2D1Brush* brush, ID2D1Brush* bgBrush);
    static void DrawSpeakerVol(ID2D1RenderTarget* rt, float cx, float cy, ID2D1Brush* brush);
    static void DrawLinkChain(ID2D1RenderTarget* rt, float cx, float cy, ID2D1Brush* brush);
    static void DrawChevron(ID2D1RenderTarget* rt, float cx, float cy, ID2D1Brush* brush);
    static void DrawPower(ID2D1RenderTarget* rt, float cx, float cy, ID2D1Brush* brush);
};
