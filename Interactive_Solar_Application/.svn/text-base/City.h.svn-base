#pragma once

#include "D3DRasterizer.h"
#include "GrassSolar.h"
#include "IGlobalInterfaces.h"
#include "GeometryWrapper.h"
#include "osg\MatrixTransform"


//structure for interactively deploying a solar panel
struct SolarPanel
{
	float Height;
	float Width;
	float Tilt;
	float Orientation;
	float OffsetX;
	float OffsetY;
	osg::Vec3d Position;
	osg::BoundingBox BB;
	osg::BoundingSphere BS;
	osg::MatrixTransform* Node;
	//int FaceStart;
	//int FaceCount;
	SolarPanel()
	{
		OffsetX = 0;
		OffsetY = 0;
		Node = NULL;
	}
 //   osg::ref_ptr<osg::Vec3Array> getVertices()
	//{

	//}
	osg::Matrix createMatrix()
	{
		osg::Matrix mat = osg::Matrix::identity();
	    mat *= osg::Matrix::scale(Width,Height,1);	
		mat *= osg::Matrix::rotate(osg::DegreesToRadians(Tilt),osg::Vec3d(1,0,0));
		mat *= osg::Matrix::rotate(osg::DegreesToRadians(Orientation),osg::Vec3d(0,0,1));
		mat *= osg::Matrix::translate(osg::Vec3d(OffsetX,OffsetY,0));
		mat *= osg::Matrix::translate(osg::Vec3d(Position));
		return mat;
	}
	osg::Matrix createNormalMatrix()
	{
		osg::Matrix mat = osg::Matrix::identity();
		mat *= osg::Matrix::rotate(osg::DegreesToRadians(Tilt),osg::Vec3d(1,0,0));
		mat *= osg::Matrix::rotate(osg::DegreesToRadians(Orientation),osg::Vec3d(0,0,1));
		return mat;
	}
	void updateMatrixTransform()
	{
		if(Node != NULL)
	       Node->setMatrix(createMatrix());
	}
	osg::Node* create(TriangleMesh& node)
	{

		osg::MatrixTransform* transNode = new osg::MatrixTransform;
		osg::ref_ptr<osg::Geode> geode = new osg::Geode;
		osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
		osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
		osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
		osg::ref_ptr<osg::Vec2Array> uvs = new osg::Vec2Array;
		osg::Matrix mat = createMatrix();
		osg::BoundingBox bb;
		osg::BoundingSphere bs;

		vertices->push_back(osg::Vec3d(0.5,0.5,0));uvs->push_back(osg::Vec2(1,1));
		vertices->push_back(osg::Vec3d(-0.5,0.5,0));uvs->push_back(osg::Vec2(0,1));
		vertices->push_back(osg::Vec3d(-0.5,0.0,0));uvs->push_back(osg::Vec2(0,0));


		vertices->push_back(osg::Vec3d(0.5,0.0,0));;uvs->push_back(osg::Vec2(1,0));
		vertices->push_back(osg::Vec3d(0.5,0.5,0));uvs->push_back(osg::Vec2(1,1));
		vertices->push_back(osg::Vec3d(-0.5,0.0,0));uvs->push_back(osg::Vec2(0,0));


		normals->push_back(osg::Vec3d(0,0,1));
		geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES,0,vertices->size()));
		geom->setVertexArray(vertices.get() );
		geom->setNormalArray(normals.get());
		geom->setNormalBinding(osg::Geometry::BIND_OVERALL);
		geode->addDrawable(geom.get());     

		transNode->addChild(geode.get());
		transNode->setMatrix(mat);
		for (int i=0;i<vertices->size();i++)
		{
			//vertices[i] = vertices[i] * mat;
			osg::Vec3 vertex = (*vertices)[i] * mat; 
			bb.expandBy(vertex);
			bs.expandBy(vertex);

		}

		node.XMin = bb.xMin();
		node.YMin = bb.yMin();
		node.ZMin = bb.zMin();

		node.XMax = bb.xMax();
		node.YMax = bb.yMax();
		node.ZMax = bb.zMax();
		node.FaceCount = 2;
		node.BBCenter = make_float3(bs.center().x(),bs.center().y(),bs.center().z());
		node.BBRadius = bs.radius();
		node.Width2Height = Width / Height;
		BB = bb;
		BS = bs;
		Node = transNode;
		return transNode;
	}

	void updateGeometryNode(TriangleMesh* node)
	{

		osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
		osg::Matrix mat = createMatrix();
		osg::BoundingBox bb;
		osg::BoundingSphere bs;

		vertices->push_back(osg::Vec3d(-0.5,0.0,0));
		vertices->push_back(osg::Vec3d(-0.5,0.5,0));
		vertices->push_back(osg::Vec3d(0.5,0.5,0));

		vertices->push_back(osg::Vec3d(-0.5,0.0,0));
		vertices->push_back(osg::Vec3d(0.5,0.5,0));
		vertices->push_back(osg::Vec3d(0.5,0.0,0));
		for (int i=0;i<vertices->size();i++)
		{
			osg::Vec3 vertex = (*vertices)[i] * mat; 
			bb.expandBy(vertex);
			bs.expandBy(vertex);

		}

		node->XMin = bb.xMin();
		node->YMin = bb.yMin();
		node->ZMin = bb.zMin();

		node->XMax = bb.xMax();
		node->YMax = bb.yMax();
		node->ZMax = bb.zMax();
		node->FaceCount = 2;
		node->BBCenter = make_float3(bs.center().x(),bs.center().y(),bs.center().z());
		node->BBRadius = bs.radius();
		node->Width2Height = Width / Height;
		BB = bb;
		BS = bs;

	}

	//TriangleMesh Node;
	void create(std::vector<osg::Vec3d>& vertices,std::vector<osg::Vec3d>& normals,std::vector<osg::Vec2>& uvs,TriangleMesh& node)
	{
		osg::Matrix mat = createMatrix();
		osg::BoundingBox bb;
		osg::BoundingSphere bs;
		vertices.clear();

		vertices.push_back(osg::Vec3d(0.5,0.5,0));uvs.push_back(osg::Vec2(1,1));
		vertices.push_back(osg::Vec3d(-0.5,0.5,0));uvs.push_back(osg::Vec2(0,1));
		vertices.push_back(osg::Vec3d(-0.5,0.0,0));uvs.push_back(osg::Vec2(0,0));


		vertices.push_back(osg::Vec3d(0.5,0.0,0));;uvs.push_back(osg::Vec2(1,0));
		vertices.push_back(osg::Vec3d(0.5,0.5,0));uvs.push_back(osg::Vec2(1,1));
		vertices.push_back(osg::Vec3d(-0.5,0.0,0));uvs.push_back(osg::Vec2(0,0));
        osg::Matrix normalMat = createNormalMatrix();

		for (int i=0;i<vertices.size();i++)
		{
			vertices[i] = vertices[i] * mat;
			bb.expandBy(vertices[i]);
			bs.expandBy(vertices[i]);
			osg::Vec3d normal = osg::Vec3d(0,0,1) * normalMat;
            normal.normalize();
			normals.push_back(normal);
		}

		node.XMin = bb.xMin();
		node.YMin = bb.yMin();
		node.ZMin = bb.zMin();

		node.XMax = bb.xMax();
		node.YMax = bb.yMax();
		node.ZMax = bb.zMax();
		node.FaceCount = 2;
		node.BBCenter = make_float3(bs.center().x(),bs.center().y(),bs.center().z());
		node.BBRadius = bs.radius();
		node.Width2Height = Width / Height;
		BB = bb;
		BS = bs;
	}

};

