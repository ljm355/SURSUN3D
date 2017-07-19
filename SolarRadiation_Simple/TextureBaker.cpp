#include "TextureBaker.h"
#include "osg\Geode"
#include <osgDB\ReadFile>
#include "CudaRenderer.h"
#include "osgDB\WriteFile"
#include "D3DRasterizer.h"

static D3DRasterizer* rasterizer;

TextureBaker::TextureBaker(void)
{
	if(!rasterizer)
		rasterizer = new D3DRasterizer;
	setColorRamp("./color_ramps/colorramp14.bmp");
}
void TextureBaker::setColorRamp( std::string colorramp_file )
{
	g_pClorRamp = osgDB::readImageFile(colorramp_file);
}

TextureBaker::~TextureBaker(void)
{

}
std::string getGeometrySpecificBakeFileName( unsigned int nodeIndex )
{
	std::stringstream ss;
	ss << nodeIndex << ".bmp";
	return ss.str();

}

std::string getGeometrySpecificSolaFileName( unsigned int nodeIndex )
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


void writeSolarFile(const char* filename,float* data,unsigned int width,unsigned int height,Stats* stats)
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
void readSolarFile(const char* filename, float*& data,unsigned int& width,unsigned int& height,Stats* stats)
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
void TextureBaker::renderSolarMap(float* rads,unsigned char*& map,unsigned int channel,unsigned int width,unsigned int height,float rmin,float rmax)
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

