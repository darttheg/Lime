#include "Texture.h"

Texture::Texture() : path("") {
	texture = driver->addTexture(irr::core::dimension2du(16, 16), "");
}

Texture::Texture(const Vector2D& size) : path("") {
	texture = driver->addTexture(irr::core::dimension2du(size.x, size.y), "");
}

Texture::Texture(const std::string imgpath) : path(imgpath) {
	if (!path.empty()) {
		load(imgpath);
	}
}

bool Texture::load(const std::string& imgpath) {
	if (!driver) {
		// For Lime.SetWindowIcon
		path = imgpath;
		return false;
	}
	if (!imgpath.empty()) {
		texture = driver->getTexture(imgpath.c_str());
		if (!texture) return false;
		path = imgpath;
		return true;
	}
	return false;
}

void Texture::createEmpty(const Vector2D& size) {
	if (texture) texture->drop();

	texture = driver->addTexture(irr::core::dimension2du(size.x, size.y), "");
}

std::string Texture::getPath() const {
	return path;
}

void Texture::keyColor(const Vector2D& pos) {
	if (texture)
		driver->makeColorKeyTexture(texture, core::position2d<s32>(pos.x, pos.y));
}

void Texture::saveTexture(std::string path) {
	irrHandler->doWriteTextureThreaded(texture, path);
}

bool Texture::append(const Texture& tex, const Vector2D& pos) {
	if (!tex.texture) return false;
	if (!texture) {
		texture = tex.texture;
		return texture != nullptr;
	}

	IImage* img = texToImg(tex.texture);
	return doAppend(img, vector2di(pos.x, pos.y));
}

bool Texture::appendFromFile(std::string filePath, const Vector2D& pos) {
	if (!texture) {
		texture = driver->getTexture(filePath.c_str());
		return texture != nullptr;
	}

	IImage* img = driver->createImageFromFile(filePath.c_str());
	return doAppend(img, vector2di(pos.x, pos.y));
}

Vector4D Texture::getPixel(const Vector2D& pos) {
	if (!texture) return Vector4D();

	IImage* img = texToImg(texture);
	if (!img) return Vector4D();

	SColor pCol = img->getPixel(pos.x, pos.y);

	return Vector4D(pCol.getRed(), pCol.getGreen(), pCol.getBlue(), pCol.getAlpha());
}

Texture Texture::crop(const Vector2D& topL, const Vector2D& bottomR) {
	if (!texture) return Texture();

	if (topL.x < 0 || topL.y < 0 || bottomR.x < 0 || bottomR.y < 0) return Texture();

	position2di pos = position2di(topL.x, topL.y);
	dimension2du dim = dimension2du(bottomR.x - topL.x, bottomR.y - topL.y);

	irr::video::IImage* img = driver->createImage(texture, pos, dim);

	ITexture* outRaw = driver->addTexture("cropped", img);
	img->drop();

	Texture out = Texture();
	out.texture = outRaw;

	return out;
}

bool Texture::doAppend(irr::video::IImage* img, vector2di pos) {
	if (!texture || !img) return false;

	IImage* base = texToImg(texture);
	img->copyTo(base, pos);

	img->drop();
	texture->drop();
	texture = driver->addTexture("", base);
	base->drop();

	return texture != nullptr;
}

irr::video::IImage* Texture::texToImg(irr::video::ITexture* tex) {
	if (!tex) return nullptr;

	core::dimension2d<u32> texSize = texture->getSize();
	video::ECOLOR_FORMAT format = texture->getColorFormat();

	void* pixelData = texture->lock();
	if (!pixelData) return nullptr;

	video::IImage* image = driver->createImageFromData(format, texSize, pixelData);

	texture->unlock();

	return image;
}

void bindTexture() {
	sol::usertype<Texture> bindType = lua->new_usertype<Texture>("Texture",
		sol::constructors<Texture(), Texture(const Vector2D& size), Texture(std::string imgpath)>()
	);
	bindType["crop"] = &Texture::crop;
	bindType["load"] = &Texture::load;
	bindType["toStr"] = &Texture::getPath;
	bindType["keyColor"] = &Texture::keyColor;
	bindType["save"] = &Texture::saveTexture;
	bindType["append"] = &Texture::append;
	bindType["appendFromFile"] = &Texture::appendFromFile;
	bindType["clear"] = &Texture::createEmpty;
	bindType["getPixelColor"] = &Texture::getPixel;
}