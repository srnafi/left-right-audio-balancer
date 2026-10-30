#include "VectorIcons.h"
#include "GraphicsResources.h"

void VectorIcons::DrawHeadphone(ID2D1RenderTarget* rt, float cx, float cy, ID2D1Brush* brush) {
    ID2D1Factory* factory = GraphicsResources::GetD2DFactory();
    ID2D1StrokeStyle* stroke = GraphicsResources::GetRoundStrokeStyle();
    if (!factory || !stroke) return;

    // Headband arc
    ID2D1PathGeometry* pBand = nullptr;
    factory->CreatePathGeometry(&pBand);
    if (pBand) {
        ID2D1GeometrySink* pSink = nullptr;
        pBand->Open(&pSink);
        pSink->BeginFigure(D2D1::Point2F(cx - 7.5f, cy + 1.0f), D2D1_FIGURE_BEGIN_HOLLOW);
        pSink->AddArc(D2D1::ArcSegment(
            D2D1::Point2F(cx + 7.5f, cy + 1.0f),
            D2D1::SizeF(7.5f, 7.5f),
            0.0f,
            D2D1_SWEEP_DIRECTION_CLOCKWISE,
            D2D1_ARC_SIZE_SMALL
        ));
        pSink->EndFigure(D2D1_FIGURE_END_OPEN);
        pSink->Close();
        pSink->Release();

        rt->DrawGeometry(pBand, brush, 2.0f, stroke);
        pBand->Release();
    }

    // Ear cups
    D2D1_ROUNDED_RECT earL = D2D1::RoundedRect(D2D1::RectF(cx - 9.0f, cy, cx - 6.0f, cy + 8.0f), 2.0f, 2.0f);
    D2D1_ROUNDED_RECT earR = D2D1::RoundedRect(D2D1::RectF(cx + 6.0f, cy, cx + 9.0f, cy + 8.0f), 2.0f, 2.0f);
    rt->FillRoundedRectangle(earL, brush);
    rt->FillRoundedRectangle(earR, brush);
}

void VectorIcons::DrawBluetooth(ID2D1RenderTarget* rt, float cx, float cy, ID2D1Brush* brush) {
    ID2D1Factory* factory = GraphicsResources::GetD2DFactory();
    ID2D1StrokeStyle* stroke = GraphicsResources::GetRoundStrokeStyle();
    if (!factory || !stroke) return;

    ID2D1PathGeometry* pPath = nullptr;
    factory->CreatePathGeometry(&pPath);
    if (pPath) {
        ID2D1GeometrySink* pSink = nullptr;
        pPath->Open(&pSink);
        pSink->BeginFigure(D2D1::Point2F(cx - 4.5f, cy - 4.5f), D2D1_FIGURE_BEGIN_HOLLOW);
        pSink->AddLine(D2D1::Point2F(cx + 4.5f, cy + 4.5f));
        pSink->AddLine(D2D1::Point2F(cx, cy + 8.0f));
        pSink->AddLine(D2D1::Point2F(cx, cy - 8.0f));
        pSink->AddLine(D2D1::Point2F(cx + 4.5f, cy - 4.5f));
        pSink->AddLine(D2D1::Point2F(cx - 4.5f, cy + 4.5f));
        pSink->EndFigure(D2D1_FIGURE_END_OPEN);
        pSink->Close();
        pSink->Release();

        rt->DrawGeometry(pPath, brush, 1.9f, stroke);
        pPath->Release();
    }
}

