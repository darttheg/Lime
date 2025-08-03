#pragma once

#include "irrlicht.h"
#include "IrrManagers.h"
#include "Vector2D.h"
#include <string>
#include <unordered_map>
#include "Image2D.h"
#include "Vector4D.h"

class EditBox {
    irr::gui::IGUIEditBox* box;
    bool border;

    wchar_t* charToWchar(const char* str);

public:
    EditBox();
    EditBox(std::string tx);
    EditBox(std::string tx, const Vector2D& pos);
    EditBox(std::string tx, const Vector2D& pos, const Vector2D& dimensions);
    EditBox(const EditBox& other);
    ~EditBox();

    std::string getText();
    void setText(std::string tx);

    void destroy();

    bool getVisible();
    void setVisible(bool vis);

    void bringToFront();
    void sendToBack();

    Vector2D getSize();
    void setSize(const Vector2D& size);

    void setBorderAlignment(int a, int b, int c, int d);
    void setTextAlignment(const Vector2D& align);
    void setMaxSize(const Vector2D& max);

    bool getWrap();
    void setWrap(bool wrap);

    Vector4D getTextColor();
    void setTextColor(const Vector4D& col);

    bool setFont(const std::string& fontName);

    Vector2D getPosition();
    void setPosition(const Vector2D& pos);

    void setParent(const Image2D& other);

    bool getDrawBorder();
    void setDrawBorder(bool enable);

    bool getIsPassword();
    void setIsPassword(bool enable);

    bool getIsEnabled();
    void setIsEnabled(bool enable);

    bool getMultiLine();
    void setMultiLine(bool enable);

    bool getAutoScroll();
    void setAutoScroll(bool enable);
};

void bindEditBox();