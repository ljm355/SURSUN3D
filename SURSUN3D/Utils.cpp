#include "Utils.h"

int m1[12]={31,28,31,30,31,30,31,31,30,31,30,31};
int m2[12]={31,29,31,30,31,30,31,31,30,31,30,31};
int monthDay2NumDay(int year,int month,int day)
{

	int numday = 0;
	int* months = m1;
	if(year%400==0 || (year%4==0 && year%100!=0)){
		months = m2;}
	for (int i=0;i<month-1;i++)
	{
		numday+=months[i];
	}
	numday += day;
	return numday;
}
void numDay2MonthDay(int year,int num,int& month,int& day)
{


	int curDay = 0;
	int* months = m1;
	if(year%400==0 || (year%4==0 && year%100!=0)){
		months = m2;}
	month = 0;
	day = 0;
	for (int i=0;i<12;i++)
	{
		if(curDay + months[i] >= num)
		{
			month = i+1;
			day = num - curDay;		
			break;
		}
		curDay += months[i];
	}


}
void printfVec3(osg::Vec3 v)
{
	printf("(%f,%f,%f)\n",v.x(),v.y(),v.z());
}

osg::Vec3 computeOrthogonalVector(const osg::Vec3& direction)
{
	float length = direction.length();
	osg::Vec3 orthogonalVector = direction ^ osg::Vec3(0.0f, 1.0f, 0.0f);
	if (orthogonalVector.normalize()<length*0.5f)
	{
		orthogonalVector = direction ^ osg::Vec3(0.0f, 0.0f, 1.0f);
		orthogonalVector.normalize();
	}
	return orthogonalVector;
}


void calLightMatrix(osg::Vec3 lightpos,osg::BoundingSphere& bb,osg::Matrixf& matView,osg::BoundingBox& frustrum)
{

	// make an orthographic projection
	osg::Vec3 lightDir(lightpos.x(), lightpos.y(), lightpos.z());
	lightDir.normalize();

	// set the position far away along the light direction
	osg::Vec3 position = bb.center() + lightDir * bb.radius() * 2.0;

	float centerDistance = (position-bb.center()).length();

	float znear = centerDistance-bb.radius();
	float zfar  = centerDistance+bb.radius();
	float zNearRatio = 0.001f;
	if (znear<zfar*zNearRatio) znear = zfar*zNearRatio;

	float top   = bb.radius();
	float right = top;

	frustrum = osg::BoundingBox(-right,-top,-1000000,right,top,0);
	matView = osg::Matrixd::lookAt(position,bb.center(),computeOrthogonalVector(lightDir));
}

bool frustrumIntersects(osg::BoundingBox frustrum,osg::BoundingSphere bs)
{
	if(bs.center().x() + bs.radius() < frustrum.xMin() || 
		bs.center().x() - bs.radius() > frustrum.xMax() || 
		bs.center().y() + bs.radius() < frustrum.yMin() || 
		bs.center().y() - bs.radius() > frustrum.yMax() || 
		bs.center().z() + bs.radius() < frustrum.zMin() || 
		bs.center().z() - bs.radius() > frustrum.zMax())
	{
		return false;
	}
	return true;
}


float3 vec3ToFloat3(osg::Vec3 v)
{
	return make_float3(v.x(),v.y(),v.z());
}

float4* loadVertexMap(LPDIRECT3DTEXTURE9 pTex)
{
	D3DLOCKED_RECT lockedRect;
	HRESULT result = pTex->LockRect(0,&lockedRect, NULL, D3DLOCK_READONLY);
	D3DSURFACE_DESC desc;
	pTex->GetLevelDesc(0,&desc);
	float4* vertexMap = new float4[desc.Width*desc.Height];
	int index = 0;
	for (int i=0;i<desc.Height;i++)
	{
		for (int j=0;j<desc.Width;j++)
		{

			unsigned char* pDataChar = (unsigned char*)(lockedRect.pBits) + i*lockedRect.Pitch+ j * 16;
			float4 vertex = make_float4(0,0,0,-9999);

			float* pData = (float*)pDataChar;
			if(pData[0] > 0)
			{
				vertex.x = pData[3]; // Blue
				vertex.y = pData[2]; // Green
				vertex.z = pData[1]; // Red
				//vertex.w = ceil(pData[0]-1);
		    	vertex.w = pData[0]-1;

			}
			vertexMap[index]=vertex;
			index++;
		}
	}
	pTex->UnlockRect(0);
	return vertexMap;
}



