
#include "City.h"
#include "Utils.h"
#include "IGlobalInterfaces.h"
#include "osgDB\WriteFile"
#include "osgText\Text"
#include "osg\Camera"
#include "osgWidget\WindowManager"
#include "osg\ref_ptr"
#include "osg\MatrixTransform"
#include "osgUtil\GLObjectsVisitor"
#include "osgViewer\Renderer"
#include "osgUtil/Optimizer"
#include "SolarDataIO.h"
//int SCREEN_WITH = 1280;
//int SCREEN_HEIGHT = 768;
City::City(osgViewer::Viewer* viewer)
{	
	g_pCalculationMasks = NULL;
	DiffuseSlot = 0;
    BakeSlot = 0;
	RasterCellResolution = 1;
	rasterizer = new D3DRasterizer;
	MinTexSize = 1;
	RasterCellResolution = 1;
	setColorRamp("./color_ramps/colorramp14.bmp");
	m_pSolarParam.time_step = 1;
	m_pSolarParam.lat = 42.3581;
	m_pSolarParam.lon = -71.0636;
	m_pSolarParam.day = 1;
	m_pSolarParam.elev=0;
	m_pSolarParam.isShadowed = 0;
	m_pSolarParam.isInstantaneous = false;
	g_pViewer = viewer;
	
}

City::~City(void)
{
}

void City::open()
{
	destroy();

	std::ifstream ifs;
	ifs.open(this->m_fileName.data(),std::ios_base::in |  std::ios::binary);
	ifs.read((char*)&DiffuseSlot,sizeof(int));
	ifs.read((char*)&BakeSlot,sizeof(int));
	unsigned int bufsize;char* buf;

	ifs.read((char*)&bufsize,sizeof(unsigned int));
	unsigned int sizeofnode = sizeof(TriangleMesh);
	NumberOfMeshes = bufsize / sizeofnode + bufsize / sizeofnode / 2;
	TriangleMeshes = new TriangleMesh[NumberOfMeshes];
	g_pCalculationMasks = new bool[NumberOfMeshes];
	setExternalValidNodeMasks(g_pCalculationMasks);
	buf = (char*)TriangleMeshes;
	ifs.read(buf,bufsize);
	NumberOfMeshes = bufsize / sizeofnode;
	m_pOrignialNumberOfNodes = NumberOfMeshes;

	ifs.read((char*)&bufsize,sizeof(unsigned int));
	NumberOfFaces = bufsize / sizeof(osg::Vec3) / 3;
	GeometryVertices = new osg::Vec3Array(bufsize / sizeof(osg::Vec3));
	buf = (char*)(&(*GeometryVertices)[0]);
	ifs.read(buf,bufsize);

	ifs.read((char*)&bufsize,sizeof(unsigned int));
	GeometryNormals = new osg::Vec3Array(bufsize / sizeof(osg::Vec3));
	buf = (char*)(&(*GeometryNormals)[0]);
	ifs.read(buf,bufsize);


	ifs.read((char*)&bufsize,sizeof(unsigned int));
	if(bufsize > 0)
	{
		DiffuseUVs = new osg::Vec2Array(bufsize / sizeof(osg::Vec2));
		char* buf = (char*)(&(*DiffuseUVs)[0]);
		ifs.read(buf,bufsize);
	}

	ifs.read((char*)&bufsize,sizeof(unsigned int));
	if(bufsize > 0)
	{
		BakeUVs = new osg::Vec2Array(bufsize / sizeof(osg::Vec2));
		char* buf = (char*)(&(*BakeUVs)[0]);
		ifs.read(buf,bufsize);
	}

	//std::map<int,int> roofwallMap;
	//for (int i=0;i<NumberOfMeshes;i++)
	//{
	//	TriangleMesh& node = TriangleMeshes[i];
	//	roofwallMap[node.ID] = TriangleMeshes+i;
	//}
	int panelIndex = NumberOfMeshes;
	TriangleMesh* pGeometryNode = TriangleMeshes+NumberOfMeshes;
	for (int i=0;i<NumberOfMeshes;i++)
	{
		TriangleMesh& roofnode = TriangleMeshes[i];

		if(roofnode.ZMax - roofnode.ZMin > 0.5)
			continue;
		int roofIndex = i;
		int wallIndex =  -1;
		TriangleMesh facadenode;
		/*if(roofnode.XMin==facadenode.XMin && roofnode.YMin==facadenode.YMin && roofnode.XMax==facadenode.XMax && roofnode.YMax==facadenode.YMax)
		{
			wallIndex = roofIndex - 1;
		}
		else
		{*/
			for (int j=0;j<NumberOfMeshes;j++)
			{
				facadenode = TriangleMeshes[j];
				if(facadenode.ZMax - facadenode.ZMin < 0.5)
					continue;
				if(roofnode.XMin==facadenode.XMin && roofnode.YMin==facadenode.YMin && roofnode.XMax==facadenode.XMax && roofnode.YMax==facadenode.YMax)
				{
					wallIndex = j;
					break;
				}

			}
		//}
		if(wallIndex < 0)
			continue;

		int id = wallIndex;
		Building building;
		building.RoofNodeIndex=roofIndex;
		building.FacadeNodeIndex=wallIndex;
		SolarPanel panel;
		panel.Tilt = 45;
		panel.Orientation = 0;
		float3 center= roofnode.BBCenter;
		panel.Position=osg::Vec3d(center.x,center.y,center.z);
		panel.Width=roofnode.XMax-roofnode.XMin;
		panel.Height=roofnode.YMax-roofnode.YMin;
		std::vector<osg::Vec3d> vertices;
		std::vector<osg::Vec3d> normals;
		std::vector<osg::Vec2> uvs;
		TriangleMesh node;
		panel.create(vertices,normals,uvs,node);
		node.ID = panelIndex;
		node.FaceStart=GeometryVertices->size()/3;
		for (int k=0;k<vertices.size();k++)
		{
			GeometryVertices->push_back(vertices[k]);
			GeometryNormals->push_back(normals[k]);
			BakeUVs->push_back(uvs[k]);
		}

		building.RoofArea = calMeshArea(building.RoofNodeIndex);
		building.FacadeArea = calMeshArea(building.FacadeNodeIndex);
		building.TotalArea = building.RoofArea + building.FacadeArea;
		building.ID = wallIndex;
		building.PanelIndex=panelIndex++;
		building.Panel=panel;
		building.Bound=panel.BB;
		building.Bound.zMax()=facadenode.ZMax;
		building.Bound.zMin()=facadenode.ZMin;
		Buildings.push_back(building);
		*pGeometryNode=node;pGeometryNode++;
	}
	NumberOfMeshes=panelIndex;
	NumberOfFaces=GeometryVertices->size()/3;
	unsigned int numofdrawables;
	ifs.read((char*)&numofdrawables,sizeof(unsigned int));
	char charBuf[255];

	for (int i=0;i<numofdrawables;i++)
	{
		TexturedDrawable tdrawable;
		unsigned int bufsize;char* buf;
		ifs.read((char*)&bufsize,sizeof(unsigned int));
		ifs.read(charBuf,bufsize);
		charBuf[bufsize] = '\0';
		tdrawable.FileName = charBuf;

		ifs.read((char*)&bufsize,sizeof(unsigned int));
		tdrawable.NodeIndices.resize(bufsize /sizeof(unsigned int));
		ifs.read((char*)&tdrawable.NodeIndices[0],bufsize);

		ifs.read((char*)&tdrawable.Width,sizeof(unsigned int));
		ifs.read((char*)&tdrawable.Height,sizeof(unsigned int));
		ifs.read((char*)&tdrawable.Area,sizeof(float));
		TexturedDrawables.push_back(tdrawable);
		unsigned int startvertex=0;
		for (int j=0;j<tdrawable.NodeIndices.size();j++)
		{
			TriangleMesh node = this->TriangleMeshes[tdrawable.NodeIndices[j]];
			m_node_2_startface_map.insert(std::pair<unsigned int,GeometryData>(tdrawable.NodeIndices[j],GeometryData(i,startvertex,startvertex+node.FaceCount*3)));
			m_node_2_drawable_map.insert(std::pair<unsigned int,unsigned int>(tdrawable.NodeIndices[j],TexturedDrawables.size() - 1));
			startvertex+=node.FaceCount*3;
		}

	}
	ifs.close();

	rasterizer->createBuffer((float3*)(&(*GeometryVertices)[0]),(float3*)(&(*GeometryNormals)[0]),(float2*)(&(*BakeUVs)[0]),NumberOfFaces * 3);
	setGeometry((float3*)(&(*GeometryVertices)[0]),NumberOfFaces*3,TriangleMeshes,NumberOfMeshes);
}

