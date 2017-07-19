#pragma once
#include "osg\BoundingBox"
#include "osg\Array"
#include "osg\Texture2D"
#include "osg\Geometry"
#include "osg\Matrix"
#include "Utils.h"
#include "CudaRenderer.h"
#include "TexturedDrawable.h"
class GeometryWrapper
{

public:
	//save GeometryWrapper to a binary file
	virtual void save(const char* filename);
	//read from a pre-saved GeometryWrapper
	virtual void open(const char* filename);
	//construct GeometryWrapper from a osg::Node, which can be imported from a wide range of 3d model formats, 
	//including .osg, .osgb, .ive,.x, .3ds, .obj, although many of the non-native formats are not always fully supported
	virtual void loadFromOSGNode(osg::Node* node);
	virtual void loadFromOSGNode(osg::Node* node,osg::Vec3d translate);
	virtual void setBakeSlot(unsigned int slot);
	virtual void setDiffusSlot(unsigned int slot);
	virtual float calMeshArea(unsigned int num);
	virtual osg::BoundingBox Extent() const { return g_mExtent; }
	virtual void Extent(osg::BoundingBox val) { g_mExtent = val; }
	TriangleMesh* TriangleMeshes;
	unsigned int NumberOfMeshes;
	unsigned int NumberOfFaces;
	std::vector<TexturedDrawable> TexturedDrawables;
	osg::ref_ptr<osg::Vec3Array> GeometryVertices;
	osg::ref_ptr<osg::Vec3Array> GeometryNormals;
	osg::ref_ptr<osg::Vec2Array> DiffuseUVs;
	osg::ref_ptr<osg::Vec2Array> BakeUVs;
protected:

	osg::BoundingBox g_mExtent;
	int BakeSlot;
	int DiffuseSlot;


	unsigned int CurNodeOffset;
	unsigned int CurFaceOffset;
    osg::Vec3d g_mTranslate;
	std::map<int,TexturedDrawable> NodeList;
	std::map<osg::Texture2D*,int> TextureList;
protected:

    std::map<unsigned int,unsigned int> m_node_2_drawable_map;
    std::map<std::string,osg::ref_ptr<osg::Image>> m_img_map;
	double m_CurrentArea;
	float m_CurWidth2Height;
	osg::BoundingBox m_CurBB;
	virtual void collectStats( osg::Node* node );
	virtual void applyNode( osg::Node* node );
	virtual bool applyDrawble( osg::Geometry* drawable,osg::Matrix matWorld);
	virtual void build_NodeDrawableMap();	
	virtual void apply(osg::Node* node);
	//std::string m_dirName;
	//std::string m_fileName;
public:
	GeometryWrapper(void);
	~GeometryWrapper(void);
	virtual void destroy();
public:
	float3* getVertices()
	{
		if(!GeometryVertices || !GeometryVertices.valid())
			return NULL;
		return (float3*)(&(*GeometryVertices)[0]);
	}
	float3* getNormals()
	{
		if(!GeometryNormals || !GeometryNormals.valid())
			return NULL;
		return (float3*)(&(*GeometryNormals)[0]);
	}
	float2* getUVs()
	{
		if(!BakeUVs || !BakeUVs.valid())
			return NULL;
		return (float2*)(&(*BakeUVs)[0]);
	}
	std::vector<TexturedDrawable> getDrawablesByDescendingOrder();
	std::vector<TexturedDrawable> getDrawables();
	//void bakeTexture( osg::Vec3 lightdir );
	//osg::Node* toOSG(bool writeDiffuse,bool writeBake);

};

