#pragma once
#include "VoxelTypes.h"
#include <string>

namespace Voxel {
	struct BlockType {
		bool isTransparent;
		bool isTranslucent;
		bool isEveryBlockSideSame;
		std::string materialName;

		int topTextureIndex = -1;
		int sideTextureIndex = -1;
		int bottomTextureIndex = -1;

		// If false - does not affect raycast.
		bool isSolid;

		std::string name;

		BlockType(std::string name, bool isTransparent, bool isEveryBlockSideSame, std::string materialName = "default", bool isTranslucent = false, bool isSolid = true);
		
		int GetBlockSideTextureIndex(BlockSide side) const;
	};
}
