#pragma once
#include <map>
#include <string>

namespace Voxel {
	class TextParser {
	public:
		void Parse(const std::string &text, char separator = ':');
	
		bool GetInt(const std::string &propertyName, int &variable) const;
		bool GetBool(const std::string &propertyName, bool &variable) const;
		float GetFloat(const std::string &propertyName, float &variable) const;
	private:
		std::map<std::string, std::string> data;

		std::string EraseCharacters(std::string s, const char value) const;
	};
}