//
struct Building
{
	int ID;
    int RoofNodeIndex;//index of the roof part
	int FacadeNodeIndex;//index of the facade part
	int PanelIndex;
	float FacadeArea;
	float RoofArea;
	float TotalArea;
	SolarPanel Panel;
	osg::BoundingBox Bound;
};

struct GeometryData  
{
	unsigned int GeometryIndex;
	unsigned int StartVertex;
	unsigned int EndVertex;
	GeometryData(unsigned int geo,unsigned int start,unsigned int end):
	GeometryIndex(geo),StartVertex(start),EndVertex(end)
	{

	}
	GeometryData()
	{

	}

};
class City : public GeometryWrapper
{
public:
	City(osgViewer::Viewer* viewer);
	~City(void);
	void open();
	//convert to osg::Node for visualization
    osg::Node* toOSG(osgEarth::Bounds bound);
	//set the .city file name for loading
	void setFileName(std::string filename,const std::string outputdir);
	void loadFromOSGNode(osg::Node* node);
	void loadFromOSGNode(osg::Node* node,osg::Vec3d translate);
	void destroy();
	//set the .city file name for loading
	void setCityNode(osg::Geode* cityNode);
public:
	std::vector<Building> Buildings;
	osg::Node* getPanelNode(float x,float y);
	osg::Node* getPanelNode(float x,float y,Building*& building);
	//void selectByPoint(float x,float y);
    void selectByPoint(osg::Vec3d center);
	//void selectByRectangle(float xmin,float ymin,float xmax,float ymax);
	void selectByCircle(osg::Vec3d center,float radius);
	void showSelection(std::vector<unsigned int>& selection);
	void hideSelection(std::vector<unsigned int>& selection);
	void setColorRamp(std::string colorramp_file);
	void updateSolarPanel(Building& buiding);
	void updateGlobalParams();
	void setMousePos(osg::Vec3d mousePos);
	osg::Node* createSolarNodes();	
	std::vector<SolarRadiation> getDailySolarRadiation(const std::vector<int>& days);
	std::vector<SolarRadiation> getMonthlySolarRadiation(const std::vector<SolarRadiation>& daily,int startDay,int endDay);
	QString getFacadeImagePath(int selIndex);
	void exportSolarData(QString dir);
protected:
	void setGeometryVisibility(GeometryData& geodata,int ison);
	osg::Geometry* nodeToGeometry(unsigned int num/*,bool writeDiffuse,bool writeBake*/);
	std::string getGeometrySpecificBakeFileName( unsigned int nodeIndex );
	std::string getGeometrySpecificSolaFileName( unsigned int nodeIndex );
	void computeSolarRadiation();
	void renderSolarMap(float* rads,unsigned char*& map,unsigned int channel,unsigned int width,unsigned int height,float rmin,float rmax);
	void symbolize();
	std::vector<unsigned int> selectedBuildings2GeometryNodes(bool updateMask = false);
	osg::Node* createText(std::string info,osg::Vec3 pos);
	osg::Node * createTextCanvas(int width,int height);
	SolarRadiation computePointSolarRadiationSingleDay(int day);
	void highlightSelection(std::vector<unsigned int>& selection);
	void unhighlightSelection(std::vector<unsigned int>& selection);
    void exportSolarData(QString srcDir,QString destDir,int nodeIndex);
private:
	float RasterCellResolution;
	int MinTexSize;
	D3DRasterizer* rasterizer;
	osg::ref_ptr<osg::Image> g_pClorRamp;
	bool* g_pCalculationMasks;
	unsigned int m_pOrignialNumberOfNodes;
	osg::Geode* g_pCityNode;
	std::string m_dirName;
	std::string m_outputdirName;
	std::string m_fileName;
	std::vector<unsigned int> m_selectedBuildings;
	std::vector<unsigned int> m_hiddenSelection;
	std::vector<unsigned int> m_selectedGeometryNodes;
	std::map<unsigned int,GeometryData> m_node_2_startface_map;
	SolarParam m_pSolarParam;
	osgViewer::Viewer* g_pViewer;
};
