#pragma once
#include "irrlicht.h"
#include <vector>

using namespace irr;
using namespace scene;
using namespace core;
using namespace video;

#define M_PI 3.14159265358979323846
#define M_PI_2 1.57079632679489661923

inline void genSphere(vector3df center, std::vector<S3DVertex>& vertices, std::vector<u16>& indices,
    float radius, float height, u32 rings, u32 sectors, SColor col) {
    
    float const R = 1.0f / (rings - 1);
    float const S = 1.0f / (sectors - 1);
    float halfHeight = height * 0.5f;
    u32 baseIndex = vertices.size();

    // Generate Top Hemisphere
    for (u32 r = 0; r <= rings / 2; ++r) {
        float v = float(r) / (rings / 2);
        float phi = v * M_PI_2;
        for (u32 s = 0; s < sectors; ++s) {
            float u = float(s) / (sectors - 1);
            float theta = u * 2.0f * M_PI;

            float x = cos(theta) * sin(phi);
            float y = cos(phi);
            float z = sin(theta) * sin(phi);

            vector3df position = vector3df(x * radius, (y * radius) + halfHeight, z * radius) + center;
            vertices.push_back(S3DVertex(position, vector3df(x, y, z), col, vector2df(u, v)));
        }
    }

    // Generate Cylinder
    u32 cylinderStart = vertices.size();
    for (u32 r = 0; r < 2; ++r) {
        float y = (r == 0) ? halfHeight : -halfHeight;
        for (u32 s = 0; s < sectors; ++s) {
            float theta = float(s) / (sectors - 1) * 2.0f * M_PI;
            float x = cos(theta) * radius;
            float z = sin(theta) * radius;

            vector3df position = vector3df(x, y, z) + center;
            vertices.push_back(S3DVertex(position, vector3df(x, 0, z), col, vector2df(float(s) / sectors, float(r))));
        }
    }

    // Generate Bottom Hemisphere
    u32 bottomStart = vertices.size();
    for (u32 r = 0; r <= rings / 2; ++r) {
        float v = float(r) / (rings / 2);
        float phi = M_PI_2 + v * M_PI_2;
        for (u32 s = 0; s < sectors; ++s) {
            float u = float(s) / (sectors - 1);
            float theta = u * 2.0f * M_PI;

            float x = cos(theta) * sin(phi);
            float y = cos(phi);
            float z = sin(theta) * sin(phi);

            vector3df position = vector3df(x * radius, (y * radius) - halfHeight, z * radius) + center;
            vertices.push_back(S3DVertex(position, vector3df(x, y, z), col, vector2df(u, v)));
        }
    }

    // Generate Index Data
    for (u32 r = 0; r < rings / 2; ++r) {
        for (u32 s = 0; s < sectors - 1; ++s) {
            u16 i1 = baseIndex + r * sectors + s;
            u16 i2 = baseIndex + r * sectors + (s + 1);
            u16 i3 = baseIndex + (r + 1) * sectors + s;

            indices.push_back(i1); indices.push_back(i2);
            indices.push_back(i1); indices.push_back(i3);
        }
    }

    for (u32 r = 0; r < 1; ++r) {
        for (u32 s = 0; s < sectors - 1; ++s) {
            u16 i1 = cylinderStart + r * sectors + s;
            u16 i2 = cylinderStart + r * sectors + (s + 1);
            u16 i3 = cylinderStart + (r + 1) * sectors + s;

            indices.push_back(i1); indices.push_back(i2);
            indices.push_back(i1); indices.push_back(i3);
        }
    }

    for (u32 r = 0; r < rings / 2; ++r) {
        for (u32 s = 0; s < sectors - 1; ++s) {
            u16 i1 = bottomStart + r * sectors + s;
            u16 i2 = bottomStart + r * sectors + (s + 1);
            u16 i3 = bottomStart + (r + 1) * sectors + s;

            indices.push_back(i1); indices.push_back(i2);
            indices.push_back(i1); indices.push_back(i3);
        }
    }
}

inline void renderSphere(vector3df pos, float rad = 5.0f, float height = 5.0f, u32 rings = 20, u32 sectors = 20, SColor col = SColor()) {
    std::vector<S3DVertex> vertices;
    std::vector<u16> indices;

    genSphere(pos, vertices, indices, rad, height, rings, sectors, col);

    driver->drawVertexPrimitiveList(vertices.data(), vertices.size(),
        indices.data(), indices.size() / 2,
        EVT_STANDARD, EPT_LINES, EIT_16BIT);
}