std::vector<unsigned int> City::selectedBuildings2GeometryNodes(bool updateMask)
{
	SolarAnalysisParams* analysisParams = IGlobalInterfaces::getInstance()->getSolarAnalysisParams();
	std::vector<unsigned int>  masks;

	if(updateMask)
	{


		//for(int i=0;i<NumberOfMeshes;i++)
		//{
		//	g_pCalculationMasks[i] = false;
		//}

		for(int i=0;i<m_pOrignialNumberOfNodes;i++)
		{
			g_pCalculationMasks[i] = true;
		}

		for(int i=m_pOrignialNumberOfNodes;i<NumberOfMeshes;i++)
		{
			g_pCalculationMasks[i] = false;
		}
	}

   for (int i=0;i<m_selectedBuildings.size();i++)
   {
	   Building& building = Buildings[m_selectedBuildings[i]];
	   if(analysisParams->RooftopIncluded)
	   {
		   masks.push_back(building.RoofNodeIndex);
	   }

	   if(analysisParams->FacadeIncluded)
	   {
		   masks.push_back(building.FacadeNodeIndex);
	   }

	   if(analysisParams->SolarPanelIncluded)
	   {
		   masks.push_back(building.PanelIndex);
		   if(updateMask)
		   {
			   g_pCalculationMasks[building.PanelIndex] = true;
			   TriangleMesh& geometryNode = TriangleMeshes[building.PanelIndex];
			   //building.Panel.updateGeometryNode(&geometryNode);
			   std::vector<osg::Vec3d> vertices;
			   std::vector<osg::Vec3d> normals;
			   std::vector<osg::Vec2> uvs;
			   building.Panel.create(vertices,normals,uvs,geometryNode);
			   int startIndex = geometryNode.FaceStart * 3;
			   int count = geometryNode.FaceCount * 3;

			   for (int k=0;k< count;k++)
			   {
				   int index = startIndex+k;
				   (*GeometryVertices)[index] = vertices[k];
				   (*GeometryNormals)[index] = normals[k];
				   (*BakeUVs)[index] = uvs[k];
			   }
			   //SetGeometryNode(&geometryNode,building.PanelIndex);	

		   }
	   }
   }	
   setExternalValidNodeMasks(g_pCalculationMasks);

   updateGeometry((float3*)(&(*GeometryVertices)[0]),NumberOfFaces*3,TriangleMeshes,NumberOfMeshes);
   rasterizer->updateGeometryBuffer((float3*)(&(*GeometryVertices)[0]),(float3*)(&(*GeometryNormals)[0]),(float2*)(&(*BakeUVs)[0]));
  return masks;
}

osg::Node* City::toOSG(osgEarth::Bounds bound)
{

	////osg::Group* root  = new osg::Group;
	//osg::Geode* geode = new osg::Geode;
	//osg::ref_ptr<osg::Program> pgm = new osg::Program;
	//pgm->setName( "solarmapper" );
	//osg::ref_ptr<osg::Shader> fs = new osg::Shader(osg::Shader::FRAGMENT);
	//fs->loadShaderSourceFromFile("city.fs");
	//osg::ref_ptr<osg::Shader> vs = new osg::Shader(osg::Shader::VERTEX);
	//vs->loadShaderSourceFromFile("city.vs");
	//pgm->addShader(vs);
	//pgm->addShader(fs);


	//osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
	//osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
	//osg::ref_ptr<osg::Vec2Array> diffuseUVs = new osg::Vec2Array;
	//osg::ref_ptr<osg::Vec2Array> bakeUVs = new osg::Vec2Array;	
	//osg::ref_ptr<osg::Vec3Array> color = new osg::Vec3Array();
	//unsigned int startvertex=0;
	//osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
	//for (int i=0;i<this->NumberOfMeshes;i++)
	//{

	//	TriangleMesh node = this->TriangleMeshes[i];
	//	for (unsigned int k=node.FaceStart;k<node.FaceStart+node.FaceCount;k++)
	//	{
	//		for (int l=0;l<3;l++)
	//		{
	//			unsigned int index = k*3 + l;
	//			vertices->push_back((*this->GeometryVertices)[index]);
	//			normals->push_back((*this->GeometryNormals)[index]);
	//			//diffuseUVs->push_back((*this->DiffuseUVs)[index]);		
	//			bakeUVs->push_back(osg::Vec2(1,1));

	//		}


	//	}
	//	startvertex+=node.FaceCount*3;
	//}	

	//color->push_back(osg::Vec3(1,0.5,0));
	//geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES,0,vertices->size()));
	//geom->setVertexArray(vertices.get() );
	//geom->setNormalArray(normals.get());
	//geom->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);


	//geom->setTexCoordArray( 1, bakeUVs.get() );

	//geom->setColorArray(color.get());
	//geom->setColorBinding(osg::Geometry::BIND_OVERALL);
	//osg::StateSet* stateset = geom->getOrCreateStateSet();
	//stateset->setAttribute(pgm.get(),osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE); 

	//stateset->addUniform(new osg::Uniform("tex",0));

	//geode->addDrawable(geom.get());



	//geode->getOrCreateStateSet()->addUniform(new osg::Uniform("bound",osg::Vec4(bound.xMin(),bound.yMin(),bound.xMax(),bound.yMax())));
	//geode->getOrCreateStateSet()->addUniform(new osg::Uniform("mousePos",osg::Vec4(0,0,0,0)));
 //   g_pCityNode = geode;

	//return geode;
	osg::Geode* geode = new osg::Geode;
	geode->setDataVariance(osg::Object::DYNAMIC);
	osg::ref_ptr<osg::Program> pgm = new osg::Program;
	pgm->setName( "solarmapper" );
	osg::ref_ptr<osg::Shader> fs = new osg::Shader(osg::Shader::FRAGMENT);
	fs->loadShaderSourceFromFile("city.fs");
	osg::ref_ptr<osg::Shader> vs = new osg::Shader(osg::Shader::VERTEX);
	vs->loadShaderSourceFromFile("city.vs");
	pgm->addShader(vs);
	pgm->addShader(fs);
	std::map<std::string,osg::ref_ptr<osg::Image>>::iterator iter;
	for (int i=0;i<TexturedDrawables.size();i++)
	{
		osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
		TexturedDrawable tdrawable = TexturedDrawables[i];
		osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
		osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
		osg::ref_ptr<osg::Vec2Array> diffuseUVs = new osg::Vec2Array;
		osg::ref_ptr<osg::Vec2Array> bakeUVs = new osg::Vec2Array;	
		osg::ref_ptr<osg::Vec3Array> color = new osg::Vec3Array();
		//osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES, 0 );
		unsigned int startvertex=0;
		for (unsigned int j=0;j<tdrawable.NodeIndices.size();j++)
		{
			TriangleMesh node = this->TriangleMeshes[tdrawable.NodeIndices[j]];

			m_node_2_startface_map.insert(std::pair<unsigned int,GeometryData>(tdrawable.NodeIndices[j],GeometryData(i,startvertex,startvertex+node.FaceCount*3)));
			for (unsigned int k=node.FaceStart;k<node.FaceStart+node.FaceCount;k++)
			{
				for (int l=0;l<3;l++)
				{
					unsigned int index = k*3 + l;
					vertices->push_back((*GeometryVertices)[index]);
					normals->push_back((*GeometryNormals)[index]);
					diffuseUVs->push_back((*DiffuseUVs)[index]);		
					bakeUVs->push_back(osg::Vec2(1,1));
					color->push_back(osg::Vec3(0,0,0));
				}


			}
			startvertex+=node.FaceCount*3;


		}

		geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES,0,vertices->size()));
		geom->setVertexArray(vertices.get() );
		geom->setNormalArray(normals.get());
		geom->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
		//geom->setSupportsDisplayList(true);

		std::string basename = QFileInfo(tdrawable.FileName.data()).fileName().toStdString();

		std::string diffuseFile = tdrawable.FileName;//m_dirName + "/" + basename;
		osg::ref_ptr<osg::Texture2D> diffuseTex = new osg::Texture2D;
		diffuseTex->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::REPEAT);
		diffuseTex->setWrap(osg::Texture2D::WRAP_T,osg::Texture2D::REPEAT);
		diffuseTex->setWrap(osg::Texture2D::WRAP_R,osg::Texture2D::REPEAT);
		iter = m_img_map.find(diffuseFile);
		if(iter != m_img_map.end())
		{
			diffuseTex->setImage(iter->second.get());
		}
		else
		{
			osg::ref_ptr<osg::Image> diffuseImg = osgDB::readImageFile(diffuseFile);
			m_img_map.insert(std::pair<std::string,osg::ref_ptr<osg::Image>>(diffuseFile,diffuseImg));
			diffuseTex->setImage(diffuseImg.get());
		}

		geom->setTexCoordArray( 0, diffuseUVs.get(),osg::Array::BIND_PER_VERTEX );
		geom->setTexCoordArray( 1, bakeUVs.get(),osg::Array::BIND_PER_VERTEX );
		geom->getOrCreateStateSet()->setTextureAttributeAndModes(0,diffuseTex.get());
		geom->setColorArray(color.get());
		geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
		geom->getOrCreateStateSet()->addUniform(new osg::Uniform("tex",0));
		//geom->setUseDisplayList(true);
		//geom->setUseVertexBufferObjects(true);
		//geom->setSupportsDisplayList(true);

		geode->addDrawable(geom.get());


	}	
	geode->getOrCreateStateSet()->setAttribute(pgm.get(),osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE); 
	geode->getOrCreateStateSet()->addUniform(new osg::Uniform("bound",osg::Vec4(bound.xMin(),bound.yMin(),bound.xMax(),bound.yMax())));
	geode->getOrCreateStateSet()->addUniform(new osg::Uniform("mousePos",osg::Vec4(0,0,0,0)));
	//osgDB::writeNodeFile(*geode,"test2.osg");
	g_pCityNode = geode;
	return geode;
}

