#include "Material.h"
#include "ShaderCallback.h"

Material::Material() {
    mat.UseMipMaps = false;
}

Material::Material(int type) {
    setMaterialType(type);
}

Material::Material(const Material& other) {
    mat = SMaterial(other.mat);
}

Material::Material(Texture& tex, int type) {
    setTexture(tex, 0);
    setMaterialType(type);
}

Material::Material(Texture& tex) {
    setTexture(tex, 0);
}

int Material::getAntiAliasing() {
    return mat.AntiAliasing;
}

void Material::setAntiAliasing(int i) {
    mat.AntiAliasing = i;
}

int Material::getMaterialType() {
    return mat.MaterialType;
}

void Material::setMaterialType(int i) {
    mat.MaterialType = (irr::video::E_MATERIAL_TYPE)i;
    if ((irr::video::E_MATERIAL_TYPE)i == E_MATERIAL_TYPE::EMT_ONETEXTURE_BLEND)
        mat.MaterialTypeParam = pack_textureBlendFunc(EBF_DST_COLOR, EBF_ZERO, EMFN_MODULATE_1X, EAS_NONE);
    else
        mat.MaterialTypeParam = 0;
}

bool Material::getFog() {
    return mat.FogEnable;
}

void Material::setFog(bool i) {
    mat.FogEnable = i;
}

bool Material::getBackface() {
    return mat.BackfaceCulling;
}

void Material::setBackface(bool i) {
    mat.BackfaceCulling = i;
}

bool Material::getFrontface() {
    return mat.FrontfaceCulling;
}

void Material::setFrontface(bool i) {
    mat.FrontfaceCulling = i;
}

bool Material::getWireframe() {
    return mat.Wireframe;
}

void Material::setWireframe(bool i) {
    mat.Wireframe = i;
}

bool Material::getLighting() {
    return mat.Lighting;
}

void Material::setLighting(bool i) {
    mat.Lighting = i;
}

Vector4D Material::getDiffuseColor() {
    return Vector4D(mat.DiffuseColor.getRed(), mat.DiffuseColor.getGreen(), mat.DiffuseColor.getBlue(), mat.DiffuseColor.getAlpha());
}

void Material::setDiffuseColor(Vector4D& color) {
    mat.DiffuseColor.set(color.w, color.x, color.y, color.z);
}

Vector4D Material::getAmbientColor() {
    return Vector4D(mat.AmbientColor.getRed(), mat.AmbientColor.getGreen(), mat.AmbientColor.getBlue(), mat.AmbientColor.getAlpha());
}

void Material::setAmbientColor(Vector4D& color) {
    mat.AmbientColor.set(color.w, color.x, color.y, color.z);
}

Vector4D Material::getEmissiveColor() {
    return Vector4D(mat.EmissiveColor.getRed(), mat.EmissiveColor.getGreen(), mat.EmissiveColor.getBlue(), mat.EmissiveColor.getAlpha());
}

void Material::setEmissiveColor(Vector4D& color) {
    mat.EmissiveColor.set(color.w, color.x, color.y, color.z);
}

Vector4D Material::getSpecularColor() {
    return Vector4D(mat.SpecularColor.getRed(), mat.SpecularColor.getGreen(), mat.SpecularColor.getBlue(), mat.SpecularColor.getAlpha());
}

void Material::setSpecularColor(Vector4D& color) {
    mat.SpecularColor.set(color.w, color.x, color.y, color.z);
}

int Material::getID() {
    return mat.ID;
}

void Material::setID(int i) {
    mat.ID = i;
}

bool Material::getGourad() {
    return mat.GouraudShading;
}

void Material::setGourad(bool i) {
    mat.GouraudShading = i;
}

bool Material::getZWrite() {
    return mat.ZWriteEnable;
}

void Material::setZWrite(bool i) {
    mat.ZWriteEnable = i;
}

int Material::getZComparison() {
    return mat.ZBuffer;
}

void Material::setZComparison(int i) {
    mat.ZBuffer = i;
}

bool Material::getPointCloud() {
    return mat.PointCloud;
}

void Material::setPointCloud(bool i) {
    mat.PointCloud = i;
}

float Material::getShine() {
    return mat.Shininess;
}

void Material::setShine(float i) {
    mat.Shininess = i;
}

