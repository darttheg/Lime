#pragma once

#include "irrlicht.h"
#include "IrrManagers.h"
#include "Vector3D.h"
#include "Vector2D.h"
#include "Vector4D.h"

class MeshBuffer {
public:
	MeshBuffer();
	~MeshBuffer();

	void pushFace(const Vector3D& v1, const Vector3D& v2, const Vector3D& v3,
		const Vector3D& n1, const Vector3D& n2, const Vector3D& n3,
		const Vector2D& uvw1, const Vector2D& uvw2, const Vector2D& uvw3,
		const Vector4D& c1, const Vector4D& c2, const Vector4D& c3);
	void clear();
	void destroy();
	void recalculateBoundingBox();
	int getVertexCount() const;
	irr::scene::SMeshBuffer* getBuffer() const;

	u16 currentIndex = 0;
private:
	irr::scene::SMeshBuffer* buffer;
	core::aabbox3d<f32> bbox;

	void addVertex(const vector3df& pos, const vector3df& normal, SColor color, const vector2df& uvw) {
		S3DVertex vertex;
		vertex.Pos = pos;
		vertex.Normal = normal;
		vertex.Color = color;
		vertex.TCoords = uvw;
		buffer->Vertices.push_back(vertex);
	}
};

void bindMeshBuffer();