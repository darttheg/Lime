#include "StaticMesh.h"
#include <filesystem>

StaticMesh::StaticMesh() : meshNode(nullptr), selector(nullptr), collisionEnabled(false),
vColor(), shadow(ESM_EXCLUDE), hadShadow(false) {
}

StaticMesh::StaticMesh(const std::string& filePath) : StaticMesh() {
    meshPath = filePath;
    loadMesh(meshPath);
}

StaticMesh::StaticMesh(const StaticMesh& other) : StaticMesh() {
    meshNode = other.meshNode;
    meshPath = other.meshPath;

    if (collisionEnabled != other.collisionEnabled)
        setCollision(other.collisionEnabled);

    collisionEnabled = other.collisionEnabled;
}

StaticMesh::StaticMesh(irr::scene::IAnimatedMeshSceneNode* node) : StaticMesh() {
    meshNode = node;
}

std::string StaticMesh::getMesh() const {
    return meshPath;
}

bool StaticMesh::loadMesh(const std::string& filePath) {
    return fullLoadMesh(filePath, false);
}

bool StaticMesh::loadMeshWithTangents(const std::string& filePath) {
    return fullLoadMesh(filePath, true);
}

bool StaticMesh::fullLoadMesh(const std::string& filePath, bool doTangents) {
    irr::scene::IAnimatedMesh* mesh = nullptr;

    if (!doTangents) {
        mesh = smgr->getMesh(filePath.c_str());
    } else {
        irr::scene::IMeshManipulator* manipulator = smgr->getMeshManipulator();
        mesh = manipulator->createAnimatedMesh(manipulator->createMeshWithTangents(smgr->getMesh(filePath.c_str())->getMesh(0)));
    }

    if (!mesh)
        return false;

    meshPath = filePath;
    meshNode = smgr->addAnimatedMeshSceneNode(mesh);
    if (!meshNode) return false;

    meshNode->grab();
    for (u32 i = 0; i < meshNode->getMaterialCount(); ++i)
        meshNode->getMaterial(i).Lighting = false;

    mesh->drop();

    if (irrHandler->defaultExclude)
        effects->excludeNodeFromLightingCalculations(meshNode);

    return true;
}

void StaticMesh::deload() {
    if (meshNode) {
        effects->removeShadowFromNode(meshNode);
        meshNode->remove();
        meshNode = nullptr;
        meshPath.clear();
    }
}

void StaticMesh::exclude() {
    if (meshNode)
        effects->excludeNodeFromLightingCalculations(meshNode);
}

bool StaticMesh::getCollision() const {
    return meshNode ? collisionEnabled : false;
}

void StaticMesh::setCollision(bool enable) {
    if (enable && meshNode) {
        selector = smgr->createTriangleSelector(meshNode);
        meshNode->setTriangleSelector(selector);
        selector->drop();
        collisionEnabled = true;
    }
    else if (meshNode) {
        meshNode->setTriangleSelector(nullptr);
        collisionEnabled = false;
    }
}

int StaticMesh::getShadows() {
    return meshNode ? shadow : 0;
}

void StaticMesh::setShadows(int i) {
    if (meshNode) {
        shadow = i;
        E_SHADOW_MODE mode = (E_SHADOW_MODE)i;
        if (!hadShadow && (mode == E_SHADOW_MODE::ESM_BOTH || mode == E_SHADOW_MODE::ESM_CAST)) {
            effects->addShadowToNode(meshNode, irrHandler->defaultShadowFiltering, mode);
            hadShadow = true;
        }
        else if (hadShadow) {
            effects->removeShadowFromNode(meshNode);
            hadShadow = false;
        }
    }
}

unsigned int StaticMesh::getVertexCount() const {
    if (!meshNode) return 0;
    unsigned int vertexCount = 0;
    irr::scene::IAnimatedMesh* mesh = meshNode->getMesh();
    if (mesh) {
        for (unsigned int i = 0; i < mesh->getMeshBufferCount(); ++i) {
            vertexCount += mesh->getMeshBuffer(i)->getVertexCount();
        }
    }
    return vertexCount;
}

unsigned int StaticMesh::getMaterialCount() const {
    return meshNode ? meshNode->getMaterialCount() : 0;
}

bool StaticMesh::loadMaterial(const Material& material, int slot) {
    if (!meshNode || slot < 0 || slot >= meshNode->getMaterialCount()) return false;

    meshNode->getMaterial(slot) = material.mat;
    //meshNode->getMaterial(slot).Lighting = false;

    return true;
}

bool StaticMesh::getVisibility() const {
    return meshNode ? meshNode->isVisible() : false;
}

void StaticMesh::setVisibility(bool visible) {
    if (meshNode) {
        meshNode->setVisible(visible);
    }
}

