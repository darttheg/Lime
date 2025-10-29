#include "FunctionsImports.h"

namespace Bind {
	video::SColor fogColor = video::SColor(255, 138, 125, 81);
	video::E_FOG_TYPE fogType = video::EFT_FOG_LINEAR;
	float startDistance = 250.0f;
	float endDistance = 1000.0f;
	float fogDensity = 0.003f;
	bool pixelFog = true;
	bool rangeFog = false;

	void setSkydome(Texture& sky) {
		if (!irrHandler || !device)
			return;
		if (irrHandler->skydome) {
			irrHandler->skydome->remove();
		}

		irrHandler->skydome = smgr->addSkyDomeSceneNode(sky.texture, 16, 8, 1.0, 2.0, 100.0);
	}

	void clearSkydome() {
		if (irrHandler && irrHandler->skydome) {
			irrHandler->skydome->getMaterial(0).setTexture(0, nullptr);
			smgr->addToDeletionQueue(irrHandler->skydome);
		}
	}

	void setSkydomeParams(u32 resX, u32 resY, float texPercent, float spherePercent, float radius) {
		irr::video::ITexture* cur = nullptr;
		if (!irrHandler || !device)
			return;
		if (irrHandler->skydome) {
			if (irrHandler->skydome->getMaterialCount() > 0) {
				cur = irrHandler->skydome->getMaterial(0).getTexture(0);
			}
			irrHandler->skydome->remove();
		}
		if (cur)
			irrHandler->skydome = smgr->addSkyDomeSceneNode(cur, resX, resY, texPercent, spherePercent, radius);
	}

	void setBackgroundColor(Vector4D& color) {
		if (driver && irrHandler) {
			irrHandler->backgroundColor = irr::video::SColor(color.getW(), color.getX(), color.getY(), color.getZ());
		}
	}

	int getObjectCount() {
		if (!irrHandler || !device)
			return 0;

		return smgr->getRootSceneNode()->getChildren().getSize();
	}

	sol::table fireRaypick(const Vector3D& start, const Vector3D& end, float debugLifetime) {
		scene::ISceneCollisionManager* collisionManager = smgr->getSceneCollisionManager();
		core::line3d<f32> ray(core::vector3df(start.getX(), start.getY(), start.getZ()), core::vector3df(end.getX(), end.getY(), end.getZ()));

		core::vector3df hitPosition;
		core::triangle3df hitTriangle;
		scene::ISceneNode* pickedNode = collisionManager->getSceneNodeAndCollisionPointFromRay(
			ray, hitPosition, hitTriangle, false);

		sol::table result = lua->create_table();

		sol::object attr = irrHandler->comp3dmap[pickedNode];

		if (pickedNode) {
			Vector3D normal = Vector3D(hitTriangle.getNormal().X, hitTriangle.getNormal().Y, hitTriangle.getNormal().Z);
			video::SMaterial material = pickedNode->getMaterial(0);
			Vector3D hit = Vector3D(hitPosition.X, hitPosition.Y, hitPosition.Z);
			result["ID"] = pickedNode->getID();
			result["attributes"] = attr;
			result["normal"] = normal;
			result["materialID"] = material.ID;
			result["hitPosition"] = hit;
			result["success"] = true;
		}
		else {
			result["ID"] = -1;
			result["attributes"] = sol::nil;
			result["normal"] = Vector3D(0, 1, 0);
			result["materialID"] = -1;
			result["hitPosition"] = end;
			result["success"] = false;
		}

		if (debugLifetime > 0) {
			DebugSceneNode* d = new DebugSceneNode(mainCamera, smgr, 0, DebugType::RAY_PICK);
			d->raypick_start = start;
			d->raypick_end = pickedNode ? Vector3D(hitPosition.X, hitPosition.Y, hitPosition.Z) : end;
			d->raypick_hit = pickedNode ? true : false;
			d->raypick_life = debugLifetime;
		}

		return result;
	}