// Generate capsule
inline SMeshBuffer* genCapsule(vector3df center, float radius, float height, u32 rings, u32 sectors) {
    SMeshBuffer* meshBuffer = new SMeshBuffer();

    float const R = 1.0f / (rings - 1);
    float const S = 1.0f / (sectors - 1);
    float halfHeight = height * 0.5f;
    u32 baseIndex = meshBuffer->Vertices.size();

    for (u32 r = 0; r <= rings / 2; ++r) {
        float phi = (float(r) / (rings / 2)) * M_PI_2;
        for (u32 s = 0; s < sectors; ++s) {
            float theta = (float(s) / (sectors - 1)) * 2.0f * M_PI;

            float x = cos(theta) * sin(phi);
            float y = cos(phi);
            float z = sin(theta) * sin(phi);

            vector3df position = vector3df(x * radius, (y * radius) + height, z * radius) + center;
            meshBuffer->Vertices.push_back(S3DVertex(position, vector3df(x, y, z), SColor(255, 255, 255, 255), vector2df(float(s) / sectors, float(r) / rings)));
        }
    }

    u32 cylinderStart = meshBuffer->Vertices.size();
    for (u32 r = 0; r < 2; ++r) {
        float y = (r == 0) ? height : 0;
        for (u32 s = 0; s < sectors; ++s) {
            float theta = (float(s) / (sectors - 1)) * 2.0f * M_PI;
            float x = cos(theta) * radius;
            float z = sin(theta) * radius;

            vector3df position = vector3df(x, y, z) + center;
            meshBuffer->Vertices.push_back(S3DVertex(position, vector3df(x, 0, z), SColor(255, 255, 255, 255), vector2df(float(s) / sectors, float(r))));
        }
    }

    u32 bottomStart = meshBuffer->Vertices.size();
    for (u32 r = 0; r <= rings / 2; ++r) {
        float phi = M_PI_2 + (float(r) / (rings / 2)) * M_PI_2;
        for (u32 s = 0; s < sectors; ++s) {
            float theta = (float(s) / (sectors - 1)) * 2.0f * M_PI;

            float x = cos(theta) * sin(phi);
            float y = cos(phi);
            float z = sin(theta) * sin(phi);

            vector3df position = vector3df(x * radius, (y * radius), z * radius) + center;
            meshBuffer->Vertices.push_back(S3DVertex(position, vector3df(x, y, z), SColor(255,255,255,255), vector2df(float(s) / sectors, float(r) / rings)));
        }
    }

    for (u32 r = 0; r < rings / 2; ++r) {
        for (u32 s = 0; s < sectors - 1; ++s) {
            u16 i1 = baseIndex + r * sectors + s;
            u16 i2 = baseIndex + r * sectors + (s + 1);
            u16 i3 = baseIndex + (r + 1) * sectors + s;
            u16 i4 = baseIndex + (r + 1) * sectors + (s + 1);

            meshBuffer->Indices.push_back(i1);
            meshBuffer->Indices.push_back(i2);
            meshBuffer->Indices.push_back(i3);

            meshBuffer->Indices.push_back(i2);
            meshBuffer->Indices.push_back(i4);
            meshBuffer->Indices.push_back(i3);
        }
    }

    for (u32 r = 0; r < 1; ++r) {
        for (u32 s = 0; s < sectors - 1; ++s) {
            u16 i1 = cylinderStart + r * sectors + s;
            u16 i2 = cylinderStart + r * sectors + (s + 1);
            u16 i3 = cylinderStart + (r + 1) * sectors + s;
            u16 i4 = cylinderStart + (r + 1) * sectors + (s + 1);

            meshBuffer->Indices.push_back(i1);
            meshBuffer->Indices.push_back(i2);
            meshBuffer->Indices.push_back(i3);

            meshBuffer->Indices.push_back(i2);
            meshBuffer->Indices.push_back(i4);
            meshBuffer->Indices.push_back(i3);
        }
    }

    for (u32 r = 0; r < rings / 2; ++r) {
        for (u32 s = 0; s < sectors - 1; ++s) {
            u16 i1 = bottomStart + r * sectors + s;
            u16 i2 = bottomStart + r * sectors + (s + 1);
            u16 i3 = bottomStart + (r + 1) * sectors + s;
            u16 i4 = bottomStart + (r + 1) * sectors + (s + 1);

            meshBuffer->Indices.push_back(i1);
            meshBuffer->Indices.push_back(i2);
            meshBuffer->Indices.push_back(i3);

            meshBuffer->Indices.push_back(i2);
            meshBuffer->Indices.push_back(i4);
            meshBuffer->Indices.push_back(i3);
        }
    }

    meshBuffer->BoundingBox.reset(meshBuffer->Vertices[0].Pos);
    for (u32 i = 1; i < meshBuffer->Vertices.size(); ++i) {
        meshBuffer->BoundingBox.addInternalPoint(meshBuffer->Vertices[i].Pos);
    }

    return meshBuffer;
}