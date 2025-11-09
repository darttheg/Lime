#pragma once

#include "irrlicht.h"
#include "IrrManagers.h"
#include "Vector2D.h"
#include "Vector3D.h"
#include "Vector4D.h"
#include "Image2D.h"
#include <string>

#include "CGUIColoredText.h"

#include "Compatible2D.h"

class Text2D : public Compatible2D {
private:
    CGUIColoredText* text;
public:
    Text2D();
    Text2D(std::string tx, std::string fontName);
    Text2D(std::string tx);
    Text2D(std::string tx, const Vector2D& pos);
    Text2D(std::string tx, const Vector2D& pos, const Vector2D& dimensions);
    Text2D(const Text2D& other);

    wchar_t* charToWchar(const char* str);

    std::string getText();
    void setText(std::string tx);

    float getOpacity();
    void setOpacity(float f);

    bool getShadows() { return text ? text->getShadow() : false; }
    void setShadows(bool v) {
        if (text) text->setShadow(v);
    }

    /*bool getWrap();
    void setWrap(bool wrap);

    bool getDrawBorder();
    void setDrawBorder(bool enable);

    bool getDrawBackground();
    void setDrawBackground(bool draw);

    Vector4D getBackgroundColor();
    void setBackgroundColor(const Vector4D& col);

    Vector4D getTextColor();
    void setTextColor(const Vector4D& col);*/

    void setBorderAlignment(int a, int b, int c, int d);
    void setTextAlignment(int h, int v);
    // void setTextAlignment(const Vector2D& align);

    bool setFont(const std::string& fontName);
    void setParent(const Image2D& other);

    void destroy() override;

    irr::gui::IGUIElement* getNode() const override { return text; }
};

void bindText2D();