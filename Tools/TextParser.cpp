#include "TextParser.h"
#include <sstream>

namespace Voxel {

	void TextParser::Parse(const std::string &text, char separator) {
		std::istringstream iss(text);

		for (std::string line; std::getline(iss, line);) {
			line = EraseCharacters(std::move(line), '\r');
			line = EraseCharacters(std::move(line), '\n');

			size_t sepPos = line.find(separator);
			std::string key = line.substr(0, sepPos);
			std::string value = line.substr(sepPos + 1);

			data.insert({ key, value });
		}
	}

	bool TextParser::GetInt(const std::string &propertyName, int &variable) const {
		auto finded = data.find(propertyName);
		if (!(finded == data.end())) {
			variable = std::stoi(finded->second);
			return true;
		}
		return false;
	}


	bool TextParser::GetBool(const std::string &propertyName, bool &variable) const {
		auto finded = data.find(propertyName);
		if (!(finded == data.end())) {
			if (finded->second == "true")
				variable = true;
			else
				variable = false;
			return true;
		}
		return false;
	}


	float TextParser::GetFloat(const std::string &propertyName, float &variable) const {
		auto finded = data.find(propertyName);
		if (!(finded == data.end())) {
			variable = std::stof(finded->second);
			return true;
		}
		return false;
	}

	std::string TextParser::EraseCharacters(std::string s, const char value) const {
		auto it = std::remove(s.begin(), s.end(), value);
		s.erase(it, s.end());

		return s;
	}
}
