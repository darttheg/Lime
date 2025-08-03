#include "EditBox.h"

wchar_t* EditBox::charToWchar(const char* str) {
    irr::core::stringw wideStr = irr::core::stringw(str);
    return const_cast<wchar_t*>(wideStr.c_str());
}

EditBox::EditBox() : EditBox("Edit Box", Vector2D(0, 0), Vector2D(256, 16)) {}

EditBox::EditBox(std::string tx) : EditBox(tx, Vector2D(0, 0), Vector2D(256, 16)) {}

EditBox::EditBox(std::string tx, const Vector2D& pos) : EditBox(tx, pos, Vector2D(256, 16)) {}

EditBox::EditBox(std::string tx, const Vector2D& pos, const Vector2D& dimensions) {
    box = guienv->addEditBox(charToWchar(tx.c_str()), irr::core::recti(
        irr::core::vector2di(pos.x, pos.y),
        irr::core::vector2di(pos.x + dimensions.x, pos.y + dimensions.y)
    ));
    border = false;
}

EditBox::EditBox(const EditBox& other) {
    box = other.box;
}

EditBox::~EditBox() {
    destroy();
}

std::string EditBox::getText() {
    if (box) {
        irr::core::stringw wideStr(box->getText());
        irr::core::stringc narrowStr(wideStr);
        return narrowStr.c_str();
    }
    return "";
}

void EditBox::setText(std::string tx) {
    if (box) box->setText(charToWchar(tx.c_str()));
}

void EditBox::destroy() {
    if (box) box->remove();
}

bool EditBox::getVisible() {
    return box ? box->isVisible() : false;
}

void EditBox::setVisible(bool vis) {
    if (box) box->setVisible(vis);
}

void EditBox::bringToFront() {
    if (box) box->bringToFront(box);
}

void EditBox::sendToBack() {
    if (box) box->sendToBack(box);
}

Vector2D EditBox::getSize() {
    if (box) {
        irr::core::recti r = box->getRelativePosition();
        return Vector2D(r.LowerRightCorner.X - r.UpperLeftCorner.X, r.LowerRightCorner.Y - r.UpperLeftCorner.Y);
    }
    return Vector2D();
}

void EditBox::setSize(const Vector2D& size) {
    if (box) {
        irr::core::recti r = box->getRelativePosition();
        r.LowerRightCorner.X = r.UpperLeftCorner.X + size.x;
        r.LowerRightCorner.Y = r.UpperLeftCorner.Y + size.y;
        box->setRelativePosition(r);
    }
}

void EditBox::setBorderAlignment(int a, int b, int c, int d) {
    if (box) {
        box->setAlignment((irr::gui::EGUI_ALIGNMENT)a, (irr::gui::EGUI_ALIGNMENT)b, (irr::gui::EGUI_ALIGNMENT)c, (irr::gui::EGUI_ALIGNMENT)d);
    }
}

void EditBox::setTextAlignment(const Vector2D& align) {
    if (box) {
        box->setTextAlignment((irr::gui::EGUI_ALIGNMENT)align.x, (irr::gui::EGUI_ALIGNMENT)align.y);
    }
}

void EditBox::setMaxSize(const Vector2D& max) {
    if (box) box->setMaxSize(irr::core::dimension2du(max.x, max.y));
}

bool EditBox::getWrap() {
    return box ? box->isWordWrapEnabled() : false;
}

void EditBox::setWrap(bool wrap) {
    if (box) box->setWordWrap(wrap);
}

Vector4D EditBox::getTextColor() {
    return box ? Vector4D(box->getOverrideColor().getRed(), box->getOverrideColor().getGreen(), box->getOverrideColor().getBlue(), box->getOverrideColor().getAlpha()) : Vector4D();
}

void EditBox::setTextColor(const Vector4D& col) {
    if (box) {
        box->setOverrideColor(irr::video::SColor(col.w, col.x, col.y, col.z));
    }
}

bool EditBox::setFont(const std::string& fontName) {
    if (!box) return false;
    irr::gui::IGUIFont* f = guienv->getBuiltInFont();
    if (fontCache[fontName]) {
        f = fontCache[fontName];
        box->setOverrideFont(f);
        return true;
    }
    box->setOverrideFont(f);
    return false;
}

Vector2D EditBox::getPosition() {
    if (box) return Vector2D(box->getRelativePosition().UpperLeftCorner.X, box->getRelativePosition().UpperLeftCorner.Y);
    return Vector2D();
}

void EditBox::setPosition(const Vector2D& pos) {
    if (box) box->setRelativePosition(irr::core::position2di(pos.x, pos.y));
}

void EditBox::setParent(const Image2D& other) {
    if (box) other.img->addChild(box);
}

bool EditBox::getDrawBorder() {
    return box ? border : false;
}

void EditBox::setDrawBorder(bool enable) {
    if (box) {
        box->setDrawBorder(enable);
        border = enable;
    }
}

bool EditBox::getIsPassword() {
    return box ? box->isPasswordBox() : false;
}

void EditBox::setIsPassword(bool enable) {
    if (box) box->setPasswordBox(enable);
}

bool EditBox::getIsEnabled() {
    return box ? box->isEnabled() : false;
}

void EditBox::setIsEnabled(bool enable) {
    if (box) box->setEnabled(enable);
}

bool EditBox::getMultiLine() {
    return box ? box->isMultiLineEnabled() : false;
}

void EditBox::setMultiLine(bool enable) {
    if (box) box->setMultiLine(enable);
}

bool EditBox::getAutoScroll() {
    return box ? box->isAutoScrollEnabled() : false;
}

void EditBox::setAutoScroll(bool enable) {
    if (box) box->setAutoScroll(enable);
}

void bindEditBox() {
    sol::usertype<EditBox> bindType = lua->new_usertype<EditBox>("EditBox",
        sol::constructors<
        EditBox(),
        EditBox(const EditBox&),
        EditBox(std::string),
        EditBox(std::string, const Vector2D&),
        EditBox(std::string, const Vector2D&, const Vector2D&)>(),

        "position", sol::property(&EditBox::getPosition, &EditBox::setPosition),
        "visible", sol::property(&EditBox::getVisible, &EditBox::setVisible),
        "size", sol::property(&EditBox::getSize, &EditBox::setSize),
        "wrap", sol::property(&EditBox::getWrap, &EditBox::setWrap),
        "textColor", sol::property(&EditBox::getTextColor, &EditBox::setTextColor),
        "text", sol::property(&EditBox::getText, &EditBox::setText),
        "drawBorder", sol::property(&EditBox::getDrawBorder, &EditBox::setDrawBorder),
        "enabled", sol::property(&EditBox::getIsEnabled, &EditBox::setIsEnabled),
        "password", sol::property(&EditBox::getIsPassword, &EditBox::setIsPassword),
        "multiLine", sol::property(&EditBox::getMultiLine, &EditBox::setMultiLine),
        "autoScroll", sol::property(&EditBox::getAutoScroll, &EditBox::setAutoScroll)
    );

    bindType["destroy"] = &EditBox::destroy;
    bindType["setFont"] = &EditBox::setFont;
    bindType["setMaxSize"] = &EditBox::setMaxSize;
    bindType["toFront"] = &EditBox::bringToFront;
    bindType["toBack"] = &EditBox::sendToBack;
    bindType["setBorderAlignment"] = &EditBox::setBorderAlignment;
    bindType["setTextAlignment"] = &EditBox::setTextAlignment;
    bindType["setParent"] = &EditBox::setParent;
}