#pragma once
#include "VoxelTypes.h"
#include <string>

namespace Voxel {
	class BlockType {
		
		bool isTransparent;
		bool isTranslucent;
		bool isEveryBlockSideSame;
		std::string materialName;
		int topTextureIndex;
		int sideTextureIndex;
		int bottomTextureIndex;
	
	public:
		std::string name;
		BlockType(std::string name, bool isTransparent, bool isEveryBlockSideSame, std::string materialName = "default", bool isTranslucent = false);
		
		int GetBlockSideTextureIndex(BlockSide side);
	
		void SetTopTextureIndex(int textureIndex) {
			topTextureIndex = textureIndex;
		}
		void SetBlockSideTextureIndex(int textureIndex) {
			sideTextureIndex = textureIndex;
		}
		void SetBottomTextureIndex(int textureIndex) {
			bottomTextureIndex = textureIndex;
		}
	
		bool GetIsTransparent() {
			return isTransparent;
		}
		bool GetIsTranslucent() { //true jeśli przepuszcza światło
			return isTranslucent;
		}
		bool GetIsSolid() {//false jeśli nie można zniszczyć (raycastBlock nie działa)
			return name != "air";
		}
		const std::string& GetMaterialName() {
			return materialName;
		}
	};
}
