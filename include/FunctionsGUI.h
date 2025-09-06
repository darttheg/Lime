#include "FunctionsImports.h"

namespace Bind {
	bool embedFont(const std::string& fontPath) {
		if (!device) return false;

		std::filesystem::path p(fontPath);
		if (p.extension() != ".xml")
			return false;

		std::string fontName = p.stem().string();

		if (fontCache.find(fontName) != fontCache.end())
			return true;

		gui::IGUIFont* font = guienv->getFont(fontPath.c_str());
		if (!font)
			return false;

		fontCache[fontName] = font;
		return true;
	}

	bool setDefaultFont(const std::string& fontName) {
		if (device) {
			auto it = fontCache.find(fontName);
			if (it != fontCache.end()) {
				irr::gui::IGUISkin* skin = guienv->getSkin();
				skin->setFont(it->second);
				defaultFont = it->first;
				return true;
			}
		}
		return false;
	}

	std::string getFontList() {
		std::string result;
		for (const auto& pair : fontCache) {
			result += pair.first + ", ";
		}
		if (!result.empty()) {
			result.pop_back();
			result.pop_back();
		}
		return result;
	}

	void clearGUI() {
		if (guienv && device)
			guienv->clear();
	}

	void setBilinearFiltering(bool enable) {
		if (device) {
			driver->getMaterial2D().TextureLayer[0].BilinearFilter = enable;
		}
	}

	void setAnisotropicFiltering(bool enable) {
		if (device) {
			driver->getMaterial2D().TextureLayer[0].AnisotropicFilter = enable;
		}
	}

	void setTrilinearFiltering(bool enable) {
		if (device) {
			driver->getMaterial2D().TextureLayer[0].TrilinearFilter = enable;
		}
	}

	void setAntiAliasing(int i) {
		if (device)
			driver->getMaterial2D().AntiAliasing = (irr::video::E_ANTI_ALIASING_MODE)i;
	}

	void queueGUI() {
		irrHandler->AddCameraToQueue(nullptr, nullptr, false, true);
	}
}

void bindGUI() {
	sol::table gui = lua->create_named_table("GUI");

	gui["ImportFont"] = &Bind::embedFont;
	gui["SetDefaultFont"] = &Bind::setDefaultFont;
	gui["GetImportedFontsList"] = &Bind::getFontList;
	gui["SetBilinearFiltering"] = &Bind::setBilinearFiltering;
	gui["SetAnisotropicFiltering"] = &Bind::setAnisotropicFiltering;
	gui["SetTrilinearFiltering"] = &Bind::setTrilinearFiltering;
	gui["SetAntiAliasing"] = &Bind::setAntiAliasing;
	gui["Clear"] = &Bind::clearGUI;
	gui["Queue"] = &Bind::queueGUI;
}