bool Material::getBiFilter() {
    return mat.getFlag(irr::video::EMF_BILINEAR_FILTER);
}

void Material::setBiFilter(bool i) {
    mat.setFlag(irr::video::EMF_BILINEAR_FILTER, i);
}

bool Material::getTriFilter() {
    return mat.getFlag(irr::video::EMF_TRILINEAR_FILTER);
}

void Material::setTriFilter(bool i) {
    mat.setFlag(irr::video::EMF_TRILINEAR_FILTER, i);
}

bool Material::getAnsiFilter() {
    return mat.getFlag(irr::video::EMF_ANISOTROPIC_FILTER);
}

void Material::setAnsiFilter(bool i) {
    mat.setFlag(irr::video::EMF_ANISOTROPIC_FILTER, i);
}

Vector2D Material::getPan(int i) {
    if (i < 0 || i >= irr::video::MATERIAL_MAX_TEXTURES) {
        return Vector2D();
    }
    auto& matT = mat.getTextureMatrix(i);
    return Vector2D(matT.getTranslation().X, matT.getTranslation().Y);
}

void Material::setPan(Vector2D scroll, int i) {
    if (i < 0 || i >= irr::video::MATERIAL_MAX_TEXTURES) return;
    irr::core::matrix4 matT = mat.getTextureMatrix(i);
    matT.setTextureTranslate(scroll.x, scroll.y);
    mat.setTextureMatrix(i, matT);
}

int Material::getWrapU(int layer) {
    return mat.TextureLayer[layer].TextureWrapU;
}

void Material::setWrapU(int layer, int i) {
    mat.TextureLayer[layer].TextureWrapU = i;
}

int Material::getWrapV(int layer) {
    return mat.TextureLayer[layer].TextureWrapV;
}

void Material::setWrapV(int layer, int i) {
    mat.TextureLayer[layer].TextureWrapV = i;
}

int Material::getWrapUV(int layer) {
    return mat.TextureLayer[layer].TextureWrapU;
}

void Material::setWrapUV(int layer, int i) {
    mat.TextureLayer[layer].TextureWrapU = i;
    mat.TextureLayer[layer].TextureWrapV = i;
}

int Material::getOpacity() {
    return mat.DiffuseColor.getAlpha();
}

void Material::setOpacity(int i) {
    i = std::clamp(i, 0, 255);
    mat.DiffuseColor.setAlpha(i);
    mat.AmbientColor.setAlpha(i);
    mat.EmissiveColor.setAlpha(i);
    mat.SpecularColor.setAlpha(i);
}

bool Material::getMipMaps() {
    return mat.UseMipMaps;
}

void Material::setMipMaps(bool i) {
    mat.UseMipMaps = i;
}

bool Material::getTexture(int slot) {
    return mat.getTexture(slot) != nullptr;
}

float Material::getTypeParam(int i) {
    return i == 0 ? mat.MaterialTypeParam : mat.MaterialTypeParam2;
}

void Material::setTypeParam(float f, int i) {
    if (i == 0) mat.MaterialTypeParam = f; else mat.MaterialTypeParam2 = f;
}

void Material::setTexture(Texture& tex, int slot) {
    if (tex.texture) {
        mat.setTexture(slot, tex.texture);
    }
}

void Material::setTextureScale(Vector2D scale, int i) {
    if (i < 0 || i >= irr::video::MATERIAL_MAX_TEXTURES) return;
    irr::core::matrix4 matT = mat.getTextureMatrix(i);
    matT.setTextureScale(scale.x, scale.y);
    mat.setTextureMatrix(i, matT);
}

Vector2D Material::getTextureScale(int i) {
    if (i < 0 || i >= irr::video::MATERIAL_MAX_TEXTURES) return Vector2D();
    auto& matT = mat.getTextureMatrix(i);
    return Vector2D(matT.getScale().X, matT.getScale().Y);
}

void Material::setMaterialFlag(int i, bool enable) {
    mat.setFlag((irr::video::E_MATERIAL_FLAG)i, enable);
}