void City::loadFromOSGNode(osg::Node* node)
{
	GeometryWrapper::loadFromOSGNode(node);
	rasterizer->createBuffer((float3*)(&(*GeometryVertices)[0]),(float3*)(&(*GeometryNormals)[0]),(float2*)(&(*BakeUVs)[0]),NumberOfFaces * 3);
	setGeometry((float3*)(&(*GeometryVertices)[0]),NumberOfFaces*3,TriangleMeshes,NumberOfMeshes);
}

void City::loadFromOSGNode(osg::Node* node,osg::Vec3d translate)
{
	GeometryWrapper::loadFromOSGNode(node,translate);
	//rasterizer->createBuffer((float3*)(&(*GeometryVertices)[0]),(float3*)(&(*GeometryNormals)[0]),(float2*)(&(*BakeUVs)[0]),NumberOfFaces * 3);
	//setGeometry((float3*)(&(*GeometryVertices)[0]),NumberOfFaces*3,TriangleMeshes,NumberOfMeshes);
}

void City::setMousePos(osg::Vec3d mousePos)
{
   g_pCityNode->getOrCreateStateSet()->getUniform("mousePos")->set(osg::Vec4(mousePos.x(),mousePos.y(),mousePos.z(),IGlobalInterfaces::getInstance()->getSolarAnalysisParams()->Radius));
   updateGlobalParams();
}
std::string City::getGeometrySpecificBakeFileName( unsigned int nodeIndex )
{
	std::stringstream ss;
	ss << nodeIndex << ".bmp";
	return ss.str();

}

std::string City::getGeometrySpecificSolaFileName( unsigned int nodeIndex )
{
	std::stringstream ss;
	ss << nodeIndex << ".dds";
	return ss.str();
}


void getSolarMapStats(float* rads,unsigned int num,Stats stats[4],Stats global_stats[4])
{

	double sum = 0;
	unsigned int count = 0;
	for (int i=0;i<num;i++)
	{
		for(int j=0;j<4;j++)
		{
			float val = rads[i*4+j];
			if(val < 0 || val != val)
				continue;
			if(stats[j].fmin > val)
				stats[j].fmin = val;
			if(stats[j].fmax < val)
				stats[j].fmax = val;
			stats[j].sum+=val;
			stats[j].validnum++;
		}

	}
	for(int j=0;j<4;j++)
	{
		stats[j].fmean  = stats[j].sum / stats[j].validnum;

		global_stats[j].validnum+=stats[j].validnum;
		global_stats[j].sum+=stats[j].sum;
		if(global_stats[j].fmin > stats[j].fmin)
			global_stats[j].fmin = stats[j].fmin;

		if(global_stats[j].fmax < stats[j].fmax)
			global_stats[j].fmax = stats[j].fmax;


	}

}


//void writeSolarFile(const char* filename,float* data,unsigned int width,unsigned int height,Stats* stats)
//{
//	std::ofstream ofs(filename,std::ios_base::out |  std::ios::binary);
//	ofs.write((char*)stats,sizeof(Stats) * 4);
//
//	ofs.write((char*)&width,sizeof(unsigned int));
//	ofs.write((char*)&height,sizeof(unsigned int));
//
//	ofs.write((char*)data,sizeof(float) * width * height * 4);
//	//ofs.write((char*)data,sizeof(SolarRadiation) * width * height);
//	ofs.flush();
//	ofs.close();
//}
//void readSolarFile(const char* filename, float*& data,unsigned int& width,unsigned int& height,Stats* stats)
//{
//
//	std::ifstream ifs;
//	ifs.open(filename,std::ios_base::in |  std::ios::binary);
//	ifs.read((char*)stats,sizeof(Stats)*4);
//	ifs.read((char*)&width,sizeof(unsigned int));
//	ifs.read((char*)&height,sizeof(unsigned int));
//	data = new float[width*height*4];
//	ifs.read((char*)data,sizeof(float)*width*height*4);
//	ifs.close();
//
//}
void City::renderSolarMap(float* rads,unsigned char*& map,unsigned int channel,unsigned int width,unsigned int height,float rmin,float rmax)
{

	float3 startColor = make_float3(1,1,0);
	float3 endColor = make_float3(0,1,0);
	float range = rmax - rmin;
	for (int i=0;i<width*height;i++)
	{
		float val = rads[i*4+channel];
		if(val < 0 || val != val)
		{
			map[i*3+0]=0;
			map[i*3+1]=0;
			map[i*3+2]=0;
			continue;
		}
		float rate = 0;
		if(range > 0)
			rate = (val - rmin) / range;
		osg::Vec2 uv(rate,0.5);

		osg::Vec4 color = g_pClorRamp->getColor(uv);
		map[i*3+0]=(unsigned char)(color.x()*255);
		map[i*3+1]=(unsigned char)(color.y()*255);
		map[i*3+2]=(unsigned char)(color.z()*255);
	}
}
QString City::getFacadeImagePath(int selIndex)
{
	
	unsigned int nodeindex = Buildings[m_selectedBuildings[selIndex]].FacadeNodeIndex;
	std::stringstream ss;
	ss << nodeindex;
	std::string name =ss.str();
	std::string dir = m_dirName + "/";
	std::string bakefilename = dir +  getGeometrySpecificBakeFileName(nodeindex);
    return QString(bakefilename.data());
	
}
void City::symbolize()
{

	
	float resol = RasterCellResolution;

	Stats global_stats[4];
	std::ifstream ifs;
	ifs.open( "./global.stats",std::ios_base::in |  std::ios::binary);
	ifs.read((char*)global_stats,sizeof(Stats)*4);
	ifs.close();

	//QString outdir = m_dirName +  "/";
	float fmin = global_stats[0].fmin;
	float fmax = global_stats[0].fmax;
	
	

	std::vector<unsigned int>& nodemasks = selectedBuildings2GeometryNodes();
	unsigned int nodenum = nodemasks.size();

	for (int i=0;i<nodenum;i++)
	{
		unsigned int nodeindex = nodemasks[i];
		std::stringstream ss;
		ss << nodeindex;
		std::string name =ss.str();
		//QString dir = QString(m_dirName.data()) + "/";

		std::string dir = m_outputdirName + "/";
		//QString texturefilename = tdrawable.FileName.data();
		std::string bakefilename = dir +  getGeometrySpecificBakeFileName(nodeindex);
		//std::string normalfilename = dir + name + "_normal" + ".dds";
		//std::string posfilename =  dir + name + "_pos" + ".dds";
		std::string shadowfilename =  dir + name + ".shadow";
		std::string solarfilename = dir + name + ".solar";
		//if(QFileInfo(bakefilename).exists())
		//{
		//	continue;
		//}

		if(!QFileInfo(solarfilename.data()).exists())
		{
			continue;
		}
		printf("%s\n",bakefilename.data());



		TriangleMesh targetNode = TriangleMeshes[nodeindex];
		float area = calMeshArea(nodeindex);// (targetNode.XMax - targetNode.XMin) * (targetNode.YMax - targetNode.YMin);
		float rate =  targetNode.Width2Height;
		float pixelcount = area / resol;
		float fheight = sqrt(pixelcount / rate);
		float fwdith = pixelcount / fheight;
		unsigned int width = (unsigned int)fwdith;
		unsigned int height = (unsigned int)fheight;
		if(width< MinTexSize)
			width = MinTexSize;
		if(height < MinTexSize)
			height =MinTexSize;

		if(width > 2048)
			width = 2048;
		if(height > 2048)
			height =2048;


		D3DFORMAT fmt = D3DFMT_A32B32G32R32F;

		//LPDIRECT3DTEXTURE9 pNormalTexture;
		//D3DXCreateTextureFromFileExA(pDevice,normalfilename.toStdString().data(),width,height,1,0,fmt,D3DPOOL_MANAGED,D3DX_FILTER_NONE,D3DX_FILTER_NONE,0,NULL,NULL, &pNormalTexture);

		//float4* mapNormals =  loadVertexMap(pNormalTexture);
		//pNormalTexture->Release();

		osg::ref_ptr<osg::Image> solarMapImage = new osg::Image;
		solarMapImage->allocateImage(width,height,1,GL_RGB,GL_UNSIGNED_BYTE);
		unsigned char* imageData = solarMapImage->data();

		unsigned int icell = 0;
		float* rads;// =  (float*)(new SolarRadiation[width*height]);

		Stats stats[4];
		//Stats* pStats = (Stats*)stats;
		//fmin = stats[0].fmin;
		//fmax = stats[0].fmax;
		SolarDataIO::readSolarFile(solarfilename.data(),(float*)rads,width,height,(Stats*)stats);

		//renderAspectMap(mapNormals,imageData,0,width,height,0,360);
		renderSolarMap((float*)rads,imageData,0,width,height,fmin,fmax);
		osgDB::writeImageFile(*solarMapImage,bakefilename);
		delete[] rads;
		printf("%d/%d\n",i,nodenum);
	}


}



