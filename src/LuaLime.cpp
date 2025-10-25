#include "LuaLime.h"

#include "Vector2D.h"
#include "Vector3D.h"
#include "Vector4D.h"
#include "Billboard.h"
#include "Camera3D.h"
#include "Material.h"
#include "StaticMesh.h"
#include "Texture.h"
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
#include "Event.h"
#include "PhysicsObject.h"
#include "RigidBody3D.h"
#include "VolumeLight.h"

#include "Sound.h"

#include "Compatible2D.h"
#include "Compatible3D.h"

#include "LimeEvents.h"

#include <sol/sol.hpp>
#include <sstream>

static std::string lua_typeof(sol::object o) {
	sol::type t = o.get_type();
	if (t != sol::type::userdata) {
		return std::string(sol::type_name(o.lua_state(), t));
	}

	sol::userdata ud = o.as<sol::userdata>();

	sol::optional<sol::table> mt = ud[sol::metatable_key];
	if (mt && mt->valid()) {
		if (auto f = mt->get<sol::optional<sol::function>>("__type"); f) {
			sol::protected_function_result r = (*f)(o);
			if (r.valid()) return r.get<std::string>();
		}
		if (auto n = mt->get<sol::optional<std::string>>("__name"); n) return *n;
	}

	return "userdata";
}

int LuaLime::initLua(irr::scene::ISceneManager* smgr, irr::video::IVideoDriver* driver) {
	lua = new sol::state(); // maybe change heap alloc
	lua->open_libraries(sol::lib::base, sol::lib::string, sol::lib::os, sol::lib::coroutine, sol::lib::jit, sol::lib::utf8, sol::lib::io, sol::lib::math, sol::lib::table, sol::lib::package, sol::lib::debug);

	lua->set_function("type", &lua_typeof);

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
	bindEvent();
	bindHook();
	bindPhysicsObject();
	bindRigidBody3D();
	bindSound();
	bindVolumeLight();

	initEvents();

	return 0;
}