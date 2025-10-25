#include "Text3D.h"

wchar_t* Text3D::charToWchar(const char* str) {
	irr::core::stringw wideStr = irr::core::stringw(str);
	return const_cast<wchar_t*>(wideStr.c_str());
}

Text3D::Text3D(const std::string& tx, const Vector3D& pos, const std::string& fontName) {
	irr::gui::IGUIFont* f = guienv->getBuiltInFont();
	if (defaultFont != "")
		f = fontCache[defaultFont];
	if (fontName != "") {
		f = fontCache[fontName];
	}

	/*
	text = smgr->addTextSceneNode(f, L"Text", color);

	myText = tx;
	setColor(col);
	setPosition(pos);
	setText(myText);
	text->grab();
	*/

	text = addColoredText3D(smgr, charToWchar(tx.c_str()), vector3df(pos.getX(), pos.getY(), pos.getZ()));
	setFont(fontName);
}

/*
Text3D::Text3D() : Text3D("Text", Vector3D(), Vector4D(255, 255, 255, 2555), "") {}
Text3D::Text3D(const std::string& tx) : Text3D(tx, Vector3D(), Vector4D(255, 255, 255, 255), "") {}
Text3D::Text3D(const std::string& tx, const Vector3D& pos) : Text3D(tx, pos, Vector4D(255, 255, 255, 255), "") {}
Text3D::Text3D(const std::string& tx, const Vector3D& pos, const Vector4D& col) : Text3D(tx, pos, col, "") {}
Text3D::Text3D(const std::string& tx, const std::string& fontName) : Text3D(tx, Vector3D(), Vector4D(255, 255, 255, 255), fontName) {}
*/

Text3D::Text3D() : Text3D("Text", Vector3D(), "") {}
Text3D::Text3D(const std::string& tx) : Text3D(tx, Vector3D(), "") {}
Text3D::Text3D(const std::string& tx, const std::string& fontName) {}
Text3D::Text3D(const std::string& tx, const Vector3D& pos) : Text3D(tx, pos, "") {}

void Text3D::destroy() {
	destroyEntry();
	smgr->addToDeletionQueue(text);
}

std::string Text3D::getText() {
	if (text)
		return myText;
	return "";
}

void Text3D::setText(const std::string& tx) {
	if (text) {
		myText = tx;
		std::wstring wideText(tx.begin(), tx.end());
		text->setText(wideText.c_str());
	}
}

void Text3D::setAlignment(int h, int v) {
	if (text) text->setAlignment((CColoredText3DSceneNode::EHAlign)h, (CColoredText3DSceneNode::EVAlign)v);
}

/*Vector4D Text3D::getColor() {
	if (text)
		return Vector4D(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());
	return Vector4D();
}

void Text3D::setColor(const Vector4D& col) {
	if (text) {
		color = irr::video::SColor(col.getW(), col.getX(), col.getY(), col.getZ());
		updateColor();
	}
}

void Text3D::updateColor() {
	text->setTextColor(color);
}*/

bool Text3D::setFont(const std::string& fontName) {
	if (fontCache[fontName]) {
		// Remove previous text object, replace it fully
		irr::core::vector3df pos = text->getPosition();
		irr::scene::ISceneNode* par = text->getParent();
		bool visible = text->isVisible();
		std::wstring wideText(myText.begin(), myText.end());
		text->remove();

		bool found = false;
		irr::gui::IGUIFont* f = guienv->getBuiltInFont();
		auto it = fontCache.find(fontName);
		if (it != fontCache.end()) {
			f = it->second;
			found = true;
		}

		/*text = smgr->addTextSceneNode(f, wideText.c_str(), color, par, pos);
		text->grab();*/
		text->setFont(f);

		return found;
	}
	return false;
}

float Text3D::getOpacity() {
	return text ? text->getOpacity() : 0.0f;
}

void Text3D::setOpacity(float f) {
	if (text) text->setOpacity(f);
}

void bindText3D() {
	sol::usertype<Text3D> bindType = lua->new_usertype<Text3D>("Text3D",
		// sol::constructors <Text3D(const std::string & tx, const std::string & fontName), Text3D(const std::string & tx, const Vector3D & pos, const Vector4D & col), Text3D(const std::string & tx), Text3D(const std::string & tx, const Vector3D & pos), Text3D()>(),
		sol::constructors <Text3D(), Text3D(const std::string &tx), Text3D(const std::string &tx, const std::string &fontName), Text3D(const std::string &tx, const Vector3D &pos), Text3D(const std::string &tx, const Vector3D &pos), Text3D()>(),

		sol::base_classes, sol::bases<Compatible3D>(),
		sol::meta_function::type, [](const Text3D&) { return "Text3D"; },

		// "textColor", sol::property(&Text3D::getColor, &Text3D::setColor),
		"text", sol::property(&Text3D::getText, &Text3D::setText),
		"opacity", sol::property(&Text3D::getOpacity, &Text3D::setOpacity),
		"shadow", sol::property(&Text3D::getShadows, &Text3D::setShadows)
	);

	bindType["setParent"] = &Text3D::setParent;
	bindType["setFont"] = &Text3D::setFont;
	bindType["setAlignment"] = &Text3D::setAlignment;
}