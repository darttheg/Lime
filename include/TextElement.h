#pragma once

#include "irrlicht.h"
#include "TextLine.h"
#include "Vector2D.h"
#include "TextLine.h"
#include <string>
#include <vector>

class TextElement
{
public:
	TextArea* element;
	int anchor = 0;

	TextElement() {
        element = new TextArea(guienv, nullptr, -1);
	}
	TextElement(const TextElement& other) {
        element = other.element;
        anchor = other.anchor;
	}

    ~TextElement() = default;

    std::string getToolTip() {
        if (element) {
            irr::core::stringw wideTip = element->getToolTipText();
            return std::string(wideTip.c_str(), wideTip.c_str() + wideTip.size());
        }
        return "";
    }

    void setToolTip(const std::string& tip) {
        if (element) {
            std::wstring wideTip(tip.begin(), tip.end());
            element->setToolTipText(wideTip.c_str());
        }
    }

	Vector2D getDimensions() {
		if (element)
			return Vector2D(element->getDimension().getWidth(), element->getDimension().getHeight());
		return Vector2D();
	}

	void setDimensions(const Vector2D& dim) {
		if (element)
			element->setDimension(irr::core::recti(0, 0, static_cast<int>(dim.x), static_cast<int>(dim.y)));
	}

	void setMaxLines(int i) {
		if (element)
			return element->setMaxLines(i);
	}

    void addLine(const TextLine& line) {
        if (element)
            element->addLine(line.myLine);
    }

    void addLines(const sol::table& lines) {
        if (element) {
            for (auto& pair : lines) {
                auto line = pair.second.as<TextLine>();
                element->addLine(line.myLine);
            }
        }
    }

    void clear() {
        if (element)
            element->clear();
    }

    int getNumLines() {
        if (element)
            return element->getNumberOfLines();
        return 0;
    }

	int getAlignment() {
        return anchor;
	}

	void setAlignment(int i) {
		if (element)
			element->setAlignment(static_cast<TextArea::Alignment>(i));
	}

    int getID() {
        if (element)
            return element->getID();
        return -1;
    }

	void setID(int i) {
		if (element)
			element->setID(i);
	}

	void setParent(const TextElement& other) {
		if (element && other.element)
			other.element->addChild(element);
	}

	bool removeChild(int id) {
		if (element) {
			auto children = element->getChildren();
			for (auto c = children.begin(); c != children.end(); ++c) {
				if ((*c)->getID() == id) {
					element->removeChild(*c);
					return true;
				}
			}
		}
		return false;
	}

    Vector2D getPosition() {
        if (element)
            return Vector2D(element->getPosition().X, element->getPosition().Y);
        return Vector2D();
    }

    void setPosition(const Vector2D& pos) {
        if (element) {
            element->setPosition(irr::core::vector2di(pos.x, pos.y));
        }
    }

    void setAnchor(int i) {
        anchor = irr::core::clamp<int>(i, 0, 8);
    }
};  

void bindText() {
	sol::usertype<TextElement> bind_type = lua->new_usertype<TextElement>("TextBox",
		sol::constructors<TextElement(), TextElement(const TextElement& other)>(),

        "position", sol::property(&TextElement::getPosition, &TextElement::setPosition),
        "dimensions", sol::property(&TextElement::getDimensions, &TextElement::setDimensions),
        "alignment", sol::property(&TextElement::getAlignment, &TextElement::setAlignment),
        "ID", sol::property(&TextElement::getID, &TextElement::setID),
        "toolTip", sol::property(&TextElement::getToolTip, &TextElement::setToolTip)
    );

    bind_type["setAnchor"] = &TextElement::setAnchor;
    bind_type["clear"] = &TextElement::clear;
    bind_type["addLine"] = &TextElement::addLine;
    bind_type["addLines"] = &TextElement::addLines;
    bind_type["getLineCount"] = &TextElement::getNumLines;
    bind_type["setMaxLines"] = &TextElement::setMaxLines;
    bind_type["setParent"] = &TextElement::setParent;
    bind_type["removeChildID"] = &TextElement::removeChild;
}