SolarRadiation City::computePointSolarRadiationSingleDay(int day)
{

	SolarAnalysisParams* analysisParam = IGlobalInterfaces::getInstance()->getSolarAnalysisParams();
	SolarParam sparam = m_pSolarParam;
    sparam.day = day;
	float4 vertexPos = make_float4(analysisParam->IntersectionPoint.x(),analysisParam->IntersectionPoint.y(),analysisParam->IntersectionPoint.z(),10000);
	float4 normal =  make_float4(analysisParam->IntersectionNormal.x(),analysisParam->IntersectionNormal.y(),analysisParam->IntersectionNormal.z(),10000);
	sparam.linke = analysisParam->Linke;
	SolarRadiation rad;
	rad.beam = rad.diffuse = rad.global = rad.reflected = -1;
	int shadowProb = 0;
	float3 normal3 = make_float3(normal);
	float slope = calculateSlope(normal3);
	float aspect = calculateAspect(normal3);
	sparam.slope = slope;
	sparam.aspect = aspect;

	rad = calculateSolarRadiation(sparam);
	//delete[] shadowInfo;
	return rad;
}




std::vector<SolarRadiation> City::getDailySolarRadiation(const std::vector<int>& days)
{
	std::vector<SolarRadiation> results;
	for (int i=0;i<days.size();i++)
	{
       results.push_back(computePointSolarRadiationSingleDay(days[i]));
	}
    return results;
}

std::vector<SolarRadiation> City::getMonthlySolarRadiation(const std::vector<SolarRadiation>& daily,int startDay,int endDay)
{

	std::vector<SolarRadiation> monthly;
	int months[12]={31,28,31,30,31,30,31,31,30,31,30,31};

	int numday = endDay-startDay+1;

    int month1,day1;
	int month2,day2;
	numDay2MonthDay(2001,startDay,month1,day1);
	numDay2MonthDay(2001,endDay,month2,day2);

	int curMonth = month1;
	int curDay = day1;
	int curNumOfDay = 1;
	while(curNumOfDay <= numday)
	{
		 SolarRadiation monthlyRad;
		 monthlyRad.Zero();
		 int numofdays = 0;
         while(curDay<=months[curMonth-1])
		 {
              monthlyRad=monthlyRad+daily[curNumOfDay-1];
			  numofdays++;
			  curNumOfDay++;
			  curDay++;
		 }
		 if(numofdays < months[curMonth-1])
		 {
              SolarRadiation mean = monthlyRad / numofdays;
			  monthlyRad=monthlyRad*((float)months[curMonth-1]/(float)numofdays);
		 }
		 curMonth++;
		 curDay=1;
         monthly.push_back(monthlyRad / 1000);
	}
	return monthly;
}

void City::computeSolarRadiation()
{
 
	if(!TriangleMeshes)
		return;
	SolarParam sparam = m_pSolarParam;
	LPDIRECT3DDEVICE9 pDevice = DXUTGetD3D9Device();

	float resol = RasterCellResolution;

	Stats global_stats[4];
	TriangleMesh tmpNodes[1];
    float fmin = FLT_MAX;
	float fmax =- FLT_MAX;


	std::vector<unsigned int>& nodemasks = selectedBuildings2GeometryNodes(true);
	unsigned int nodenum = nodemasks.size();


	SolarAnalysisParams* analysisParams = IGlobalInterfaces::getInstance()->getSolarAnalysisParams();
	int progress = 1;
	for (int i=0;i<nodenum;i++)
	{

		unsigned int nodeindex = nodemasks[i];
		std::stringstream ss;
		ss << nodeindex;
		std::string name = ss.str();
		std::string  dir =  m_outputdirName + "/";
		std::string  bakefilename = dir + getGeometrySpecificBakeFileName(nodeindex);
		std::string  normalfilename = dir + name + "_normal" + ".dds";
		std::string  posfilename =  dir + name + "_pos" + ".dds";

		std::string  normalfilenameSURF = dir + name + "_normal" + ".surf";
		std::string  posfilenameSURF =  dir + name + "_pos" + ".surf";
		std::string  shadowfilename =  dir + name + ".shadow";
		std::string  solarfilename = dir + name + ".solar";

		TriangleMesh targetNode = TriangleMeshes[nodeindex];
		float area = calMeshArea(nodeindex);// (targetNode.XMax - targetNode.XMin) * (targetNode.YMax - targetNode.YMin);
		float rate =  targetNode.Width2Height;//(float)tdrawable.Width / (float)tdrawable.Height;
		bool isRoof = true;
		float pixelcount = area / resol;

		float fheight = sqrt(pixelcount / rate);
		float fwdith = pixelcount / fheight;
		unsigned int width = (unsigned int)fwdith;
		unsigned int height = (unsigned int)fheight;

		if(width< MinTexSize)
			width = MinTexSize;
		if(height < MinTexSize)
			height =MinTexSize;

		if(width > 2048)
			width = 2048;
		if(height > 2048)
			height =2048;


		if(!QFileInfo(posfilename.data()).exists() || !QFileInfo(normalfilename.data()).exists())
		{
			rasterizer->setTextureSize(width,height);
			tmpNodes[0] = targetNode;
			rasterizer->render(tmpNodes,1);
			D3DXSaveSurfaceToFileA(posfilename.data(),D3DXIMAGE_FILEFORMAT::D3DXIFF_DDS,rasterizer->pDestSurfacePos,NULL,NULL);
			D3DXSaveSurfaceToFileA(normalfilename.data(),D3DXIMAGE_FILEFORMAT::D3DXIFF_DDS,rasterizer->pDestSurfaceNormal,NULL,NULL);

		}
		//if(!QFileInfo(posfilename.data()).exists() || !QFileInfo(normalfilename.data()).exists())
		//{
		//	continue;
		//}

		D3DFORMAT fmt = D3DFMT_A32B32G32R32F;

		LPDIRECT3DTEXTURE9 pNormalTexture;
		D3DXCreateTextureFromFileExA(pDevice,normalfilename.data(),width,height,1,0,fmt,D3DPOOL_MANAGED,D3DX_FILTER_NONE,D3DX_FILTER_NONE,0,NULL,NULL, &pNormalTexture);

		LPDIRECT3DTEXTURE9 pPosTexture;
		D3DXCreateTextureFromFileExA(pDevice,posfilename.data(),width,height,1,0,fmt,D3DPOOL_MANAGED,D3DX_FILTER_NONE,D3DX_FILTER_NONE,0,NULL,NULL, &pPosTexture);


		float4* mapVertices = loadVertexMap(pPosTexture);
		float4* mapNormals =  loadVertexMap(pNormalTexture);

		if(!QFileInfo(normalfilenameSURF.data()).exists())
		{
			SolarDataIO::writeFloat4Texture(normalfilenameSURF.data(),(float*)mapNormals,width,height);
		}

		if(!QFileInfo(posfilenameSURF.data()).exists())
		{
			SolarDataIO::writeFloat4Texture(posfilenameSURF.data(),(float*)mapVertices,width,height);
		}

		//float4* mapVertices = new float4[Width*Height];
		//float4* mapNormals = new float4[Width*Height];

		pPosTexture->Release();
		pNormalTexture->Release();


		int numdays = analysisParams->LastDay - analysisParams->FirstDay+1;
		int firstDay  = analysisParams->FirstDay;
		if(analysisParams->IsInstantaneous)
		{
			numdays = 1;
			sparam.time = analysisParams->Time;
			sparam.isInstantaneous = analysisParams->IsInstantaneous;
			firstDay = analysisParams->Day;
		}
		std::vector<SolarRadiation*> radsSeries;
		unsigned int icell = 0;
		unsigned int cellcount = width * height;


		for (int j=firstDay;j< firstDay+numdays;j++)
		{
		
			    IGlobalInterfaces::getInstance()->setMessage(QString("progress: %1/%2").arg(progress++).arg(nodenum*numdays)); 
			    IGlobalInterfaces::getInstance()->setProgress(0,nodenum*numdays,progress++);
				sparam.day = j;
				if(analysisParams->IsInstantaneous)
					sparam.day = analysisParams->Day;
				std::vector<SunVector> sunvectors = getSunVectors(sparam);

				std::vector<float3> lights = sunVector2LightDir(sunvectors);
				bool* shadowInfo = new bool[lights.size()];
				////lights.clear();
				////lights.push_back(make_float3(0.2,-1,0.1));
				unsigned int numlights = lights.size();

				std::vector<Box> frustrums(lights.size());
				std::vector<float> viewMatrices(lights.size() * 16);

				setLights(&lights[0],lights.size());
				bool* shadow = new bool[width*height*lights.size()];
				for (int k=0;k<lights.size();k++)
				{
					osg::BoundingSphere bs(osg::Vec3(targetNode.BBCenter.x,targetNode.BBCenter.y,targetNode.BBCenter.z),targetNode.BBRadius);
					osg::Matrixf m;
					osg::BoundingBox bb;
					osg::Vec3 lightdir(lights[k].x,lights[k].y,lights[k].z);			
					calLightMatrix(lightdir,bs,m,bb);
					Box frustrum;
					frustrum.MinMax[0] = make_float3(bb.xMin(),bb.yMin(),bb.zMin());
					frustrum.MinMax[1] = make_float3(bb.xMax(),bb.yMax(),bb.zMax());
					frustrums[k] = frustrum;
					float* pSrc = (float*)m.ptr();
					float* pDest = &viewMatrices[0] + 16 * k;
					memcpy(pDest,pSrc,sizeof(float) * 16);
				}
				setTargetGeometry(&targetNode);

				 //t = clock();
				executeKernelShadowCasting(shadow,mapVertices,mapNormals,width,height,&viewMatrices[0],&frustrums[0]);
				//seconds =(float)(clock() - t)/CLOCKS_PER_SEC;
				//printf("GPU time: %f,%f rays/s\n",seconds,width*height*numlights/seconds );
			

			  // t = clock();

				SolarRadiation* rads =  new SolarRadiation[width*height];

				sparam.shadowInfo = shadowInfo;

				for (icell =0;icell<cellcount;icell++)
				{
						SolarRadiation rad;
						//rad.beam = rad.diffuse = rad.global = rad.reflected = -1;
						//rads[icell] = rad;
						if(mapNormals[icell].w < -1)
						{
							continue;
						}
						int shadowProb = 0;
						for (int ilight =0;ilight<lights.size();ilight++)
						{
							sparam.shadowInfo[ilight] = shadow[ilight*cellcount+icell];
							if(!shadow[ilight*cellcount+icell])
								shadowProb++;
						}
						float3 normal = make_float3(mapNormals[icell]);
						float slope = calculateSlope(normal);
						float aspect = calculateAspect(normal);
						sparam.slope = slope;
						sparam.aspect = aspect;
			    		rad = calculateSolarRadiation(sparam);
						//rad.global =  !shadow[0*cellcount+icell];
						rads[icell] = rad;
				}
                radsSeries.push_back(rads);
				//seconds =(float)(clock() - t)/CLOCKS_PER_SEC;
				//printf("CPU time: %f\n",seconds );
				//printf("width=%d, height=%d\n",width,height);
				delete[] shadow;
				delete[] shadowInfo;
				if(analysisParams->IsInstantaneous)
					break;
		}
		Stats stats[4];
		SolarRadiation* radsSum =  new SolarRadiation[width*height];

		for (icell =0;icell<cellcount;icell++)
		{
			radsSum[icell].Zero();
		}

		for (int j=0;j<radsSeries.size();j++)
		{

			for (icell =0;icell<cellcount;icell++)
			{
				SolarRadiation rad;
				radsSum[icell] = radsSum[icell] + radsSeries[j][icell];
			}
		}
		getSolarMapStats((float*)radsSum,width*height,stats,global_stats);
		SolarDataIO::writeSolarFile(solarfilename.data(),(float*)radsSum,width,height,stats);
		printf("%d/%d\n",i,nodenum);
		for (int j=0;j<radsSeries.size();j++)
		{
			delete[] radsSeries[j];
		}
		radsSeries.clear();
		delete[] radsSum;
		delete[] mapVertices;
		delete[] mapNormals;

	}
	for (int i=0;i<4;i++)
	{
		global_stats[i].fmean = global_stats[i].sum / global_stats[i].validnum;
	}
	if(global_stats[0].validnum > 0)
	{
		std::ofstream ofs("./global.stats",std::ios_base::out |  std::ios::binary);
		ofs.write((char*)global_stats,sizeof(Stats) * 4);
		ofs.flush();
		ofs.close();
	}

   IGlobalInterfaces::getInstance()->setProgress(0,0,0);
}


