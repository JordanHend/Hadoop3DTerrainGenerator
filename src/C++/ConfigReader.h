#ifndef CONFIG_READER_H
#define CONFIG_READER_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
class ConfigReader
{
public:
	ConfigReader();
	void read(float &heightscale, float &blockscale, float &radius, float &r, float &g, float &b);
	~ConfigReader();
};

#endif