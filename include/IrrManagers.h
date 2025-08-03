#pragma once
#include "irrlicht.h"
#include "IrrHandling.h"
#include "LimeReceiver.h"
#include "Sound.h"
#include "LightManager.h"
#include <map>
#include "NetworkHandler.h"

inline irr::IrrlichtDevice* device = nullptr;
inline irr::video::IVideoDriver* driver = nullptr;
inline irr::scene::ISceneManager* smgr = nullptr;
inline irr::gui::IGUIEnvironment* guienv = nullptr;
inline irr::video::IGPUProgrammingServices* gpu = nullptr;
inline IrrHandling* irrHandler = nullptr;
inline LimeReceiver* receiver = nullptr;
inline irrklang::ISoundEngine* sound = nullptr;
inline SoundManager* soundManager = nullptr;
inline EffectHandler* effects = nullptr;
inline std::map<std::string, gui::IGUIFont*> fontCache;
inline std::string defaultFont;
inline CLightManager* lightManager = nullptr;
inline NetworkHandler* networkHandler = nullptr;

inline irr::scene::ICameraSceneNode* mainCamera = nullptr;
inline irr::scene::ISceneNode* mainCameraForward = nullptr;