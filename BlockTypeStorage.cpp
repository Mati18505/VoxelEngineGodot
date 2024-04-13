#include "BlockTypeStorage.h"

Voxel::BlockID Voxel::BlockTypeStorage::GetBlockTypeIDFromName(const std::string &typeName) const {
	using namespace Voxel;

	auto finded = std::find_if(blockTypes.begin(), blockTypes.end(), [&typeName](const BlockType& blockType) { return blockType.name == typeName; });

	if (finded == blockTypes.end())
		throw std::invalid_argument("BlockType name does not exist!");

	return finded - blockTypes.begin();
}
