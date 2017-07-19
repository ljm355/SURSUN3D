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
	void bakeTexture(const char* out_texture_dir, osg::Vec3 lightdir );

	//************************************
	// Method:    toOSG //construct a osg::Node textured with shadow maps
	// FullName:  TextureBaker::toOSG
	// Access:    public 
	// Returns:   osg::Node*
	// Qualifier:
	// Parameter: const char * out_texture_dir //directory to read the shadow maps
	//************************************
	osg::Node* toOSG(const char* out_texture_dir);
	void TextureBaker::loadFromOSGNode(osg::Node* node);
protected:
    TriangleMesh createDummyMesh(std::vector<unsigned int>& nodeIndices);
};

