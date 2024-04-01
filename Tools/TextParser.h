#pragma once
#include <map>
#include <string>

namespace Voxel {
	class TextParser {
	public:
		void Parse(const std::string &text, char separator = ':');
	
		bool GetInt(const std::string &propertyName, int &variable);
		bool GetBool(const std::string &propertyName, bool &variable);
		float GetFloat(const std::string &propertyName, float &variable);
	private:
		std::map<std::string, std::string> data;

		std::string EraseCharacters(const std::string &string, const char value);
	};
}