osg::Node* City::createText(std::string info,osg::Vec3 pos)
{
	osg::ref_ptr<osgText::Font> font = osgText::readFontFile("fonts/SimHei.ttf");
	osg::Geode* geode = new osg::Geode;
	osg::ref_ptr<osgText::Text> text = new osgText::Text;
	//text->set(osgText::String::ENCODING_UTF8);
	text->setFont(font);
	text->setColor(osg::Vec4(0,0,0,1));
	text->setCharacterSize(18);
	//text->setPosition(osg::Vec3(50,SCREEN_HEIGHT-40,-1.0f));
	text->setPosition(pos);
	// the default layout is left to right, typically used in languages
	// originating from europe such as English, French, German, Spanish etc..
	text->setLayout(osgText::Text::LEFT_TO_RIGHT);
	text->setText(info);
	geode->addDrawable(text.get());
	osg::StateSet * stateset = geode->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
	//osg::BlendFunc* bf = new osg::BlendFunc(
	//	osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA );
	//stateset->setAttributeAndModes( bf );
	//osg::AlphaFunc* af = new osg::AlphaFunc(
	//	osg::AlphaFunc::GREATER, 0.05f );
	//stateset->setAttributeAndModes( af );
	//stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	stateset->setMode(GL_BLEND, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
	return geode;
}



