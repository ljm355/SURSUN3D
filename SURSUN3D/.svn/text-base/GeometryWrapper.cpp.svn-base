#include "GeometryWrapper.h"
#include "osg\Group"
#include "osg\Geode"
#include "osgDB\fstream"
typedef std::vector<osg::Drawable*> Drawables;
typedef std::map<osg::Texture2D*,TexturedDrawable> TextureDrawableItem;
GeometryWrapper::GeometryWrapper()
{
	DiffuseSlot = 0;
	BakeSlot = 1;
	TriangleMeshes = NULL;
	GeometryVertices = NULL;
	GeometryNormals = NULL;
	DiffuseUVs = NULL;
	BakeUVs = NULL;
	g_mTranslate = osg::Vec3d(0,0,0);
}
GeometryWrapper::~GeometryWrapper(void)
{
	destroy();
}
void GeometryWrapper::apply(osg::Node* node)
{
	g_mExtent.init();
	GeometryVertices = new osg::Vec3Array;
	GeometryNormals  = new osg::Vec3Array;
	if(DiffuseSlot > -1)
	{
		DiffuseUVs = new osg::Vec2Array;
	}
	if(BakeSlot > -1)
	{
		BakeUVs = new osg::Vec2Array;
	}

	NumberOfFaces = 0;
	NumberOfMeshes = 0;
	collectStats(node);

	CurNodeOffset = 0;
	CurFaceOffset = 0;
	TriangleMeshes = new TriangleMesh[NumberOfMeshes];
	NumberOfMeshes = 0;
	osg::Group* group = dynamic_cast<osg::Group*>(node);
	if(group)
	{
		for (int i=0;i<group->getNumChildren();i++)
		{
			applyNode(group->getChild(i));
		}
	}
	else
	{
		applyNode(node);
	}
	TriangleMeshes-=NumberOfMeshes;
	NumberOfFaces = CurFaceOffset;
	TexturedDrawables = getDrawablesByDescendingOrder();
	build_NodeDrawableMap();
}
void GeometryWrapper::collectStats( osg::Node* node )
{

	osg::Group* group = dynamic_cast<osg::Group*>(node);
	osg::Geode* geode = dynamic_cast<osg::Geode*>(node);

	if(group)
	{
		for (int i=0;i<group->getNumChildren();i++)
		{
			collectStats(group->getChild(i));

		}
	}
	else if(geode)
	{

		for (int i=0;i<geode->getNumDrawables();i++)
		{
			osg::Drawable* drawable = geode->getDrawable(i);
			osg::Geometry* drawableGeo = dynamic_cast<osg::Geometry*>(drawable );
			if(!drawableGeo)
				continue;

			//if(drawableGeo->getNumTexCoordArrays() < 1)
			//	continue;
			//osg::Texture2D* tex = dynamic_cast<osg::Texture2D*>(drawable->getOrCreateStateSet()->getTextureAttribute(TextureSlot,osg::StateAttribute::TEXTURE));
			//if(!tex)
			//	continue;				
			//osg::Image* image =tex->getImage();
			//if(!image || !image->valid())
			//	continue;
			osg::MatrixList matlist = geode->getWorldMatrices();
			osg::Matrix matWorld = osg::Matrix::identity();
			for (int i=0;i<matlist.size();i++)
			{
				matWorld = matlist[i] * matWorld;
			}
			for(int i=0;i<drawableGeo->getNumPrimitiveSets();i++) 
			{
				osg::PrimitiveSet* primitiveSet = drawableGeo->getPrimitiveSet(i);
				//NumberOfFaces += primitiveSet->getNumIndices() / 3;
			}
			NumberOfMeshes++;

		}
	}
}
void GeometryWrapper::build_NodeDrawableMap()
{
	m_node_2_drawable_map.clear();
	for (int i=0;i<TexturedDrawables.size();i++)
	{

		TexturedDrawable tdrawable = TexturedDrawables[i];
		for (int j=0;j<tdrawable.NodeIndices.size();j++)
		{
			m_node_2_drawable_map.insert(std::pair<unsigned int,unsigned int>(tdrawable.NodeIndices[j],i));
		}
	}
}
void GeometryWrapper::applyNode( osg::Node* node )
{
	osg::Group* group = dynamic_cast<osg::Group*>(node);
	osg::Geode* geode = dynamic_cast<osg::Geode*>(node);
	if(group)
	{
		for (int i=0;i<group->getNumChildren();i++)
		{
			applyNode(group->getChild(i));
		}
	}
	else if(geode)
	{

		for (int i=0;i<geode->getNumDrawables();i++)
		{
			osg::Drawable* drawable = geode->getDrawable(i);
			osg::Geometry* drawableGeo = dynamic_cast<osg::Geometry*>(drawable );
			if(!drawableGeo)
				continue;
			osg::Texture2D* tex = dynamic_cast<osg::Texture2D*>(drawable->getOrCreateStateSet()->getTextureAttribute(BakeSlot,osg::StateAttribute::TEXTURE));
			if(!tex)
			{
                tex = dynamic_cast<osg::Texture2D*>(drawable->getOrCreateStateSet()->getTextureAttribute(DiffuseSlot,osg::StateAttribute::TEXTURE));
			}

			if(!tex)
			{
				tex = dynamic_cast<osg::Texture2D*>(geode->getOrCreateStateSet()->getTextureAttribute(BakeSlot,osg::StateAttribute::TEXTURE));
			}
			if(!tex)
			{
				tex = dynamic_cast<osg::Texture2D*>(geode->getOrCreateStateSet()->getTextureAttribute(DiffuseSlot,osg::StateAttribute::TEXTURE));
			}

			//if(!tex || !tex->getImage() ||  !tex->getImage()->valid())
			//{
   //             continue;
			//}

			osg::MatrixList matlist = geode->getWorldMatrices();
			osg::Matrix matWorld = osg::Matrix::identity();
			//for (int i=0;i<matlist.size();i++)
			//{
			//	matWorld = matlist[i] * matWorld;
			//}
			//printf("%d\n",CurNodeOffset);

			matWorld = matlist[0];
			unsigned int curFaceOffset = CurFaceOffset;
			applyDrawble(drawableGeo,matWorld);
			TriangleMesh geonode;
			osg::BoundingBox bb = m_CurBB;//drawableGeo->getBound();
			osg::BoundingSphere bs(bb);
			geonode.ID = CurNodeOffset;
			geonode.FaceStart = curFaceOffset;
			geonode.FaceCount = CurFaceOffset - curFaceOffset;
			geonode.XMin = bb.xMin();
			geonode.YMin = bb.yMin();
			geonode.ZMin = bb.zMin();
			geonode.XMax = bb.xMax();
			geonode.YMax = bb.yMax();
			geonode.ZMax = bb.zMax();	
			geonode.Width2Height = m_CurWidth2Height;
			geonode.BBCenter = make_float3(bs.center().x(),bs.center().y(),bs.center().z());
			geonode.BBRadius = bs.radius();
			*TriangleMeshes = geonode;
			TriangleMeshes++;
			NumberOfMeshes++;

			CurNodeOffset++;
			//if(CurNodeOffset == 23)
			//{
   //             printf("");
			//}

			TexturedDrawable tdrawable;
			bool shouldAddNode = false;
			if(tex && tex->getImage() && tex->getImage()->valid())
			{

				std::map<osg::Texture2D*,int>::iterator iter = TextureList.find(tex); 
				if(iter != TextureList.end())
				{
					std::map<int,TexturedDrawable>::iterator iter2 = NodeList.find(iter->second);	
					if(iter2 != NodeList.end())
					{
						tdrawable = iter2->second;	
						iter2->second.Area += m_CurrentArea;
						iter2->second.NodeIndices.push_back(CurNodeOffset-1);
						//iter2->second.NumOfNodes = iter2->second.NodeIndices.size();
					}

				}
				else   
				{					
					////tdrawable.Texture = tex;
					tdrawable.Area = 0;
					tdrawable.Width = tex->getImage()->s();
					tdrawable.Height = tex->getImage()->t();
					tdrawable.FileName = tex->getImage()->getFileName();
					TextureList.insert(std::pair<osg::Texture2D*,int>(tex, NodeList.size()));
					tdrawable.NodeIndices.push_back(CurNodeOffset-1);
					shouldAddNode = true;
				}
			}
			else
			{
				//tdrawable.Texture = tex;
				tdrawable.Width =  DEFAULT_TEXTURE_SIZE;
				tdrawable.Height = DEFAULT_TEXTURE_SIZE;
				tdrawable.FileName = "";
				tdrawable.Area = 0;
				tdrawable.NodeIndices.push_back(CurNodeOffset-1);
				shouldAddNode = true;
			}

			if(shouldAddNode)
			{
				//tdrawable.NumOfNodes = tdrawable.NodeIndices.size();
				tdrawable.Area += m_CurrentArea;
				NodeList.insert(std::pair<int,TexturedDrawable>(NodeList.size(), tdrawable));
			}
		}

	}


}
float GeometryWrapper::calMeshArea(unsigned int num)
{

	TriangleMesh node = this->TriangleMeshes[num];
	float area = 0;
	for (unsigned int k=node.FaceStart;k<(node.FaceStart+node.FaceCount);k++)
	{
		osg::Vec3 p1 = (*GeometryVertices)[k * 3];
		osg::Vec3 p2 = (*GeometryVertices)[k * 3 + 1];
		osg::Vec3 p3 = (*GeometryVertices)[k * 3 + 2];

		area+= calTriangleArea(p1,p2,p3);
	}
	return area;

}
class FaceVisitor
{
public:
	FaceVisitor(osg::PrimitiveSet* pset)
	{
        pSet = pset;

	}
public:
	osg::PrimitiveSet* pSet;
	std::vector<unsigned int> getFaceIndices()
	{

		std::vector<unsigned int> indices;
		unsigned int idx = pSet->getNumIndices();
		unsigned int numofprims;


		if(pSet->getMode() == osg::PrimitiveSet::TRIANGLE_FAN)
		{
			numofprims = idx-2;
			for (int i=0;i<numofprims;i++)
			{
				indices.push_back(pSet->index(0));
				indices.push_back(pSet->index(i + 1));
				indices.push_back(pSet->index(i + 2));
			}
			//indices.push_back(pSet->index(0));
			//indices.push_back(pSet->index(idx-1));
			//indices.push_back(pSet->index(1));
		}
		else if (pSet->getMode() == osg::PrimitiveSet::TRIANGLES)
		{
			for (int i=0;i<idx;i++)
			{
				indices.push_back(pSet->index(i));
			}
		}
		else if (pSet->getMode() == osg::PrimitiveSet::TRIANGLE_STRIP)
		{
			numofprims = (idx - 2) / 2;
			for (int i=0;i<numofprims;i++)
			{
				indices.push_back(pSet->index(i * 2));
				indices.push_back(pSet->index(i * 2 + 1));
				indices.push_back(pSet->index(i * 2 + 2));

				indices.push_back(pSet->index(i * 2 + 2));		
				indices.push_back(pSet->index(i * 2 + 1));
				indices.push_back(pSet->index(i * 2 + 3));
			}

			if(numofprims + 2 % 2 != 0)
			{
				indices.push_back(pSet->index(idx-3));		
				indices.push_back(pSet->index(idx-2));
				indices.push_back(pSet->index(idx-1));
			}


		}
	/*	else
		{
			printf("´íÎó\n");
		}*/


		return indices;
	}
};
bool GeometryWrapper::applyDrawble( osg::Geometry* drawable,osg::Matrix matWorld)
{
	m_CurrentArea = 0;
	m_CurBB.init();
	m_CurWidth2Height = 1;
	unsigned int startFace = CurFaceOffset;

	osg::Vec3Array* vertices = (osg::Vec3Array *) drawable->getVertexArray();
	osg::Vec3Array* normals = (osg::Vec3Array *) drawable->getNormalArray();
	osg::Vec2Array* diffuseuvs = NULL;
	osg::Vec2Array* bakeuvs = NULL;
	osg::Vec3Array* diffuseuvs3 = NULL;
	osg::Vec3Array* bakeuvs3 = NULL;
	
	if(DiffuseSlot > -1)
	{
		diffuseuvs = (osg::Vec2Array *) drawable->getTexCoordArray(DiffuseSlot);
		if(drawable->getTexCoordArray(DiffuseSlot) && drawable->getTexCoordArray(DiffuseSlot)->getType() == osg::Array::Vec3ArrayType)
		{
			diffuseuvs = NULL;
			diffuseuvs3 = (osg::Vec3Array*) drawable->getTexCoordArray(DiffuseSlot);
		}
	}

	osg::Vec2Array* uvs_bake = NULL;
	if(BakeSlot > -1)
	{


	   if(drawable->getTexCoordArray(BakeSlot) && drawable->getTexCoordArray(BakeSlot)->getType() == osg::Array::Vec2ArrayType)
	   {
		   bakeuvs3 = NULL;
		   bakeuvs = (osg::Vec2Array*) drawable->getTexCoordArray(BakeSlot);
		   if(!bakeuvs)
		   {
			   bakeuvs = diffuseuvs;
		   }
	   }
	   else if(drawable->getTexCoordArray(BakeSlot) && drawable->getTexCoordArray(BakeSlot)->getType() == osg::Array::Vec3ArrayType)
	   {
		   bakeuvs = NULL;
		   bakeuvs3 = (osg::Vec3Array*) drawable->getTexCoordArray(BakeSlot);
		   if(!bakeuvs3)
		   {
			   bakeuvs3 = diffuseuvs3;
		   }
	   }

	   if(!bakeuvs && !bakeuvs3)
	   {
		   if(diffuseuvs)
		   {
			   bakeuvs = diffuseuvs;
		   }
		   else if(diffuseuvs3)
		   {
			   bakeuvs3 = diffuseuvs3;
		   }
	   }
	}

	osg::BoundingBox uvbb;
	uvbb.init();
	for(int i=0;i<drawable->getNumPrimitiveSets();i++) 
	{
		osg::PrimitiveSet* primitiveSet = drawable->getPrimitiveSet(i);
		FaceVisitor visitor(primitiveSet);
		//unsigned int facenum = visitor.getFaceNum();
		std::vector<unsigned int> indices = visitor.getFaceIndices();

		for(int k=0;k< indices.size() / 3;k++) 
		{			
			
			CurFaceOffset++;

			unsigned int k1 = indices[k * 3];
			unsigned int k2 = indices[k * 3 + 1];
			unsigned int k3 = indices[k * 3 + 2];


			osg::Vec3 p1 = (*vertices)[k1] * matWorld-g_mTranslate;
			osg::Vec3 p2 = (*vertices)[k2] * matWorld-g_mTranslate;
			osg::Vec3 p3 = (*vertices)[k3] * matWorld-g_mTranslate; 

			m_CurBB.expandBy(p1);
			m_CurBB.expandBy(p2);
			m_CurBB.expandBy(p3);


			m_CurrentArea+=calTriangleArea(p1,p2,p3);
       
			GeometryVertices->push_back(p1);
			GeometryVertices->push_back(p2);
			GeometryVertices->push_back(p3);

			osg::Vec3 n1 = (*normals)[k1];
			osg::Vec3 n2 = (*normals)[k2];
			osg::Vec3 n3 = (*normals)[k3];

			GeometryNormals->push_back(n1);
			GeometryNormals->push_back(n2);
			GeometryNormals->push_back(n3);;

			if(DiffuseSlot > -1)
			{
				if(diffuseuvs)
				{
					osg::Vec2 t1 = (*diffuseuvs)[k1];
					osg::Vec2 t2 = (*diffuseuvs)[k2];
					osg::Vec2 t3 = (*diffuseuvs)[k3]; 
					DiffuseUVs->push_back(t1);
					DiffuseUVs->push_back(t2);
					DiffuseUVs->push_back(t3);
				}
				else if(diffuseuvs3)
				{
					osg::Vec3 t1 = (*diffuseuvs3)[k1];
					osg::Vec3 t2 = (*diffuseuvs3)[k2];
					osg::Vec3 t3 = (*diffuseuvs3)[k3]; 
					DiffuseUVs->push_back(osg::Vec2(t1.x(),t1.y()));
					DiffuseUVs->push_back(osg::Vec2(t2.x(),t2.y()));;
					DiffuseUVs->push_back(osg::Vec2(t3.x(),t3.y()));
				}
				else
				{
					//return false;
					DiffuseUVs->push_back(osg::Vec2(-1,-1));
					DiffuseUVs->push_back(osg::Vec2(-1,-1));
					DiffuseUVs->push_back(osg::Vec2(-1,-1));
				}

			}
	
	
			if(BakeSlot > -1)
			{

				if(bakeuvs)
				{
					osg::Vec2 t1 = (*bakeuvs)[k1];
					osg::Vec2 t2 = (*bakeuvs)[k2];
					osg::Vec2 t3 = (*bakeuvs)[k3]; 
					BakeUVs->push_back(t1);
					BakeUVs->push_back(t2);
					BakeUVs->push_back(t3);
					uvbb.expandBy(osg::Vec3(t1.x(),t1.y(),0));
					uvbb.expandBy(osg::Vec3(t2.x(),t2.y(),0));
					uvbb.expandBy(osg::Vec3(t3.x(),t3.y(),0));
				}
				else if(bakeuvs3)
				{
					osg::Vec3 t1 = (*bakeuvs3)[k1];
					osg::Vec3 t2 = (*bakeuvs3)[k2];
					osg::Vec3 t3 = (*bakeuvs3)[k3]; 
					BakeUVs->push_back(osg::Vec2(t1.x(),t1.y()));
					BakeUVs->push_back(osg::Vec2(t2.x(),t2.y()));;
					BakeUVs->push_back(osg::Vec2(t3.x(),t3.y()));
					uvbb.expandBy(osg::Vec3(t1.x(),t1.y(),0));
					uvbb.expandBy(osg::Vec3(t2.x(),t2.y(),0));
					uvbb.expandBy(osg::Vec3(t3.x(),t3.y(),0));
				}
				else
				{
					//return false;
					BakeUVs->push_back(osg::Vec2(-1,-1));
					BakeUVs->push_back(osg::Vec2(-1,-1));
					BakeUVs->push_back(osg::Vec2(-1,-1));
				}
				
			}


		}

	}

	//printf("%f,%f,%f,%f\n",uvbb.xMin(),uvbb.xMax(),uvbb.yMin(),uvbb.yMax());
	//if(BakeSlot > -1 && uvbb.xMin() < 0 || uvbb.xMax() > 1 || uvbb.yMin() < 0 || uvbb.yMax() > 1)
	if(BakeSlot > -1 && (abs(uvbb.xMin()) > 0.00001 || abs(uvbb.xMax() -1) > 0.00001 || abs(uvbb.yMin()) > 0.00001 || abs(uvbb.yMax() -1) > 0.00001))
	{


		osg::Vec2 minUV(uvbb.xMin(),uvbb.yMin());
		osg::Vec2 maxUV(uvbb.xMax(),uvbb.yMax());
		osg::Vec2 diffUV = (maxUV - minUV);
		m_CurWidth2Height = diffUV.x() / diffUV.y();
		diffUV.x() = 1.0 / diffUV.x();
		diffUV.y() = 1.0 / diffUV.y();
		for (unsigned int i=startFace;i<CurFaceOffset;i++)
		{
			for (int j=0;j<3;j++)
			{
				unsigned int index = i * 3 + j;
				osg::Vec2 uv = (*BakeUVs)[index];
				uv = uv - minUV;
				uv.x() = uv.x() * diffUV.x();
				uv.y() = uv.y() * diffUV.y();
				(*BakeUVs)[index] =  uv;
			}


		}

	}
	g_mExtent.expandBy(m_CurBB);
	return true;
}
bool compare_texture_size (TexturedDrawable t1, TexturedDrawable t2)
{

	if(t1.Width > t2.Width)
		return true;
	else if(t1.Width == t2.Width)
	{
		if(t1.Height > t2.Height)
			return true;
		return false;
	}
	return false;

}
std::vector<TexturedDrawable> GeometryWrapper::getDrawablesByDescendingOrder()
{
	std::list<TexturedDrawable> drawableList;
	std::map<osg::Texture2D*,int>::iterator iter;	
	for(iter = TextureList.begin();iter != TextureList.end();++iter)
	{
		TexturedDrawable tdrawable = NodeList.find(iter->second)->second;
		drawableList.push_back(tdrawable);
	}
	drawableList.sort(compare_texture_size);
	std::vector<TexturedDrawable> drawableArray;
	std::list<TexturedDrawable>::iterator it;

	for (it=drawableList.begin(); it!=drawableList.end(); ++it)
		drawableArray.push_back(*it);
	drawableList.clear();
	return drawableArray;

}
std::vector<TexturedDrawable> GeometryWrapper::getDrawables()
{

	std::map<osg::Texture2D*,int>::iterator iter;	
	std::vector<TexturedDrawable> drawableArray;
	for(iter = TextureList.begin();iter != TextureList.end();++iter)
	{
		TexturedDrawable tdrawable = NodeList.find(iter->second)->second; 
		drawableArray.push_back(tdrawable);
	}

	return drawableArray;

}
void GeometryWrapper::loadFromOSGNode(osg::Node* node)
{
	g_mTranslate = osg::Vec3d(0,0,0);
	destroy();
	apply(node);
}

