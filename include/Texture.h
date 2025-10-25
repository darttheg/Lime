#pragma once

#include <string>
#include "irrlicht.h"
#include "IrrManagers.h"
#include "Vector2D.h"
#include "Vector4D.h"

class Texture {
public:
    irr::video::ITexture* texture;
    std::string path;

    Texture();
    Texture(const Vector2D& size);
    Texture(const std::string imgpath);
    Texture(const std::string imgpath, sol::table options);

    bool load(const std::string& imgpath);
    std::string getPath() const;
    void keyColor(const Vector2D& pos);
    void saveTexture(std::string path);
    void createEmpty(const Vector2D& size);
    bool append(const Texture& tex, const Vector2D& pos); // Copies texture to another
    bool appendFromFile(std::string filePath, const Vector2D& pos); // Copies texture to another
    Vector4D getPixel(const Vector2D& pos);
    Texture makeNormalMap(float height = 1.0f);
    sol::object destroy();

    Texture crop(const Vector2D& topL, const Vector2D& bottomR);

    bool doAppend(irr::video::IImage* img, irr::core::vector2di pos);

    irr::video::IImage* texToImg(irr::video::ITexture* tex);
};

void bindTexture();