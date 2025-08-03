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

    buffer->Vertices[currentIndex] = S3DVertex(vector3df(v1.x, v1.y, v1.z), vector3df(n1.x, n1.y, n1.z), SColor(c1.w, c1.x, c1.y, c1.z), vector2df(uvw1.x, uvw1.y));
    buffer->Vertices[currentIndex + 1] = S3DVertex(vector3df(v2.x, v2.y, v2.z), vector3df(n2.x, n2.y, n2.z), SColor(c2.w, c2.x, c2.y, c2.z), vector2df(uvw2.x, uvw2.y));
    buffer->Vertices[currentIndex + 2] = S3DVertex(vector3df(v3.x, v3.y, v3.z), vector3df(n3.x, n3.y, n3.z), SColor(c3.w, c3.x, c3.y, c3.z), vector2df(uvw3.x, uvw3.y));

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

irr::scene::SMeshBuffer* MeshBuffer::getBuffer() const {
	return buffer;
}

void bindMeshBuffer() {
    sol::usertype<MeshBuffer> bind_type = lua->new_usertype<MeshBuffer>("MeshBuffer",
        sol::constructors<MeshBuffer()>()
    );

    bind_type["pushFace"] = &MeshBuffer::pushFace;
    bind_type["destroy"] = &MeshBuffer::destroy;
    bind_type["clear"] = &MeshBuffer::clear;
    bind_type["getVertexCount"] = &MeshBuffer::getVertexCount;
}