LPDIRECT3DTEXTURE9 resizeTexture( LPDIRECT3DSURFACE9 srcTex,int dest_width,int dest_height)
{
	LPDIRECT3DDEVICE9 pDevice = DXUTGetD3D9Device();
	D3DFORMAT fmt = D3DFMT_A32B32G32R32F;
	const char* tmp_filename = "srcTexLiangjm.dds";
	D3DXSaveSurfaceToFileA(tmp_filename,D3DXIMAGE_FILEFORMAT::D3DXIFF_DDS,srcTex,NULL,NULL);
	LPDIRECT3DTEXTURE9 pTexture;
	D3DXCreateTextureFromFileExA(pDevice, tmp_filename, dest_width, dest_height, 0, 0, 
		fmt, D3DPOOL_MANAGED, D3DX_DEFAULT, 
		D3DX_DEFAULT, 0, NULL, NULL, &pTexture);
	return pTexture;

}
//场景坐标系Y为正北,-X为正东
osg::Vec3 solarAngle2Vector(float azimuth,float alt)
{
	osg::Vec3 vsun(0,1,0);
	osg::Vec3 horizontal(1,0,0);
	osg::Matrix matSun = osg::Matrix::rotate(azimuth, osg::Vec3(0,0,-1));
	vsun = vsun * matSun;
    horizontal = horizontal * matSun;
	matSun = osg::Matrix::rotate(alt, horizontal);
	vsun = vsun * matSun;
	vsun.normalize();
	//vsun.y() *= -1;
	return vsun;

}

std::vector<float3> sunVector2LightDir( std::vector<SunVector> sunvectors )
{
	std::vector<float3> vsuns;
   for (int i=0;i<sunvectors.size();i++)
   {
	   osg::Vec3 vsun = solarAngle2Vector(osg::DegreesToRadians(sunvectors[i].azimuth),osg::DegreesToRadians(sunvectors[i].alt));
	   vsuns.push_back(make_float3(vsun.x(),vsun.y(),vsun.z()));
   }
   return vsuns;
} 

//The aspect categories represent the number degrees of east and they increase counterclockwise: 90deg is North, 180 is West, 270 is South 360 is East.
//The aspect value 0 is used to indicate undefined aspect in flat areas with slope=0. 
//
//The slope output raster map contains slope values, 
//stated in degrees of inclination from the horizontal if format=degrees option (the default) is chosen, 
//and in percent rise if format=percent option is chosen. Category and color table files are generated. 
//***********N90
         
//*******W180    E360 

//***********S270

//***********N0
         
//*******W270    E90 

//***********S180
//tmpval.elev = 0;
//if (tmpval.aspect != 0.) {
//	if (tmpval.aspect < 90.)
//		tmpval.aspect = 90. - tmpval.aspect;
//	else
//		tmpval.aspect = 450. - tmpval.aspect;
//}

//float calculateAspect( osg::Vec3 normal )
//{
//		float aspect = 90;
//		normal.normalize();
//		osg::Vec3 normalxy = normal;
//		normalxy.z() = 0;
//		normalxy.normalize();
//		float cosa = acos(normalxy * osg::Vec3(1,0,0));
//		aspect = osg::RadiansToDegrees(cosa);
//		if(normal.y() < 0)
//			aspect = 360 - aspect;
//		//if(aspect == 0)
//		//	aspect = 360;
//		return aspect;
//}

float calculateAspect( float3 normal )
{
	float aspect = 0;
	if(normal.x ==0 && normal.y	== 0)
		return aspect;
	float3 normalxy = normalize(normal);
	normalxy.z = 0;
    normalxy=normalize(normalxy);
	float cosa = acos(dot(normalxy,make_float3(1,0,0)));
	aspect = osg::RadiansToDegrees(cosa);
	if(normal.y < 0)
		aspect = 360 - aspect;
	return aspect;
}

float calculateSlope( float3 normal )
{
	float cosa = acos(dot(normal,make_float3(0,0,1)));
	float slope = osg::RadiansToDegrees(cosa);
	return slope;
}
//slope_radians = ATAN ( √ ( [dz/dx]2 + [dz/dy]2 ) ) 
float calculateSlope(float4* mapvertices,unsigned int width,unsigned int height,unsigned int x,unsigned int y )
{
	unsigned int x2 = x + 1;
	if(x2 > width - 1)
		x2 = x - 1;
	unsigned int y2 = y + 1;
	if(y2 > height - 1)
		y2 = y - 1;
	float4 px = mapvertices[y*width+x2];
	float4 py = mapvertices[y2*width+x];
	float4 p =  mapvertices[y*width+x];
	float xlen = length(px-p);
	float dzx = 0;
    float dzy = 0;
	if(xlen > 0)
	{
	   dzx = (px.z-p.z)/xlen;
	   dzx = dzx*dzx;
	}

	float ylen = length(px-p);
	if(ylen > 0)
	{
		dzy = (py.z-p.z)/ylen;
		dzy = dzy*dzy;
	}
	
	float slope = atan(sqrt(dzx+dzy));
	slope = osg::DegreesToRadians(slope);
	return slope;
}

float calTriangleArea(osg::Vec3 p0,osg::Vec3 p1,osg::Vec3 p2)
{

	float first,second,third;
	first = (p0-p1).length();
	second = (p0-p2).length();
	third = (p1-p2).length();
	float s,area;
	s = (first+second+third)/2;

	area = sqrt(s*(s-first)*(s-second)*(s-third));
	if(area != area)
	{
		area = 0;
	}
	return area;
}
