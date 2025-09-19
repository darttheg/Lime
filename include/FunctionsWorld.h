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

		irrHandler->skydome = smgr->addSkyDomeSceneNode(sky.texture, 16, 8, 0.9, 2.0, 100.0);
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
			irrHandler->backgroundColor = irr::video::SColor(color.w, color.x, color.y, color.z);
		}
	}

	int getObjectCount() {
		if (!irrHandler || !device)
			return 0;

		return smgr->getRootSceneNode()->getChildren().getSize();
	}

	sol::table fireRaypick(Vector3D start, Vector3D end, float debugLifetime) {
		scene::ISceneCollisionManager* collisionManager = smgr->getSceneCollisionManager();
		core::line3d<f32> ray(core::vector3df(start.x, start.y, start.z), core::vector3df(end.x, end.y, end.z));

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

	sol::table fireRaypick2D(Vector2D screenCoord, float length, float debugLifetime) {
		sol::table result = lua->create_table();
		scene::ISceneCollisionManager* collisionManager = smgr->getSceneCollisionManager();
		scene::ICameraSceneNode* cam = smgr->getActiveCamera();
		if (!cam || length == 0.0f) return result;

		core::line3df ray = collisionManager->getRayFromScreenCoordinates(core::position2di(screenCoord.x, screenCoord.y), cam);
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
		startDistance = dist.x;
		endDistance = dist.y;
		applyFogSettings();
	}

	// Set fog color
	void setFogColor(Vector4D color) {
		fogColor = video::SColor(static_cast<u32>(color.w), static_cast<u32>(color.x), static_cast<u32>(color.y), static_cast<u32>(color.z));
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
		startDistance = dist.x;
		endDistance = dist.y;
		fogColor = video::SColor(static_cast<u32>(color.w), static_cast<u32>(color.x), static_cast<u32>(color.y), static_cast<u32>(color.z));
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
		smgr->setAmbientLight(video::SColorf(static_cast<u32>(color.x) / 255.0f, static_cast<u32>(color.y) / 255.0f, static_cast<u32>(color.z) / 255.0f, static_cast<u32>(color.w) / 255.0f));
	}

	void setShadows(bool enable) {
		if (irrHandler)
			irrHandler->stencil = enable;
	}

	void setShadowColor(const Vector4D& color) {
		smgr->setShadowColor(video::SColor(static_cast<u32>(color.x), static_cast<u32>(color.y), static_cast<u32>(color.z), static_cast<u32>(color.w)));
	}

	void setShadowOpacity(int opacity) {
		SColor s = smgr->getShadowColor();
		s.setAlpha(opacity);
		smgr->setShadowColor(s);
	}

	Vector2D toScreenPosition(const Vector3D& pos) {
		irr::core::vector3df world = irr::core::vector3df(pos.x, pos.y, pos.z);
		irr::core::vector2di screen = smgr->getSceneCollisionManager()->getScreenCoordinatesFrom3DPosition(world, smgr->getActiveCamera());

		return Vector2D(screen.X, screen.Y);
	}

	Texture renderCameraOutput(const Camera3D& c, const Vector2D& size, bool renderGUI) {
		irr::video::ITexture* tx = 0;

		irr::scene::ICameraSceneNode* cur = c.camera;
		if (!cur)
			cur = mainCamera;

		if (device && cur && driver->queryFeature(video::EVDF_RENDER_TO_TARGET)) {
			tx = driver->addRenderTargetTexture(core::dimension2d<u32>(size.x, size.y), "RTT1");

			driver->beginScene(true, true, irrHandler->backgroundColor);

			irrHandler->setCameraMatrix(cur);

			smgr->setActiveCamera(cur);

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

	void clearScene(bool includeModels) {
		if (smgr && device) {
			smgr->clear();
			if (includeModels)
				smgr->getMeshCache()->clear();
		}
	}

	bool preloadMesh(std::string filePath) {
		if (!driver) return false;
		return smgr->getMesh(filePath.c_str()) != nullptr;
	}

	bool preloadTexture(std::string filePath) {
		if (!driver) return false;
		return driver->getTexture(filePath.c_str()) != nullptr;
	}

	bool unloadMesh(std::string filePath) {
		irr::scene::IMesh* mesh = smgr->getMesh(filePath.c_str());
		if (mesh) {
			smgr->getMeshCache()->removeMesh(mesh);
			return true;
		}
		return false;
	}

	bool unloadTexture(std::string filePath) {
		irr::video::ITexture* texture = driver->getTexture(filePath.c_str());
		if (texture) {
			driver->removeTexture(texture);
			return true;
		}
		return false;
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
		driver->drawPixel(pos.x, pos.y, SColor(color.w, color.x, color.y, color.z));
	}

	void drawLine2D(const Vector2D& start, const Vector2D& end, const Vector4D& color) {
		if (!driver) return;
		driver->draw2DLine(vector2di(start.x, start.y), vector2di(end.x, end.y), SColor(color.w, color.x, color.y, color.z));
	}

	void drawLine3D(const Vector3D& start, const Vector3D& end, const Vector4D& color) {
		if (!driver) return;

		SMaterial m;
		m.Lighting = false;
		driver->setMaterial(m);
		driver->setTransform(video::ETS_WORLD, core::matrix4());

		driver->draw3DLine(vector3df(start.x, start.y, start.z), vector3df(end.x, end.y, end.z), SColor(color.w, color.x, color.y, color.z));
	}

	void drawRectangle2D(const Vector2D& cornerTopLeft, const Vector2D& cornerBottomRight, const Vector4D& color) {
		if (!driver) return;
		driver->draw2DRectangleOutline(recti(cornerTopLeft.x, cornerTopLeft.y, cornerBottomRight.x, cornerBottomRight.y), SColor(color.w, color.x, color.y, color.z));

		/*
		if (outline)
			driver->draw2DRectangleOutline(recti(cornerTopLeft.x, cornerTopLeft.y, cornerBottomRight.x, cornerBottomRight.y), SColor(color.w, color.x, color.y, color.z));
		else
			driver->draw2DRectangle(SColor(color.w, color.x, color.y, color.z), recti(cornerTopLeft.x, cornerTopLeft.y, cornerBottomRight.x, cornerBottomRight.y));
		*/
	}

	void drawRectangle3D(const Vector3D& minEdge, const Vector3D& maxEdge, const Vector4D& color) {
		if (!driver) return;
		aabbox3df box = aabbox3df(vector3df(minEdge.x, minEdge.y, minEdge.z), vector3df(maxEdge.x, maxEdge.y, maxEdge.z));
		driver->draw3DBox(box, SColor(color.w, color.x, color.y, color.z));
	}

	void drawPolygon2D(int resolution, const Vector2D& pos, float radius, const Vector4D& color) {
		if (!driver) return;
		driver->draw2DPolygon(vector2di(pos.x, pos.y), radius, SColor(color.w, color.x, color.y, color.z), resolution);
	}
}

void bindWorld() {
	sol::table world = lua->create_named_table("World");

	world["SetSkydome"] = &Bind::setSkydome;
	world["SetSkydomeParameters"] = &Bind::setSkydomeParams;
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
	world["UnloadMesh"] = &Bind::unloadMesh;
	world["UnloadTexture"] = &Bind::unloadTexture;
	world["SetShadowColor"] = &Bind::setShadowColor;
	world["SetShadowOpacity"] = &Bind::setShadowOpacity;
	world["SetLightManagementMode"] = &Bind::setLightManagementMode;
	world["SetTextureCreationFlag"] = &Bind::setTextureCreationFlag;
	world["drawLine2D"] = &Bind::drawLine2D;
	world["drawLine3D"] = &Bind::drawLine3D;
	world["drawPixel"] = &Bind::drawPixel;
	world["drawBox2D"] = &Bind::drawRectangle2D;
	world["drawBox3D"] = &Bind::drawRectangle3D;
	world["drawPolygon2D"] = &Bind::drawPolygon2D;
}