///´´½¨µØÍ¼
osg::Node * City::createTextCanvas(int width,int height)
{
	osg::ref_ptr<osg::Geode> HUDGeode = new osg::Geode;
	osg::ref_ptr<osg::Geometry> HUDBackgroundGeometry = new osg::Geometry();
	osg::ref_ptr<osg::Vec3Array> HUDBackgroundVertices = new osg::Vec3Array;
	HUDBackgroundVertices->push_back( osg::Vec3( 0,    0,-1) );
	HUDBackgroundVertices->push_back( osg::Vec3(width,  0,-1) );
	HUDBackgroundVertices->push_back( osg::Vec3(width,1000,-1) );
	HUDBackgroundVertices->push_back( osg::Vec3(   0,1000,-1) );

	osg::ref_ptr<osg::DrawElementsUInt> HUDBackgroundIndices =
		new osg::DrawElementsUInt(osg::PrimitiveSet::POLYGON, 0);
	HUDBackgroundIndices->push_back(0);
	HUDBackgroundIndices->push_back(1);
	HUDBackgroundIndices->push_back(2);
	HUDBackgroundIndices->push_back(3);

	osg::ref_ptr<osg::Vec4Array> HUDcolors = new osg::Vec4Array;
	HUDcolors->push_back(osg::Vec4(0.0f,0.0f,0.0f,1.0f));

	osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array(4);
	(*texcoords)[0].set(0.0f,0.0f);
	(*texcoords)[1].set(1.0f,0.0f);
	(*texcoords)[2].set(1.0f,1.0f);
	(*texcoords)[3].set(0.0f,1.0f);

	HUDBackgroundGeometry->setTexCoordArray(0,texcoords.get());

	HUDBackgroundGeometry->addPrimitiveSet(HUDBackgroundIndices.get());
	HUDBackgroundGeometry->setVertexArray(HUDBackgroundVertices.get());
	HUDBackgroundGeometry->setColorArray(HUDcolors.get());
	HUDBackgroundGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
	HUDGeode->addDrawable(HUDBackgroundGeometry.get());
	osg::StateSet * stateset = HUDGeode->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
	//osg::BlendFunc* bf = new osg::BlendFunc(
	//	osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA );
	//stateset->setAttributeAndModes( bf );
	//// Turn on alpha testing
	//osg::AlphaFunc* af = new osg::AlphaFunc(
	//	osg::AlphaFunc::GREATER, 0.05f );
	//stateset->setAttributeAndModes( af );
	//// Turn on alpha testing
	//stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

	osg::ref_ptr<osg::Texture2D> texLegend = new osg::Texture2D;
	texLegend->setDataVariance(osg::Object::DYNAMIC);
	texLegend->setImage(g_pClorRamp.get());
	stateset->setTextureAttributeAndModes(0,texLegend.get(),osg::StateAttribute::ON);
	stateset->setMode(GL_BLEND, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
	osg::ref_ptr<osg::Program> program = new osg::Program;

	char vertexShaderSource[] = 
		"void main(void)\n"
		"{\n"
		"   gl_TexCoord[0] = gl_MultiTexCoord0;\n"
		"   gl_Position    = gl_ModelViewProjectionMatrix *  gl_Vertex;\n"
		"}\n";



	char fragmentShaderSource[] = 
		"uniform sampler2D tex; \n"
		"\n"
		"void main(void) \n"
		"{\n"
		"       vec4 color = texture2D(tex, vec2(gl_TexCoord[0].x,0.5));\n"
		"       gl_FragColor = color;\n"
		"}\n";

	program->addShader(new osg::Shader(osg::Shader::FRAGMENT, fragmentShaderSource));
	program->addShader(new osg::Shader(osg::Shader::VERTEX, vertexShaderSource));

	stateset->addUniform(new osg::Uniform("tex",0));
	stateset->setAttribute(program.get(),osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

	osg::Projection * HUDProjectionMatrix = new osg::Projection;

	HUDProjectionMatrix->setMatrix(osg::Matrix::ortho2D(0,width,0,height));
	osg::ref_ptr<osg::MatrixTransform> HUDModelViewMatrix = new osg::MatrixTransform;
	HUDModelViewMatrix->setMatrix(osg::Matrix::translate(0,height-50,0));
	// above it in the scene graph:
	HUDModelViewMatrix->setReferenceFrame(osg::Transform::ABSOLUTE_RF);

	HUDProjectionMatrix->addChild(HUDModelViewMatrix.get());
	HUDModelViewMatrix->addChild( HUDGeode.get() );

	return HUDProjectionMatrix;
}

osg::Node* City::createSolarNodes()
{

    updateGlobalParams();
	computeSolarRadiation();
	symbolize();

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	std::vector<unsigned int>& nodemasks = selectedBuildings2GeometryNodes(false);
	unsigned int nodenum = nodemasks.size();
	for (int i=0;i<nodenum;i++)
	{
		unsigned int nodeindex = nodemasks[i];
		osg::ref_ptr<osg::Geometry> geom = nodeToGeometry(nodeindex);
		geode->addDrawable(geom.get());
	}

	geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	showSelection(m_hiddenSelection);m_hiddenSelection.clear();
	hideSelection(m_selectedBuildings);

	osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
	unsigned int width, height;
	wsi->getScreenResolution(osg::GraphicsContext::ScreenIdentifier(0), width, height);
	osg::ref_ptr<osgWidget::WindowManager> wm = new osgWidget::WindowManager(
		IGlobalInterfaces::getInstance()->getSolarAnalysisParams()->Viewer,
		width,
		height,
		0xF0000000,
		osgWidget::WindowManager::WM_USE_RENDERBINS
		);

	wm->setPointerFocusMode(osgWidget::WindowManager::PFM_SLOPPY);



	wm->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
	Stats global_stats[4];
	std::ifstream ifs;
	ifs.open("global.stats",std::ios_base::in |  std::ios::binary);
	ifs.read((char*)global_stats,sizeof(Stats)*4);
	ifs.close();
	float scale = 1;
	if(global_stats[0].fmax > 10000)
		scale = 0.001;
	float fmin = global_stats[0].fmin * scale;
	float fmax = global_stats[0].fmax * scale;
	std::stringstream ss;
	int prec = log(fmin * 100);
	ss.precision(prec);


	if(!IGlobalInterfaces::getInstance()->getSolarAnalysisParams()->IsInstantaneous)
	{
		if(scale < 1)
		{
			ss << "Minimum=" <<fmin <<"                                                                                                              " 
				<< "Maximum=" << fmax<<" [kWh/m-2]" << std::endl;
		}
		else
		{
			ss << "Minimum=" <<fmin <<"                                                                                                              " 
				<< "Maximum=" << fmax<<" [Wh/m-2]" << std::endl;
		}

	}
	else
	{
		ss << "Minimum=" <<fmin <<"                                                                                                              " 
			<< "Maximum=" << fmax<<" [W/m-2]" << std::endl;
	}



	osg::ref_ptr<osg::Camera> camera=wm->createParentOrthoCamera();

	osg::ref_ptr<osg::Node> canvas = createTextCanvas(width,height);
	camera->addChild(canvas.get());
	osg::ref_ptr<osg::Node> text1 = createText(ss.str(),osg::Vec3(10,height-40,-1.0f));
	camera->addChild(text1.get());

	std::string months[12]={"January" , "February" , "March" , "April", "May","June", "July", "August", "September", "October","November", "December"};

	std::stringstream ssDayRange;
	int month1,day1;
	int month2,day2;
	if(!IGlobalInterfaces::getInstance()->getSolarAnalysisParams()->IsInstantaneous)
	{
		numDay2MonthDay(2001,IGlobalInterfaces::getInstance()->getSolarAnalysisParams()->FirstDay,month1,day1);
		numDay2MonthDay(2001,IGlobalInterfaces::getInstance()->getSolarAnalysisParams()->LastDay,month2,day2);
		ssDayRange << "From " << months[month1-1] <<" "<< day1 <<  " to " << months[month2-1] <<" "<< day2;
	}
	else
	{
		int h=IGlobalInterfaces::getInstance()->getSolarAnalysisParams()->Time.hour;
		int m = IGlobalInterfaces::getInstance()->getSolarAnalysisParams()->Time.minute;
		int s = IGlobalInterfaces::getInstance()->getSolarAnalysisParams()->Time.second;

		ssDayRange << "Day:" << IGlobalInterfaces::getInstance()->getSolarAnalysisParams()->Day << " " ;
		if(h < 10)
			ssDayRange << "0";
		ssDayRange << h << ":";
		if(m < 10)
			ssDayRange << "0";
		ssDayRange << m << ":";
		if(s < 10)
			ssDayRange << "0";
		ssDayRange << s;
		
	}
	osg::ref_ptr<osg::Node> text2 = createText(ssDayRange.str(),osg::Vec3(width / 2-80,height-40,-1.0f));
	camera->addChild(text2.get());


	camera->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
	osg::Group* root = new osg::Group;
	root->addChild(geode.get());
	root->addChild(camera.get());
	root->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
	root->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);


	osg::ref_ptr<osg::Program> program = new osg::Program;

	static char vertexShaderSource[] = 
		"void main(void)\n"
		"{\n"
		"   gl_TexCoord[0] = gl_MultiTexCoord0;\n"
		"   gl_TexCoord[1] = gl_MultiTexCoord1;\n"
		"   gl_Position    = gl_ModelViewProjectionMatrix *  gl_Vertex;\n"
		"}\n";



	static char fragmentShaderSource[] = 
		"uniform sampler2D tex; \n"
		"uniform int slot; \n"
		"\n"
		"void main(void) \n"
		"{\n"
		"    vec4 color = vec4(1,0,0,1);\n"
		"    if(slot < 1)\n"
		"       color = texture2D(tex, gl_TexCoord[0]);\n"
		"    else \n"
		"       color = texture2D(tex, gl_TexCoord[1]);\n"
		"    gl_FragColor = color;\n"
		"}\n";

	program->addShader(new osg::Shader(osg::Shader::FRAGMENT, fragmentShaderSource));
	program->addShader(new osg::Shader(osg::Shader::VERTEX, vertexShaderSource));
	geode->getOrCreateStateSet()->addUniform(new osg::Uniform("tex",BakeSlot));
	geode->getOrCreateStateSet()->addUniform(new osg::Uniform("slot",BakeSlot));
	geode->getOrCreateStateSet()->setAttribute(program.get(),osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	geode->getOrCreateStateSet()->setDataVariance(osg::Object::DYNAMIC);
	IGlobalInterfaces::getInstance()->setMessage("Creating Texture Atlas.\n");
	osgUtil::Optimizer::TextureAtlasVisitor atlasVisitor;
	geode->accept(atlasVisitor);
	atlasVisitor.optimize();
	IGlobalInterfaces::getInstance()->setMessage("Completed.\n");
	return root;
}
osg::Geometry* City::nodeToGeometry(unsigned int num)
{
	TriangleMesh node = TriangleMeshes[num];
	TexturedDrawable tdrawable;
	osg::Geometry* geom = new osg::Geometry;
	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
	//osg::ref_ptr<osg::Vec2Array> diffuseUVs = new osg::Vec2Array;
	osg::ref_ptr<osg::Vec2Array> bakeUVs = new osg::Vec2Array;

	for (unsigned int k=node.FaceStart;k<node.FaceStart+node.FaceCount;k++)
	{
		for (int l=0;l<3;l++)
		{
			unsigned int index = k*3 + l;
			vertices->push_back((*GeometryVertices)[index]);
			normals->push_back((*GeometryNormals)[index]);
			//if(writeBake && BakeSlot > -1)
				bakeUVs->push_back((*BakeUVs)[index]);
	/*		if(writeDiffuse && DiffuseSlot > -1)
				diffuseUVs->push_back((*DiffuseUVs)[index]);*/
		}
	}
	geom->getOrCreateStateSet()->setDataVariance(osg::Object::STATIC);
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES,0,vertices->size()));
	geom->setVertexArray( vertices.get() );
	geom->setNormalArray(normals.get());
	geom->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
	std::map<std::string,osg::ref_ptr<osg::Image>>::iterator iter;
	/*if(writeDiffuse && DiffuseSlot > -1)
	{
		std::string diffuseFile = QFileInfo(tdrawable.FileName.data()).fileName().toStdString();
		osg::ref_ptr<osg::Texture2D> diffuseTex = new osg::Texture2D;
		diffuseTex->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::REPEAT);
		diffuseTex->setWrap(osg::Texture2D::WRAP_T,osg::Texture2D::REPEAT);
		diffuseTex->setWrap(osg::Texture2D::WRAP_R,osg::Texture2D::REPEAT);
		iter = m_img_map.find(diffuseFile);
		if(iter != m_img_map.end())
		{
			diffuseTex->setImage(iter->second.get());
		}
		else
		{
			osg::ref_ptr<osg::Image> diffuseImg = osgDB::readImageFile(diffuseFile);
			m_img_map.insert(std::pair<std::string,osg::ref_ptr<osg::Image>>(diffuseFile,diffuseImg));
			diffuseTex->setImage(diffuseImg.get());
		}
		geom->setTexCoordArray( DiffuseSlot, diffuseUVs );
		geom->getOrCreateStateSet()->setTextureAttributeAndModes(DiffuseSlot,diffuseTex.get());
	}
	if(writeBake && BakeSlot > -1)
	{	*/
		std::stringstream ss;
		ss << num;
		std::string name =ss.str().data();
		std::string dir = m_outputdirName + "/";
		//std::string dir = getInstanceDir() + "/";

		//QString texturefilename = tdrawable.FileName.data();
		std::string bakeFile = dir + getGeometrySpecificBakeFileName(num);
	    osg::ref_ptr<osg::Texture2D> bakeTex = new osg::Texture2D;
		
	    osg::ref_ptr<osg::Image> bakeImg = osgDB::readImageFile(bakeFile);
	    bakeTex->setImage(bakeImg.get());
		geom->setTexCoordArray( BakeSlot, bakeUVs );
		geom->getOrCreateStateSet()->setTextureAttributeAndModes(BakeSlot,bakeTex.get());
	//}

	return geom;
}



