#pragma once
#include <irrlicht.h>
#include <vector>
#include "ColoredTextCodes.h"
using namespace irr;

class CGUIColoredText : public gui::IGUIElement {
public:
    CGUIColoredText(gui::IGUIEnvironment* env,
        gui::IGUIElement* parent,
        s32 id,
        const core::rect<s32>& rect)
        : gui::IGUIElement(gui::EGUIET_ELEMENT, env, parent, id, rect) {
        setTabStop(false);
        setSubElement(true);
        Font = nullptr;
        Shadow = false;
        setText(L"");
    }

    ~CGUIColoredText() noexcept override {}

    void setOverrideFont(gui::IGUIFont* f) { Font = f; Dirty = true; }
    gui::IGUIFont* getOverrideFont() const { return Font; }
    void setOverrideColor(video::SColor c) { OverrideColor = c; OverrideColorUsed = true; Dirty = true; }
    void clearOverrideColor() { OverrideColorUsed = false; Dirty = true; }
    void setTextAlignment(gui::EGUI_ALIGNMENT h, gui::EGUI_ALIGNMENT v) { AlignH = h; AlignV = v; Dirty = true; }
    void setWordWrap(bool e) { WordWrap = e; Dirty = true; }
    void setLineSpacing(int px) { LineSpacing = px; Dirty = true; }
    void setText(const wchar_t* t) override { Text = t ? t : L""; Dirty = true; }
    const wchar_t* getText() const override { return Text.c_str(); }

    void setOpacity(f32 v) { opacity = core::clamp(v, 0.f, 1.f); }
    f32 getOpacity() const { return opacity; }

    void draw() override {
        if (!IsVisible) return;
        if (!Font) Font = Environment->getSkin()->getFont();
        if (!Font) return;
        if (Dirty) layout_();

        const core::rect<s32> clip = AbsoluteClippingRect;
        const s32 lineH = Font->getDimension(L"A").Height;
        const s32 totalH = (s32)Lines.size() * lineH + (Lines.empty() ? 0 : ((int)Lines.size() - 1) * LineSpacing);

        core::position2di base(AbsoluteRect.UpperLeftCorner);
        if (AlignV == gui::EGUIA_CENTER)      base.Y += (AbsoluteRect.getHeight() - totalH) / 2;
        else if (AlignV == gui::EGUIA_LOWERRIGHT) base.Y += (AbsoluteRect.getHeight() - totalH);

        for (size_t li = 0; li < Lines.size(); ++li) {
            const auto& segs = Lines[li];
            s32 x = base.X;
            s32 lineW = LineWidths[li];
            if (AlignH == gui::EGUIA_CENTER)        x += (AbsoluteRect.getWidth() - lineW) / 2;
            else if (AlignH == gui::EGUIA_LOWERRIGHT) x += (AbsoluteRect.getWidth() - lineW);
            s32 y = base.Y + (s32)li * (lineH + LineSpacing);

            for (const auto& run : segs) {
                const core::dimension2du d = Font->getDimension(run.text.c_str());

                // Shadow
                if (Shadow || run.shadow) {
                    video::SColor sc((run.color.getAlpha() * 3) / 4, 0, 0, 0);
                    sc.setAlpha((u32)((f32)sc.getAlpha() * opacity));
                    Font->draw(run.text.c_str(),
                        core::rect<s32>(x + 1, y + 1, x + 1 + (s32)d.Width, y + 1 + (s32)d.Height),
                        sc, false, false, &clip);
                }

                // Main text
                const int passes = run.bold ? 2 : 1;
                const int italicShift = run.italic ? 1 : 0;
                for (int p = 0; p < passes; ++p) {
                    video::SColor c = run.color;
                    c.setAlpha((u32)((f32)c.getAlpha() * opacity));
                    Font->draw(run.text.c_str(),
                        core::rect<s32>(x + p + italicShift, y,
                            x + p + italicShift + (s32)d.Width, y + (s32)d.Height),
                        c, false, false, &clip);
                }

                // Underline / strike
                if (run.underline || run.strike) {
                    auto* drv = Environment->getVideoDriver();
                    const s32 baseY = y + (s32)d.Height - 2;
                    const s32 midY = y + (s32)d.Height / 2;
                    video::SColor lc = run.color;
                    lc.setAlpha((u32)((f32)lc.getAlpha() * opacity));
                    if (run.underline) drv->draw2DRectangle(lc, core::rect<s32>(x, baseY, x + (s32)d.Width, baseY + 1), &clip);
                    if (run.strike)    drv->draw2DRectangle(lc, core::rect<s32>(x, midY, x + (s32)d.Width, midY + 1), &clip);
                }

                x += (s32)d.Width;
            }
        }
        IGUIElement::draw();
    }