	sol::table fireRaypick2D(const Vector2D& screenCoord, float length, float debugLifetime) {
		sol::table result = lua->create_table();
		scene::ISceneCollisionManager* collisionManager = smgr->getSceneCollisionManager();
		scene::ICameraSceneNode* cam = smgr->getActiveCamera();
		if (!cam || length == 0.0f) return result;

		core::line3df ray = collisionManager->getRayFromScreenCoordinates(core::position2di(screenCoord.getX(), screenCoord.getY()), cam);
		core::vector3df dir = ray.getVector().normalize();
		ray.end = ray.start + dir * length;

		core::vector3df hitPosition;
		core::triangle3df hitTriangle;
		scene::ISceneNode* pickedNode = collisionManager->getSceneNodeAndCollisionPointFromRay(
			ray, hitPosition, hitTriangle);

		sol::object attr = irrHandler->comp3dmap[pickedNode];

		if (pickedNode) {
			Vector3D normal = Vector3D(hitTriangle.getNormal().X, hitTriangle.getNormal().Y, hitTriangle.getNormal().Z);
			video::SMaterial material = pickedNode->getMaterial(0);
			Vector3D hit = Vector3D(hitPosition.X, hitPosition.Y, hitPosition.Z);
			result["ID"] = pickedNode->getID();
			result["attributes"] = attr;
			result["normal"] = normal;
			result["materialID"] = material.ID;
			result["hitPosition"] = hit;
			result["success"] = true;
		}
		else {
			result["ID"] = -1;
			result["attributes"] = sol::nil;
			result["normal"] = Vector3D(0, 1, 0);
			result["materialID"] = -1;
			result["hitPosition"] = Vector3D(ray.end.X, ray.end.Y, ray.end.Z);
			result["success"] = false;
		}

		if (debugLifetime > 0) {
			DebugSceneNode* d = new DebugSceneNode(mainCamera, smgr, 0, DebugType::RAY_PICK);
			d->raypick_start = Vector3D(ray.start.X, ray.start.Y, ray.start.Z);
			d->raypick_end = pickedNode ? Vector3D(hitPosition.X, hitPosition.Y, hitPosition.Z) : Vector3D(ray.end.X, ray.end.Y, ray.end.Z);
			d->raypick_hit = pickedNode ? true : false;
			d->raypick_life = debugLifetime;
		}

		return result;
	}

	void applyFogSettings() {
		driver->setFog(
			fogColor,
			fogType,
			startDistance,
			endDistance,
			fogDensity,
			pixelFog,
			rangeFog
		);
	}

	void setFogDistances(Vector2D dist) {
		startDistance = dist.getX();
		endDistance = dist.getY();
		applyFogSettings();
	}

	// Set fog color
	void setFogColor(Vector4D color) {
		fogColor = video::SColor(static_cast<u32>(color.getW()), static_cast<u32>(color.getX()), static_cast<u32>(color.getY()), static_cast<u32>(color.getZ()));
		applyFogSettings();
	}

	// Set fog type (linear, exponential, or exponential)
	void setFogType(int i) {
		switch (i) {
		case 0: fogType = video::EFT_FOG_LINEAR; break;
		case 1: fogType = video::EFT_FOG_EXP; break;
		case 2: fogType = video::EFT_FOG_EXP2; break;
		default: fogType = video::EFT_FOG_LINEAR; break;
		}
		applyFogSettings();
	}

	// Set fog density (for exponential fog types)
	void setFogDensity(float density) {
		fogDensity = density;
		applyFogSettings();
	}

	// Enable or disable per-pixel fog
	void setFogPixel(bool var) {
		pixelFog = var;
		applyFogSettings();
	}

	// Use range-based fog for better depth calculations, more expensive however
	void useDistanceForFog(bool var) {
		rangeFog = var;
		applyFogSettings();
	}

	void setFogSettings(Vector2D dist, Vector4D color, int i, float density, bool var, bool var1) {
		startDistance = dist.getX();
		endDistance = dist.getY();
		fogColor = video::SColor(static_cast<u32>(color.getW()), static_cast<u32>(color.getX()), static_cast<u32>(color.getY()), static_cast<u32>(color.getZ()));
		switch (i) {
		case 0: fogType = video::EFT_FOG_LINEAR; break;
		case 1: fogType = video::EFT_FOG_EXP; break;
		case 2: fogType = video::EFT_FOG_EXP2; break;
		default: fogType = video::EFT_FOG_LINEAR; break;
		}
		fogDensity = density;
		pixelFog = var;
		rangeFog = var1;
		applyFogSettings();
	}

