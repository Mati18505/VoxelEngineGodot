#pragma once
#include "BlockType.h"

namespace Voxel {
	BlockType::BlockType(std::string name, bool isTransparent, bool isEveryBlockSideSame, std::string materialName, bool isTranslucent)
	{
		this->name = name;
		this->isTransparent = isTransparent;
		this->isEveryBlockSideSame = isEveryBlockSideSame;
		this->materialName = materialName;
		this->isTranslucent = isTranslucent;
	}
	
	int BlockType::GetBlockSideTextureIndex(BlockSide side) {
		if (isEveryBlockSideSame)
			return sideTextureIndex;
		if (side == BlockSide::TOP)
			return topTextureIndex;
		if (side == BlockSide::BOTTOM)
			return bottomTextureIndex;
		return sideTextureIndex;
	}
}