void GeometryWrapper::loadFromOSGNode( osg::Node* node,osg::Vec3d translate )
{
	g_mTranslate = translate;
	destroy();
	apply(node);
}

void GeometryWrapper::save(const char* filename)
{
	//m_node_2_drawable_map.clear();
	osgDB::ofstream ofs(filename,std::ios_base::out |  std::ios::binary);
	//ofs.write((char*)&NumberOfMeshes,sizeof(unsigned int));
	//ofs.write((char*)&NumberOfFaces,sizeof(unsigned int));
	ofs.write((char*)&DiffuseSlot,sizeof(unsigned int));
	ofs.write((char*)&BakeSlot,sizeof(unsigned int));
	unsigned int bufsize;char* buf;

	buf = (char*)TriangleMeshes;
	bufsize = NumberOfMeshes * sizeof(TriangleMesh);
	ofs.write((char*)&bufsize,sizeof(unsigned int));
	ofs.write(buf,bufsize);


	buf = (char*)(&(*GeometryVertices)[0]);
	bufsize = GeometryVertices->size() * sizeof(osg::Vec3);
	ofs.write((char*)&bufsize,sizeof(unsigned int));
	ofs.write(buf,bufsize);


	buf = (char*)(&(*GeometryNormals)[0]);
	bufsize = GeometryNormals->size() * sizeof(osg::Vec3);
	ofs.write((char*)&bufsize,sizeof(unsigned int));
	ofs.write(buf,bufsize);


	if(DiffuseSlot > -1)
	{
		buf = (char*)(&(*DiffuseUVs)[0]);
		bufsize = DiffuseUVs->size() * sizeof(osg::Vec2);
		ofs.write((char*)&bufsize,sizeof(unsigned int));
		ofs.write(buf,bufsize);
	}
	else
	{
		bufsize = 0;
		ofs.write((char*)&bufsize,sizeof(unsigned int));
	}

	if(BakeSlot > -1)
	{
		buf = (char*)(&(*BakeUVs)[0]);
		bufsize = BakeUVs->size() * sizeof(osg::Vec2);
		ofs.write((char*)&bufsize,sizeof(unsigned int));
		ofs.write(buf,bufsize);
	}
	else
	{
		bufsize = 0;
		ofs.write((char*)&bufsize,sizeof(unsigned int));
	}


	unsigned int numofdrawables = TexturedDrawables.size();
	ofs.write((char*)&numofdrawables,sizeof(unsigned int));
	for (int i=0;i<TexturedDrawables.size();i++)
	{

		TexturedDrawable tdrawable = TexturedDrawables[i];
		//static char buf[255];
		unsigned int bufsize;char* buf;
		bufsize = tdrawable.FileName.size();
		ofs.write((char*)&bufsize,sizeof(unsigned int));
		ofs.write(tdrawable.FileName.data(),bufsize);
		bufsize = tdrawable.NodeIndices.size() * sizeof(unsigned int);
		ofs.write((char*)&bufsize,sizeof(unsigned int));
		ofs.write((char*)&tdrawable.NodeIndices[0],bufsize);
		ofs.write((char*)&tdrawable.Width,sizeof(unsigned int));
		ofs.write((char*)&tdrawable.Height,sizeof(unsigned int));
		ofs.write((char*)&tdrawable.Area,sizeof(float));
		//for (int j=0;j<tdrawable.NodeIndices.size();j++)
		//{
		//	m_node_2_drawable_map.insert(std::pair<unsigned int,unsigned int>(tdrawable.NodeIndices[j],i));
		//}
	}
	ofs.flush();
	ofs.close();
}
void GeometryWrapper::open(const char* filename)
{
	destroy();
	m_node_2_drawable_map.clear();
	osgDB::ifstream ifs;
	ifs.open(filename,std::ios_base::in |  std::ios::binary);
	ifs.read((char*)&DiffuseSlot,sizeof(int));
	ifs.read((char*)&BakeSlot,sizeof(int));
	unsigned int bufsize;char* buf;

	ifs.read((char*)&bufsize,sizeof(unsigned int));
	unsigned int sizeofnode = sizeof(TriangleMesh);
	NumberOfMeshes = bufsize / sizeofnode;
	TriangleMeshes = new TriangleMesh[NumberOfMeshes];
	buf = (char*)TriangleMeshes;
	ifs.read(buf,bufsize);
	NumberOfMeshes = bufsize / sizeofnode;
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
		for (int j=0;j<tdrawable.NodeIndices.size();j++)
		{
			m_node_2_drawable_map.insert(std::pair<unsigned int,unsigned int>(tdrawable.NodeIndices[j],TexturedDrawables.size() - 1));
		}

	}
	ifs.close();
}

void GeometryWrapper::destroy()
{

	DiffuseSlot = 0;
	BakeSlot = 1;
	GeometryVertices = NULL;
	GeometryNormals = NULL;
	DiffuseUVs = NULL;
	BakeUVs = NULL;
	//g_mTranslate = osg::Vec3d(0,0,0);
	NodeList.clear();
	TextureList.clear();
	m_img_map.clear();
	TexturedDrawables.clear();
	if(TriangleMeshes)
	{
		delete[] TriangleMeshes;
		TriangleMeshes = NULL;
	}
	m_node_2_drawable_map.clear();
	destroyGeometry();
}

void GeometryWrapper::setBakeSlot( unsigned int slot )
{
	this->BakeSlot = slot;
}

void GeometryWrapper::setDiffusSlot( unsigned int slot )
{
	this->DiffuseSlot = slot;
}