	void setAmbientColor(const Vector4D& color) {
		smgr->setAmbientLight(video::SColorf(static_cast<u32>(color.getX()) / 255.0f, static_cast<u32>(color.getY()) / 255.0f, static_cast<u32>(color.getZ()) / 255.0f, static_cast<u32>(color.getW()) / 255.0f));
	}

	void setShadows(bool enable) {
		if (irrHandler)
			irrHandler->stencil = enable;
	}

	void setShadowColor(const Vector4D& color) {
		smgr->setShadowColor(video::SColor(static_cast<u32>(color.getX()), static_cast<u32>(color.getY()), static_cast<u32>(color.getZ()), static_cast<u32>(color.getW())));
	}

	void setShadowOpacity(int opacity) {
		SColor s = smgr->getShadowColor();
		s.setAlpha(opacity);
		smgr->setShadowColor(s);
	}

	Vector2D toScreenPosition(const Vector3D& pos) {
		irr::core::vector3df world = irr::core::vector3df(pos.getX(), pos.getY(), pos.getZ());
		irr::core::vector2di screen = smgr->getSceneCollisionManager()->getScreenCoordinatesFrom3DPosition(world, smgr->getActiveCamera());

		return Vector2D(screen.X, screen.Y);
	}

	Texture renderCameraOutput(const Camera3D& c, const Vector2D& size, bool renderGUI) {
		irr::video::ITexture* tx = 0;

		irr::scene::ICameraSceneNode* cur = c.camera;
		if (!cur)
			cur = mainCamera;

		if (device && cur && driver->queryFeature(video::EVDF_RENDER_TO_TARGET)) {
			tx = driver->addRenderTargetTexture(core::dimension2d<u32>(size.getX(), size.getY()), "RTT1");

			smgr->setActiveCamera(cur);
			c.camera->updateAbsolutePosition();
			c.forwardChild->updateAbsolutePosition();
			c.camera->setTarget(c.forwardChild->getAbsolutePosition());
			irrHandler->setCameraMatrix(cur);

			driver->setRenderTarget(tx, true, true, irrHandler->backgroundColor);
			smgr->drawAll();

			if (renderGUI)
				guienv->drawAll();
		}

		// Draw scene again, but does not account for queued cameras
		smgr->setActiveCamera(mainCamera);
		driver->setRenderTarget(0, true, true, irrHandler->backgroundColor);

		Texture tex = Texture();
		tex.texture = tx;
		tex.path = "Render Target Texture";

		return tex;
	}

	void clearScene(bool includeTextures) {
		if (!smgr) return;

		if (irrHandler->skydome) irrHandler->skydome->remove();

		core::array<ISceneNode*> stack;
		stack.push_back(smgr->getRootSceneNode());
		while (!stack.empty()) {
			ISceneNode* n = stack.getLast();
			stack.erase(stack.size() - 1);
			for (auto* c : n->getChildren()) stack.push_back(c);
			n->remove();
		}

		IMeshCache* cache = smgr->getMeshCache();
		for (u32 i = 0; i < cache->getMeshCount(); ++i) {
			IAnimatedMesh* am = cache->getMeshByIndex(i);
			if (!am) continue;
			IMesh* m = am->getMesh(0);
			if (!m) continue;
			for (u32 j = 0; j < m->getMeshBufferCount(); ++j)
				if (auto* mb = m->getMeshBuffer(j))
					driver->removeHardwareBuffer(mb);
		}
		cache->clear();

		if (includeTextures)
			driver->removeAllTextures();
	}

	bool preloadMesh(sol::variadic_args va) {
		if (!driver || !smgr) return false;
		return irrHandler->preload.enqueueMeshes(va);
		/*
		bool all = true;
		for (sol::stack_object v : va) {
			sol::optional<std::string_view> s = v.as<sol::optional<std::string_view>>();
			if (!s) return false;
			all = all && (smgr->getMesh(std::string(*s).c_str()) != nullptr);
		}
		return all;
		*/
	}

	bool preloadTexture(sol::variadic_args va) {
		if (!driver) return false;
		return irrHandler->preload.enqueueTextures(va);
		/*
		bool all = true;
		for (sol::stack_object v : va) {
			sol::optional<std::string_view> s = v.as<sol::optional<std::string_view>>();
			if (!s) return false;
			all = all && (driver->getTexture(std::string(*s).c_str()) != nullptr);
		}
		return all;
		*/
	}

