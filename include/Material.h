#pragma once

#include "irrlicht.h"
#include "IrrManagers.h"
#include "Texture.h"
#include "Vector2D.h"
#include "Vector3D.h"
#include "Vector4D.h"
#include "LuaLime.h"

class Material {
public:
    irr::video::SMaterial mat;

    Material();
    Material(const Material& other);
    Material(Texture& tex);

    int getAntiAliasing();
    void setAntiAliasing(int i);

    int getMaterialType();
    void setMaterialType(int i);

    bool getFog();
    void setFog(bool i);

    bool getBackface();
    void setBackface(bool i);

    bool getFrontface();
    void setFrontface(bool i);

    bool getWireframe();
    void setWireframe(bool i);

    bool getLighting();
    void setLighting(bool i);

    Vector4D getDiffuseColor();
    void setDiffuseColor(Vector4D& color);

    Vector4D getAmbientColor();
    void setAmbientColor(Vector4D& color);

    Vector4D getEmissiveColor();
    void setEmissiveColor(Vector4D& color);

    Vector4D getSpecularColor();
    void setSpecularColor(Vector4D& color);

    bool getGourad();
    void setGourad(bool i);

    bool getZWrite();
    void setZWrite(bool i);

    int getZComparison();
    void setZComparison(int i);

    bool getPointCloud();
    void setPointCloud(bool i);

    float getShine();
    void setShine(float i);

    bool getBiFilter();
    void setBiFilter(bool i);

    bool getTriFilter();
    void setTriFilter(bool i);

    bool getAnsiFilter();
    void setAnsiFilter(bool i);

    Vector2D getPan(int i);
    void setPan(Vector2D scroll, int i);

    int getWrapU(int layer);
    void setWrapU(int layer, int i);

    int getWrapV(int layer);
    void setWrapV(int layer, int i);

    int getWrapUV(int layer);
    void setWrapUV(int layer, int i);

    int getOpacity();
    void setOpacity(int i);

    bool getMipMaps();
    void setMipMaps(bool i);

    void setTexture(Texture& tex, int slot);
    bool getTexture(int slot);

    int getID();
    void setID(int i);

    void setTextureScale(Vector2D scale, int i);
    Vector2D getTextureScale(int i);

    void setMaterialFlag(int i, bool enable);
};

void bindMaterial();