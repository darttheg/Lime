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
    for (u32 i = 1; i < meshBuffer->Vertices.size(); ++i)
        meshBuffer->BoundingBox.addInternalPoint(meshBuffer->Vertices[i].Pos);

    return meshBuffer;
}

inline SMeshBuffer* genPlane(const vector3df& center, f32 width, f32 depth, u32 nx = 1, u32 ny = 1) {
    nx = std::max<u32>(1, nx);
    ny = std::max<u32>(1, ny);

    SMeshBuffer* mb = new SMeshBuffer();
    const f32 hx = width * 0.5f, hz = depth * 0.5f;
    const u32 base = (u32)mb->Vertices.size();
    const u32 vx = nx + 1, vy = ny + 1;

    for (u32 y = 0; y < vy; ++y) {
        const f32 v = (f32)y / ny;
        const f32 z = -hz + v * depth;
        for (u32 x = 0; x < vx; ++x) {
            const f32 u = (f32)x / nx;
            const f32 px = -hx + u * width;
            vector3df pos = vector3df(px, 0.f, z) + center;
            mb->Vertices.push_back(S3DVertex(
                pos,
                vector3df(0, 1, 0),
                SColor(255, 255, 255, 255),
                vector2df(u, 1.f - v)
            ));
        }
    }

    for (u32 y = 0; y < ny; ++y) {
        for (u32 x = 0; x < nx; ++x) {
            const u32 i0 = base + y * vx + x;
            const u32 i1 = base + y * vx + x + 1;
            const u32 i2 = base + (y + 1) * vx + x;
            const u32 i3 = base + (y + 1) * vx + x + 1;

            mb->Indices.push_back((u16)i0); mb->Indices.push_back((u16)i2); mb->Indices.push_back((u16)i1);
            mb->Indices.push_back((u16)i1); mb->Indices.push_back((u16)i2); mb->Indices.push_back((u16)i3);
        }
    }

    mb->BoundingBox.reset(mb->Vertices[0].Pos);
    for (u32 i = 1; i < mb->Vertices.size(); ++i)
        mb->BoundingBox.addInternalPoint(mb->Vertices[i].Pos);

    return mb;
}

inline SMeshBuffer* genCube(const vector3df& center, f32 sx, f32 sy, f32 sz) {
    SMeshBuffer* mb = new SMeshBuffer();
    const f32 hx = sx * 0.5f, hy = sy * 0.5f, hz = sz * 0.5f;

    struct Face { vector3df n; vector3df a, b, c, d; } faces[6] = {
        { { 0, 1, 0}, { -hx, hy, -hz}, {  hx, hy, -hz}, {  hx, hy,  hz}, { -hx, hy,  hz} }, // +Y top
        { { 0,-1, 0}, { -hx,-hy,  hz}, {  hx,-hy,  hz}, {  hx,-hy, -hz}, { -hx,-hy, -hz} }, // -Y bottom
        { { 0, 0, 1}, { -hx, hy,  hz}, {  hx, hy,  hz}, {  hx,-hy,  hz}, { -hx,-hy,  hz} }, // +Z front
        { { 0, 0,-1}, {  hx, hy, -hz}, { -hx, hy, -hz}, { -hx,-hy, -hz}, {  hx,-hy, -hz} }, // -Z back
        { { 1, 0, 0}, {  hx, hy,  hz}, {  hx, hy, -hz}, {  hx,-hy, -hz}, {  hx,-hy,  hz} }, // +X right
        { {-1, 0, 0}, { -hx, hy, -hz}, { -hx, hy,  hz}, { -hx,-hy,  hz}, { -hx,-hy, -hz} }, // -X left
    };

    const vector2df uv[4] = { {0,0}, {1,0}, {1,1}, {0,1} };
    const u32 base = mb->Vertices.size();

    for (u32 f = 0; f < 6; ++f) {
        const Face& F = faces[f];
        const vector3df pts[4] = { F.a + center, F.b + center, F.c + center, F.d + center };
        for (u32 k = 0; k < 4; ++k)
            mb->Vertices.push_back(S3DVertex(pts[k], F.n, SColor(255, 255, 255, 255), uv[k]));
        u16 i = (u16)(base + f * 4);
        mb->Indices.push_back(i + 0); mb->Indices.push_back(i + 1); mb->Indices.push_back(i + 2);
        mb->Indices.push_back(i + 0); mb->Indices.push_back(i + 2); mb->Indices.push_back(i + 3);
    }

    mb->BoundingBox.reset(mb->Vertices[0].Pos);
    for (u32 i = 1; i < mb->Vertices.size(); ++i) mb->BoundingBox.addInternalPoint(mb->Vertices[i].Pos);
    return mb;
}