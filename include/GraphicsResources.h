#pragma once
#include <windows.h>
#include <d2d1.h>
#include <dwrite.h>

namespace Palette {
    inline D2D1_COLOR_F CardTop()        { return D2D1::ColorF(0x1C2029, 0.98f); }
    inline D2D1_COLOR_F CardBottom()     { return D2D1::ColorF(0x12151B, 1.0f); }
    inline D2D1_COLOR_F CardBorder()     { return D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.12f); }
    inline D2D1_COLOR_F PrimaryText()    { return D2D1::ColorF(0xF4F6FA); }
    inline D2D1_COLOR_F SecondaryText()  { return D2D1::ColorF(0xAAB3C2); }
    inline D2D1_COLOR_F DimText()        { return D2D1::ColorF(0x6F7889); }
    inline D2D1_COLOR_F AccentBlue()     { return D2D1::ColorF(0x5AA2FF); }
    inline D2D1_COLOR_F AccentPurple()   { return D2D1::ColorF(0x9B7BFF); }
    inline D2D1_COLOR_F TrackBg()        { return D2D1::ColorF(0x202530); }
    inline D2D1_COLOR_F BadgeBg()        { return D2D1::ColorF(0x141720); }
    inline D2D1_COLOR_F BadgeBorder()    { return D2D1::ColorF(0x232733); }
    inline D2D1_COLOR_F Divider()        { return D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.06f); }
    inline D2D1_COLOR_F Good()           { return D2D1::ColorF(0x4ADE80); }
    inline D2D1_COLOR_F Danger()         { return D2D1::ColorF(0xF08A85); }
}

class GraphicsResources {
public:
    static bool Initialize();
    static void Release();

    static ID2D1Factory* GetD2DFactory();
    static IDWriteFactory* GetDWriteFactory();
    static ID2D1StrokeStyle* GetRoundStrokeStyle();

    // Text formats
    static IDWriteTextFormat* FontDevice();
    static IDWriteTextFormat* FontSubtitle();
    static IDWriteTextFormat* FontDropdown();
    static IDWriteTextFormat* FontHeroNum();
    static IDWriteTextFormat* FontHeroPercent();
    static IDWriteTextFormat* FontHeroTitle();
    static IDWriteTextFormat* FontHeroHint();
    static IDWriteTextFormat* FontBadge();
    static IDWriteTextFormat* FontReadout();
    static IDWriteTextFormat* FontPill();
    static IDWriteTextFormat* FontDrawerTitle();
    static IDWriteTextFormat* FontDrawerSub();

    static void ConfigureTrimmingNoWrap(IDWriteFactory* pFactory, IDWriteTextFormat* pFormat);
};