	int getMeshCacheCount() {
		return (driver && smgr) ? smgr->getMeshCache()->getMeshCount() : 0;
	}

	int getTextureCacheCount() {
		return (driver) ? driver->getTextureCount() : 0;
	}

	static ITexture* getCheckerError() {
		ITexture* checker = driver->getTexture("limeError");
		if (!checker) {
			const SColor L(255, 153, 229, 80), W(255, 255, 255, 255);
			IImage* img = driver->createImage(ECF_A8R8G8B8, dimension2du(2, 2));
			img->setPixel(0, 0, L); img->setPixel(1, 0, W);
			img->setPixel(0, 1, W); img->setPixel(1, 1, L);
			checker = driver->addTexture("limeError", img);
			img->drop();
		}
		return checker;
	}

	static ITexture* getBlankTexture() {
		ITexture* checker = driver->getTexture("limeBlank");
		if (!checker) {
			const SColor L(0, 0, 0, 0);
			IImage* img = driver->createImage(ECF_A8R8G8B8, dimension2du(1, 1));
			img->setPixel(0, 0, L);
			checker = driver->addTexture("limeBlank", img);
			img->drop();
		}
		return checker;
	}

	Texture getChecker() {
		return Texture(getCheckerError());
	}

	Texture getBlank() {
		return Texture(getBlankTexture());
	}

	void unloadMesh(std::string path, bool safe = false) {
		if (!driver || !smgr) return;

		IMesh* m = smgr->getMesh(path.c_str());
		if (!m) return;

		if (safe) {
			IMesh* cube = nullptr;
			if (auto* gc = smgr->getGeometryCreator())
				cube = gc->createCubeMesh(irr::core::vector3df(10.f, 10.f, 10.f));
			if (!cube) return;

			irr::core::array<irr::scene::ISceneNode*> stack;
			if (ISceneNode* root = smgr->getRootSceneNode()) stack.push_back(root);

			while (!stack.empty()) {
				auto* node = stack.getLast();
				stack.erase(stack.size() - 1);
				for (auto* c : node->getChildren()) stack.push_back(c);

				if (node->getType() != irr::scene::ESNT_MESH) continue;

				IMeshSceneNode* meshNode = static_cast<irr::scene::IMeshSceneNode*>(node);
				if (meshNode->getMesh() == m)
					meshNode->setMesh(cube);

				SMaterial& mat = meshNode->getMaterial(0);
				mat.setTexture(0, getCheckerError());
				mat.setFlag(E_MATERIAL_FLAG::EMF_BILINEAR_FILTER, false);
				mat.setFlag(E_MATERIAL_FLAG::EMF_LIGHTING, false);
				mat.setFlag(E_MATERIAL_FLAG::EMF_FOG_ENABLE, false);
			}

			cube->drop();
		}

		smgr->getMeshCache()->removeMesh(m);
	}

	void unloadTexture(std::string path, bool safe = false) {
		if (!driver || !smgr) return;

		ITexture* t = driver->getTexture(path.c_str());
		if (!t) return;

		if (safe) {
			ITexture* checker = getCheckerError();

			core::array<ISceneNode*> stack;
			if (ISceneNode* root = smgr->getRootSceneNode()) stack.push_back(root);

			while (!stack.empty()) {
				ISceneNode* node = stack.getLast();
				stack.erase(stack.size() - 1);
				for (auto* c : node->getChildren()) stack.push_back(c);

				if (!(node->getType() == ESNT_MESH || node->getType() == ESNT_SKY_DOME)) continue;

				for (u32 i = 0; i < node->getMaterialCount(); ++i) {
					SMaterial& mat = node->getMaterial(i);
					for (u32 l = 0; l < MATERIAL_MAX_TEXTURES; ++l) {
						if (mat.getTexture(l) == t)
							mat.setTexture(l, checker);
					}
					mat.setFlag(E_MATERIAL_FLAG::EMF_BILINEAR_FILTER, false);
					mat.setFlag(E_MATERIAL_FLAG::EMF_LIGHTING, false);
					mat.setFlag(E_MATERIAL_FLAG::EMF_FOG_ENABLE, false);
				}
			}
		}

		driver->removeTexture(t);
	}

	bool getAreTexturesPreloading() {
		if (!driver) return false;
		return irrHandler->preload.texturesActive();
	}

