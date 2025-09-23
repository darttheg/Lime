#include "Hitbox.h"

Hitbox::Hitbox(float rad, float h) {
	radius = rad;
	height = h;
	lod = 1;
	construct();
}

Hitbox::Hitbox() : Hitbox(1, 0) {}

Hitbox::Hitbox(const Hitbox& other) {
	radius = other.radius;
	height = other.height;
	lod = other.lod;
	active = other.active;
	collision = other.collision;

	construct();
}

bool Hitbox::getVisible() {
	return node ? visible : false;
}

void Hitbox::setVisible(bool v) {
	if (node) {
		visible = v;
		updateMaterial(true, false);
	}
}

int Hitbox::getLOD() {
	return node ? lod : 0;
}

void Hitbox::setLOD(int i) {
	i = irr::core::clamp<int>(i, 0, 2);
	if (node) {
		lod = i;
		construct();
	}
}

bool Hitbox::getCollision() {
	return node ? collision : false;
}

void Hitbox::setCollision(bool enable) {
	if (collision && enable) return;
	if (enable && !active) return;

	if (!collision && enable) {
		irr::scene::ITriangleSelector* selector = smgr->createOctreeTriangleSelector(node->getMesh(), node);
		node->setTriangleSelector(selector);
		selector->drop();
		collision = true;
		return;
	}

	node->setTriangleSelector(0);
	collision = false;
}

int Hitbox::getID() {
	return node ? node->getID() : -1;
}

void Hitbox::setID(int i) {
	if (node) node->setID(i);
}

bool Hitbox::getActive() {
	return node ? active : false;
}

void Hitbox::setActive(bool a) {
	if (node) {
		active = a;
		updateMaterial(false, true);
		if (!active && collision)
			setCollision(false);
	}
}

void Hitbox::destroy() {
	if (node) destroyEntry();
	if (node) node->remove();
	if (holder) holder->remove();
}

void Hitbox::updateMaterial(bool updateOpacity, bool updateColor) {
	if (!node) return;

	irr::scene::IMeshManipulator* meshManipulator = device->getSceneManager()->getMeshManipulator();

	if (updateOpacity)
		meshManipulator->setVertexColorAlpha(node->getMesh(), visible ? 255 : 0);

	if (updateColor) {
		if (active)
			meshManipulator->setVertexColors(node->getMesh(), SColor(255, 255, 255, 0));
		else
			meshManipulator->setVertexColors(node->getMesh(), SColor(255, 0, 0, 255));
	}
}

void Hitbox::construct() {
	if (!holder)
		holder = smgr->addEmptySceneNode();

	int rings = 5;
	int sectors = 10;

	switch (lod) {
	default:
		rings = 7;
		sectors = 7;
		break;
	case 1:
		rings = 10;
		sectors = 10;
		break;
	case 2:
		rings = 20;
		sectors = 12;
		break;
	}

	SMeshBuffer* meshBuffer = genCapsule(vector3df(), radius, height, rings, sectors);

	SMesh* mesh = new SMesh();
	mesh->addMeshBuffer(meshBuffer);
	mesh->recalculateBoundingBox();
	meshBuffer->drop();

	if (node)
		node->remove();
	node = smgr->addMeshSceneNode(mesh);
	mesh->drop();
	node->getMaterial(0).Wireframe = true;
	node->getMaterial(0).FogEnable = false;
	node->getMaterial(0).Lighting = false;
	node->getMaterial(0).MaterialType = EMT_TRANSPARENT_VERTEX_ALPHA;

	updateMaterial(true, true);

	node->setParent(holder);
}

float Hitbox::getRadius() {
	return node ? radius : 0.0f;
}

void Hitbox::setRadius(float r) {
	if (!node || (node && radius == r)) return;

	radius = r;
	construct();
}

float Hitbox::getHeight() {
	return node ? height : 0.0f;
}

void Hitbox::setHeight(float h) {
	if (!node || (node && radius == h)) return;

	height = h;
	construct();
}

Vector2D Hitbox::getAttributes() {
	return node ? Vector2D(radius, height) : Vector2D();
}

void Hitbox::setAttributes(const Vector2D& att) {
	if (!node) return;

	if (att.x != radius || att.y != height) {
		radius = att.x;
		height = att.y;
		construct();
	}
}

