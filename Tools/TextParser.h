#pragma once
#include <iostream>
#include <fstream>
#include <map>

#include <memory>
#include <string>

namespace Voxel {
	class TextParser {
		std::map<std::string, std::string> data;
	
	public:
		void Parse(std::string fileName, char separator = ':') {
			std::ifstream input;
			input.open(fileName);
			if (!input.is_open())
			{
				return;
			}
			while (!input.eof())
			{
				std::string line;
				std::getline(input, line, '\n');
	
				int index = 0;
				std::string lineValue[2];
				for (int i = 0; i < line.size(); i++)
				{
					if (line[i] == separator)
						index++;
					else
						lineValue[index] += line[i];
				}
	
				data.insert(std::pair<std::string, std::string>(lineValue[0], lineValue[1]));
			}
			//for (std::map<std::string, void*>::iterator it = variablesToRead.begin(); it != variablesToRead.end(); ++it)
	
	
			input.close();
		}
	
		bool GetInt(std::string propertyName, int& variable) {
			auto finded = data.find(propertyName);
			if (!(finded == data.end())) {
				variable = std::stoi(finded->second);
				return true;
			}
			return false;
		}
		
		bool GetBool(std::string propertyName, bool& variable) {
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
		
		float GetFloat(std::string propertyName, float& variable) {
			auto finded = data.find(propertyName);
			if (!(finded == data.end())) {
				variable = std::stof(finded->second);
				return true;
			}
			return false;
		}
	};
}
