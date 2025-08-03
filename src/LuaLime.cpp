#include "LuaLime.h"

#include "Vector2D.h"
#include "Vector3D.h"
#include "Vector4D.h"
#include "Billboard.h"
#include "Camera3D.h"
#include "Material.h"
#include "StaticMesh.h"
#include "Texture.h"
#include "Light.h"
#include "Text3D.h"
#include "WaterMesh.h"
#include "Image2D.h"
#include "Text2D.h"
#include "EditBox.h"
#include "ParticleSystem.h"
#include "Trail.h"
#include "Warden.h"
#include "Empty.h"
#include "LegacyLight.h"
#include "Hitbox.h"
#include "Packet.h"
#include "MeshBuffer.h"

#include "Compatible2D.h"
#include "Compatible3D.h"

#include <sol/sol.hpp>
#include <sstream>

int LuaLime::initLua(irr::scene::ISceneManager* smgr, irr::video::IVideoDriver* driver) {
	lua = new sol::state(); // maybe change heap alloc
	lua->open_libraries(sol::lib::base, sol::lib::string, sol::lib::os, sol::lib::coroutine, sol::lib::jit, sol::lib::utf8, sol::lib::io, sol::lib::math, sol::lib::table, sol::lib::package);

	// warden
	bindWarden();

	// comp
	bindCompatible2D();
	bindCompatible3D();

	// objs
	bindVector2D();
	bindVector3D();
	bindVector4D();
	bindBillboard();
	bindCamera3D();
	bindMaterial();
	bindStaticMesh();
	bindTexture();
	bindLight();
	bindText3D();
	bindWater();
	bindImage2D();
	bindText2D();
	bindEditBox();
	bindParticleSystem();
	bindTrail();
	bindEmpty();
	bindLegacyLight();
	bindHitbox();
	bindPacket();
	bindMeshBuffer();

	return 0;
}