	bool getAreMeshesPreloading() {
		if (!driver) return false;
		return irrHandler->preload.meshesActive();
	}

	bool getIsPreloading() {
		if (!driver) return false;
		return irrHandler->preload.texturesActive() || irrHandler->preload.meshesActive();
	}

	void setLightManagementMode(int i) {
		i = irr::core::clamp<int>(i, 0, 2);
		if (lightManager)
			lightManager->setMode(i);

		if (i > 0)
			smgr->setLightManager(lightManager);
		else
			smgr->setLightManager(0);
	}

	void setTextureCreationFlag(int f, bool v) {
		if (!driver) return;

		E_TEXTURE_CREATION_FLAG out = E_TEXTURE_CREATION_FLAG::ETCF_ALLOW_NON_POWER_2;

		switch (f) {
		case 1:
			out = E_TEXTURE_CREATION_FLAG::ETCF_ALWAYS_16_BIT;
			break;
		case 2:
			out = E_TEXTURE_CREATION_FLAG::ETCF_ALWAYS_32_BIT;
			break;
		case 3:
			out = E_TEXTURE_CREATION_FLAG::ETCF_CREATE_MIP_MAPS;
			break;
		case 4:
			out = E_TEXTURE_CREATION_FLAG::ETCF_NO_ALPHA_CHANNEL;
			break;
		case 5:
			out = E_TEXTURE_CREATION_FLAG::ETCF_OPTIMIZED_FOR_QUALITY;
			break;
		case 6:
			out = E_TEXTURE_CREATION_FLAG::ETCF_OPTIMIZED_FOR_SPEED;
			break;
		}

		if ((static_cast<u32>(out) & static_cast<u32>(ETCF_NO_ALPHA_CHANNEL)) && irrHandler->driverType == E_DRIVER_TYPE::EDT_OPENGL)
			return;

		driver->setTextureCreationFlag(out, v);
	}

	void drawPixel(const Vector2D& pos, const Vector4D& color) {
		if (!driver) return;
		driver->drawPixel(pos.getX(), pos.getY(), SColor(color.getW(), color.getX(), color.getY(), color.getZ()));
	}

	void drawLine2D(const Vector2D& start, const Vector2D& end, const Vector4D& color) {
		if (!driver) return;
		driver->draw2DLine(vector2di(start.getX(), start.getY()), vector2di(end.getX(), end.getY()), SColor(color.getW(), color.getX(), color.getY(), color.getZ()));
	}

	void drawLine3D(const Vector3D& start, const Vector3D& end, const Vector4D& color) {
		if (!driver) return;

		SMaterial m;
		m.Lighting = false;
		driver->setMaterial(m);
		driver->setTransform(video::ETS_WORLD, core::matrix4());

		driver->draw3DLine(vector3df(start.getX(), start.getY(), start.getZ()), vector3df(end.getX(), end.getY(), end.getZ()), SColor(color.getW(), color.getX(), color.getY(), color.getZ()));
	}

	void drawRectangle2D(const Vector2D& cornerTopLeft, const Vector2D& cornerBottomRight, const Vector4D& color) {
		if (!driver) return;
		driver->draw2DRectangleOutline(recti(cornerTopLeft.getX(), cornerTopLeft.getY(), cornerBottomRight.getX(), cornerBottomRight.getY()), SColor(color.getW(), color.getX(), color.getY(), color.getZ()));

		/*
		if (outline)
			driver->draw2DRectangleOutline(recti(cornerTopLeft.x, cornerTopLeft.y, cornerBottomRight.x, cornerBottomRight.y), SColor(color.w, color.x, color.y, color.z));
		else
			driver->draw2DRectangle(SColor(color.w, color.x, color.y, color.z), recti(cornerTopLeft.x, cornerTopLeft.y, cornerBottomRight.x, cornerBottomRight.y));
		*/
	}

	void drawRectangle3D(const Vector3D& minEdge, const Vector3D& maxEdge, const Vector4D& color = Vector4D(255,255,255,255)) {
		if (!driver) return;
		aabbox3df box = aabbox3df(vector3df(minEdge.getX(), minEdge.getY(), minEdge.getZ()), vector3df(maxEdge.getX(), maxEdge.getY(), maxEdge.getZ()));
		driver->draw3DBox(box, SColor(color.getW(), color.getX(), color.getY(), color.getZ()));
	}

