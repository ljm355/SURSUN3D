#include "SolarDataIO.h"
#include "osgDB\fstream"
//#include <fstream>

SolarDataIO::SolarDataIO(void)
{
}


SolarDataIO::~SolarDataIO(void)
{
}
void SolarDataIO::writeSolarFile(const char* filename,float* data,unsigned int width,unsigned int height,Stats* stats)
{
	std::ofstream ofs(filename,std::ios_base::out |  std::ios::binary);
	ofs.write((char*)stats,sizeof(Stats) * 4);

	ofs.write((char*)&width,sizeof(unsigned int));
	ofs.write((char*)&height,sizeof(unsigned int));

	ofs.write((char*)data,sizeof(float) * width * height * 4);
	//ofs.write((char*)data,sizeof(SolarRadiation) * width * height);
	ofs.flush();
	ofs.close();
}
void SolarDataIO::readSolarFile(const char* filename, float*& data,unsigned int& width,unsigned int& height,Stats* stats)
{

	std::ifstream ifs;
	ifs.open(filename,std::ios_base::in |  std::ios::binary);
	ifs.read((char*)stats,sizeof(Stats)*4);
	ifs.read((char*)&width,sizeof(unsigned int));
	ifs.read((char*)&height,sizeof(unsigned int));
	data = new float[width*height*4];
	ifs.read((char*)data,sizeof(float)*width*height*4);
	ifs.close();

}

void SolarDataIO::readFloat4Texture(const char* filename, float*& data,unsigned int& width,unsigned int& height)
{

	std::ifstream ifs;
	ifs.open(filename,std::ios_base::in |  std::ios::binary);
	ifs.read((char*)&width,sizeof(unsigned int));
	ifs.read((char*)&height,sizeof(unsigned int));
	data = new float[width*height*4];
	ifs.read((char*)data,sizeof(float)*width*height*4);
	ifs.close();

}
void SolarDataIO::writeFloat4Texture(const char* filename,float* data,unsigned int width,unsigned int height)
{

	std::ofstream ofs(filename,std::ios_base::out |  std::ios::binary);

	ofs.write((char*)&width,sizeof(unsigned int));
	ofs.write((char*)&height,sizeof(unsigned int));

	ofs.write((char*)data,sizeof(float) * width * height * 4);
	//ofs.write((char*)data,sizeof(SolarRadiation) * width * height);
	ofs.flush();
	ofs.close();
}