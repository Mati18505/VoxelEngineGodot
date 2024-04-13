#pragma once
#include "BlockType.h"

namespace Voxel {
	BlockType::BlockType(std::string name, bool isTransparent, bool isEveryBlockSideSame, std::string materialName, bool isTranslucent, bool isSolid) :
		name(name), isTransparent(isTransparent), isEveryBlockSideSame(isEveryBlockSideSame), materialName(materialName), isTranslucent(isTranslucent), isSolid(isSolid)
	{}
	
	int BlockType::GetBlockSideTextureIndex(BlockSide side) const {
		if (isEveryBlockSideSame)
			return sideTextureIndex;
		if (side == BlockSide::TOP)
			return topTextureIndex;
		if (side == BlockSide::BOTTOM)
			return bottomTextureIndex;
		return sideTextureIndex;
	}
}
