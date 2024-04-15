#pragma once
#include <string>
#include <core\variant\dictionary.h>
#include <map>

namespace Voxel {
	class TextureDictionary {
	public:
		TextureDictionary(const Dictionary &textures);
		int GetBlockTextureIndex(const std::string &blockName) const;

	private:
		// Block name, textureIndex
		std::map<std::string, int> textureIndexDictionary;
		void GenerateTextureIndexDictionary(const Dictionary &textures);
	};
}
