#include "Camera3D.h"

Camera3D::Camera3D() {
    camera = createCamera();

    if (mainCamera)
        smgr->setActiveCamera(mainCamera);
    else
        setActive();

    camera->grab();
}

Camera3D::Camera3D(const Camera3D& other) {
    destroy();
    camera = other.camera;
    camera->grab();
}

irr::scene::ICameraSceneNode* Camera3D::createCamera() {
    forwardChild = smgr->addEmptySceneNode();
    leftChild = smgr->addEmptySceneNode();
    irr::scene::ICameraSceneNode* cam = smgr->addCameraSceneNode();

    cam->addChild(forwardChild);
    forwardChild->setPosition(irr::core::vector3df(0, 0, 1));

    cam->addChild(leftChild);
    leftChild->setPosition(irr::core::vector3df(-1, 0, 0));

    return cam;
}

void Camera3D::destroy() {
    if (smgr->getActiveCamera() == camera) {
        smgr->setActiveCamera(nullptr);
    }
    if (forwardChild) forwardChild->remove();
    if (leftChild) leftChild->remove();
    // if (d) d->remove();
    if (camera) camera->remove();
    camera = nullptr;
}

Vector3D Camera3D::getPosition() {
    return Vector3D(camera->getPosition().X, camera->getPosition().Y, camera->getPosition().Z);
}

void Camera3D::setPosition(const Vector3D& pos) {
    camera->setPosition(irr::core::vector3df(pos.x, pos.y, pos.z));
}

Vector3D Camera3D::getRotation() {
    return Vector3D(camera->getRotation().X, camera->getRotation().Y, camera->getRotation().Z);
}

void Camera3D::setRotation(const Vector3D& rot) {
    float clampedX = std::clamp(rot.x, -89.0f, 89.0f);
    camera->setRotation(irr::core::vector3df(clampedX, rot.y, rot.z));
}

void Camera3D::setTargetBind(bool val) {
    camera->bindTargetAndRotation(val);
}

float Camera3D::getNearPlane() {
    return camera->getNearValue();
}

void Camera3D::setNearPlane(float val) {
    camera->setNearValue(val);
}

float Camera3D::getFarPlane() {
    return camera->getFarValue();
}

void Camera3D::setFarPlane(float val) {
    camera->setFarValue(val);
}

Vector2D Camera3D::getPlanes() {
    return Vector2D(camera->getNearValue(), camera->getFarValue());
}

void Camera3D::setPlanes(const Vector2D& planes) {
    camera->setNearValue(planes.x);
    camera->setFarValue(planes.y);
}

float Camera3D::getFOV() {
    return camera->getFOV() * irr::core::RADTODEG;
}

void Camera3D::setFOV(float val) {
    camera->setFOV(irr::core::DEGTORAD * val);
}

bool Camera3D::getVisible() {
    return camera->isVisible();
}

void Camera3D::setVisible(bool val) {
    camera->setVisible(val);
}

Vector3D Camera3D::getForward() {
    irr::core::vector3df f = forwardChild->getAbsolutePosition() - camera->getAbsolutePosition();
    f.normalize();
    return Vector3D(f.X, f.Y, f.Z);
}

Vector3D Camera3D::getLeft() {
    irr::core::vector3df f = leftChild->getAbsolutePosition() - camera->getAbsolutePosition();
    f.normalize();
    return Vector3D(f.X, f.Y, f.Z);
}

Vector3D Camera3D::getUp() {
    irr::core::vector3df f = camera->getUpVector();
    return Vector3D(f.X, f.Y, f.Z);
}

void Camera3D::setUp(const Vector3D& up) {
    camera->setUpVector(irr::core::vector3df(up.x, up.y, up.z));
}

void Camera3D::setActive() {
    if (!camera->isVisible()) return;

    smgr->setActiveCamera(camera);
    mainCamera = camera;
    mainCameraForward = forwardChild;
}

float Camera3D::getAspect() {
    return camera ? camera->getAspectRatio() : 0.0f;
}

void Camera3D::setAspect(float a) {
    if (camera) {
        camera->setAspectRatio(a);
    }
}

bool Camera3D::getOrtho() {
    return camera->isTrulyOrthogonal;
}

void Camera3D::setOrtho(bool val) {
    if (camera)
        camera->isTrulyOrthogonal = val;
}

bool Camera3D::getDebug() {
    return (camera && d);
}

void Camera3D::setDebug(bool visible) {
    if (camera) {
        if (visible && !d) {
            d = new DebugSceneNode(camera, smgr, 0, DebugType::CAMERA);
        }
        else if (!visible && d) {
            d->remove();
            d = nullptr;
        }
    }
}

void Camera3D::addToRenderQueue() {
    if (camera) {
        irrHandler->AddCameraToQueue(camera, forwardChild, true, false);
    }
}

void bindCamera3D() {
    sol::usertype<Camera3D> bindType = lua->new_usertype<Camera3D>("Camera",
        sol::constructors<Camera3D(), Camera3D(const Camera3D & other)>(),

        sol::base_classes, sol::bases<Compatible3D>(),

        "position", sol::property(&Camera3D::getPosition, &Camera3D::setPosition),
        "rotation", sol::property(&Camera3D::getRotation, &Camera3D::setRotation),
        "up", sol::property(&Camera3D::getUp, &Camera3D::setUp),
        "viewPlanes", sol::property(&Camera3D::getPlanes, &Camera3D::setPlanes),
        "fieldOfView", sol::property(&Camera3D::getFOV, &Camera3D::setFOV),
        "visible", sol::property(&Camera3D::getVisible, &Camera3D::setVisible),
        "aspectRatio", sol::property(&Camera3D::getAspect, &Camera3D::setAspect),
        "debug", sol::property(&Camera3D::getDebug, &Camera3D::setDebug),
        "orthogonal", sol::property(&Camera3D::getOrtho, &Camera3D::setOrtho)
    );

    bindType["getForward"] = &Camera3D::getForward;
    bindType["getLeft"] = &Camera3D::getLeft;
    bindType["setActive"] = &Camera3D::setActive;
    bindType["destroy"] = &Camera3D::destroy;
    bindType["queue"] = &Camera3D::addToRenderQueue;
}