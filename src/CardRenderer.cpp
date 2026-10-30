#include "CardRenderer.h"
#include "GraphicsResources.h"
#include "VectorIcons.h"
#include "Widgets.h"
#include "AvrcpHelper.h"
#include <cmath>
#include <algorithm>

void CardRenderer::Render(ID2D1DCRenderTarget* rt, HDC memDC, AppState& app, int targetHeight) {
    if (!rt) return;

    RECT rc = { 0, 0, CARD_WIDTH, targetHeight };
    rt->BindDC(memDC, &rc);

    rt->BeginDraw();
    // Clear with 100% transparent black outside the rounded card
    rt->Clear(D2D1::ColorF(0, 0, 0, 0.0f));

    D2D1_RECT_F cardRect = D2D1::RectF(0.5f, 0.5f, (float)CARD_WIDTH - 0.5f, (float)targetHeight - 0.5f);

    // 1. Card Background Linear Gradient
    ID2D1LinearGradientBrush* pCardBgBrush = nullptr;
    ID2D1GradientStopCollection* pStopsBg = nullptr;
    D2D1_GRADIENT_STOP stopsBg[2] = {
        { 0.0f, Palette::CardTop() },
        { 1.0f, Palette::CardBottom() }
    };
    rt->CreateGradientStopCollection(stopsBg, 2, &pStopsBg);
    if (pStopsBg) {
        rt->CreateLinearGradientBrush(
            D2D1::LinearGradientBrushProperties(D2D1::Point2F(0, 0), D2D1::Point2F(0, cardRect.bottom)),
            pStopsBg, &pCardBgBrush);
        pStopsBg->Release();
    }

    // Ear Sliders Accent Gradient (Blue to Purple)
    ID2D1LinearGradientBrush* pAccentGradient = nullptr;
    ID2D1GradientStopCollection* pStopsAccent = nullptr;
    D2D1_GRADIENT_STOP stopsAccent[2] = {
        { 0.0f, Palette::AccentBlue() },
        { 1.0f, Palette::AccentPurple() }
    };
    rt->CreateGradientStopCollection(stopsAccent, 2, &pStopsAccent);
    if (pStopsAccent) {
        rt->CreateLinearGradientBrush(
            D2D1::LinearGradientBrushProperties(D2D1::Point2F(48, 0), D2D1::Point2F(284, 0)),
            pStopsAccent, &pAccentGradient);
        pStopsAccent->Release();
    }

    // Master Gradient (Silver/Grey to White)
    ID2D1LinearGradientBrush* pMasterGradient = nullptr;
    ID2D1GradientStopCollection* pStopsMaster = nullptr;
    D2D1_GRADIENT_STOP stopsMaster[2] = {
        { 0.0f, D2D1::ColorF(0x8B95A5) },
        { 1.0f, D2D1::ColorF(0xC9D2DE) }
    };
    rt->CreateGradientStopCollection(stopsMaster, 2, &pStopsMaster);
    if (pStopsMaster) {
        rt->CreateLinearGradientBrush(
            D2D1::LinearGradientBrushProperties(D2D1::Point2F(48, 0), D2D1::Point2F(284, 0)),
            pStopsMaster, &pMasterGradient);
        pStopsMaster->Release();
    }

    // Solid Brushes
    ID2D1SolidColorBrush* pCardBorder = nullptr;
    ID2D1SolidColorBrush* pTextPrimary = nullptr;
    ID2D1SolidColorBrush* pTextSecondary = nullptr;
    ID2D1SolidColorBrush* pTextDim = nullptr;
    ID2D1SolidColorBrush* pAccentBlue = nullptr;
    ID2D1SolidColorBrush* pTrack = nullptr;
    ID2D1SolidColorBrush* pWhite = nullptr;
    ID2D1SolidColorBrush* pBadgeBg = nullptr;
    ID2D1SolidColorBrush* pBadgeBorder = nullptr;
    ID2D1SolidColorBrush* pDivider = nullptr;

    rt->CreateSolidColorBrush(Palette::CardBorder(), &pCardBorder);
    rt->CreateSolidColorBrush(Palette::PrimaryText(), &pTextPrimary);
    rt->CreateSolidColorBrush(Palette::SecondaryText(), &pTextSecondary);
    rt->CreateSolidColorBrush(Palette::DimText(), &pTextDim);
    rt->CreateSolidColorBrush(Palette::AccentBlue(), &pAccentBlue);
    rt->CreateSolidColorBrush(Palette::TrackBg(), &pTrack);
    rt->CreateSolidColorBrush(D2D1::ColorF(0xFFFFFF), &pWhite);
    rt->CreateSolidColorBrush(Palette::BadgeBg(), &pBadgeBg);
    rt->CreateSolidColorBrush(Palette::BadgeBorder(), &pBadgeBorder);
    rt->CreateSolidColorBrush(Palette::Divider(), &pDivider);

    // Draw Anti-Aliased Rounded Card Background
    D2D1_ROUNDED_RECT cardRounded = D2D1::RoundedRect(cardRect, CARD_RADIUS, CARD_RADIUS);
    if (pCardBgBrush) rt->FillRoundedRectangle(cardRounded, pCardBgBrush);
    if (pCardBorder) rt->DrawRoundedRectangle(cardRounded, pCardBorder, 1.0f);

    float padX = 14.0f;
    float curY = 14.0f;

    // ==========================================
    // 1. Header: Device Tile + Lock & Gear Buttons
    // ==========================================
    app.rectDevice = D2D1::RectF(padX, curY, cardRect.right - padX - 56.0f, curY + 32.0f);

    if (app.hoverDevice > 0.01f) {
        ID2D1SolidColorBrush* pHover = nullptr;
        rt->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.06f * app.hoverDevice), &pHover);
        if (pHover) {
            rt->FillRoundedRectangle(D2D1::RoundedRect(app.rectDevice, 8.0f, 8.0f), pHover);
            pHover->Release();
        }
    }

    // Device Badge Tile (30x30 rounded squircle)
    D2D1_RECT_F tileRect = D2D1::RectF(app.rectDevice.left, curY, app.rectDevice.left + 30.0f, curY + 30.0f);
    ID2D1SolidColorBrush* pTileFill = nullptr;
    ID2D1SolidColorBrush* pTileBorder = nullptr;
    rt->CreateSolidColorBrush(D2D1::ColorF(0x232B3C), &pTileFill);
    rt->CreateSolidColorBrush(D2D1::ColorF(0x385A8A), &pTileBorder);
    if (pTileFill) {
        rt->FillRoundedRectangle(D2D1::RoundedRect(tileRect, 10.0f, 10.0f), pTileFill);
        pTileFill->Release();
    }
    if (pTileBorder) {
        rt->DrawRoundedRectangle(D2D1::RoundedRect(tileRect, 10.0f, 10.0f), pTileBorder, 1.0f);
        pTileBorder->Release();
    }

    float tileCx = (tileRect.left + tileRect.right) * 0.5f;
    float tileCy = (tileRect.top + tileRect.bottom) * 0.5f;
    if (app.isBluetooth) {
        VectorIcons::DrawBluetooth(rt, tileCx, tileCy, pAccentBlue);
    } else {
        VectorIcons::DrawHeadphone(rt, tileCx, tileCy, pAccentBlue);
    }

    // Device Name & Subtitle
    D2D1_RECT_F devNameRect = D2D1::RectF(tileRect.right + 9.0f, curY - 2.0f, app.rectDevice.right - 16.0f, curY + 16.0f);
    rt->DrawText(app.currentDeviceName.c_str(), (UINT32)app.currentDeviceName.length(), GraphicsResources::FontDevice(), devNameRect, pTextPrimary, D2D1_DRAW_TEXT_OPTIONS_CLIP);

    std::wstring subTitle = app.isBluetooth ? L"Bluetooth \x00B7 Stereo" : L"Output \x00B7 Stereo";
    D2D1_RECT_F subTitleRect = D2D1::RectF(tileRect.right + 9.0f, curY + 16.0f, app.rectDevice.right - 16.0f, curY + 30.0f);
    rt->DrawText(subTitle.c_str(), (UINT32)subTitle.length(), GraphicsResources::FontSubtitle(), subTitleRect, pTextDim, D2D1_DRAW_TEXT_OPTIONS_CLIP);

    VectorIcons::DrawChevron(rt, app.rectDevice.right - 8.0f, curY + 15.0f, pTextSecondary);

    // Lock Button (24x24)
    app.rectBtnLock = D2D1::RectF(cardRect.right - padX - 52.0f, curY + 3.0f, cardRect.right - padX - 28.0f, curY + 27.0f);
    if (app.locked || app.hoverLock > 0.01f) {
        ID2D1SolidColorBrush* pBtnBg = nullptr;
        rt->CreateSolidColorBrush(app.locked ? D2D1::ColorF(0x5AA2FF, 0.20f) : D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.08f * app.hoverLock), &pBtnBg);
        if (pBtnBg) {
            rt->FillRoundedRectangle(D2D1::RoundedRect(app.rectBtnLock, 7.0f, 7.0f), pBtnBg);
            pBtnBg->Release();
        }
    }
    VectorIcons::DrawPadlock(rt, (app.rectBtnLock.left + app.rectBtnLock.right) * 0.5f,
                                 (app.rectBtnLock.top + app.rectBtnLock.bottom) * 0.5f,
                                 app.locked, app.locked ? pAccentBlue : pTextDim);

    // Gear Button (24x24)
    app.rectBtnGear = D2D1::RectF(cardRect.right - padX - 24.0f, curY + 3.0f, cardRect.right - padX, curY + 27.0f);
    if (app.drawerOpen || app.hoverGear > 0.01f) {
        ID2D1SolidColorBrush* pBtnBg = nullptr;
        rt->CreateSolidColorBrush(app.drawerOpen ? D2D1::ColorF(0x5AA2FF, 0.20f) : D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.08f * app.hoverGear), &pBtnBg);
        if (pBtnBg) {
            rt->FillRoundedRectangle(D2D1::RoundedRect(app.rectBtnGear, 7.0f, 7.0f), pBtnBg);
            pBtnBg->Release();
        }
    }
    VectorIcons::DrawSliders(rt, (app.rectBtnGear.left + app.rectBtnGear.right) * 0.5f,
                                 (app.rectBtnGear.top + app.rectBtnGear.bottom) * 0.5f,
                                 app.drawerOpen ? pAccentBlue : pTextDim, pCardBgBrush);

    curY += 46.0f;

    // ==========================================
    // 2. Hero Master Number & Offset Readout
    // ==========================================
    wchar_t masterStr[16];
    swprintf_s(masterStr, L"%d", (int)std::round(app.masterVol * 100.0f));
    D2D1_RECT_F heroNumRect = D2D1::RectF(padX, curY - 6.0f, padX + 55.0f, curY + 38.0f);
    rt->DrawText(masterStr, (UINT32)wcslen(masterStr), GraphicsResources::FontHeroNum(), heroNumRect, pTextPrimary);

    D2D1_RECT_F heroPctRect = D2D1::RectF(padX + 56.0f, curY + 12.0f, padX + 75.0f, curY + 34.0f);
    rt->DrawText(L"%", 1, GraphicsResources::FontHeroPercent(), heroPctRect, pTextSecondary);

    // Balance Right Label & Description
    float diff = (app.rightVol - app.leftVol) * 100.0f;
    std::wstring balanceDesc;
    if (std::abs(diff) < 0.5f) {
        balanceDesc = L"Centred";
    } else if (diff < 0) {
        balanceDesc = L"R is " + std::to_wstring((int)std::round(std::abs(diff))) + L" quieter";
    } else {
        balanceDesc = L"L is " + std::to_wstring((int)std::round(std::abs(diff))) + L" quieter";
    }

    D2D1_RECT_F balTitleRect = D2D1::RectF(cardRect.right - padX - 110.0f, curY + 2.0f, cardRect.right - padX, curY + 18.0f);
    rt->DrawText(L"Balance", 7, GraphicsResources::FontHeroTitle(), balTitleRect, pTextDim);

    D2D1_RECT_F balDescRect = D2D1::RectF(cardRect.right - padX - 110.0f, curY + 18.0f, cardRect.right - padX, curY + 34.0f);
    rt->DrawText(balanceDesc.c_str(), (UINT32)balanceDesc.length(), GraphicsResources::FontHeroHint(), balDescRect, pTextDim);

    curY += 46.0f;

    // ==========================================
    // 3. Ear Sliders: Left and Right (36px high rows)
    // ==========================================
    // LEFT EAR ROW
    D2D1_RECT_F badgeL = D2D1::RectF(padX, curY + 5.0f, padX + 26.0f, curY + 31.0f);
    rt->FillRoundedRectangle(D2D1::RoundedRect(badgeL, 8.0f, 8.0f), pBadgeBg);
    rt->DrawRoundedRectangle(D2D1::RoundedRect(badgeL, 8.0f, 8.0f), pBadgeBorder, 1.0f);
    rt->DrawText(L"L", 1, GraphicsResources::FontBadge(), badgeL, pTextSecondary);

    app.rectSliderLeft = D2D1::RectF(padX + 34.0f, curY, cardRect.right - padX - 44.0f, curY + 36.0f);
    Widgets::DrawSlider(rt, app.rectSliderLeft, app.leftVol, app.leftMeter, false,
                        app.haloL, pTrack, pAccentGradient, pWhite);

    wchar_t lValStr[16];
    swprintf_s(lValStr, L"%d%%", (int)std::round(app.leftVol * 100.0f));
    D2D1_RECT_F valLRect = D2D1::RectF(cardRect.right - padX - 40.0f, curY + 7.0f, cardRect.right - padX, curY + 29.0f);
    rt->DrawText(lValStr, (UINT32)wcslen(lValStr), GraphicsResources::FontReadout(), valLRect, pTextSecondary);

    curY += 38.0f;

    // RIGHT EAR ROW
    D2D1_RECT_F badgeR = D2D1::RectF(padX, curY + 5.0f, padX + 26.0f, curY + 31.0f);
    rt->FillRoundedRectangle(D2D1::RoundedRect(badgeR, 8.0f, 8.0f), pBadgeBg);
    rt->DrawRoundedRectangle(D2D1::RoundedRect(badgeR, 8.0f, 8.0f), pBadgeBorder, 1.0f);
    rt->DrawText(L"R", 1, GraphicsResources::FontBadge(), badgeR, pTextSecondary);

    app.rectSliderRight = D2D1::RectF(padX + 34.0f, curY, cardRect.right - padX - 44.0f, curY + 36.0f);
    Widgets::DrawSlider(rt, app.rectSliderRight, app.rightVol, app.rightMeter, false,
                        app.haloR, pTrack, pAccentGradient, pWhite);

    wchar_t rValStr[16];
    swprintf_s(rValStr, L"%d%%", (int)std::round(app.rightVol * 100.0f));
    D2D1_RECT_F valRRect = D2D1::RectF(cardRect.right - padX - 40.0f, curY + 7.0f, cardRect.right - padX, curY + 29.0f);
    rt->DrawText(rValStr, (UINT32)wcslen(rValStr), GraphicsResources::FontReadout(), valRRect, pTextSecondary);

    curY += 40.0f;

    // Divider Line
    rt->DrawLine(D2D1::Point2F(padX, curY), D2D1::Point2F(cardRect.right - padX, curY), pDivider, 1.0f);
    curY += 10.0f;

    // ==========================================
    // 4. Master Volume Row (Speaker Icon + MasterGradient Slider)
    // ==========================================
    VectorIcons::DrawSpeakerVol(rt, padX + 11.0f, curY + 13.0f, pTextDim);

    app.rectSliderMaster = D2D1::RectF(padX + 34.0f, curY, cardRect.right - padX - 44.0f, curY + 26.0f);
    Widgets::DrawSlider(rt, app.rectSliderMaster, app.masterVol, 0.0f, true,
                        app.haloM, pTrack, pMasterGradient, pWhite);

    D2D1_RECT_F valMRect = D2D1::RectF(cardRect.right - padX - 40.0f, curY + 3.0f, cardRect.right - padX, curY + 23.0f);
    rt->DrawText(masterStr, (UINT32)wcslen(masterStr), GraphicsResources::FontReadout(), valMRect, pTextDim);

    curY += 34.0f;

    // ==========================================
    // 5. Action Footer: Linked Pill + Centre Button
    // ==========================================
    app.rectBtnLink = D2D1::RectF(padX, curY, padX + 106.0f, curY + 28.0f);
    D2D1_ROUNDED_RECT pillLink = D2D1::RoundedRect(app.rectBtnLink, 14.0f, 14.0f);

    ID2D1SolidColorBrush* pLinkBg = nullptr;
    ID2D1SolidColorBrush* pLinkBorder = nullptr;
    rt->CreateSolidColorBrush(app.linked ? D2D1::ColorF(0x162238) : D2D1::ColorF(0x181E29), &pLinkBg);
    rt->CreateSolidColorBrush(app.linked ? D2D1::ColorF(0x253D66) : D2D1::ColorF(0x222938), &pLinkBorder);
    if (pLinkBg) { rt->FillRoundedRectangle(pillLink, pLinkBg); pLinkBg->Release(); }
    if (pLinkBorder) { rt->DrawRoundedRectangle(pillLink, pLinkBorder, 1.0f); pLinkBorder->Release(); }

    VectorIcons::DrawLinkChain(rt, app.rectBtnLink.left + 18.0f, (app.rectBtnLink.top + app.rectBtnLink.bottom) * 0.5f,
                               app.linked ? pAccentBlue : pTextSecondary);

    std::wstring linkStr = app.linked ? (std::abs(diff) < 0.5f ? L"Linked  sync" : (diff < 0 ? L"Linked  R \x2212" + std::to_wstring((int)std::round(std::abs(diff))) : L"Linked  R +" + std::to_wstring((int)std::round(std::abs(diff))))) : L"Unlinked";
    D2D1_RECT_F textLinkRect = D2D1::RectF(app.rectBtnLink.left + 28.0f, curY + 4.0f, app.rectBtnLink.right - 6.0f, curY + 24.0f);
    rt->DrawText(linkStr.c_str(), (UINT32)linkStr.length(), GraphicsResources::FontPill(), textLinkRect, app.linked ? pAccentBlue : pTextSecondary);

    // Centre Button
    app.rectBtnCentre = D2D1::RectF(cardRect.right - padX - 58.0f, curY, cardRect.right - padX, curY + 28.0f);
    D2D1_ROUNDED_RECT pillCentre = D2D1::RoundedRect(app.rectBtnCentre, 14.0f, 14.0f);
    ID2D1SolidColorBrush* pCentreBg = nullptr;
    ID2D1SolidColorBrush* pCentreBorder = nullptr;
    rt->CreateSolidColorBrush(D2D1::ColorF(0x181E29), &pCentreBg);
    rt->CreateSolidColorBrush(D2D1::ColorF(0x222938), &pCentreBorder);
    if (pCentreBg) { rt->FillRoundedRectangle(pillCentre, pCentreBg); pCentreBg->Release(); }
    if (pCentreBorder) { rt->DrawRoundedRectangle(pillCentre, pCentreBorder, 1.0f); pCentreBorder->Release(); }

    D2D1_RECT_F textCentreRect = D2D1::RectF(app.rectBtnCentre.left, curY + 4.0f, app.rectBtnCentre.right, curY + 24.0f);
    rt->DrawText(L"Centre", 6, GraphicsResources::FontPill(), textCentreRect, pTextPrimary);

    curY += 38.0f;

    // ==========================================
    // 6. Settings Drawer (Collapsible)
    // ==========================================
    if (targetHeight > CARD_BASE_HEIGHT + 10) {
        rt->DrawLine(D2D1::Point2F(padX, curY), D2D1::Point2F(cardRect.right - padX, curY), pDivider, 1.0f);
        curY += 12.0f;

        // Row 1: Start with Windows
        D2D1_RECT_F titleBoot = D2D1::RectF(padX, curY, cardRect.right - padX - 60.0f, curY + 16.0f);
        rt->DrawText(L"Start with Windows", 18, GraphicsResources::FontDrawerTitle(), titleBoot, pTextPrimary);

        const wchar_t* subBootStr = L"Adds LeftRightAudioBalancer to the Startup folder.";
        D2D1_RECT_F subBoot = D2D1::RectF(padX, curY + 16.0f, cardRect.right - padX - 60.0f, curY + 32.0f);
        rt->DrawText(subBootStr, (UINT32)wcslen(subBootStr), GraphicsResources::FontDrawerSub(), subBoot, pTextDim);

        // Toggle Switch (36x20)
        app.rectDrawerBoot = D2D1::RectF(cardRect.right - padX - 36.0f, curY + 3.0f, cardRect.right - padX, curY + 23.0f);
        D2D1_ROUNDED_RECT swTrack = D2D1::RoundedRect(app.rectDrawerBoot, 10.0f, 10.0f);
        ID2D1SolidColorBrush* pSwTrack = nullptr;
        rt->CreateSolidColorBrush(app.startOnBoot ? D2D1::ColorF(0x5AA2FF) : D2D1::ColorF(0x262E3D), &pSwTrack);
        if (pSwTrack) { rt->FillRoundedRectangle(swTrack, pSwTrack); pSwTrack->Release(); }

        float swKnobX = app.startOnBoot ? (app.rectDrawerBoot.right - 10.0f) : (app.rectDrawerBoot.left + 10.0f);
        rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(swKnobX, curY + 13.0f), 7.0f, 7.0f), pWhite);

        curY += 46.0f;

        // Row 2: Bluetooth per-ear fix
        D2D1_RECT_F titleFix = D2D1::RectF(padX, curY, cardRect.right - padX - 74.0f, curY + 16.0f);
        rt->DrawText(L"Bluetooth per-ear fix", 21, GraphicsResources::FontDrawerTitle(), titleFix, pTextPrimary);

        const wchar_t* subFixStr = L"Windows hands volume to the headset, so L/R never splits. This turns that off and does volume locally. Needs admin, then a reconnect.";
        D2D1_RECT_F subFix = D2D1::RectF(padX, curY + 16.0f, cardRect.right - padX - 66.0f, curY + 76.0f);
        rt->DrawText(subFixStr, (UINT32)wcslen(subFixStr), GraphicsResources::FontDrawerSub(), subFix, pTextDim);

        bool fixApplied = AvrcpHelper::IsApplied();
        app.rectDrawerFix = D2D1::RectF(cardRect.right - padX - 58.0f, curY + 2.0f, cardRect.right - padX, curY + 26.0f);
        D2D1_ROUNDED_RECT pillFix = D2D1::RoundedRect(app.rectDrawerFix, 12.0f, 12.0f);
        ID2D1SolidColorBrush* pFixBg = nullptr;
        ID2D1SolidColorBrush* pFixBorder = nullptr;
        ID2D1SolidColorBrush* pFixText = nullptr;

        if (fixApplied) {
            rt->CreateSolidColorBrush(D2D1::ColorF(0x133824), &pFixBg);
            rt->CreateSolidColorBrush(D2D1::ColorF(0x245C3C), &pFixBorder);
            rt->CreateSolidColorBrush(Palette::Good(), &pFixText);
        } else {
            rt->CreateSolidColorBrush(D2D1::ColorF(0x181E29), &pFixBg);
            rt->CreateSolidColorBrush(D2D1::ColorF(0x222938), &pFixBorder);
            rt->CreateSolidColorBrush(Palette::PrimaryText(), &pFixText);
        }

        if (pFixBg) { rt->FillRoundedRectangle(pillFix, pFixBg); pFixBg->Release(); }
        if (pFixBorder) { rt->DrawRoundedRectangle(pillFix, pFixBorder, 1.0f); pFixBorder->Release(); }
        D2D1_RECT_F textFixRect = D2D1::RectF(app.rectDrawerFix.left, curY + 5.0f, app.rectDrawerFix.right, curY + 23.0f);
        rt->DrawText(fixApplied ? L"\x2713 On" : L"Apply", fixApplied ? 4 : 5, GraphicsResources::FontPill(), textFixRect, pFixText);
        if (pFixText) pFixText->Release();

        // If fix applied, show "Revert" button right below it
        if (fixApplied) {
            app.rectDrawerRevert = D2D1::RectF(cardRect.right - padX - 58.0f, curY + 32.0f, cardRect.right - padX, curY + 56.0f);
            D2D1_ROUNDED_RECT pillRev = D2D1::RoundedRect(app.rectDrawerRevert, 12.0f, 12.0f);
            ID2D1SolidColorBrush* pRevBg = nullptr;
            ID2D1SolidColorBrush* pRevBorder = nullptr;
            ID2D1SolidColorBrush* pRevText = nullptr;
            rt->CreateSolidColorBrush(D2D1::ColorF(0x181E29), &pRevBg);
            rt->CreateSolidColorBrush(D2D1::ColorF(0x222938), &pRevBorder);
            rt->CreateSolidColorBrush(Palette::PrimaryText(), &pRevText);

            if (pRevBg) { rt->FillRoundedRectangle(pillRev, pRevBg); pRevBg->Release(); }
            if (pRevBorder) { rt->DrawRoundedRectangle(pillRev, pRevBorder, 1.0f); pRevBorder->Release(); }
            D2D1_RECT_F textRevRect = D2D1::RectF(app.rectDrawerRevert.left, curY + 35.0f, app.rectDrawerRevert.right, curY + 53.0f);
            rt->DrawText(L"Revert", 6, GraphicsResources::FontPill(), textRevRect, pRevText);
            if (pRevText) pRevText->Release();
        } else {
            app.rectDrawerRevert = {};
        }

        curY += 82.0f;

        // Row 3: Quit Button (Full Width with Red Power Icon)
        app.rectDrawerQuit = D2D1::RectF(padX, curY, cardRect.right - padX, curY + 36.0f);
        D2D1_ROUNDED_RECT quitRect = D2D1::RoundedRect(app.rectDrawerQuit, 10.0f, 10.0f);

        ID2D1SolidColorBrush* pQuitBg = nullptr;
        ID2D1SolidColorBrush* pQuitBorder = nullptr;
        ID2D1SolidColorBrush* pQuitText = nullptr;
        rt->CreateSolidColorBrush(D2D1::ColorF(0x1A151C), &pQuitBg);
        rt->CreateSolidColorBrush(D2D1::ColorF(0x2E1E24), &pQuitBorder);
        rt->CreateSolidColorBrush(Palette::Danger(), &pQuitText);

        if (pQuitBg) { rt->FillRoundedRectangle(quitRect, pQuitBg); pQuitBg->Release(); }
        if (pQuitBorder) { rt->DrawRoundedRectangle(quitRect, pQuitBorder, 1.0f); pQuitBorder->Release(); }

        float qCx = (app.rectDrawerQuit.left + app.rectDrawerQuit.right) * 0.5f;
        float qCy = (app.rectDrawerQuit.top + app.rectDrawerQuit.bottom) * 0.5f;

        // Vector Power icon + Quit text precisely centered
        VectorIcons::DrawPower(rt, qCx - 18.0f, qCy, pQuitText);
        D2D1_RECT_F textQuitRect = D2D1::RectF(qCx - 8.0f, qCy - 9.0f, qCx + 26.0f, qCy + 10.0f);
        rt->DrawText(L"Quit", 4, GraphicsResources::FontPill(), textQuitRect, pQuitText);

        if (pQuitText) pQuitText->Release();
    }

    // ==========================================
    // 7. Device Dropdown Overlay
    // ==========================================
    app.rectDropdownItems.clear();
    if (app.deviceDropdownOpen && !app.deviceList.empty()) {
        float popY = app.rectDevice.bottom + 4.0f;
        float popH = (float)app.deviceList.size() * 30.0f + 8.0f;
        D2D1_RECT_F popRect = D2D1::RectF(padX, popY, cardRect.right - padX, popY + popH);

        ID2D1SolidColorBrush* pPopBg = nullptr;
        rt->CreateSolidColorBrush(D2D1::ColorF(0x181C26), &pPopBg);
        if (pPopBg) {
            rt->FillRoundedRectangle(D2D1::RoundedRect(popRect, 10.0f, 10.0f), pPopBg);
            pPopBg->Release();
        }
        rt->DrawRoundedRectangle(D2D1::RoundedRect(popRect, 10.0f, 10.0f), pCardBorder, 1.0f);

        float itemY = popY + 4.0f;
        for (const auto& dev : app.deviceList) {
            D2D1_RECT_F itemRect = D2D1::RectF(popRect.left + 4.0f, itemY, popRect.right - 4.0f, itemY + 26.0f);
            app.rectDropdownItems.push_back(itemRect);

            bool isCurrent = (dev.id == app.currentDeviceId);
            if (isCurrent) {
                ID2D1SolidColorBrush* pItemSel = nullptr;
                rt->CreateSolidColorBrush(D2D1::ColorF(0x5AA2FF, 0.15f), &pItemSel);
                if (pItemSel) {
                    rt->FillRoundedRectangle(D2D1::RoundedRect(itemRect, 6.0f, 6.0f), pItemSel);
                    pItemSel->Release();
                }
            }
            D2D1_RECT_F textRect = D2D1::RectF(itemRect.left + 8.0f, itemRect.top + 3.0f, itemRect.right - 8.0f, itemRect.bottom);
            rt->DrawText(dev.name.c_str(), (UINT32)dev.name.length(), GraphicsResources::FontDropdown(), textRect, isCurrent ? pAccentBlue : pTextPrimary, D2D1_DRAW_TEXT_OPTIONS_CLIP);
            itemY += 30.0f;
        }
    }

    // Cleanup resources
    if (pCardBgBrush) pCardBgBrush->Release();
    if (pAccentGradient) pAccentGradient->Release();
    if (pMasterGradient) pMasterGradient->Release();
    if (pCardBorder) pCardBorder->Release();
    if (pTextPrimary) pTextPrimary->Release();
    if (pTextSecondary) pTextSecondary->Release();
    if (pTextDim) pTextDim->Release();
    if (pAccentBlue) pAccentBlue->Release();
    if (pTrack) pTrack->Release();
    if (pWhite) pWhite->Release();
    if (pBadgeBg) pBadgeBg->Release();
    if (pBadgeBorder) pBadgeBorder->Release();
    if (pDivider) pDivider->Release();

    rt->EndDraw();
}