Vector3D StaticMesh::getPosition() {
    return meshNode ? Vector3D(meshNode->getPosition().X, meshNode->getPosition().Y, meshNode->getPosition().Z) : Vector3D();
}

void StaticMesh::setPosition(const Vector3D& pos) {
    if (meshNode) {
        meshNode->setPosition(irr::core::vector3df(pos.x, pos.y, pos.z));
    }
}

Vector3D StaticMesh::getRotation() {
    return meshNode ? Vector3D(meshNode->getRotation().X, meshNode->getRotation().Y, meshNode->getRotation().Z) : Vector3D();
}

void StaticMesh::setRotation(const Vector3D& rot) {
    if (meshNode) {
        meshNode->setRotation(irr::core::vector3df(rot.x, rot.y, rot.z));
    }
}

Vector3D StaticMesh::getScale() {
    return meshNode ? Vector3D(meshNode->getScale().X, meshNode->getScale().Y, meshNode->getScale().Z) : Vector3D();
}

void StaticMesh::setScale(const Vector3D& scale) {
    if (meshNode) {
        meshNode->setScale(irr::core::vector3df(scale.x, scale.y, scale.z));
    }
}

int StaticMesh::getID() {
    return meshNode ? meshNode->getID() : -1;
}

void StaticMesh::setID(int i) {
    if (meshNode) {
        meshNode->setID(i);
    }
}

int StaticMesh::getFrame() {
    return meshNode ? meshNode->getFrameNr() : 0;
}

void StaticMesh::setFrame(int i) {
    if (meshNode) {
        meshNode->setCurrentFrame(i);
    }
}

int StaticMesh::getFrameCount() {
    return meshNode ? meshNode->getEndFrame() - meshNode->getStartFrame() + 1 : 0;
}

sol::table StaticMesh::getBoneData(irr::scene::IBoneSceneNode* bone) {
    sol::table boneInfo = lua->create_table();
    boneInfo["name"] = "";
    boneInfo["head"] = Vector3D();
    boneInfo["tail"] = Vector3D();
    boneInfo["rotation"] = Vector3D();

    if (bone) {
        Vector3D head(bone->getPosition().X, bone->getPosition().Y, bone->getPosition().Z);
        Vector3D tail(
            bone->getAbsolutePosition().X,
            bone->getAbsolutePosition().Y,
            bone->getAbsolutePosition().Z
        );
        boneInfo["name"] = bone->getName();
        boneInfo["rotation"] = Vector3D(bone->getRotation().X, bone->getRotation().Y, bone->getRotation().Z);
        boneInfo["headPosition"] = head;
        boneInfo["tailPosition"] = tail;
        boneInfo["length"] = sqrt(pow(tail.x - head.x, 2) + pow(tail.y - head.y, 2) + pow(tail.z - head.z, 2));
    }

    return boneInfo;
}

sol::table StaticMesh::getBoneInfoByIndex(int i) {
    irr::scene::IBoneSceneNode* bone = nullptr;
    if (meshNode) {
        bone = meshNode->getJointNode(i);
    }
    return getBoneData(bone);
}

sol::table StaticMesh::getBoneInfoByName(const std::string& name) {
    irr::scene::IBoneSceneNode* bone = nullptr;
    if (meshNode) {
        bone = meshNode->getJointNode(name.c_str());
    }
    return getBoneData(bone);
}

void StaticMesh::normalizeNormals(bool enable) {
    if (meshNode) {
        meshNode->setMaterialFlag(irr::video::EMF_NORMALIZE_NORMALS, enable);
    }
}

bool StaticMesh::getDebug() {
    return meshNode ? meshNode->isDebugDataVisible() : false;
}

void StaticMesh::setDebug(bool visible) {
    if (meshNode) {
        meshNode->setDebugDataVisible(visible ? irr::scene::EDS_FULL : irr::scene::EDS_OFF);
    }
}

sol::table StaticMesh::getBoundingBox() {
    sol::table result = lua->create_table();
    result["min"] = Vector3D();
    result["max"] = Vector3D();

    if (meshNode) {
        core::aabbox3d<f32> bb = meshNode->getTransformedBoundingBox();
        result["min"] = Vector3D(bb.MinEdge.X, bb.MinEdge.Y, bb.MinEdge.Z);
        result["max"] = Vector3D(bb.MaxEdge.X, bb.MaxEdge.Y, bb.MaxEdge.Z);
    }

    return result;
}

void StaticMesh::recalculateBoundingBox() {
    if (!meshNode) return;
    meshNode->getMesh()->getMeshBuffer(0)->recalculateBoundingBox();
}

void StaticMesh::makePlanarMapping() {
    if (meshNode) {
        smgr->getMeshManipulator()->makePlanarTextureMapping(meshNode->getMesh(), 0.004f);
    }
}