osg::Node* City::getPanelNode(float x,float y,Building*& building)
{

	for (int i=0;i<Buildings.size();i++)
	{
		building = &Buildings[i];
		TriangleMesh node = TriangleMeshes[building->FacadeNodeIndex];

		if(node.XMax < x || node.YMax < y || node.XMin > x || node.YMin > y)
			continue;
		//m_selectedBuildings.clear();
		//m_selectedBuildings.push_back(i);
		return	building->Panel.create(node);
	}
	return NULL;
}


void City::selectByPoint(osg::Vec3d center)
{	
	
	m_selectedBuildings.clear();
	double curDist = FLT_MAX;
	int index = -1;
	for (int i=0;i<Buildings.size();i++)
	{
	    Building& building = Buildings[i];
		TriangleMesh& node = TriangleMeshes[building.RoofNodeIndex];
		if(node.XMax < center.x() || node.YMax < center.y() || node.XMin > center.x() || node.YMin > center.y())
			continue;
		osg::Vec3d center(node.BBCenter.x,node.BBCenter.y,0);
		center.z() = 0;
		double dist = (center-osg::Vec3d()).length();
		if(dist < curDist)
		{
			curDist = dist;
			index = i;
		}

	}
	if(index < 0)
		return;
	m_selectedBuildings.push_back(index);
	highlightSelection(m_selectedBuildings);
	SolarAnalysisParams* params = IGlobalInterfaces::getInstance()->getSolarAnalysisParams();
	//g_pCityNode->getOrCreateStateSet()->getUniform("radius")->set(1.0f);
	//osg::Vec4 bound(0,0,0,0);
	//g_pCityNode->getOrCreateStateSet()->getUniform("bound")->set(bound);

}

//void City::selectByRectangle( float xmin,float ymin,float xmax,float ymax )
//{	
//	m_selectedBuildings.clear();
//	for (int i=0;i<Buildings.size();i++)
//	{
//		Building& building = Buildings[i];
//		TriangleMesh node = TriangleMeshes[building.FacadeNodeIndex];
//		if(node.XMax < xmin || node.YMax < ymin || node.XMin > xmax || node.YMin > ymax)
//			continue;
//		m_selectedBuildings.push_back(i);
//	}
//	highlightSelection(m_selectedBuildings);
//	SolarAnalysisParams* params = IGlobalInterfaces::getInstance()->getSolarAnalysisParams();
//	///g_pCityNode->getOrCreateStateSet()->getUniform("radius")->set(0.001f);
//	g_pCityNode->getOrCreateStateSet()->getUniform("mousePos")->set(osg::Vec4(0,0,0,0));
//	osg::Vec4 bound(0,0,0,0);
//	g_pCityNode->getOrCreateStateSet()->getUniform("bound")->set(params->getBound());
//}

 int sign(int a)
 {
	 if(a  > 0)
		 return 1;
	 return -1;
 }
 bool isInsideTriangle(osg::Vec3d& A ,osg::Vec3d& B,osg::Vec3d& C,osg::Vec3d& P) 
 {
	int planeAB = (A.x()-P.x())*(B.y()-P.y())-(B.x() - P.x())*(A.y()-P.y());
	int planeBC = (B.x()-P.x())*(C.y()-P.y())-(C.x() - P.x())*(B.y()-P.y());
	int planeCA = (C.x()-P.x())*(A.y()-P.y())-(A.x() - P.x())*(C.y()-P.y());
	return sign(planeAB)==sign(planeBC) && sign(planeBC)==sign(planeCA);
}
void City::selectByCircle( osg::Vec3d center,float radius )
{
	center.z()=0;	 
	m_selectedBuildings.clear();
	osg::BoundingSphere bs;
	bs.set(center,radius); 
	osg::BoundingBox bb;
	bb.expandBy(bs);
	bb.set(bb.xMin(),bb.yMin(),-10000,bb.xMax(),bb.yMax(),10000);
	for (int i=0;i<Buildings.size();i++)
	{

		Building& building = Buildings[i];
		TriangleMesh& roofnode = TriangleMeshes[building.RoofNodeIndex];
		TriangleMesh& wallnode = TriangleMeshes[building.FacadeNodeIndex];
		osg::BoundingBox bb2;
		bb2.set(roofnode.XMin,roofnode.YMin,wallnode.ZMin,roofnode.XMax,roofnode.YMax,wallnode.ZMax);
		//if(bb2.xMin() > bb.xMin() && bb.xMax() < bb2.xMax() && bb2.yMin() > bb.yMin() && bb.yMax() < bb2.yMax())
		//{
		//	m_selectedBuildings.push_back(i);
		//	continue;
		//}
		if(!bb.intersects(bb2))
			continue;
		GeometryData& geodata = m_node_2_startface_map[building.RoofNodeIndex];
		osg::Geometry* geom = (osg::Geometry*)g_pCityNode->getDrawable(geodata.GeometryIndex);
		osg::Vec3Array* vertices = (osg::Vec3Array*)geom->getVertexArray();

		bool isContained = false;
		if((bb2.center()-bb.center()).length() < bb.radius())
			isContained = true;
		for (int j=geodata.StartVertex;j+3<geodata.EndVertex;j+=3)
		{
			osg::Vec3d p1 = (*vertices)[j]; 
			osg::Vec3d p2 = (*vertices)[j+1]; 
			osg::Vec3d p3 = (*vertices)[j+2]; 
			if(isContained && (bb.contains(p1) || bb.contains(p2) || bb.contains(p3)))
			{
				m_selectedBuildings.push_back(i);
				break;
			}
			int num_segments = 8;
		    bool contains = false;
			for(int ii = 0; ii < num_segments; ii++)
			{
				float theta = 2.0f * 3.1415926f * float(ii) / float(num_segments);//get the current angle

				float x = radius * cosf(theta);//calculate the x component
				float y = radius * sinf(theta);//calculate the y component
				osg::Vec3d point(x+center.x(),y+center.y(),0);
				if(isInsideTriangle(p1,p2,p3,point))
				{
					m_selectedBuildings.push_back(i);
					contains = true;
					break;
				}
				//glVertex2f(x + cx, y + cy);//output vertex
				
			}
			if(contains)
			{
			    continue;
			}

		}

		//for (int j=geodata.StartVertex;j<geodata.EndVertex;j++)
		//{
		//     osg::Vec3d p = (*GeometryVertices)[j]; 
		//	 p.z() = 0;
		//	 if(bb.contains(p))
		//	 {
		//		 m_selectedBuildings.push_back(i);
		//		 break;
		//	 }

		//}
	}

	//for (int i=0;i<selection.size();i++)
	//{
	//	unsigned int nodeindex = selection[i];
	//	Building& building = Buildings[nodeindex];
	//	GeometryData& geodata = m_node_2_startface_map[building.RoofNodeIndex];
	//	osg::Geometry* geom = (osg::Geometry*)g_pCityNode->getDrawable(geodata.GeometryIndex);
	//	osg::Vec3Array* color = (osg::Vec3Array*)geom->getColorArray();
	//	for (int j=geodata.StartVertex;j<geodata.EndVertex;j++)
	//	{
	//		(*color)[j] = osg::Vec3(0.5,0,0);
	//	}

	//	geom->dirtyDisplayList();
	//}
	highlightSelection(m_selectedBuildings);
	SolarAnalysisParams* params = IGlobalInterfaces::getInstance()->getSolarAnalysisParams();
	{
		g_pCityNode->getOrCreateStateSet()->getUniform("mousePos")->set(osg::Vec4(params->IntersectionPoint.x(),params->IntersectionPoint.y(),params->IntersectionPoint.z(),params->Radius));
		//g_pCityNode->getOrCreateStateSet()->getUniform("radius")->set(params->CircleRadius);
		osg::Vec4 bound(0,0,0,0);
		g_pCityNode->getOrCreateStateSet()->getUniform("bound")->set(bound);

	}
}