void Material::toShader(std::string vs, std::string ps) {
    if (!driver || !gpu) return;
    if (driver->getDriverType() != EDT_DIRECT3D9 && driver->getDriverType() != EDT_OPENGL) return;

    io::path vsPath = vs.c_str();
    bool hasPS = !ps.empty();
    io::path psPath = hasPS ? io::path(ps.c_str()) : io::path("");

    ShaderCallback* cb = new ShaderCallback(irrHandler->useHighLevelShaders, irrHandler->useCGShaders, device);

    s32 mt = -1;
    if (irrHandler->useHighLevelShaders)
        mt = gpu->addHighLevelShaderMaterialFromFiles(vsPath, "vertexMain", EVST_VS_1_1, psPath, hasPS ? "pixelMain" : nullptr, EPST_PS_1_1, cb, mat.MaterialType, 0, irrHandler->useCGShaders ? EGSL_CG : EGSL_DEFAULT);
    else
        mt = gpu->addShaderMaterialFromFiles(vsPath, psPath, cb, mat.MaterialType, 0);

    mat.MaterialType = (E_MATERIAL_TYPE)mt;

    cb->drop();
}

void Material::toShaderSingle(std::string vs) {
    if (!irrHandler->useHighLevelShaders) return;
    toShader(vs, vs);
}

void bindMaterial() {
    sol::usertype<Material> bindType = lua->new_usertype<Material>("Material",
        sol::constructors<Material(), Material(int type), Material(const Texture& tex), Material(const Texture& tex, int type), Material(const Material & other)>(),

        "type", sol::property(&Material::getMaterialType, &Material::setMaterialType),
        "fog", sol::property(&Material::getFog, &Material::setFog),
        "shaderParameter", sol::property(&Material::getTypeParam, &Material::setTypeParam),
        "backfaceCulling", sol::property(&Material::getBackface, &Material::setBackface),
        "frontfaceCulling", sol::property(&Material::getFrontface, &Material::setFrontface),
        "antiAliasing", sol::property(&Material::getAntiAliasing, &Material::setAntiAliasing),
        "wireframe", sol::property(&Material::getWireframe, &Material::setWireframe),
        "lighting", sol::property(&Material::getLighting, &Material::setLighting),
        "diffuseColor", sol::property(&Material::getDiffuseColor, &Material::setDiffuseColor),
        "ambientColor", sol::property(&Material::getAmbientColor, &Material::setAmbientColor),
        "emissiveColor", sol::property(&Material::getEmissiveColor, &Material::setEmissiveColor),
        "specularColor", sol::property(&Material::getSpecularColor, &Material::setSpecularColor),
        "gouradShading", sol::property(&Material::getGourad, &Material::setGourad),
        "zWrite", sol::property(&Material::getZWrite, &Material::setZWrite),
        "zComparison", sol::property(&Material::getZComparison, &Material::setZComparison),
        "pointCloud", sol::property(&Material::getPointCloud, &Material::setPointCloud),
        "bilinearFiltering", sol::property(&Material::getBiFilter, &Material::setBiFilter),
        "trilinearFiltering", sol::property(&Material::getTriFilter, &Material::setTriFilter),
        "anisotropicFiltering", sol::property(&Material::getAnsiFilter, &Material::setAnsiFilter),
        "shininess", sol::property(&Material::getShine, &Material::setShine),
        "opacity", sol::property(&Material::getOpacity, &Material::setOpacity),
        "mipmaps", sol::property(&Material::getMipMaps, &Material::setMipMaps),
        "textureWrap", sol::property(&Material::getWrapUV, &Material::setWrapUV),
        "textureWrapU", sol::property(&Material::getWrapU, &Material::setWrapU),
        "textureWrapV", sol::property(&Material::getWrapV, &Material::setWrapV),
        "ID", sol::property(&Material::getID, &Material::setID)
    );

    bindType["getTextureTranslation"] = &Material::getPan;
    bindType["setTextureTranslation"] = &Material::setPan;
    bindType["setTextureUVWrapping"] = &Material::setWrapUV;
    bindType["setTextureUWrapping"] = &Material::setWrapU;
    bindType["setTextureVWrapping"] = &Material::setWrapV;
    bindType["setTexture"] = &Material::setTexture;

    bindType["setTextureScale"] = &Material::setTextureScale;
    bindType["getTextureScale"] = &Material::setTextureScale;

    bindType["setMaterialFlag"] = &Material::setMaterialFlag;

    bindType["toShader"] = &Material::toShader;
    bindType["toShaderHLSL"] = &Material::toShaderSingle;
}