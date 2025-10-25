#include "MeshBuffer.h"

using namespace irr;
using namespace core;
using namespace scene;

MeshBuffer::MeshBuffer() {
	buffer = new irr::scene::SMeshBuffer();
    bbox.reset(vector3df(0,0,0));
}

MeshBuffer::~MeshBuffer() {
    currentIndex = 0;
	buffer->drop();
}

void MeshBuffer::pushFace(const Vector3D& v1, const Vector3D& v2, const Vector3D& v3,
                          const Vector3D& n1, const Vector3D& n2, const Vector3D& n3,
                          const Vector2D& uvw1, const Vector2D& uvw2, const Vector2D& uvw3,
                          const Vector4D& c1, const Vector4D& c2, const Vector4D& c3) {
    if (!buffer) return;

    buffer->Vertices.reallocate(buffer->Vertices.size() + 3);
    buffer->Vertices.set_used(buffer->Vertices.size() + 3);

    buffer->Vertices[currentIndex] = S3DVertex(vector3df(v1.getX(), v1.getY(), v1.getZ()), vector3df(n1.getX(), n1.getY(), n1.getZ()), SColor(c1.getW(), c1.getX(), c1.getY(), c1.getZ()), vector2df(uvw1.getX(), uvw1.getY()));
    buffer->Vertices[currentIndex + 1] = S3DVertex(vector3df(v2.getX(), v2.getY(), v2.getZ()), vector3df(n2.getX(), n2.getY(), n2.getZ()), SColor(c2.getW(), c2.getX(), c2.getY(), c2.getZ()), vector2df(uvw2.getX(), uvw2.getY()));
    buffer->Vertices[currentIndex + 2] = S3DVertex(vector3df(v3.getX(), v3.getY(), v3.getZ()), vector3df(n3.getX(), n3.getY(), n3.getZ()), SColor(c3.getW(), c3.getX(), c3.getY(), c3.getZ()), vector2df(uvw3.getX(), uvw3.getY()));

    if (currentIndex == 0)
        bbox.reset(buffer->Vertices[currentIndex].Pos);

    bbox.addInternalPoint(buffer->Vertices[currentIndex].Pos);
    bbox.addInternalPoint(buffer->Vertices[currentIndex + 1].Pos);
    bbox.addInternalPoint(buffer->Vertices[currentIndex + 2].Pos);

    buffer->Indices.reallocate(buffer->Indices.size() + 3);
    buffer->Indices.set_used(buffer->Indices.size() + 3);

    buffer->Indices[currentIndex] = currentIndex;
    buffer->Indices[currentIndex + 1] = currentIndex + 1;
    buffer->Indices[currentIndex + 2] = currentIndex + 2;

    buffer->recalculateBoundingBox();

    currentIndex += 3;
}

void MeshBuffer::clear() {
    if (buffer) {
        buffer->drop();
        delete buffer;
        currentIndex = 0;
        buffer = new irr::scene::SMeshBuffer();
    }
}

void MeshBuffer::destroy() {
    if (buffer) {
        buffer->drop();
        delete buffer;
        currentIndex = 0;
    }
}

void MeshBuffer::recalculateBoundingBox() {
    if (!buffer) return;

    bbox.reset(buffer->Vertices[0].Pos);
    for (s32 i = 1; i < buffer->Vertices.size(); i++) {
        bbox.addInternalPoint(buffer->Vertices[i].Pos);
    }
}

int MeshBuffer::getVertexCount() const {
    return buffer ? buffer->Vertices.size() : 0;
}

void MeshBuffer::createCapsule(float radius, float height, int rings, int sectors) {
    if (buffer) {
        buffer->drop();
        buffer = new irr::scene::SMeshBuffer();
    }

    buffer = genCapsule(vector3df(0,-height/2, 0), radius, height, rings, sectors);
}

void MeshBuffer::createCube(float sizeX, float sizeY, float sizeZ) {
    if (buffer) {
        buffer->drop();
        buffer = new irr::scene::SMeshBuffer();
    }

    buffer = genCube(vector3df(0), sizeX, sizeY, sizeZ);
}

void MeshBuffer::createPlane(float sizeX, float sizeZ, float segX, float segZ) {
    if (buffer) {
        buffer->drop();
        buffer = new irr::scene::SMeshBuffer();
    }

    buffer = genPlane(vector3df(0), sizeX, sizeZ, segX, segZ);
}

irr::scene::SMeshBuffer* MeshBuffer::getBuffer() const {
	return buffer;
}

void bindMeshBuffer() {
    sol::usertype<MeshBuffer> bindType = lua->new_usertype<MeshBuffer>("MeshBuffer",
        sol::constructors<MeshBuffer()>(),
        sol::meta_function::type, [](const MeshBuffer&) { return "MeshBuffer"; }
    );

    bindType["pushFace"] = &MeshBuffer::pushFace;
    bindType["destroy"] = &MeshBuffer::destroy;
    bindType["clear"] = &MeshBuffer::clear;
    bindType["getVertexCount"] = &MeshBuffer::getVertexCount;
    bindType["createCapsule"] = &MeshBuffer::createCapsule;
    bindType["createCube"] = &MeshBuffer::createCube;
    bindType["createPlane"] = &MeshBuffer::createPlane;
}
