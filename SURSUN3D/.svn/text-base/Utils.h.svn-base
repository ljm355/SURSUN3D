#pragma once
//#include "common.h"
#include "GeometryWrapper.h"
#include "GrassSolar.h"
#include <d3d9.h>
#include "DXUT.h"
#include <cutil_math.h>
#include "osg\BoundingBox"
#include "osg\Array"
#include "osg\Texture2D"
#include "osg\Geometry"
#include "osg\Matrix"
void printfVec3(osg::Vec3 v);

osg::Vec3 computeOrthogonalVector(const osg::Vec3& direction);

//calculate the minimum bounding volume of the view frustrum for a given viewing direction and a given bounding box
void calLightMatrix(osg::Vec3 lightpos,osg::BoundingSphere& bb,osg::Matrixf& matView,osg::BoundingBox& frustrum);
//box-sphere intersection test
bool frustrumIntersects(osg::BoundingBox frustrum,osg::BoundingSphere bs);
float3 vec3ToFloat3(osg::Vec3 v);

//load a texture map containing floating-point vectors
float4* loadVertexMap(LPDIRECT3DTEXTURE9 pTex);
//resize a texture map
LPDIRECT3DTEXTURE9 resizeTexture( LPDIRECT3DSURFACE9 srcTex,int dest_width,int dest_height);
int monthDay2NumDay(int year,int month,int day);

//represent a date by
void numDay2MonthDay(int year,int num,int& month,int& day);

//convert a solar vector from spherical coordinate system (described by azimuth and altitude angle) to cartesian coordinate system 
osg::Vec3 solarAngle2Vector(float azimuth,float alt);

//convert the spherical-space solar vectors to cartesian-space 3d vectors
std::vector<float3> sunVector2LightDir(std::vector<SunVector> sunvectors);
//calculate aspect (defined by GRASS GIS) from normal vector
float calculateAspect(osg::Vec3 normal);
float calculateAspect( float3 normal );
////calculate slope (defined by GRASS GIS) from normal vector
float calculateSlope(float4* mapvertices,unsigned int width,unsigned int height,unsigned int x,unsigned int y);
float calculateSlope( float3 normal );

//calculate the area of a triangle
float calTriangleArea(osg::Vec3 p0,osg::Vec3 p1,osg::Vec3 p2);