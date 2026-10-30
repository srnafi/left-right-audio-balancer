#include "GraphicsResources.h"

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

static ID2D1Factory* s_pD2DFactory = nullptr;
static IDWriteFactory* s_pDWriteFactory = nullptr;
static ID2D1StrokeStyle* s_pRoundStrokeStyle = nullptr;

static IDWriteTextFormat* s_pFontDevice = nullptr;
static IDWriteTextFormat* s_pFontSubtitle = nullptr;
static IDWriteTextFormat* s_pFontDropdown = nullptr;
static IDWriteTextFormat* s_pFontHeroNum = nullptr;
static IDWriteTextFormat* s_pFontHeroPercent = nullptr;
static IDWriteTextFormat* s_pFontHeroTitle = nullptr;
static IDWriteTextFormat* s_pFontHeroHint = nullptr;
static IDWriteTextFormat* s_pFontBadge = nullptr;
static IDWriteTextFormat* s_pFontReadout = nullptr;
static IDWriteTextFormat* s_pFontPill = nullptr;
static IDWriteTextFormat* s_pFontDrawerTitle = nullptr;
static IDWriteTextFormat* s_pFontDrawerSub = nullptr;

void GraphicsResources::ConfigureTrimmingNoWrap(IDWriteFactory* pFactory, IDWriteTextFormat* pFormat) {
    if (!pFactory || !pFormat) return;
    pFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    IDWriteInlineObject* pEllipsis = nullptr;
    if (SUCCEEDED(pFactory->CreateEllipsisTrimmingSign(pFormat, &pEllipsis))) {
        DWRITE_TRIMMING trimming = { DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0 };
        pFormat->SetTrimming(&trimming, pEllipsis);
        pEllipsis->Release();
    }
}

bool GraphicsResources::Initialize() {
    if (s_pD2DFactory) return true;

    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &s_pD2DFactory);
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&s_pDWriteFactory);
    if (!s_pD2DFactory || !s_pDWriteFactory) return false;

    // Stroke style for smooth vector icons
    D2D1_STROKE_STYLE_PROPERTIES sp = D2D1::StrokeStyleProperties(
        D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND,
        D2D1_LINE_JOIN_ROUND, 10.0f, D2D1_DASH_STYLE_SOLID, 0.0f
    );
    s_pD2DFactory->CreateStrokeStyle(&sp, nullptr, 0, &s_pRoundStrokeStyle);

    // Typography precisely matching docs/screenshot.png and docs/settings.png
    s_pDWriteFactory->CreateTextFormat(L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD,
        DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 12.0f, L"en-us", &s_pFontDevice);

    s_pDWriteFactory->CreateTextFormat(L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 10.5f, L"en-us", &s_pFontSubtitle);

    s_pDWriteFactory->CreateTextFormat(L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD,
        DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 12.0f, L"en-us", &s_pFontDropdown);

    s_pDWriteFactory->CreateTextFormat(L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_BOLD,
        DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 36.0f, L"en-us", &s_pFontHeroNum);

    s_pDWriteFactory->CreateTextFormat(L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 14.0f, L"en-us", &s_pFontHeroPercent);

    s_pDWriteFactory->CreateTextFormat(L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD,
        DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 11.5f, L"en-us", &s_pFontHeroTitle);

    s_pDWriteFactory->CreateTextFormat(L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 11.5f, L"en-us", &s_pFontHeroHint);

    s_pDWriteFactory->CreateTextFormat(L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_BOLD,
        DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 11.0f, L"en-us", &s_pFontBadge);

    s_pDWriteFactory->CreateTextFormat(L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 12.0f, L"en-us", &s_pFontReadout);

    s_pDWriteFactory->CreateTextFormat(L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_MEDIUM,
        DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 11.5f, L"en-us", &s_pFontPill);

    s_pDWriteFactory->CreateTextFormat(L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD,
        DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 12.0f, L"en-us", &s_pFontDrawerTitle);

    s_pDWriteFactory->CreateTextFormat(L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 10.0f, L"en-us", &s_pFontDrawerSub);

    // Prevent text overflow & wrapping collision: configure single-line character ellipsis
    ConfigureTrimmingNoWrap(s_pDWriteFactory, s_pFontDevice);
    ConfigureTrimmingNoWrap(s_pDWriteFactory, s_pFontSubtitle);
    ConfigureTrimmingNoWrap(s_pDWriteFactory, s_pFontDropdown);
    s_pFontHeroHint->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    s_pFontHeroTitle->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    s_pFontPill->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

    s_pFontDevice->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    s_pFontSubtitle->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    s_pFontDropdown->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    s_pFontHeroNum->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    s_pFontHeroPercent->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    s_pFontHeroTitle->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
    s_pFontHeroTitle->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    s_pFontHeroHint->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
    s_pFontHeroHint->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    s_pFontBadge->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    s_pFontBadge->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    s_pFontReadout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
    s_pFontReadout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    s_pFontPill->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    s_pFontPill->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    return true;
}

