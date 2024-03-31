#include "TextureDictionary.h"
#include <core\variant\variant.h>

namespace Voxel {
	TextureDictionary::TextureDictionary(const Dictionary& textures) {
		GenerateTextureIndexDictionary(textures);
	}

	int TextureDictionary::GetBlockTextureIndex(const std::string &blockName) {
		if (blockName == "" || textureIndexDictionary.count(blockName) == 0)
			return 0;
		else
			return textureIndexDictionary.at(blockName);
	}
	void TextureDictionary::GenerateTextureIndexDictionary(const Dictionary &textures) {
		for(int i = 0; i < textures.size(); i++)
		{
			const Variant &key = textures.get_key_at_index(i);
			const Variant &value = textures.get_value_at_index(i);
			String textureName = key;
			int textureIndex = value;

			textureIndexDictionary.emplace(textureName.utf8(), textureIndex);
		}
	}
}

