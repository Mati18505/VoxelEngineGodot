#pragma once
#include "VoxelTypes.h"

namespace Voxel {
class World;

class Biome {
public:
	const std::string biomeName;

	float majorFloraZoneScale;
	float majorFloraZoneThreshold;
	float majorFloraPlacementScale;
	float majorFloraPlacementThreshold;
	bool placeMajorFlora;

	// TODO: z�o�a

	BlockID atmosphereBlock{ 0 }, layer1stBlock{ 0 }, layer2ndBlock{ 0 }, layer3rdBlock{ 0 };

	Biome(std::string biomeName) :
			biomeName(biomeName) {}
};
}
