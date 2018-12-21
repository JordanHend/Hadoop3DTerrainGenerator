#include "ConfigReader.h"



ConfigReader::ConfigReader()
{
}

void ConfigReader::read(float &heightscale, float &blockscale, float &radius, float &r, float &g, float &b)
{

	std::string line;
	std::ifstream file("config.cfg");
	if (file.is_open())
	{
		while (std::getline(file, line))
		{
				std::istringstream is_line(line);
				std::string key;
				if (std::getline(is_line, key, '='))
				{
					std::string value;
					if (std::getline(is_line, value))
					{
						if (key == "heightscale")
							heightscale = std::atoi(value.c_str());
						if (key == "blockscale")
							blockscale = std::atoi(value.c_str());
						if (key == "ambient_r")
							r = std::atof(value.c_str());
						if (key == "ambient_g")
							g = std::atof(value.c_str());
						if (key == "ambient_b")
							b = std::atof(value.c_str());
						if (key == "radius")
							radius = std::atoi(value.c_str());
					}
				}
			
		}
	}
}

ConfigReader::~ConfigReader()
{
}
