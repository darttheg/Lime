#pragma once

#include "irrlicht.h"
#include "Texture.h"
#include <string>
#include <vector>

class TextLine
{
public:
	Line* myLine;
	float lifetime;

	TextLine() : lifetime(-1) {
		myLine = new Line();
	}

	TextLine(const std::string& font) : lifetime(-1) {
		myLine = new Line();
		setFont(font);
	}

	~TextLine() {
		destroy();
	}

	void destroy() {
		if (myLine)
			delete myLine;
		lifetime = -1;
	}

	void clear() {
		if (myLine)
			myLine->setStrings(std::vector<irr::core::stringw>());
	}

	void addString(const std::string& text, const Vector3D& color) {
		myLine->addString(text.c_str(), irr::video::SColor(255, color.x, color.y, color.z));
	}

	void setStrings(sol::table tbl) {
		std::vector<irr::core::stringw> stringVector;

		for (auto& pair : tbl) {
			sol::object key = pair.first;
			sol::object value = pair.second;

			if (value.is<std::string>()) {
				std::string luaString = value.as<std::string>();
				stringVector.push_back(irr::core::stringw(luaString.c_str()));
			}
		}

		myLine->setStrings(stringVector);
	}

	void setImages(sol::table tbl) {
		std::vector<irr::video::ITexture*> imageVector;

		for (auto& pair : tbl) {
			sol::object key = pair.first;
			sol::object value = pair.second;

			if (value.is<Texture>()) {
				imageVector.push_back(value.as<irr::video::ITexture*>());
			}
		}

		myLine->setImages(imageVector);
	}

	void addImage(const Texture& tex) {
		if (tex.texture)
			myLine->addImage(tex.texture);
	}

	bool setFont(const std::string& fontName) {
		auto it = fontCache.find(fontName);
		if (it != fontCache.end()) {
			myLine->setFont(it->second);
			return true;
		}
		return false;
	}

	float getLifetime() {
		return lifetime;
	}

	void setLifetime(float life) {
		lifetime = life;
	}
};

void bindLine() {
	sol::usertype<TextLine> bind_type = lua->new_usertype<TextLine>("Line",
		sol::constructors<TextLine(), TextLine(const std::string& font)>(),

		"life", sol::property(&TextLine::getLifetime, &TextLine::setLifetime)
	);

	bind_type["clear"] = &TextLine::clear;
	bind_type["addString"] = &TextLine::addString;
	bind_type["addImage"] = &TextLine::addImage;
	bind_type["setStrings"] = &TextLine::setStrings;
	bind_type["setImages"] = &TextLine::setImages;
	bind_type["setFont"] = &TextLine::setFont;
}