#include "Text2D.h"

Text2D::Text2D() : Text2D("Text", Vector2D(0, 0), Vector2D(256, 16)) {}

Text2D::Text2D(std::string tx, std::string fontName) : Text2D(tx, Vector2D(0, 0), Vector2D(256, 16)) {
	setFont(fontName);
}

Text2D::Text2D(std::string tx) : Text2D(tx, Vector2D(0, 0), Vector2D(256, 16)) {}

Text2D::Text2D(std::string tx, const Vector2D& pos) : Text2D(tx, pos, Vector2D(256, 16)) {}

Text2D::Text2D(std::string tx, const Vector2D& pos, const Vector2D& dimensions) {
	text = addColoredText(guienv, charToWchar(tx.c_str()), irr::core::recti(irr::core::vector2di(pos.getX(), pos.getY()), irr::core::vector2di(pos.getX() + dimensions.getX(), pos.getY() + dimensions.getY())));
	// text = guienv->addStaticText(charToWchar(tx.c_str()), irr::core::recti(irr::core::vector2di(pos.getX(), pos.getY()), irr::core::vector2di(pos.getX() + dimensions.getX(), pos.getY() + dimensions.getY())));
	// text->setBackgroundColor(irr::video::SColor(0, 180, 180, 180));
}

Text2D::Text2D(const Text2D& other) {
	text = other.text;
}

wchar_t* Text2D::charToWchar(const char* str) {
	irr::core::stringw wideStr = irr::core::stringw(str);
	return const_cast<wchar_t*>(wideStr.c_str());
}

std::string Text2D::getText() {
	if (text) {
		irr::core::stringw wideStr(text->getText());
		irr::core::stringc narrowStr(wideStr);
		return narrowStr.c_str();
	}

	return "";
}

void Text2D::setText(std::string tx) {
	if (text)
		text->setText(charToWchar(tx.c_str()));
}

void Text2D::destroy() {
	if (text) {
		Compatible2D::clean();
		text->remove();
	}
}

void Text2D::setBorderAlignment(int a, int b, int c, int d) {
	if (text)
		text->setAlignment((irr::gui::EGUI_ALIGNMENT)a, (irr::gui::EGUI_ALIGNMENT)b, (irr::gui::EGUI_ALIGNMENT)c, (irr::gui::EGUI_ALIGNMENT)d);
}

void Text2D::setTextAlignment(int h, int v) {
	if (text)
		text->setTextAlignment((irr::gui::EGUI_ALIGNMENT)h, (irr::gui::EGUI_ALIGNMENT)v);
}

float Text2D::getOpacity() {
	return text ? text->getOpacity() : 0.0f;
}

void Text2D::setOpacity(float f) {
	if (text) text->setOpacity(f);
}

/*bool Text2D::getWrap() {
	return text ? text->isWordWrapEnabled() : false;
}

void Text2D::setWrap(bool wrap) {
	if (text)
		text->setWordWrap(wrap);
}

bool Text2D::getDrawBackground() {
	return text ? text->isDrawBackgroundEnabled() : false;
}

void Text2D::setDrawBackground(bool draw) {
	if (text)
		text->setDrawBackground(draw);
}

Vector4D Text2D::getBackgroundColor() {
	return text ? Vector4D(text->getBackgroundColor().getRed(), text->getBackgroundColor().getGreen(), text->getBackgroundColor().getBlue(), text->getBackgroundColor().getAlpha()) : Vector4D();
}

void Text2D::setBackgroundColor(const Vector4D& col) {
	if (text)
		text->setBackgroundColor(irr::video::SColor(col.getW(), col.getX(), col.getY(), col.getZ()));
}

Vector4D Text2D::getTextColor() {
	return text ? Vector4D(text->getOverrideColor().getRed(), text->getOverrideColor().getGreen(), text->getOverrideColor().getBlue(), text->getOverrideColor().getAlpha()) : Vector4D();
}

void Text2D::setTextColor(const Vector4D& col) {
	if (text)
		text->setOverrideColor(irr::video::SColor(col.getW(), col.getX(), col.getY(), col.getZ()));
}*/

bool Text2D::setFont(const std::string& fontName) {
	if (!text)
		return false;
	if (fontCache[fontName]) {
		irr::gui::IGUIFont* f = guienv->getBuiltInFont();
		auto it = fontCache.find(fontName);
		if (it != fontCache.end()) {
			f = it->second;
			text->setOverrideFont(f);
			return true;
		}
		text->setOverrideFont(f);
		return false;
	}
	return false;
}

void Text2D::setParent(const Image2D& other) {
	if (text) {
		other.img->addChild(text);
	}
}

/*bool Text2D::getDrawBorder() {
	return text ? text->isDrawBorderEnabled() : false;
}

void Text2D::setDrawBorder(bool enable) {
	if (text)
		text->setDrawBorder(enable);
}*/


void bindText2D() {
	sol::usertype<Text2D> bindType = lua->new_usertype<Text2D>("Text2D",
		sol::constructors <Text2D(), Text2D(std::string tx), Text2D(std::string tx, std::string fontName), Text2D(std::string tx, const Vector2D & pos), Text2D(std::string tx, const Vector2D & pos, const Vector2D & dimensions)>(),

		sol::base_classes, sol::bases<Compatible2D>(),
		sol::meta_function::type, [](const Text2D&) { return "Text2D"; },

		"text", sol::property(&Text2D::getText, &Text2D::setText),
		"opacity", sol::property(&Text2D::getOpacity, &Text2D::setOpacity),
		"shadow", sol::property(&Text2D::getShadows, &Text2D::setShadows)

		/*"wrap", sol::property(&Text2D::getWrap, &Text2D::setWrap),

		"backgroundColor", sol::property(
			[](Text2D& c) { return Vector4D{ [&] { return c.getBackgroundColor(); }, [&](auto v) { c.setBackgroundColor(v); } }; },
			[](Text2D& c, const Vector4D& v) { c.setBackgroundColor(v); }
		),
		"textColor", sol::property(
			[](Text2D& c) { return Vector4D{ [&] { return c.getTextColor(); }, [&](auto v) { c.setTextColor(v); } }; },
			[](Text2D& c, const Vector4D& v) { c.setTextColor(v); }
		),
		"drawBorder", sol::property(&Text2D::getDrawBorder, &Text2D::setDrawBorder)*/
	);

	bindType["setFont"] = &Text2D::setFont;
	bindType["setBorderAlignment"] = &Text2D::setBorderAlignment;
	bindType["setAlignment"] = &Text2D::setTextAlignment; // Use GUI_ALIGNMENT
}