void City::setGeometryVisibility(GeometryData& geodata,int ison)
{
	osg::Geometry* geom = (osg::Geometry*)g_pCityNode->getDrawable(geodata.GeometryIndex);
	osg::Vec2Array* uvs = (osg::Vec2Array*)geom->getTexCoordArray(1);
	for (int i=geodata.StartVertex;i<geodata.EndVertex;i++)
	{
		(*uvs)[i]=osg::Vec2(ison,ison);
	}
	geom->dirtyDisplayList();

}

//************************************
// Method:    showSelection
// FullName:  City::showSelection
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: std::vector<unsigned int> & selection
//************************************
void City::showSelection(std::vector<unsigned int>& selection)
{
	for (int i=0;i<selection.size();i++)
	{
		unsigned int nodeindex = selection[i];
		Building& building = Buildings[nodeindex];
		//TriangleMesh& roofNode = TriangleMeshes[building.RoofNodeIndex];
		//TriangleMesh& facadeNode = TriangleMeshes[building.FacadeNodeIndex];
		setGeometryVisibility(m_node_2_startface_map[building.RoofNodeIndex],1);
		setGeometryVisibility(m_node_2_startface_map[building.FacadeNodeIndex],1);
	}
}

void City::hideSelection(std::vector<unsigned int>& selection)
{
	for (int i=0;i<selection.size();i++)
	{
		unsigned int nodeindex = selection[i];
		Building& building = Buildings[nodeindex];
		//TriangleMesh& roofNode = TriangleMeshes[building.RoofNodeIndex];
		//TriangleMesh& facadeNode = TriangleMeshes[building.FacadeNodeIndex];
		setGeometryVisibility(m_node_2_startface_map[building.RoofNodeIndex],0);
		setGeometryVisibility(m_node_2_startface_map[building.FacadeNodeIndex],0);
		m_hiddenSelection.push_back(nodeindex);
	}
}

void City::highlightSelection(std::vector<unsigned int>& selection)
{

	for (int i=0;i<selection.size();i++)
	{
		unsigned int nodeindex = selection[i];
		Building& building = Buildings[nodeindex];
		GeometryData& geodata = m_node_2_startface_map[building.RoofNodeIndex];
		osg::Geometry* geom = (osg::Geometry*)g_pCityNode->getDrawable(geodata.GeometryIndex);
		osg::Vec3Array* color = (osg::Vec3Array*)geom->getColorArray();
		for (int j=geodata.StartVertex;j<geodata.EndVertex;j++)
		{
		   (*color)[j] = osg::Vec3(0.0,0.5,0);
		}

		geom->dirtyDisplayList();
	}
}
void City::unhighlightSelection(std::vector<unsigned int>& selection)
{
	for (int i=0;i<selection.size();i++)
	{
		unsigned int nodeindex = selection[i];
		Building& building = Buildings[nodeindex];
		GeometryData& geodata = m_node_2_startface_map[building.RoofNodeIndex];
		osg::Geometry* geom = (osg::Geometry*)g_pCityNode->getDrawable(geodata.GeometryIndex);
		osg::Vec3Array* color = (osg::Vec3Array*)geom->getColorArray();
		for (int j=geodata.StartVertex;j<geodata.EndVertex;j++)
		{
			(*color)[j] = osg::Vec3(0,0,0);
		}

		geom->dirtyDisplayList();
	}
}

void City::setColorRamp( std::string colorramp_file )
{
	g_pClorRamp = osgDB::readImageFile(colorramp_file);
}

void City::updateSolarPanel( Building& buiding )
{
    
	std::vector<osg::Vec3d> vertices;
	std::vector<osg::Vec3d> normals;
	std::vector<osg::Vec2> uvs;
	TriangleMesh& node = TriangleMeshes[buiding.PanelIndex];
	buiding.Panel.create(vertices,normals,uvs,node);
	int start = node.FaceStart * 3;
	int end = start + node.FaceCount * 3;
	int i = 0;
	for (int k=start;k<end;k++)
	{
		(*GeometryVertices)[k]=vertices[i];
		(*GeometryNormals)[k]=normals[i];
		(*BakeUVs)[k]=uvs[i];
		i++;
	}
	//SetGeometryNode(&node,buiding.PanelIndex);

}

void City::updateGlobalParams()
{
	SolarAnalysisParams* params = IGlobalInterfaces::getInstance()->getSolarAnalysisParams();
	unhighlightSelection(m_selectedBuildings);
	if(params->QueryMode== SpatialQueryMode::Point)
	{
		selectByCircle(params->IntersectionPoint,0.1);
	}
	else if (params->QueryMode== SpatialQueryMode::Circle)
	{
		selectByCircle(params->IntersectionPoint,params->Radius);
	}
	/*else if (params->QueryMode== SpatialQueryMode::Rect)
	{
	selectByRectangle(params->XMin,params->YMin,params->XMax,params->YMax);
	}*/
	m_pSolarParam.time_step = params->Step;
	m_pSolarParam.bsky = params->Coefbh;
	m_pSolarParam.dsky = params->Coefdh;
	m_pSolarParam.linke = params->Linke;
	m_pSolarParam.elev = params->Elevation;
	m_pSolarParam.lon = params->Lon;
	m_pSolarParam.lat = params->Lat;
	m_pSolarParam.isInstantaneous = params->IsInstantaneous;
	//static bool show = true;
	//show = !show;
	//if(show)
	//  showSelection(m_hiddenSelection);
	//else
	//  hideSelection(m_selectedBuildings);

}
void City::exportSolarData(QString srcDir,QString destDir,int nodeIndex)
{
	TriangleMesh targetNode = TriangleMeshes[nodeIndex];
	std::stringstream ss;
	ss << targetNode.ID;
	QString name = ss.str().data();
	QString  normalfilename = destDir + name + "_normal" + ".surf";
	QString  posfilename =  destDir + name + "_pos" + ".surf";
	QString  solarfilename = destDir + name + ".solar";

	std::stringstream ss_src;
	ss_src << nodeIndex;
	QString name_src = ss_src.str().data();
	QString  normalfilename_src = srcDir + name_src + "_normal" + ".surf";
	QString  posfilename_src =  srcDir + name_src + "_pos" + ".surf";
	QString  solarfilename_src = srcDir + name_src + ".solar";
	if(QFileInfo(normalfilename_src).exists())
	   QFile::copy(normalfilename_src,normalfilename); 
	if(QFileInfo(posfilename_src).exists())
	   QFile::copy(posfilename_src,posfilename); 
	if(QFileInfo(solarfilename_src).exists())
	   QFile::copy(solarfilename_src,solarfilename); 

}

void City::exportSolarData(QString dir)
{
	QString destDir = dir;

	if(!QDir(destDir).exists())
    	QDir(destDir).mkdir(".");
	//QString solarDir = destDir + "/solar_data/";
	//if(!QDir(solarDir).exists())
	//	QDir(destDir).mkdir("./solar_data/");
	QString  srcDir =  QString(m_outputdirName.data()) + QString("/");
	for (int i=0;i<m_selectedBuildings.size();i++)
	{
		Building& building = Buildings[m_selectedBuildings[i]];
		exportSolarData(srcDir,destDir,building.RoofNodeIndex);
    	exportSolarData(srcDir,destDir,building.FacadeNodeIndex);
	}
}

void City::setFileName(std::string filename,const std::string outputdir )
{
	m_fileName = filename;
	QFileInfo fileInfo(filename.data());
	QString dir = fileInfo.absolutePath();
	m_dirName = dir.toLocal8Bit().data();
	m_outputdirName = outputdir.data();
	QDir dir2(m_outputdirName.data());
	if(!dir2.exists())
	{
		dir2.mkdir(".");
	}
}


void City::destroy()
{
	GeometryWrapper::destroy();
    m_node_2_startface_map.clear();
	m_selectedBuildings.clear();
	m_node_2_drawable_map.clear();
	this->Buildings.clear();
	if(g_pCalculationMasks)
	{
		delete g_pCalculationMasks;
		g_pCalculationMasks = NULL;
	}

}

void City::setCityNode( osg::Geode* cityNode )
{
	g_pCityNode = cityNode;
}
