#pragma once
#include "GrassSolar.h"
class SolarDataIO
{
public:
	SolarDataIO(void);
	~SolarDataIO(void);
	static void readSolarFile(const char* filename, float*& data,unsigned int& width,unsigned int& height,Stats* stats);
	static void writeSolarFile(const char* filename,float* data,unsigned int width,unsigned int height,Stats* stats);
	static void readFloat4Texture(const char* filename, float*& data,unsigned int& width,unsigned int& height);
	static void writeFloat4Texture(const char* filename,float* data,unsigned int width,unsigned int height);
};

