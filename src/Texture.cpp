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

Texture::Texture(const std::string imgpath, sol::table options) : path(imgpath) {
	// Works but driver doesn't apply these changes when creating a texture, so useless.
	if (!path.empty()) {
		// Parse options
		if (!driver) return;

		bool pow2 = driver->getTextureCreationFlag(ETCF_ALLOW_NON_POWER_2);
		bool always16 = driver->getTextureCreationFlag(ETCF_ALWAYS_16_BIT);
		bool always32 = driver->getTextureCreationFlag(ETCF_ALWAYS_32_BIT);
		bool mipmaps = driver->getTextureCreationFlag(ETCF_CREATE_MIP_MAPS);
		bool noalpha = driver->getTextureCreationFlag(ETCF_NO_ALPHA_CHANNEL);
		bool quality = driver->getTextureCreationFlag(ETCF_OPTIMIZED_FOR_QUALITY);
		bool speed = driver->getTextureCreationFlag(ETCF_OPTIMIZED_FOR_SPEED);

		if (options["BitDepth"].valid() && options["BitDepth"].is<int>()) {
			sol::object obj = options["BitDepth"];
			if (obj.as<int>() == 0) {
				driver->setTextureCreationFlag(ETCF_ALWAYS_16_BIT, true);
				driver->setTextureCreationFlag(ETCF_ALWAYS_32_BIT, false);
			} else {
				driver->setTextureCreationFlag(ETCF_ALWAYS_16_BIT, false);
				driver->setTextureCreationFlag(ETCF_ALWAYS_32_BIT, true);
			}
		}

		if (options["Optimize"].valid() && options["Optimize"].is<int>()) {
			sol::object obj = options["Optimize"];
			if (obj.as<int>() == 0) {
				driver->setTextureCreationFlag(ETCF_OPTIMIZED_FOR_SPEED, true);
				driver->setTextureCreationFlag(ETCF_OPTIMIZED_FOR_QUALITY, false);
			}
			else {
				driver->setTextureCreationFlag(ETCF_OPTIMIZED_FOR_SPEED, false);
				driver->setTextureCreationFlag(ETCF_OPTIMIZED_FOR_QUALITY, true);
			}
		}

		if (options["AllowNonPower2"].valid() && options["AllowNonPower2"].is<bool>()) {
			sol::object obj = options["AllowNonPower2"];
			driver->setTextureCreationFlag(ETCF_ALLOW_NON_POWER_2, obj.as<bool>());
		}

		if (options["CreateMipmaps"].valid() && options["CreateMipmaps"].is<bool>()) {
			sol::object obj = options["CreateMipmaps"];
			driver->setTextureCreationFlag(ETCF_CREATE_MIP_MAPS, obj.as<bool>());
		}

		if (options["NoAlpha"].valid() && options["NoAlpha"].is<bool>()) {
			sol::object obj = options["NoAlpha"];
			driver->setTextureCreationFlag(ETCF_NO_ALPHA_CHANNEL, obj.as<bool>());
		}

		// Create with new flags
		load(imgpath);

		// Done
		driver->setTextureCreationFlag(ETCF_ALLOW_NON_POWER_2, pow2);
		driver->setTextureCreationFlag(ETCF_ALWAYS_16_BIT, always16);
		driver->setTextureCreationFlag(ETCF_ALWAYS_32_BIT, always32);
		driver->setTextureCreationFlag(ETCF_CREATE_MIP_MAPS, mipmaps);
		driver->setTextureCreationFlag(ETCF_NO_ALPHA_CHANNEL, noalpha);
		driver->setTextureCreationFlag(ETCF_OPTIMIZED_FOR_QUALITY, quality);
		driver->setTextureCreationFlag(ETCF_OPTIMIZED_FOR_SPEED, speed);
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

Texture Texture::makeNormalMap(float height) {
	if (!texture) return Texture();

	ITexture* t = driver->getTexture(path.c_str());
	driver->makeNormalMapTexture(t, height);
	Texture out = Texture();
	out.texture = t;
	return out;
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
		sol::constructors<Texture(), Texture(const Vector2D& size), Texture(std::string imgpath)/*, Texture(std::string imgpath, sol::table options)*/>()
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
	bindType["createNormalMap"] = &Texture::makeNormalMap;
}