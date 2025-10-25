#include "ParticleSystem.h"

ParticleSystem::ParticleSystem() {
	ps = smgr->addParticleSystemSceneNode(false);
}

void ParticleSystem::setEmitter(int i, sol::table params) {
	if (!ps)
		return;

	scene::IParticleEmitter* em = nullptr;

	irr::core::vector3df pos = irr::core::vector3df();

	if (params["position"]) {
		Vector3D p = TblFrom3D(params["position"]);
		pos = irr::core::vector3df(p.getX(), p.getY(), p.getZ());
	}

	switch (i) {
	default:
	{
		// No special params needed
		em = ps->createPointEmitter();
		break;
	}
	case 1:
	{
		// Define aabb
		irr::core::vector3df minEdge = irr::core::vector3df(-3, -3, -3);
		irr::core::vector3df maxEdge = irr::core::vector3df(3, 3, 3);

		if (params["minEdge"]) {
			Vector3D p = TblFrom3D(params["minEdge"]);
			minEdge = irr::core::vector3df(p.getX(), p.getY(), p.getZ());
		}
		if (params["maxEdge"]) {
			Vector3D p = TblFrom3D(params["maxEdge"]);
			maxEdge = irr::core::vector3df(p.getX(), p.getY(), p.getZ());
		}

		em = ps->createBoxEmitter(irr::core::aabbox3df(minEdge + pos, maxEdge + pos));
		break;
	}
	case 2:
	{
		// Define radius
		float radius = 3.0f;
		if (params["radius"]) {
			radius = params["radius"];
		}

		em = ps->createSphereEmitter(pos, radius);
		break;
	}
	case 3:
	{
		// Define radius and ring thickness
		float radius = 3.0f;
		if (params["radius"]) {
			radius = params["radius"];
		}
		float thickness = 1.0f;
		if (params["thickness"]) {
			radius = params["thickness"];
		}

		em = ps->createRingEmitter(pos, radius, thickness);
		break;
	}
	}

	if (params["velocity"]) {
		Vector3D p = TblFrom3D(params["velocity"]);
		em->setDirection(irr::core::vector3df(p.getX(), p.getY(), p.getZ()));
	}

	em->setMaxAngleDegrees(params["maxAngle"] ? params["maxAngle"] : 25.0f);

	float min = 20.0f;
	float max = 250.0f;
	if (params["lifeTime"]) {
		Vector2D p = TblFrom2D(params["lifeTime"]);
		min = p.getX();
		max = p.getY();
	}
	em->setMinLifeTime(min);
	em->setMaxLifeTime(max);

	min = 5.0f;
	max = 15.0f;
	if (params["particlesPerSecond"]) {
		Vector2D p = TblFrom2D(params["particlesPerSecond"]);
		min = p.getX();
		max = p.getY();
	}
	em->setMinParticlesPerSecond(min);
	em->setMaxParticlesPerSecond(max);

	irr::video::SColor startCol = irr::video::SColor(255, 255, 255, 255);
	irr::video::SColor endCol = irr::video::SColor(255, 255, 255, 255);
	if (params["minStartingColor"]) {
		Vector4D c = TblFrom4D(params["minStartingColor"]);
		startCol = irr::video::SColor(c.getW(), c.getX(), c.getY(), c.getZ());
	}
	if (params["maxStartingColor"]) {
		Vector4D c = TblFrom4D(params["maxStartingColor"]);
		endCol = irr::video::SColor(c.getW(), c.getX(), c.getY(), c.getZ());
	}
	em->setMinStartColor(startCol);
	em->setMaxStartColor(endCol);

	min = 1.0f;
	max = 1.2f;
	if (params["startSize"]) {
		Vector2D p = TblFrom2D(params["startSize"]);
		min = p.getX();
		max = p.getY();
	}
	em->setMinStartSize(irr::core::dimension2df(min, min));
	em->setMaxStartSize(irr::core::dimension2df(max, max));

	if (em) {
		ps->setEmitter(em);
		em->drop();
	}
}