	void drawPolygon2D(int resolution, const Vector2D& pos, float radius, const Vector4D& color) {
		if (!driver) return;
		driver->draw2DPolygon(vector2di(pos.getX(), pos.getY()), radius, SColor(color.getW(), color.getX(), color.getY(), color.getZ()), resolution);
	}

	void setUseHighLevelShaders(bool v) {
		if (irrHandler)
			irrHandler->useHighLevelShaders = v;
	}

	void setUseCGShaders(bool v) {
		if (irrHandler) {
			irrHandler->useCGShaders = v;

			if (driver && (irrHandler->useCGShaders && !driver->queryFeature(video::EVDF_CG)))
				irrHandler->useCGShaders = false;
		}
	}

	bool queryFeature(int i) {
		return driver->queryFeature((irr::video::E_VIDEO_DRIVER_FEATURE)i);
	}

	bool addArchive(const std::string& path) {
		if (device)
			return device->getFileSystem()->addFileArchive(path.c_str(), true, false, irr::io::EFAT_UNKNOWN);
		return false;
	}
}

void bindWorld() {
	sol::table world = lua->create_named_table("World");

	world["SetSkydome"] = &Bind::setSkydome;
	world["SetSkydomeParameters"] = &Bind::setSkydomeParams;
	world["ClearSkydome"] = &Bind::clearSkydome;
	world["SetBackgroundColor"] = &Bind::setBackgroundColor;
	world["GetObjectCount"] = &Bind::getObjectCount;
	world["FireRaypick3D"] = &Bind::fireRaypick;
	world["FireRaypick2D"] = &Bind::fireRaypick2D;
	world["SetFogDistances"] = &Bind::setFogDistances;
	world["SetFogColor"] = &Bind::setFogColor;
	world["SetFogType"] = &Bind::setFogType;
	world["SetFogDensity"] = &Bind::setFogDensity;
	world["SetPixelFog"] = &Bind::setFogPixel;
	world["SetRangeFog"] = &Bind::useDistanceForFog;
	world["SetFogParameters"] = &Bind::setFogSettings;
	world["SetAmbientColor"] = &Bind::setAmbientColor;
	world["ConvertToScreenPosition"] = &Bind::toScreenPosition;
	world["SetShadows"] = &Bind::setShadows;
	world["GetRenderTexture"] = &Bind::renderCameraOutput;
	world["Clear"] = &Bind::clearScene;
	world["PreloadMesh"] = &Bind::preloadMesh;
	world["PreloadTexture"] = &Bind::preloadTexture;
	world["IsPreloadingMeshes"] = &Bind::getAreMeshesPreloading;
	world["IsPreloadingTextures"] = &Bind::getAreTexturesPreloading;
	world["IsPreloading"] = &Bind::getIsPreloading;
	world["AddArchive"] = &Bind::addArchive;
	world["GetMeshCacheCount"] = &Bind::getMeshCacheCount;
	world["GetTextureCacheCount"] = &Bind::getTextureCacheCount;
	world["UnloadMeshByPath"] = &Bind::unloadMesh;
	world["UnloadTextureByPath"] = &Bind::unloadTexture;
	world["SetShadowColor"] = &Bind::setShadowColor;
	world["SetShadowOpacity"] = &Bind::setShadowOpacity;
	world["SetLightManagementMode"] = &Bind::setLightManagementMode;
	world["SetTextureCreationFlag"] = &Bind::setTextureCreationFlag;
	/* These do not work as they are called prior to rendering. Queue them?
	world["DrawLine2D"] = &Bind::drawLine2D;
	world["DrawLine3D"] = &Bind::drawLine3D;
	world["DrawPixel"] = &Bind::drawPixel;
	world["DrawBox2D"] = &Bind::drawRectangle2D;
	world["DrawBox3D"] = &Bind::drawRectangle3D;
	world["DrawPolygon2D"] = &Bind::drawPolygon2D;
	*/
	world["SetUseHighLevelShaders"] = &Bind::setUseHighLevelShaders;
	world["SetUseCGShaders"] = &Bind::setUseCGShaders;
	world["GetFeatureSupported"] = &Bind::queryFeature;
	world["GetErrorTexture"] = &Bind::getChecker;
	world["GetBlankTexture"] = &Bind::getBlank;
}