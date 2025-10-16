#pragma once

#include "irrlicht.h"
#include "IrrManagers.h"
#include "Material.h"
#include "StaticMesh.h"
#include "Vector2D.h"
#include "Vector3D.h"
#include "LuaLime.h"

#include "Compatible3D.h"

class VolumeLight : public Compatible3D {
private:
	irr::scene::IVolumeLightSceneNode* vol;
public:
	VolumeLight();
	VolumeLight(const Vector4D& footCol, const Vector4D& tailCol);
	VolumeLight(const Vector4D& footCol, const Vector4D& tailCol, int segmentU, int segmentV);

	void destroy() override;
	irr::scene::ISceneNode* getNode() const override { return vol; }

	int getSegmentU();
	void setSegmentU(int u);
	int getSegmentV();
	void setSegmentV(int v);

	void setSegments(int uv);
	
	Vector4D getFootColor();
	void setFootColor(const Vector4D& c);

	Vector4D getTailColor();
	void setTailColor(const Vector4D& c);
};

void bindVolumeLight();