#include "PerlinNoise.h"

struct PerlinObject {
private:
public:
	uint32_t seed = 0;
	uint32_t octaves = 0;

	siv::PerlinNoise noise;

	void setSeed(uint32_t s) {
		seed = s;
		noise.reseed(seed);
	}

	void setOctaves(uint32_t o) {
		octaves = o;
	}

	float getValueAt(uint32_t type = 0, float x = 0, float y = 0, float z = 0) {
		switch (type) {
		case 1:
			return noise.noise2D(x, y); break;
		case 2:
			return noise.noise3D(x, y, z); break;
		default:
			return noise.noise1D(x); break;
		}
	}
};