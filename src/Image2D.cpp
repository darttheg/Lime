#include "Image2D.h"

Image2D::Image2D() {
}

Image2D::Image2D(const Texture& tex) : Image2D(tex, Vector2D(0,0)) {
}

Image2D::Image2D(const Texture& tex, const Vector2D& pos) : Image2D(tex, pos, Vector2D(tex.texture->getSize().Width, tex.texture->getSize().Height)) {
}

Image2D::Image2D(const Texture& tex, const Vector2D& pos, const Vector2D& dimensions) {
	img = guienv->addImage(tex.texture, irr::core::vector2di(pos.x, pos.y));
}

Image2D::Image2D(const Image2D& other) {
	img = other.img; // Remove?
}

Vector4D Image2D::getColor() {
	return img ? Vector4D(img->getColor().getRed(), img->getColor().getGreen(), img->getColor().getBlue(), img->getColor().getAlpha()) : Vector4D();
}

void Image2D::setColor(Vector4D& color) {
	if (img)
		img->setColor(irr::video::SColor(color.w, color.x, color.y, color.z));
}

void Image2D::setImage(const Texture& tex) {
	if (img) {
		img->setImage(tex.texture);
		Compatible2D::setSize(Vector2D(tex.texture->getSize().Width, tex.texture->getSize().Height));
	}
}

void Image2D::destroy() {
	if (img) {
		Compatible2D::clean();
		img->remove();
	}
}

bool Image2D::scalesToFit() {
	if (img)
		return img->isImageScaled();
	return false;
}

void Image2D::setScalesToFit(bool scale) {
	if (img)
		img->setScaleImage(scale);
}

void Image2D::setBorderAlignment(int a, int b, int c, int d) {
	if (img)
		img->setAlignment((irr::gui::EGUI_ALIGNMENT)a, (irr::gui::EGUI_ALIGNMENT)b, (irr::gui::EGUI_ALIGNMENT)c, (irr::gui::EGUI_ALIGNMENT)d);
}

bool Image2D::getUseAlpha() {
	return img ? img->isAlphaChannelUsed() : false;
}

void Image2D::setUseAlpha(bool enable) {
	if (img)
		img->setUseAlphaChannel(enable);
}

void Image2D::setParent(const Image2D& other) {
	if (img)
		other.img->addChild(img);
}

void bindImage2D() {
	sol::usertype<Image2D> bind_type = lua->new_usertype<Image2D>("Image2D",
		sol::constructors <Image2D(const Texture & tex), Image2D(const Texture & tex, const Vector2D & pos), Image2D(const Texture & tex, const Vector2D & pos, const Vector2D & dimensions), Image2D(const Image2D & other)>(),

		sol::base_classes, sol::bases<Compatible2D>(),

		"useAlpha", sol::property(&Image2D::getUseAlpha, &Image2D::setUseAlpha),
		"scaleToFit", sol::property(&Image2D::scalesToFit, &Image2D::setScalesToFit),
		"color", sol::property(&Image2D::getColor, &Image2D::setColor)
	);

	bind_type["destroy"] = &Image2D::destroy;
	bind_type["load"] = &Image2D::setImage;
	bind_type["setBorderAlignment"] = &Image2D::setBorderAlignment;
}