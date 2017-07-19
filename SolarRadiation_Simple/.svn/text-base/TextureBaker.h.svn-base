#pragma once
#include "GeometryWrapper.h"
class TextureBaker : public GeometryWrapper
{
public:
	TextureBaker(void);
	~TextureBaker(void);
	//************************************
	// Method:    bakeTexture //employ the ray-caster to generate shadow maps
	// FullName:  TextureBaker::bakeTexture
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: const char * out_texture_dir //directory to save the shadow maps
	// Parameter: osg::Vec3 lightdir
	//************************************
	void TextureBaker::bakeTexture(const char* out_texture_dir, SolarParam sparam,unsigned short startDay,unsigned short endDay,float rasterCellSize );
	void TextureBaker::loadFromOSGNode(osg::Node* node);
	//************************************
	// Method:    toOSG //construct a osg::Node textured with shadow maps
	// FullName:  TextureBaker::toOSG
	// Access:    public 
	// Returns:   osg::Node*
	// Qualifier:
	// Parameter: const char * out_texture_dir //directory to read the shadow maps
	//************************************
	osg::Node* toOSG(const char* out_texture_dir);
	void setColorRamp( std::string colorramp_file );
	void symbolize(const char* out_texture_dir);
protected:
	void renderSolarMap(float* rads,unsigned char*& map,unsigned int channel,unsigned int width,unsigned int height,float rmin,float rmax);
	TriangleMesh createDummyMesh(std::vector<unsigned int>& nodeIndices);
	osg::ref_ptr<osg::Image> g_pClorRamp;
};