bool Hitbox::overlaps(const Hitbox& other) {
	if (!active || !other.active) return false;

	if (node->getTransformedBoundingBox().intersectsWithBox(other.node->getTransformedBoundingBox())) {
		float radSumSQ = (radius + other.radius) * (radius + other.radius);

		// Edge cases
		if (radius == 0 && other.radius == 0)
			return node->getAbsolutePosition() == other.node->getAbsolutePosition();
		if (height <= 0 && other.height <= 0)
			return (node->getAbsolutePosition() - other.node->getAbsolutePosition()).getLengthSQ() <= radSumSQ;

		vector3df myBottom = vector3df();
		vector3df myTop = vector3df(0, height, 0);

		node->getAbsoluteTransformation().transformVect(myBottom);
		node->getAbsoluteTransformation().transformVect(myTop);

		vector3df otherBottom = vector3df();
		vector3df otherTop = vector3df(0, other.height, 0);

		other.node->getAbsoluteTransformation().transformVect(otherBottom);
		other.node->getAbsoluteTransformation().transformVect(otherTop);

		// Cylindrical check
		vector3df myAxis = myTop - myBottom;
		vector3df otherAxis = otherTop - otherBottom;
		vector3df positionDelta = myBottom - otherBottom;

		float myAxisDot = myAxis.dotProduct(myAxis);
		float otherAxisDot = otherAxis.dotProduct(otherAxis);
		float axisCrossDot = myAxis.dotProduct(otherAxis);
		float myAxisPositionDot = myAxis.dotProduct(positionDelta);
		float otherAxisPositionDot = otherAxis.dotProduct(positionDelta);

		float determinant = myAxisDot * otherAxisDot - axisCrossDot * axisCrossDot;
		float myClosest, otherClosest;

		if (determinant != 0.0f) {
			myClosest = (axisCrossDot * otherAxisPositionDot - otherAxisDot * myAxisPositionDot) / determinant;
			otherClosest = (myAxisDot * otherAxisPositionDot - axisCrossDot * myAxisPositionDot) / determinant;
		}
		else {
			myClosest = 0.0f;
			otherClosest = (axisCrossDot > otherAxisDot ? myAxisPositionDot / axisCrossDot : otherAxisPositionDot / otherAxisDot);
		}

		myClosest = core::clamp<float>(myClosest, 0.0f, 1.0f);
		otherClosest = core::clamp<float>(otherClosest, 0.0f, 1.0f);

		vector3df myClosestPoint = myBottom + myAxis * myClosest;
		vector3df otherClosestPoint = otherBottom + otherAxis * otherClosest;

		return (myClosestPoint - otherClosestPoint).getLengthSQ() <= radSumSQ;
	}
	return false;
}

bool Hitbox::pointInside(const Vector3D& point) {
	vector3df p = vector3df(point.x, point.y, point.z);

	if (!node->getTransformedBoundingBox().isPointInside(p)) return false;

	if (height == 0) return (node->getAbsolutePosition() - p).getLengthSQ() <= radius * radius;

	// Find closest point on line to point
	vector3df myBottom = vector3df();
	vector3df myTop = vector3df(0, height, 0);

	node->getAbsoluteTransformation().transformVect(myBottom);
	node->getAbsoluteTransformation().transformVect(myTop);

	vector3df myAxis = myTop - myBottom;
	float segLength = myAxis.getLength();

	if (segLength == 0) return false;

	vector3df myDir = myAxis / segLength;

	float t = (p - myBottom).dotProduct(myDir) / segLength;
	t = core::clamp<float>(t, 0.0f, 1.0f);

	vector3df closestPoint = myBottom + myDir * (t * segLength);
	
	return (p - closestPoint).getLengthSQ() <= radius * radius;
}


void bindHitbox() {
	sol::usertype<Hitbox> bindType = lua->new_usertype<Hitbox>("Hitbox",
		sol::constructors<Hitbox(), Hitbox(float radius, float height)>(),

		sol::base_classes, sol::bases<Compatible3D>(),

		"active", sol::property(&Hitbox::getActive, &Hitbox::setActive),
		"debug", sol::property(&Hitbox::getVisible, &Hitbox::setVisible),
		"levelOfDetail", sol::property(&Hitbox::getLOD, &Hitbox::setLOD),
		"collision", sol::property(&Hitbox::getCollision, &Hitbox::setCollision),
		"ID", sol::property(&Hitbox::getID, &Hitbox::setID),
		"radius", sol::property(&Hitbox::getRadius, &Hitbox::setRadius),
		"height", sol::property(&Hitbox::getHeight, &Hitbox::setHeight),

		"size", sol::property(
			[](Hitbox& c) { return Vector2D{ [&] { return c.getAttributes(); }, [&](auto v) { c.setAttributes(v); } }; },
			[](Hitbox& c, const Vector2D& v) { c.setAttributes(v); }
		)
	);

	bindType["destroy"] = &Hitbox::destroy;
	bindType["overlaps"] = &Hitbox::overlaps;
	bindType["pointInside"] = &Hitbox::pointInside;
}