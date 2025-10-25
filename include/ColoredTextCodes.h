#pragma once
#include "irrlicht.h"
using namespace irr;

inline static video::SColor charToColorCode(wchar_t c) {
    switch (c) {
    case L'0': return video::SColor(255, 0x00, 0x00, 0x00);
    case L'1': return video::SColor(255, 0x00, 0x00, 0xAA);
    case L'2': return video::SColor(255, 0x00, 0xAA, 0x00);
    case L'3': return video::SColor(255, 0x00, 0xAA, 0xAA);
    case L'4': return video::SColor(255, 0xAA, 0x00, 0x00);
    case L'5': return video::SColor(255, 0xAA, 0x00, 0xAA);
    case L'6': return video::SColor(255, 0xFF, 0xAA, 0x00);
    case L'7': return video::SColor(255, 0xAA, 0xAA, 0xAA);
    case L'8': return video::SColor(255, 0x55, 0x55, 0x55);
    case L'9': return video::SColor(255, 0x55, 0x55, 0xFF);
    case L'a': case L'A': return video::SColor(255, 0x55, 0xFF, 0x55);
    case L'b': case L'B': return video::SColor(255, 0x55, 0xFF, 0xFF);
    case L'c': case L'C': return video::SColor(255, 0xFF, 0x55, 0x55);
    case L'd': case L'D': return video::SColor(255, 0xFF, 0x55, 0xFF);
    case L'e': case L'E': return video::SColor(255, 0xFF, 0xFF, 0x55);
    case L'f': case L'F': return video::SColor(255, 0xFF, 0xFF, 0xFF);
    default: return video::SColor(255, 0xFF, 0xFF, 0xFF);
    }
}