TriangleMesh TextureBaker::createDummyMesh(std::vector<unsigned int>& nodeIndices)
{
	osg::BoundingBox bb;
	bb.init();
	for (unsigned int j=0;j<nodeIndices.size();j++)
	{
		TriangleMesh mesh = this->TriangleMeshes[nodeIndices[j]];
		bb.expandBy(osg::BoundingBox(mesh.XMin,mesh.YMin,mesh.ZMin,mesh.XMax,mesh.YMax,mesh.ZMax));
	}
	TriangleMesh geonode;
	osg::BoundingSphere bs(bb);
	geonode.ID = 0;
	geonode.FaceStart = 0;
	geonode.FaceCount = 0;
	geonode.XMin = bb.xMin();
	geonode.YMin = bb.yMin();
	geonode.ZMin = bb.zMin();
	geonode.XMax = bb.xMax();
	geonode.YMax = bb.yMax();
	geonode.ZMax = bb.zMax();	
	geonode.Width2Height = 1;
	geonode.BBCenter = make_float3(bs.center().x(),bs.center().y(),bs.center().z());
	geonode.BBRadius = bs.radius();
	return geonode;
}
void TextureBaker::bakeTexture(const char* out_texture_dir, SolarParam sparam,unsigned short startDay,unsigned short endDay,float rasterCellSize  )
{
	std::vector<osg::Vec2i> textureSizes;
	rasterizer->createBuffer(getVertices(),getNormals(),getUVs(),NumberOfFaces * 3);
	LPDIRECT3DDEVICE9 pDevice = DXUTGetD3D9Device();

	float resol = 1;

	Stats global_stats[4];
	TriangleMesh tmpNodes[1];
	float fmin = FLT_MAX;
	float fmax =- FLT_MAX;

	int progress = 1;	
	std::string  dir =  std::string(out_texture_dir) + "/";
	for (int i=0;i<TexturedDrawables.size();i++)
	{
		osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
		TexturedDrawable tdrawable = TexturedDrawables[i];
		osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
		osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
		osg::ref_ptr<osg::Vec2Array> diffuseUVs = new osg::Vec2Array;
		osg::ref_ptr<osg::Vec2Array> bakeUVs = new osg::Vec2Array;

		std::string bakefileName = dir+  tdrawable.getBakeFileName();
		std::string  normalfilename = dir+  tdrawable.getNormalFileName();
		std::string  posfilename =  dir+  tdrawable.getPosFileName();
		std::string  shadowfilename =  dir+  tdrawable.getShadowFileName();
		std::string  solarfilename = dir+  tdrawable.getSolarFileName();

		//TriangleMesh targetNode = TriangleMeshes[nodeindex];
		float area = 0;
		for (unsigned int j=0;j<tdrawable.NodeIndices.size();j++)
		{
			TriangleMesh targetNode = this->TriangleMeshes[tdrawable.NodeIndices[j]];
		    area += calMeshArea(tdrawable.NodeIndices[j]);// (targetNode.XMax - targetNode.XMin) * (targetNode.YMax - targetNode.YMin);
		}
		float rate =  (float)tdrawable.Width/tdrawable.Height;//(float)tdrawable.Width / (float)tdrawable.Height;
		float pixelcount = area / resol;
		float fheight = sqrt(pixelcount / rate);
		float fwdith = pixelcount / fheight;
		unsigned int width = (unsigned int)fwdith;
		unsigned int height = (unsigned int)fheight;

		float minTexSize = 32;
		float maxTexSize = 2048;
		if(width< minTexSize)
			width = minTexSize;
		if(height < minTexSize)
			height =minTexSize;

		if(width > maxTexSize)
			width = maxTexSize;
		if(height > maxTexSize)
			height = maxTexSize;
		textureSizes.push_back(osg::Vec2i(width,height));
		rasterizer->setTextureSize(width,height);
		//osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES, 0 );
#pragma region node
		//for (unsigned int j=0;j<tdrawable.NodeIndices.size();j++)
		//{
		//	TriangleMesh targetNode = this->TriangleMeshes[tdrawable.NodeIndices[j]];
		//	tmpNodes[0] = targetNode;
		//	rasterizer->render(tmpNodes,1);
		//}
		for (int j=0;j<tdrawable.NodeIndices.size();j++)
		{				
			tmpNodes[j] = TriangleMeshes[tdrawable.NodeIndices[j]];
		}
		rasterizer->render(tmpNodes,tdrawable.NodeIndices.size());	
#pragma endregion node
			D3DXSaveSurfaceToFileA(posfilename.data(),D3DXIMAGE_FILEFORMAT::D3DXIFF_DDS,rasterizer->pDestSurfacePos,NULL,NULL);
			D3DXSaveSurfaceToFileA(normalfilename.data(),D3DXIMAGE_FILEFORMAT::D3DXIFF_DDS,rasterizer->pDestSurfaceNormal,NULL,NULL);


		D3DFORMAT fmt = D3DFMT_A32B32G32R32F;

		LPDIRECT3DTEXTURE9 pNormalTexture;
		D3DXCreateTextureFromFileExA(pDevice,normalfilename.data(),width,height,1,0,fmt,D3DPOOL_MANAGED,D3DX_FILTER_NONE,D3DX_FILTER_NONE,0,NULL,NULL, &pNormalTexture);

		LPDIRECT3DTEXTURE9 pPosTexture;
		D3DXCreateTextureFromFileExA(pDevice,posfilename.data(),width,height,1,0,fmt,D3DPOOL_MANAGED,D3DX_FILTER_NONE,D3DX_FILTER_NONE,0,NULL,NULL, &pPosTexture);


		float4* mapVertices = loadVertexMap(pPosTexture);
		float4* mapNormals =  loadVertexMap(pNormalTexture);
		//float4* mapVertices = new float4[Width*Height];
		//float4* mapNormals = new float4[Width*Height];

		pPosTexture->Release();
		pNormalTexture->Release();


		int numdays = endDay - startDay;

		std::vector<SolarRadiation*> radsSeries;
		unsigned int icell = 0;
		unsigned int cellcount = width * height;
#pragma region days
		TriangleMesh targetNode;
		if(tdrawable.NodeIndices.size() > 1)
		   targetNode = createDummyMesh(tdrawable.NodeIndices);
		else 
		   targetNode =  this->TriangleMeshes[tdrawable.NodeIndices[0]];
		/*for (unsigned int j=0;j<tdrawable.NodeIndices.size();j++)
		{
			TriangleMesh targetNode = this->TriangleMeshes[tdrawable.NodeIndices[j]];*/
		for (int nDay=startDay;nDay<=endDay;nDay++)
		{
			//IGlobalInterfaces::getInstance()->getSolarAnalysisParams()->MainWin->statusBar()->showMessage(QString("progress: %1/%2").arg(progress++).arg(nodenum*numdays)); 

			sparam.day = nDay;
			std::vector<SunVector> sunvectors = getSunVectors(sparam);

			std::vector<float3> lights = sunVector2LightDir(sunvectors);
			bool* shadowInfo = new bool[lights.size()];
			//lights.clear();
			//lights.push_back(make_float3(0.2,-1,0.1));
			unsigned int numlights = lights.size();

			std::vector<Box> frustrums(lights.size());
			std::vector<float>  viewMatrices(lights.size() * 16);

			setLights(&lights[0],lights.size());
			bool* shadow = new bool[width*height*lights.size()];
			for (int nLight=0;nLight<lights.size();nLight++)
			{
				osg::BoundingSphere bs(osg::Vec3(targetNode.BBCenter.x,targetNode.BBCenter.y,targetNode.BBCenter.z),targetNode.BBRadius);
				osg::Matrixf m;
				osg::BoundingBox bb;
				osg::Vec3 lightdir(lights[nLight].x,lights[nLight].y,lights[nLight].z);			
				calLightMatrix(lightdir,bs,m,bb);
				Box frustrum;
				frustrum.MinMax[0] = make_float3(bb.xMin(),bb.yMin(),bb.zMin());
				frustrum.MinMax[1] = make_float3(bb.xMax(),bb.yMax(),bb.zMax());
				frustrums[nLight] = frustrum;
				float* pSrc = (float*)m.ptr();
				float* pDest = &viewMatrices[0] + 16 * nLight;
				memcpy(pDest,pSrc,sizeof(float) * 16);
			}
			setTargetGeometry(&targetNode);
			executeKernelShadowCasting(shadow,mapVertices,mapNormals,width,height,&viewMatrices[0],&frustrums[0]);

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
				rads[icell] = rad;
			}
			radsSeries.push_back(rads);
			delete[] shadow;
		}
		//}
#pragma endregion days
			// t = clock();

		
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
		writeSolarFile(solarfilename.data(),(float*)radsSum,width,height,stats);
		//printf("%d/%d\n",i,nodenum);
		for (int j=0;j<radsSeries.size();j++)
		{
			delete[] radsSeries[j];
		}

		delete[] mapVertices;
		delete[] mapNormals;

	}

	//global statistics
	for (int i=0;i<4;i++)
	{
		global_stats[i].fmean = global_stats[i].sum / global_stats[i].validnum;
	}
	if(global_stats[0].validnum > 0)
	{
		std::ofstream ofs(dir + "global.stats",std::ios_base::out |  std::ios::binary);
		ofs.write((char*)global_stats,sizeof(Stats) * 4);
		ofs.flush();
		ofs.close();
	}

}
void TextureBaker::symbolize(const char* out_texture_dir)
{

	std::string dir = std::string(out_texture_dir) + "/";
	Stats global_stats[4];
	std::ifstream ifs;
	ifs.open( dir + "global.stats",std::ios_base::in |  std::ios::binary);
	ifs.read((char*)global_stats,sizeof(Stats)*4);
	ifs.close();

	//QString outdir = m_dirName +  "/";
	float fmin = global_stats[0].fmin;
	float fmax = global_stats[0].fmax;

	for (int i=0;i<TexturedDrawables.size();i++)
	{
		TexturedDrawable tdrawable = TexturedDrawables[i];
		osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
		osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
		osg::ref_ptr<osg::Vec2Array> diffuseUVs = new osg::Vec2Array;
		osg::ref_ptr<osg::Vec2Array> bakeUVs = new osg::Vec2Array;

		std::string  bakefileName = dir+  tdrawable.getBakeFileName();
		std::string  normalfilename = dir+  tdrawable.getNormalFileName();
		std::string  posfilename =  dir+  tdrawable.getPosFileName();
		std::string  shadowfilename =  dir+  tdrawable.getShadowFileName();
		std::string  solarfilename = dir+  tdrawable.getSolarFileName();

		if(!QFileInfo(solarfilename.data()).exists())
		{
			continue;
		}

		unsigned int width;// = textureSizese[i].x();
		unsigned int height;// =  textureSizese[i].y();
        Stats stats[4];
		//Stats* pStats = (Stats*)stats;
		float* rads;
		readSolarFile(solarfilename.data(),rads,width,height,(Stats*)stats);
		D3DFORMAT fmt = D3DFMT_A32B32G32R32F;

		osg::ref_ptr<osg::Image> solarMapImage = new osg::Image;
		solarMapImage->allocateImage(width,height,1,GL_RGB,GL_UNSIGNED_BYTE);
		unsigned char* imageData = solarMapImage->data();

		unsigned int icell = 0;
		//float* rads =  (float*)(new SolarRadiation[width*height]);

		
		//renderAspectMap(mapNormals,imageData,0,width,height,0,360);
		renderSolarMap((float*)rads,imageData,0,width,height,fmin,fmax);
		osgDB::writeImageFile(*solarMapImage,bakefileName);
		delete[] rads;
	}


}
osg::Node* TextureBaker::toOSG(const char* out_texture_dir)
{
	osg::Geode* geode = new osg::Geode;
	for (int i=0;i<TexturedDrawables.size();i++)
	{
		osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
		TexturedDrawable tdrawable = TexturedDrawables[i];
		osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
		osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
		osg::ref_ptr<osg::Vec2Array> diffuseUVs = new osg::Vec2Array;
		osg::ref_ptr<osg::Vec2Array> bakeUVs = new osg::Vec2Array;
		//osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES, 0 );
		for (unsigned int j=0;j<tdrawable.NodeIndices.size();j++)
		{
			TriangleMesh node = this->TriangleMeshes[tdrawable.NodeIndices[j]];
			for (unsigned int k=node.FaceStart;k<node.FaceStart+node.FaceCount;k++)
			{
				for (int l=0;l<3;l++)
				{
					unsigned int index = k*3 + l;
					vertices->push_back((*GeometryVertices)[index]);
					normals->push_back((*GeometryNormals)[index]);
					if(BakeSlot > -1)
						bakeUVs->push_back((*BakeUVs)[index]);
				}
			}

		}
		geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES,0,vertices->size()));
		geom->setVertexArray( vertices.get() );
		geom->setNormalArray(normals.get());
		geom->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
		std::map<std::string,osg::ref_ptr<osg::Image>>::iterator iter;

		if(BakeSlot > -1)
		{
			std::string bakeFile = std::string(out_texture_dir) + "/" +  tdrawable.getBakeFileName();
			osg::ref_ptr<osg::Texture2D> bakeTex = new osg::Texture2D;
			iter = m_img_map.find(bakeFile);
			if(iter != m_img_map.end())
			{
				bakeTex->setImage(iter->second.get());
			}
			else
			{
				osg::ref_ptr<osg::Image> bakeImg = osgDB::readImageFile(bakeFile);
				m_img_map.insert(std::pair<std::string,osg::ref_ptr<osg::Image>>(bakeFile,bakeImg));
				bakeTex->setImage(bakeImg.get());
			}
			geom->setTexCoordArray( BakeSlot, bakeUVs );
			geom->getOrCreateStateSet()->setTextureAttributeAndModes(BakeSlot,bakeTex.get());
		}
		geode->addDrawable(geom.get());

	}
	geode->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);

	m_img_map.clear();
	return geode;

}
void TextureBaker::loadFromOSGNode( osg::Node* node )
{
	GeometryWrapper::loadFromOSGNode(node);
	setGeometry(getVertices(),NumberOfFaces*3,TriangleMeshes,this->NumberOfMeshes);
}
