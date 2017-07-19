#ifndef COMMON_H
#define COMMON_H

#include "CudaRenderer.h"
#include <vector>
#include "qstring.h"
#include "qfileinfo.h"
#include <QDir>
#include "DXUT.h"
//using namespace std;

#define DEFAULT_TEXTURE_SIZE 1024

struct TexturedDrawable
{
public:
	std::string FileName;
	std::vector<unsigned int> NodeIndices;
	unsigned int Width;
	unsigned int Height;
	float Area;
public:
	TexturedDrawable()
	{
		FileName = "";
		Width = 0;
		Height = 0;
		Area = 0;
	}

	std::string getPosFileName()
	{
		QFileInfo fileInfo(FileName.data());
		QString basename = fileInfo.baseName();
		QString fix = QString("_position.dds");
		QString combined = basename + fix;
		std::string result = qPrintable(combined);
		//printf("%s",result.data());
		return result;
	}
	std::string  getNormalFileName()
	{
		QFileInfo fileInfo(FileName.data());
		QString basename = fileInfo.baseName();
		QString fix = QString("_normal.dds");
		QString combined = basename + fix;
		std::string result = qPrintable(combined);
		return result;
	}
	std::string getShadowFileName()
	{
		QFileInfo fileInfo(FileName.data());
		QString basename = fileInfo.baseName();
		QString fix = QString("_shadow.dds");
		QString combined = basename + fix;
		std::string result = qPrintable(combined);
		return result;
	}
	std::string getBakeFileName()
	{
		QFileInfo fileInfo(FileName.data());
		QString basename = fileInfo.baseName();
		QString fix = QString("_bake.bmp");
		QString combined = basename + fix;
		std::string result = qPrintable(combined);
		return result;
	}
	std::string getDiffuseFileName()
	{
		QFileInfo fileInfo(FileName.data());
		QString basename = fileInfo.baseName();
		QString fix = QString(".dds");
		QString combined = basename + fix;
		std::string result = qPrintable(combined);
		return result;
	}
	std::string getSolarFileName()
	{
		QFileInfo fileInfo(FileName.data());
		QString basename = fileInfo.baseName();
		QString fix = QString(".solar");
		QString combined = basename + fix;
		std::string result = qPrintable(combined);
		return result;
	}

};


#endif