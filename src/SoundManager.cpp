#include "SoundManager.h"

SoundManager::SoundManager() {
	soundEngine = irrklang::createIrrKlangDevice();
}