void ParticleSystem::addAffector(int i, sol::table params) {
	irr::scene::IParticleAffector* pa;

	switch (i) {
	default:
	{
		irr::core::vector3df pos = ps->getPosition();
		float spd = 1.0f;
		bool attract = true; // Assume this is always true?
		bool aX = true;
		bool aY = true;
		bool aZ = true;

		if (params["attractPosition"]) {
			Vector3D c = TblFrom3D(params["attractPosition"]);
			pos = irr::core::vector3df(c.getX(), c.getY(), c.getZ());
		}

		if (params["attractAxis"]) {
			Vector3D c = TblFrom3D(params["attractAxis"]);
			aX, aY, aZ = c.getX(), c.getY(), c.getZ();
		}

		pa = ps->createAttractionAffector(pos, spd, attract, aX, aY, aZ);
		break;
	}
	case 1:
	{
		irr::video::SColor targetColor;
		int timeNeeded = 1000.0;

		if (params["targetColor"]) {
			Vector4D c = TblFrom4D(params["targetColor"]);
			targetColor = irr::video::SColor(c.getW(), c.getX(), c.getY(), c.getZ());
		}

		if (params["time"]) {
			timeNeeded = params["time"];
		}

		pa = ps->createFadeOutParticleAffector(targetColor, timeNeeded);
		break;
	}
	case 2:
	{
		irr::core::vector3df grav = irr::core::vector3df(0.0, -0.03, 0.0);
		int t = 1000.0;

		if (params["gravity"]) {
			Vector3D c = TblFrom3D(params["gravity"]);
			grav = irr::core::vector3df(c.getX(), c.getY(), c.getZ());
		}

		if (params["timeToTakeOver"]) {
			t = params["timeToTakeOver"];
		}

		pa = ps->createGravityAffector(grav, t);
		break;
	}
	case 3:
	{
		irr::core::vector3df rotor = irr::core::vector3df(2.0, 2.0, 2.0);

		if (params["rotationSpeed"]) {
			Vector3D r = TblFrom3D(params["rotationSpeed"]);
			rotor = irr::core::vector3df(r.getX(), r.getY(), r.getZ());
		}

		pa = ps->createRotationAffector(rotor);
		break;
	}
	case 4:
	{
		irr::core::dimension2df d = irr::core::dimension2df(0.5, 0.5);

		if (params["scalar"]) {
			Vector2D s = TblFrom2D(params["scalar"]);
			d = irr::core::dimension2df(s.getX(), s.getY());
		}

		pa = ps->createScaleParticleAffector(d);
		break;
	}
	}

	ps->addAffector(pa);
	pa->drop();
}

void ParticleSystem::removeAffectors() {
	if (ps)
		ps->removeAllAffectors();
}

void ParticleSystem::removeParticles() {
	if (ps)
		ps->clearParticles();
}

void ParticleSystem::setParticleGlobalBehavior(bool track) {
	if (ps)
		ps->setParticlesAreGlobal(track);
}

bool ParticleSystem::getActive() {
	return ps ? ps->active : false;
}

void ParticleSystem::setActive(bool active) {
	if (ps)
		ps->active = active;
}

void ParticleSystem::emitOnce(int amount) {
	if (ps)
		ps->spark(device->getTimer()->getTime(), amount);
}

void ParticleSystem::loadMaterial(const Material& mat) {
	if (ps)
		ps->getMaterial(0) = mat.mat;
}

bool ParticleSystem::getDebug() {
	return (ps && ps->isDebugDataVisible());
}

void ParticleSystem::setDebug(bool visible) {
	if (ps) {
		if (visible)
			ps->setDebugDataVisible(irr::scene::EDS_FULL);
		else
			ps->setDebugDataVisible(irr::scene::EDS_OFF);
	}
}

void ParticleSystem::destroy() {
	if (ps) {
		destroyEntry();
		smgr->addToDeletionQueue(ps);
	}
}

void bindParticleSystem() {
	sol::usertype<ParticleSystem> bindType = lua->new_usertype<ParticleSystem>("ParticleSystem",
		sol::constructors <ParticleSystem()>(),

		sol::base_classes, sol::bases<Compatible3D>(),
		sol::meta_function::type, [](const ParticleSystem&) { return "ParticleSystem"; },

		"debug", sol::property(&ParticleSystem::getDebug, &ParticleSystem::setDebug),
		"active", sol::property(&ParticleSystem::getActive, &ParticleSystem::setActive)
	);

	bindType["setDoAbsoluteTracking"] = &ParticleSystem::setParticleGlobalBehavior;
	bindType["setEmitter"] = &ParticleSystem::setEmitter;
	bindType["addAffector"] = &ParticleSystem::addAffector;
	bindType["clearAffectors"] = &ParticleSystem::removeAffectors;
	bindType["clearParticles"] = &ParticleSystem::removeParticles;
	bindType["loadMaterial"] = &ParticleSystem::loadMaterial;
	bindType["spark"] = &ParticleSystem::emitOnce;
}