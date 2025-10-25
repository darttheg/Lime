#pragma once

#include "irrlicht.h"
#include "IrrManagers.h"
#include "StaticMesh.h"
#include "Vector2D.h"
#include "Vector3D.h"
#include "Vector4D.h"
#include <string>

#include "CColoredText3DSceneNode.h"

#include "Compatible3D.h"

class Text3D : public Compatible3D {
public:
    //irr::scene::ITextSceneNode* text;
    CColoredText3DSceneNode* text;
    // irr::video::SColor color;
    int opacity;
    std::string myText;

    wchar_t* charToWchar(const char* str);

    Text3D();
    Text3D(const std::string& tx);
    Text3D(const std::string& tx, const Vector3D& pos);
    // Text3D(const std::string& tx, const Vector3D& pos, const Vector4D& col);
    Text3D(const std::string& tx, const std::string& fontName);
    // Text3D(const std::string& tx, const Vector3D& pos, const Vector4D& col, const std::string& fontName);
    Text3D(const std::string& tx, const Vector3D& pos, const std::string& fontName);

    std::string getText();
    void setText(const std::string& tx);

    void setAlignment(int h, int v); // Use EHAlign & EVAlign

    float getOpacity();
    void setOpacity(float f);

    bool getShadows() { return text ? text->getShadow() : false; }
    void setShadows(bool v) {
        if (text) text->setShadow(v);
    }

    /*Vector4D getColor();
    void setColor(const Vector4D& col);

    void updateColor();*/

    bool setFont(const std::string& fontName);

    void destroy() override;

    irr::scene::ISceneNode* getNode() const override { return text; }
};

void bindText3D();