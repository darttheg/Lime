#include "Image2D.h"

Image2D::Image2D() {
}

Image2D::Image2D(const Texture& tex) {
	img = guienv->addImage(tex.texture, irr::core::vector2di(0, 0));
}

Image2D::Image2D(const Texture& tex, const Vector2D& pos) {
	img = guienv->addImage(tex.texture, irr::core::vector2di(pos.x, pos.y));
}

Image2D::Image2D(const Texture& tex, const Vector2D& pos, const Vector2D& dimensions) : Image2D(tex, pos) {
	setSize(dimensions);
}

Image2D::Image2D(const Image2D& other) {
	img = other.img;
}

Vector4D Image2D::getColor() {
	return img ? Vector4D(img->getColor().getRed(), img->getColor().getGreen(), img->getColor().getBlue(), img->getColor().getAlpha()) : Vector4D();
}

void Image2D::setColor(Vector4D& color) {
	if (img)
		img->setColor(irr::video::SColor(color.w, color.x, color.y, color.z));
}

bool Image2D::getVisible() {
	return img ? img->isVisible() : false;
}

void Image2D::setVisible(bool vis) {
	if (img)
		img->setVisible(vis);
}

Vector2D Image2D::getPosition() {
	if (img)
		return Vector2D(img->getRelativePosition().UpperLeftCorner.X, img->getRelativePosition().UpperLeftCorner.Y);
	return Vector2D();
}

void Image2D::setPosition(const Vector2D& pos) {
	if (img)
		img->setRelativePosition(irr::core::position2di(pos.x, pos.y));
}

Vector2D Image2D::getSize() {
	if (img) {
		irr::core::recti r = img->getRelativePosition();
		return Vector2D(r.LowerRightCorner.X - r.UpperLeftCorner.X, r.LowerRightCorner.Y - r.UpperLeftCorner.Y);
	}
	return Vector2D();
}

void Image2D::setSize(const Vector2D& size) {
	if (img) {
		irr::core::recti r = img->getRelativePosition();
		r.LowerRightCorner.X = r.UpperLeftCorner.X + size.x;
		r.LowerRightCorner.Y = r.UpperLeftCorner.Y + size.y;
		img->setRelativePosition(r);
		if (button)
			updateButton();
	}
}

void Image2D::setImage(const Texture& tex) {
	if (img)
		img->setImage(tex.texture);
}

void Image2D::destroy() {
	if (img)
		img->remove();
}

void Image2D::bringToFront() {
	if (img)
		img->bringToFront(img);
}

void Image2D::sendToBack() {
	if (img)
		img->sendToBack(img);
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

void Image2D::setMaxSize(const Vector2D& max) {
	if (img)
		img->setMaxSize(irr::core::dimension2du(max.x, max.y));
}

bool Image2D::getEnabled() {
	return img ? img->isEnabled() : false;
}

void Image2D::setEnabled(bool enable) {
	if (img)
		img->setEnabled(enable);
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

		"position", sol::property(&Image2D::getPosition, &Image2D::setPosition),
		"visible", sol::property(&Image2D::getVisible, &Image2D::setVisible),
		"size", sol::property(&Image2D::getSize, &Image2D::setSize),
		"enabled", sol::property(&Image2D::getEnabled, &Image2D::setEnabled),
		"useAlpha", sol::property(&Image2D::getUseAlpha, &Image2D::setUseAlpha),
		"scaleToFit", sol::property(&Image2D::scalesToFit, &Image2D::setScalesToFit),
		"color", sol::property(&Image2D::getColor, &Image2D::setColor)
	);

	bind_type["destroy"] = &Image2D::destroy;
	bind_type["load"] = &Image2D::setImage;
	bind_type["setMaxSize"] = &Image2D::setMaxSize;
	bind_type["toFront"] = &Image2D::bringToFront;
	bind_type["toBack"] = &Image2D::sendToBack;
	bind_type["setBorderAlignment"] = &Image2D::setBorderAlignment;
}