void VectorIcons::DrawPadlock(ID2D1RenderTarget* rt, float cx, float cy, bool locked, ID2D1Brush* brush) {
    ID2D1Factory* factory = GraphicsResources::GetD2DFactory();
    ID2D1StrokeStyle* stroke = GraphicsResources::GetRoundStrokeStyle();
    if (!factory || !stroke) return;

    ID2D1PathGeometry* pPath = nullptr;
    factory->CreatePathGeometry(&pPath);
    if (pPath) {
        ID2D1GeometrySink* pSink = nullptr;
        pPath->Open(&pSink);
        float sw = 3.8f;
        if (locked) {
            pSink->BeginFigure(D2D1::Point2F(cx - sw, cy - 1.0f), D2D1_FIGURE_BEGIN_HOLLOW);
            pSink->AddLine(D2D1::Point2F(cx - sw, cy - 4.2f));
            pSink->AddArc(D2D1::ArcSegment(D2D1::Point2F(cx + sw, cy - 4.2f), D2D1::SizeF(sw, sw), 0, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
            pSink->AddLine(D2D1::Point2F(cx + sw, cy - 1.0f));
        } else {
            pSink->BeginFigure(D2D1::Point2F(cx - sw, cy - 1.0f), D2D1_FIGURE_BEGIN_HOLLOW);
            pSink->AddLine(D2D1::Point2F(cx - sw, cy - 5.5f));
            pSink->AddArc(D2D1::ArcSegment(D2D1::Point2F(cx + sw, cy - 5.5f), D2D1::SizeF(sw, sw), 0, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
            pSink->AddLine(D2D1::Point2F(cx + sw, cy - 3.0f));
        }
        pSink->EndFigure(D2D1_FIGURE_END_OPEN);
        pSink->Close();
        pSink->Release();

        rt->DrawGeometry(pPath, brush, 1.8f, stroke);
        pPath->Release();
    }

    D2D1_ROUNDED_RECT body = D2D1::RoundedRect(D2D1::RectF(cx - 5.5f, cy - 1.0f, cx + 5.5f, cy + 7.5f), 2.0f, 2.0f);
    rt->DrawRoundedRectangle(body, brush, 1.8f);
    rt->DrawLine(D2D1::Point2F(cx, cy + 2.0f), D2D1::Point2F(cx, cy + 4.5f), brush, 1.8f, stroke);
}

void VectorIcons::DrawSliders(ID2D1RenderTarget* rt, float cx, float cy, ID2D1Brush* brush, ID2D1Brush* bgBrush) {
    ID2D1StrokeStyle* stroke = GraphicsResources::GetRoundStrokeStyle();
    float y1 = cy - 4.8f;
    float y2 = cy;
    float y3 = cy + 4.8f;
    float span = 8.0f;

    rt->DrawLine(D2D1::Point2F(cx - span, y1), D2D1::Point2F(cx + span, y1), brush, 1.6f, stroke);
    rt->DrawLine(D2D1::Point2F(cx - span, y2), D2D1::Point2F(cx + span, y2), brush, 1.6f, stroke);
    rt->DrawLine(D2D1::Point2F(cx - span, y3), D2D1::Point2F(cx + span, y3), brush, 1.6f, stroke);

    float kr = 2.4f;
    D2D1_ELLIPSE k1 = D2D1::Ellipse(D2D1::Point2F(cx - 3.2f, y1), kr, kr);
    D2D1_ELLIPSE k2 = D2D1::Ellipse(D2D1::Point2F(cx + 3.8f, y2), kr, kr);
    D2D1_ELLIPSE k3 = D2D1::Ellipse(D2D1::Point2F(cx - 0.5f, y3), kr, kr);

    if (bgBrush) {
        rt->FillEllipse(k1, bgBrush);
        rt->FillEllipse(k2, bgBrush);
        rt->FillEllipse(k3, bgBrush);
    }
    rt->DrawEllipse(k1, brush, 1.6f);
    rt->DrawEllipse(k2, brush, 1.6f);
    rt->DrawEllipse(k3, brush, 1.6f);
}

void VectorIcons::DrawSpeakerVol(ID2D1RenderTarget* rt, float cx, float cy, ID2D1Brush* brush) {
    ID2D1Factory* factory = GraphicsResources::GetD2DFactory();
    ID2D1StrokeStyle* stroke = GraphicsResources::GetRoundStrokeStyle();
    if (!factory || !stroke) return;

    ID2D1PathGeometry* pPath = nullptr;
    factory->CreatePathGeometry(&pPath);
    if (pPath) {
        ID2D1GeometrySink* pSink = nullptr;
        pPath->Open(&pSink);
        pSink->BeginFigure(D2D1::Point2F(cx - 5.0f, cy - 2.5f), D2D1_FIGURE_BEGIN_FILLED);
        pSink->AddLine(D2D1::Point2F(cx - 2.0f, cy - 2.5f));
        pSink->AddLine(D2D1::Point2F(cx + 1.8f, cy - 5.5f));
        pSink->AddLine(D2D1::Point2F(cx + 1.8f, cy + 5.5f));
        pSink->AddLine(D2D1::Point2F(cx - 2.0f, cy + 2.5f));
        pSink->AddLine(D2D1::Point2F(cx - 5.0f, cy + 2.5f));
        pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
        pSink->Close();
        pSink->Release();

        rt->DrawGeometry(pPath, brush, 1.5f, stroke);
        pPath->Release();
    }

    // Small sound wave arc
    ID2D1PathGeometry* pWave = nullptr;
    factory->CreatePathGeometry(&pWave);
    if (pWave) {
        ID2D1GeometrySink* pSink = nullptr;
        pWave->Open(&pSink);
        pSink->BeginFigure(D2D1::Point2F(cx + 4.0f, cy - 3.0f), D2D1_FIGURE_BEGIN_HOLLOW);
        pSink->AddArc(D2D1::ArcSegment(D2D1::Point2F(cx + 4.0f, cy + 3.0f), D2D1::SizeF(3.5f, 3.5f), 0, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
        pSink->EndFigure(D2D1_FIGURE_END_OPEN);
        pSink->Close();
        pSink->Release();

        rt->DrawGeometry(pWave, brush, 1.5f, stroke);
        pWave->Release();
    }
}

void VectorIcons::DrawLinkChain(ID2D1RenderTarget* rt, float cx, float cy, ID2D1Brush* brush) {
    D2D1_ROUNDED_RECT link1 = D2D1::RoundedRect(D2D1::RectF(cx - 6.0f, cy - 3.0f, cx + 0.5f, cy + 3.0f), 3.0f, 3.0f);
    D2D1_ROUNDED_RECT link2 = D2D1::RoundedRect(D2D1::RectF(cx - 0.5f, cy - 3.0f, cx + 6.0f, cy + 3.0f), 3.0f, 3.0f);
    rt->DrawRoundedRectangle(link1, brush, 1.5f);
    rt->DrawRoundedRectangle(link2, brush, 1.5f);
}

void VectorIcons::DrawChevron(ID2D1RenderTarget* rt, float cx, float cy, ID2D1Brush* brush) {
    ID2D1Factory* factory = GraphicsResources::GetD2DFactory();
    ID2D1StrokeStyle* stroke = GraphicsResources::GetRoundStrokeStyle();
    if (!factory || !stroke) return;

    ID2D1PathGeometry* pPath = nullptr;
    factory->CreatePathGeometry(&pPath);
    if (pPath) {
        ID2D1GeometrySink* pSink = nullptr;
        pPath->Open(&pSink);
        pSink->BeginFigure(D2D1::Point2F(cx - 4.0f, cy - 2.0f), D2D1_FIGURE_BEGIN_HOLLOW);
        pSink->AddLine(D2D1::Point2F(cx, cy + 2.0f));
        pSink->AddLine(D2D1::Point2F(cx + 4.0f, cy - 2.0f));
        pSink->EndFigure(D2D1_FIGURE_END_OPEN);
        pSink->Close();
        pSink->Release();

        rt->DrawGeometry(pPath, brush, 2.0f, stroke);
        pPath->Release();
    }
}

void VectorIcons::DrawPower(ID2D1RenderTarget* rt, float cx, float cy, ID2D1Brush* brush) {
    ID2D1Factory* factory = GraphicsResources::GetD2DFactory();
    ID2D1StrokeStyle* stroke = GraphicsResources::GetRoundStrokeStyle();
    if (!factory || !stroke) return;

    ID2D1PathGeometry* pPath = nullptr;
    factory->CreatePathGeometry(&pPath);
    if (pPath) {
        ID2D1GeometrySink* pSink = nullptr;
        pPath->Open(&pSink);
        // Circle arc from -45 deg around clockwise to +45 deg (270 degrees)
        pSink->BeginFigure(D2D1::Point2F(cx + 3.8f, cy - 3.8f), D2D1_FIGURE_BEGIN_HOLLOW);
        pSink->AddArc(D2D1::ArcSegment(
            D2D1::Point2F(cx - 3.8f, cy - 3.8f),
            D2D1::SizeF(5.4f, 5.4f),
            0.0f,
            D2D1_SWEEP_DIRECTION_CLOCKWISE,
            D2D1_ARC_SIZE_LARGE
        ));
        pSink->EndFigure(D2D1_FIGURE_END_OPEN);
        pSink->Close();
        pSink->Release();

        rt->DrawGeometry(pPath, brush, 1.8f, stroke);
        pPath->Release();
    }
    // Vertical tick through the top opening
    rt->DrawLine(D2D1::Point2F(cx, cy - 6.2f), D2D1::Point2F(cx, cy - 1.2f), brush, 1.8f, stroke);
}