void StaticMesh::setHardwareHint(int i) {
    if (meshNode) {
        meshNode->getMesh()->setHardwareMappingHint((irr::scene::E_HARDWARE_MAPPING)i);
    }
}

Vector4D StaticMesh::getVColor() {
    return Vector4D(vColor.getRed(), vColor.getGreen(), vColor.getBlue(), vColor.getAlpha());
}

void StaticMesh::setVColor(const Vector4D& col) {
    vColor = irr::video::SColor(col.w, col.x, col.y, col.z);
    if (meshNode) {
        irr::scene::IMeshManipulator* meshManipulator = device->getSceneManager()->getMeshManipulator();
        meshManipulator->setVertexColors(meshNode->getMesh(), vColor);
    }
}

bool StaticMesh::writeToFile(std::string path) {
    if (!meshNode) return false;

    IMesh* mesh = meshNode->getMesh();
    IMeshWriter* writer = smgr->createMeshWriter(EMESH_WRITER_TYPE::EMWT_COLLADA);

    irr::io::IWriteFile* fs = device->getFileSystem()->createAndWriteFile(path.c_str());
    return writer->writeMesh(fs, mesh, 0);
}

bool StaticMesh::loadMeshViaBuffer(const MeshBuffer& b) {
    if (meshNode) meshNode->drop();
    SMesh* m = new SMesh();
    m->addMeshBuffer(b.getBuffer());

    m->recalculateBoundingBox();

    irr::scene::IMeshManipulator* meshManipulator = device->getSceneManager()->getMeshManipulator();
    meshNode = smgr->addAnimatedMeshSceneNode(meshManipulator->createAnimatedMesh(m));

    m->drop();

    if (!meshNode)
        return false;

    if (irrHandler->defaultExclude)
        effects->excludeNodeFromLightingCalculations(meshNode);

    return true;
}

void StaticMesh::setAutomaticCulling(bool enable) {
    if (!meshNode) return;

    meshNode->setAutomaticCulling(enable ? EAC_BOX : EAC_OFF);
}

void bindStaticMesh() {
    sol::usertype<StaticMesh> bindType = lua->new_usertype<StaticMesh>("Mesh",
        sol::constructors<StaticMesh(), StaticMesh(const std::string & filePath), StaticMesh(const StaticMesh & other)>(),

        sol::base_classes, sol::bases<Compatible3D>(),

        "collision", sol::property(&StaticMesh::getCollision, &StaticMesh::setCollision),
        "visible", sol::property(&StaticMesh::getVisibility, &StaticMesh::setVisibility),
        "position", sol::property(&StaticMesh::getPosition, &StaticMesh::setPosition),
        "rotation", sol::property(&StaticMesh::getRotation, &StaticMesh::setRotation),
        "scale", sol::property(&StaticMesh::getScale, &StaticMesh::setScale),
        "ID", sol::property(&StaticMesh::getID, &StaticMesh::setID),
        "frame", sol::property(&StaticMesh::getFrame, &StaticMesh::setFrame),
        "debug", sol::property(&StaticMesh::getDebug, &StaticMesh::setDebug),
        "vertexColor", sol::property(&StaticMesh::getVColor, &StaticMesh::setVColor),
        "shadows", sol::property(&StaticMesh::getShadows, &StaticMesh::setShadows));

    bindType["load"] = &StaticMesh::loadMesh;
    bindType["loadWithTangents"] = &StaticMesh::loadMeshWithTangents;
    bindType["loadFromBuffer"] = &StaticMesh::loadMeshViaBuffer;
    bindType["loadMaterial"] = &StaticMesh::loadMaterial;
    bindType["destroy"] = &StaticMesh::deload;
    bindType["getVertexCount"] = &StaticMesh::getVertexCount;
    bindType["getMaterialCount"] = &StaticMesh::getMaterialCount;
    bindType["toStr"] = &StaticMesh::getMesh;
    bindType["getBoneDataByIndex"] = &StaticMesh::getBoneInfoByIndex;
    bindType["getBoneDataByName"] = &StaticMesh::getBoneInfoByName;
    bindType["getFrameCount"] = &StaticMesh::getFrameCount;
    bindType["normalizeNormals"] = &StaticMesh::normalizeNormals;
    bindType["getBoundingBox"] = &StaticMesh::getBoundingBox;
    bindType["toPlanarMapping"] = &StaticMesh::makePlanarMapping;
    bindType["setHardwareMappingHint"] = &StaticMesh::setHardwareHint;
    bindType["ignoreLighting"] = &StaticMesh::exclude;
    bindType["writeToFile"] = &StaticMesh::writeToFile;
    bindType["setAutomaticCulling"] = &StaticMesh::setAutomaticCulling;
}