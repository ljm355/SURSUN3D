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
}


TextureBaker::~TextureBaker(void)
{

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
void TextureBaker::bakeTexture(const char* out_texture_dir, osg::Vec3 lightdir )
{

	rasterizer->createBuffer(getVertices(),getNormals(),getUVs(),NumberOfFaces * 3);
	std::vector<float3> lights;
	lights.push_back(make_float3(lightdir.x(),lightdir.y(),lightdir.z()));
	std::vector<Box> frustrums(lights.size());
	std::vector<float>  viewMatrices(lights.size() * 16);
	LPDIRECT3DDEVICE9 pDevice = DXUTGetD3D9Device();

	//setGeometry(getVertices(),NumberOfFaces*3,TriangleMeshes,this->NumberOfMeshes);
	setLights(&lights[0],lights.size());
	float resol = 0.01;

	TriangleMesh tmpNodes[1000];

	unsigned int nodenum = NumberOfMeshes;
	//if(m_nodeMask.size() > 0)
	//{
	//	nodenum= m_nodeMask.size();
	//}
	for (int i=0;i<TexturedDrawables.size();i++)
	{
		TexturedDrawable tdrawable = TexturedDrawables[i];

		QFileInfo info(tdrawable.FileName.data());
		QString name = info.baseName();
		QString dir = QString(out_texture_dir) + "/";

		int width = (int)tdrawable.Width;
		int height = (int)tdrawable.Height;
		//QString texturefilename = tdrawable.FileName.data();
		QString bakefilename =  dir + "/" +  QString(tdrawable.getBakeFileName().data());
		QString normalfilename = dir + name + "_normal" + ".dds";
		QString posfilename =  dir + name + "_pos" + ".dds";
		QString shadowfilename =  dir + name + "_shadow" + ".dat";
		//if(!QFileInfo(posfilename).exists() || !QFileInfo(normalfilename).exists())
		//{
			rasterizer->setTextureSize(width,height);
			for (int n=0;n<tdrawable.NodeIndices.size();n++)
			{				
				tmpNodes[n] = TriangleMeshes[tdrawable.NodeIndices[n]];
				//calNodeArea(tdrawable.NodeIndices[n]);
			}
			rasterizer->render(tmpNodes,tdrawable.NodeIndices.size());	
			D3DXSaveSurfaceToFileA(posfilename.toStdString().data(),D3DXIMAGE_FILEFORMAT::D3DXIFF_DDS,rasterizer->pDestSurfacePos,NULL,NULL);
			D3DXSaveSurfaceToFileA(normalfilename.toStdString().data(),D3DXIMAGE_FILEFORMAT::D3DXIFF_DDS,rasterizer->pDestSurfaceNormal,NULL,NULL);
		//}		


		if(!QFileInfo(posfilename).exists() || !QFileInfo(normalfilename).exists())
		{
			continue;
		}

		D3DFORMAT fmt = D3DFMT_A32B32G32R32F;

		LPDIRECT3DTEXTURE9 pNormalTexture;
		D3DXCreateTextureFromFileExA(pDevice,normalfilename.toStdString().data(),width,height,1,0,fmt,D3DPOOL_MANAGED,D3DX_FILTER_NONE,D3DX_FILTER_NONE,0,NULL,NULL, &pNormalTexture);

		LPDIRECT3DTEXTURE9 pPosTexture;
		D3DXCreateTextureFromFileExA(pDevice,posfilename.toStdString().data(),width,height,1,0,fmt,D3DPOOL_MANAGED,D3DX_FILTER_NONE,D3DX_FILTER_NONE,0,NULL,NULL, &pPosTexture);



		float4* mapVertices = loadVertexMap(pPosTexture);
		float4* mapNormals =  loadVertexMap(pNormalTexture);
		std::vector<float4> tmpVertices(width*height);
		std::vector<float4> tmpNormals(width*height);
		memcpy(&tmpVertices[0],mapVertices,sizeof(float4)*width*height);
		memcpy(&mapNormals[0],mapNormals,sizeof(float4)*width*height);

		bool* shadow = new bool[width*height];
		pPosTexture->Release();
		pNormalTexture->Release();
		TriangleMesh targetNode;
		if(tdrawable.NodeIndices.size() > 1)
			targetNode = createDummyMesh(tdrawable.NodeIndices);
		else 
			targetNode =  this->TriangleMeshes[tdrawable.NodeIndices[0]];
		//for (int n=0;n<tdrawable.NodeIndices.size();n++)
		//{				
			//TriangleMesh targetNode = TriangleMeshes[tdrawable.NodeIndices[n]];
		for (int j=0;j<lights.size();j++)
		{
			osg::BoundingSphere bs(osg::Vec3(targetNode.BBCenter.x,targetNode.BBCenter.y,targetNode.BBCenter.z),targetNode.BBRadius);
			osg::Matrixf m;
			osg::BoundingBox bb;
			osg::Vec3 lightdir(lights[j].x,lights[j].y,lights[j].z);			
			calLightMatrix(lightdir,bs,m,bb);
			Box frustrum;
			frustrum.MinMax[0] = make_float3(bb.xMin(),bb.yMin(),bb.zMin());
			frustrum.MinMax[1] = make_float3(bb.xMax(),bb.yMax(),bb.zMax());
			frustrums[j] = frustrum;
			float* pSrc = (float*)m.ptr();
			float* pDest = &viewMatrices[0] + 16 * j;
			memcpy(pDest,pSrc,sizeof(float) * 16);
		}
		setTargetGeometry(&targetNode);
		//executeKernelFrustrumCullingCPU(width,height,lights[0],frustrums[0],&viewMatrices[0],TriangleMeshes,NumberOfMeshes,&targetNode);
		executeKernelShadowCasting(shadow,mapVertices,mapNormals,width,height,&viewMatrices[0],&frustrums[0]);
		//}


		osg::ref_ptr<osg::Image> img = new osg::Image;
		img->allocateImage(width,height,1,GL_RGB,GL_UNSIGNED_BYTE);
		unsigned char* data = img->data();
		int lightindex = lights.size() - 1;

		unsigned int cellcount = width * height;

		for (int j =0;j<cellcount;j++)
		{
			if(mapNormals[j].w < -1)
				continue;// make_float3(1,0,0);
			float3 normal = normalize(make_float3(mapNormals[j]))*0.5+0.5;
			int isShadowed = shadow[j];
			if(isShadowed)
				normal = normal * 0.25;
			data[j*3]=   (unsigned char)(normal.x*255);
			data[j*3+1]= (unsigned char)(normal.y*255);
			data[j*3+2]= (unsigned char)(normal.z*255);
		}

		osgDB::writeImageFile(*img,bakefilename.toStdString());
		printf("%d/%d\n",i,nodenum);
		delete[] mapVertices;
		delete[] mapNormals;
		delete[] shadow;
	}

	destroyGeometry();
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