    bool OnEvent(const SEvent& e) override { return IGUIElement::OnEvent(e); }

    void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions*) const override {
        IGUIElement::serializeAttributes(out, 0);
        out->addString("Text", Text.c_str());
        out->addBool("WordWrap", WordWrap);
        out->addInt("LineSpacing", LineSpacing);
        out->addBool("Shadow", Shadow);
    }
    void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions*) override {
        IGUIElement::deserializeAttributes(in, 0);
        setText(in->getAttributeAsStringW("Text").c_str());
        WordWrap = in->getAttributeAsBool("WordWrap");
        LineSpacing = in->getAttributeAsInt("LineSpacing");
        Shadow = in->getAttributeAsBool("Shadow");
        Dirty = true;
    }

    bool getShadow() { return Shadow; }
    void setShadow(bool s) { Shadow = s; }

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
        std::vector<Run> runs; Run cur; cur.color = OverrideColorUsed ? OverrideColor : video::SColor(255, 255, 255, 255); cur.shadow = Shadow;
        const wchar_t* s = Text.c_str(); const size_t N = Text.size();
        for (size_t i = 0; i < N; ++i) {
            wchar_t c = s[i];
            if ((c == L'§' || c == L'&') && i + 1 < N) {
                wchar_t n = s[i + 1];
                if ((n >= L'0' && n <= L'9') || (n >= L'a' && n <= L'f') || (n >= L'A' && n <= L'F')) { pushRun_(runs, cur); cur.color = charToColorCode(n); i += 1; continue; }
                switch (n) {
                case L'l': case L'L': pushRun_(runs, cur); cur.bold = true; i += 1; continue;
                case L'n': case L'N': pushRun_(runs, cur); cur.underline = true; i += 1; continue;
                case L'm': case L'M': pushRun_(runs, cur); cur.strike = true; i += 1; continue;
                case L'o': case L'O': pushRun_(runs, cur); cur.italic = true; i += 1; continue;
                case L'r': case L'R': pushRun_(runs, cur); cur = Run(); cur.color = OverrideColorUsed ? OverrideColor : video::SColor(255, 255, 255, 255); cur.shadow = Shadow; i += 1; continue;
                default: break;
                }
            }

            if (c == L'\n') {
                pushRun_(runs, cur);
                Run nl = cur;
                nl.text = L"\n";
                runs.push_back(nl);
                continue;
            }

            cur.text += c;
        }
        pushRun_(runs, cur);

            const s32 maxW = AbsoluteRect.getWidth();
            const bool wrap = WordWrap && maxW > 0;
            std::vector<Run> line; s32 currentW = 0;
            auto flushLine = [&]() { Lines.push_back(line); LineWidths.push_back(currentW); line.clear(); currentW = 0; };

            for (const auto& r : runs) {
                if (r.text == L"\n") { flushLine(); continue; }
                    if (!wrap) { line.push_back(r); currentW += (s32)Font->getDimension(r.text.c_str()).Width; continue; }
                core::stringw remaining = r.text; Run style = r;
                    while (!remaining.empty()) {
                        s32 take = (s32)remaining.size();
                        core::stringw word = remaining; // greedy
                        u32 w = Font->getDimension(word.c_str()).Width;
                        if (currentW > 0 && currentW + (s32)w > maxW) { flushLine(); }
                        Run wr = style; wr.text = word; line.push_back(wr); currentW += (s32)w; break;
                    }
            }
        if (!line.empty()) flushLine();
    }

    f32 opacity = 1.0f;
    core::stringw Text; gui::IGUIFont* Font = nullptr; video::SColor OverrideColor{ 255,255,255,255 }; bool OverrideColorUsed = false; gui::EGUI_ALIGNMENT AlignH = gui::EGUIA_UPPERLEFT; gui::EGUI_ALIGNMENT AlignV = gui::EGUIA_UPPERLEFT; bool WordWrap = false, Shadow = true; int LineSpacing = 0; bool Dirty = true; std::vector<std::vector<Run>> Lines; std::vector<s32> LineWidths;
};

inline CGUIColoredText* addColoredText(gui::IGUIEnvironment* env,
    const wchar_t* text,
    const core::rect<s32>& rect,
    gui::IGUIElement* parent = nullptr,
    s32 id = -1)
{
    if (!parent) parent = env->getRootGUIElement();
    auto* el = new CGUIColoredText(env, parent, id, rect);
    el->drop();
    el->setText(text);
    return el;
}