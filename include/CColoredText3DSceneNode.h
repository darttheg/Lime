#pragma once
#include <irrlicht.h>
#include <vector>
#include "ColoredTextCodes.h"
using namespace irr;

class CColoredText3DSceneNode : public scene::ISceneNode {
public:
    enum class EHAlign { Left, Center, Right };
    enum class EVAlign { Top, Middle, Bottom };

    CColoredText3DSceneNode(scene::ISceneNode* parent,
        scene::ISceneManager* mgr,
        s32 id = -1)
        : scene::ISceneNode(parent, mgr, id) {
        AutomaticCullingState = scene::EAC_OFF;
        Font = mgr->getGUIEnvironment()->getSkin() ? mgr->getGUIEnvironment()->getSkin()->getFont() : nullptr;
        DefaultColor = video::SColor(255, 255, 255, 255);
        Shadow = false;
        AlignH = EHAlign::Left;
        AlignV = EVAlign::Top;
        LineSpacing = 0;
        Box.reset(core::vector3df(0, 0, 0));
    }

    ~CColoredText3DSceneNode() override {}

    void setText(const core::stringw& t) { Text = t; Dirty = true; }
    void setFont(gui::IGUIFont* f) { Font = f; Dirty = true; }
    void setDefaultColor(video::SColor c) { DefaultColor = c; Dirty = true; }
    bool getShadow() { return Shadow; }
    void setShadow(bool s) { Shadow = s; }
    void setAlignment(EHAlign h, EVAlign v) { AlignH = h; AlignV = v; }
    void setLineSpacing(s32 px) { LineSpacing = px; Dirty = true; }
    void setWorldPosition(const core::vector3df& p) { setPosition(p); }

    void setScreenOffset(const core::position2di& ofs) { ScreenOffset = ofs; }

    void OnRegisterSceneNode() override {
        if (IsVisible) SceneManager->registerNodeForRendering(this, scene::ESNRP_TRANSPARENT);
        ISceneNode::OnRegisterSceneNode();
    }

    void render() override {
        if (!Font) return;
        if (Dirty) layout_();

        video::IVideoDriver* drv = SceneManager->getVideoDriver();
        scene::ICameraSceneNode* cam = SceneManager->getActiveCamera();
        scene::ISceneCollisionManager* coll = SceneManager->getSceneCollisionManager();
        if (!drv || !cam || !coll) return;

        core::position2di origin2d =
            coll->getScreenCoordinatesFrom3DPosition(getAbsolutePosition(), cam);

        // Cull if behind camera or off-screen
        const core::vector3df toPt = getAbsolutePosition() - cam->getAbsolutePosition();
        const core::vector3df camDir = cam->getTarget() - cam->getAbsolutePosition();
        if (toPt.dotProduct(camDir) <= 0.f) return;
        if (!drv->getViewPort().isPointInside(origin2d)) return;

        origin2d += ScreenOffset;

        const s32 lineH = Font->getDimension(L"A").Height;
        const s32 totalH =
            (s32)Lines.size() * lineH + (Lines.empty() ? 0 : ((s32)Lines.size() - 1) * LineSpacing);
        if (AlignV == EVAlign::Middle) origin2d.Y -= totalH / 2;
        else if (AlignV == EVAlign::Bottom) origin2d.Y -= totalH;

        const core::rect<s32>* clip = nullptr;

        for (size_t li = 0; li < Lines.size(); ++li) {
            s32 x = origin2d.X;
            const s32 lineW = LineWidths[li];
            if (AlignH == EHAlign::Center) x -= lineW / 2;
            else if (AlignH == EHAlign::Right) x -= lineW;

            const s32 y = origin2d.Y + (s32)li * (lineH + LineSpacing);
            const auto& segs = Lines[li];

            for (const auto& r : segs) {
                if (r.text.empty()) continue;
                const core::dimension2du d = Font->getDimension(r.text.c_str());

                // shadow
                if (Shadow || r.shadow) {
                    video::SColor sc((r.color.getAlpha() * 3) / 4, 0, 0, 0);
                    sc.setAlpha((u32)((f32)sc.getAlpha() * opacity));
                    Font->draw(r.text.c_str(),
                        core::rect<s32>(x + 1, y + 1, x + 1 + (s32)d.Width, y + 1 + (s32)d.Height),
                        sc, false, false, clip);
                }

                const int italicShift = r.italic ? 1 : 0;
                const int passes = r.bold ? 2 : 1;
                for (int p = 0; p < passes; ++p) {
                    video::SColor c = r.color;
                    c.setAlpha((u32)((f32)c.getAlpha() * opacity));
                    Font->draw(r.text.c_str(),
                        core::rect<s32>(x + p + italicShift, y,
                            x + p + italicShift + (s32)d.Width,
                            y + (s32)d.Height),
                        c, false, false, clip);
                }

                if (r.underline || r.strike) {
                    const s32 baseY = y + (s32)d.Height - 2;
                    const s32 midY = y + (s32)d.Height / 2;
                    video::SColor lc = r.color;
                    lc.setAlpha((u32)((f32)lc.getAlpha() * opacity));
                    if (r.underline)
                        drv->draw2DRectangle(lc, core::rect<s32>(x, baseY, x + (s32)d.Width, baseY + 1), clip);
                    if (r.strike)
                        drv->draw2DRectangle(lc, core::rect<s32>(x, midY, x + (s32)d.Width, midY + 1), clip);
                }

                x += (s32)d.Width;
            }
        }
    }