void GraphicsResources::Release() {
    if (s_pRoundStrokeStyle) { s_pRoundStrokeStyle->Release(); s_pRoundStrokeStyle = nullptr; }
    if (s_pFontDevice) { s_pFontDevice->Release(); s_pFontDevice = nullptr; }
    if (s_pFontSubtitle) { s_pFontSubtitle->Release(); s_pFontSubtitle = nullptr; }
    if (s_pFontDropdown) { s_pFontDropdown->Release(); s_pFontDropdown = nullptr; }
    if (s_pFontHeroNum) { s_pFontHeroNum->Release(); s_pFontHeroNum = nullptr; }
    if (s_pFontHeroPercent) { s_pFontHeroPercent->Release(); s_pFontHeroPercent = nullptr; }
    if (s_pFontHeroTitle) { s_pFontHeroTitle->Release(); s_pFontHeroTitle = nullptr; }
    if (s_pFontHeroHint) { s_pFontHeroHint->Release(); s_pFontHeroHint = nullptr; }
    if (s_pFontBadge) { s_pFontBadge->Release(); s_pFontBadge = nullptr; }
    if (s_pFontReadout) { s_pFontReadout->Release(); s_pFontReadout = nullptr; }
    if (s_pFontPill) { s_pFontPill->Release(); s_pFontPill = nullptr; }
    if (s_pFontDrawerTitle) { s_pFontDrawerTitle->Release(); s_pFontDrawerTitle = nullptr; }
    if (s_pFontDrawerSub) { s_pFontDrawerSub->Release(); s_pFontDrawerSub = nullptr; }

    if (s_pDWriteFactory) { s_pDWriteFactory->Release(); s_pDWriteFactory = nullptr; }
    if (s_pD2DFactory) { s_pD2DFactory->Release(); s_pD2DFactory = nullptr; }
}

ID2D1Factory* GraphicsResources::GetD2DFactory() { return s_pD2DFactory; }
IDWriteFactory* GraphicsResources::GetDWriteFactory() { return s_pDWriteFactory; }
ID2D1StrokeStyle* GraphicsResources::GetRoundStrokeStyle() { return s_pRoundStrokeStyle; }

IDWriteTextFormat* GraphicsResources::FontDevice() { return s_pFontDevice; }
IDWriteTextFormat* GraphicsResources::FontSubtitle() { return s_pFontSubtitle; }
IDWriteTextFormat* GraphicsResources::FontDropdown() { return s_pFontDropdown; }
IDWriteTextFormat* GraphicsResources::FontHeroNum() { return s_pFontHeroNum; }
IDWriteTextFormat* GraphicsResources::FontHeroPercent() { return s_pFontHeroPercent; }
IDWriteTextFormat* GraphicsResources::FontHeroTitle() { return s_pFontHeroTitle; }
IDWriteTextFormat* GraphicsResources::FontHeroHint() { return s_pFontHeroHint; }
IDWriteTextFormat* GraphicsResources::FontBadge() { return s_pFontBadge; }
IDWriteTextFormat* GraphicsResources::FontReadout() { return s_pFontReadout; }
IDWriteTextFormat* GraphicsResources::FontPill() { return s_pFontPill; }
IDWriteTextFormat* GraphicsResources::FontDrawerTitle() { return s_pFontDrawerTitle; }
IDWriteTextFormat* GraphicsResources::FontDrawerSub() { return s_pFontDrawerSub; }