    const core::aabbox3d<f32>& getBoundingBox() const override { return Box; }
    u32 getMaterialCount() const override { return 0; }
    video::SMaterial& getMaterial(u32) override { return Dummy; }

    void setOpacity(f32 v) { opacity = core::clamp(v, 0.f, 1.f); }
    f32 getOpacity() const { return opacity; }

private:
    struct Run { core::stringw text; video::SColor color; bool bold = false, underline = false, strike = false, italic = false, shadow = false; };

    static bool hexNibble(wchar_t c, u8& out) {
        if (c >= L'0' && c <= L'9') { out = (u8)(c - L'0'); return true; }
        if (c >= L'a' && c <= L'f') { out = (u8)(10 + c - L'a'); return true; }
        if (c >= L'A' && c <= L'F') { out = (u8)(10 + c - L'A'); return true; }
        return false;
    }

    void pushRun_(std::vector<Run>& runs, Run& cur) { if (!cur.text.empty()) { runs.push_back(cur); cur.text = L""; } }

    void layout_() {
        Lines.clear(); LineWidths.clear(); Dirty = false;
        if (!Font) return;

        std::vector<Run> runs;
        Run cur; cur.color = DefaultColor; cur.shadow = Shadow;

        const wchar_t* s = Text.c_str(); const size_t N = Text.size();
        for (size_t i = 0; i < N; ++i) {
            const wchar_t c = s[i];
            if ((c == L'§' || c == L'&') && i + 1 < N) {
                const wchar_t n = s[i + 1];
                if ((n >= L'0' && n <= L'9') || (n >= L'a' && n <= L'f') || (n >= L'A' && n <= L'F')) { pushRun_(runs, cur); cur.color = charToColorCode(n); i += 1; continue; }
                switch (n) {
                case L'l': case L'L': pushRun_(runs, cur); cur.bold = true; i += 1; continue;
                case L'n': case L'N': pushRun_(runs, cur); cur.underline = true; i += 1; continue;
                case L'm': case L'M': pushRun_(runs, cur); cur.strike = true; i += 1; continue;
                case L'o': case L'O': pushRun_(runs, cur); cur.italic = true; i += 1; continue;
                case L'r': case L'R': pushRun_(runs, cur); cur = Run(); cur.color = DefaultColor; cur.shadow = Shadow; i += 1; continue;
                default: break;
                }
            }
            if (c == L'\n') { pushRun_(runs, cur); Run nl = cur; nl.text = L"\n"; runs.push_back(nl); continue; }
            cur.text += c;
        }
        pushRun_(runs, cur);

        std::vector<Run> line; s32 wAccum = 0;
        auto flushLine = [&]() { Lines.push_back(line); LineWidths.push_back(wAccum); line.clear(); wAccum = 0; };

        for (const auto& r : runs) {
            if (r.text == L"\n") { flushLine(); continue; }
            line.push_back(r);
            wAccum += (s32)Font->getDimension(r.text.c_str()).Width;
        }
        if (!line.empty()) flushLine();
    }

    gui::IGUIFont* Font = nullptr;
    core::stringw Text;
    video::SColor DefaultColor;
    bool Shadow = false;
    EHAlign AlignH;
    EVAlign AlignV;
    s32 LineSpacing = 0;
    core::position2di ScreenOffset{ 0,0 };
    f32 opacity = 1.0f;

    std::vector<std::vector<Run>> Lines;
    std::vector<s32> LineWidths;
    bool Dirty = true;

    core::aabbox3d<f32> Box;
    video::SMaterial Dummy;
};

inline CColoredText3DSceneNode* addColoredText3D(
    scene::ISceneManager* smgr,
    const wchar_t* text,
    const core::vector3df& pos = core::vector3df(0, 0, 0),
    scene::ISceneNode* parent = nullptr,
    s32 id = -1)
{
    if (!smgr) return nullptr;
    if (!parent) parent = smgr->getRootSceneNode();

    auto* node = new CColoredText3DSceneNode(parent, smgr, id);
    node->setPosition(pos);
    node->setText(text);
    node->drop();
    return node;
}