#pragma once

#ifndef  CudaRenderer_h
#define  CudaRenderer_h

#include "cutil_inline.h"

#define INFINITY FLT_MAX
#define EPSILON  0.001
#define FlipImage 0
//#define MAX(a, b) (((a) > (b)) ? (a) : (b))
//#define MIN(a, b) (((a) < (b)) ? (a) : (b))

//structure of a ray
typedef struct Ray
{
public:
	float3 Origin;
	float3 Dir;
	float3 DirInverse;
	int3 Sign;
	__host__ __device__ 
		Ray(float3 o, float3 d) {
			Origin = o;
			Dir = d;
			DirInverse = make_float3(1/d.x, 1/d.y, 1/d.z);
			Sign.x = (DirInverse.x < 0);
			Sign.y = (DirInverse.y < 0);
			Sign.z = (DirInverse.z < 0);
	}
};

//information of a triangle mesh represented by a 'TriangleMesh'
typedef struct __align__(16)
{
public:
	unsigned int ID;
	unsigned int FaceCount;//number of triangle faces
	unsigned int FaceStart;//index of the first vertex
	float Width2Height;//ratio of the width to the height of the original texture map
	float XMin;
	float YMin;
	float ZMin;
	float XMax;
	float YMax;
	float ZMax;
    float BBRadius;//bounding sphere radius
	float3 BBCenter;//bounding sphere center
	//__host__ __device__ float3 GetCenter()
	//{
 //        return make_float3(BBCenter.x,BBCenter.y,BBCenter.z);
	//}
} TriangleMesh;

typedef struct
{
public:
	float3 MinMax[2];
}Box;

typedef struct
{
public:
	float Radius;
	float3 Center;
} Sphere;


//set the masks for all TriangleMeshes to specify which nodes are to be included for calculation and which are not
void setExternalValidNodeMasks(bool*& masks);

//get the masks for all TriangleMeshes
bool* getNodeMasks();

//specify the TriangleMesh intended for the current calculation
void setTargetGeometry(TriangleMesh* targetGeometry);

//************************************
// Method:    executeKernelShadowCasting//execute the ray-caster to compute shadow masks for a raster map with an array of solar vectors
// FullName:  executeKernelShadowCasting
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: bool * output//computed shadow masks for each solar vector (number of solar vectors )
// Parameter: float4 * map_vertices//raster map containing the position vectors
// Parameter: float4 * map_normals//raster map containing the normal vectors
// Parameter: int width //number of columns for the raster map
// Parameter: int height //number of rows for the raster map
// Parameter: float * matViews //array of view matrices pre-computed for each solar vector
// Parameter: Box * frustrums //array of frustrum bounding volumes pre-computed for each solar vector
//************************************
void executeKernelShadowCasting(bool* output,float4* map_vertices,float4* map_normals,
								  int width, int height,float* matViews,Box* frustrums);


//************************************
// Method:    setGeometry //copy the triangle meshes from CPU to GPU
// FullName:  setGeometry
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: float3 * vertices //face vertices from all the TriangleMeshes grouped together as an array
// Parameter: unsigned int numofvertices
// Parameter: TriangleMesh * geometryNodes //all the TriangleMeshes for potential calculation
// Parameter: unsigned int numofnodes
//************************************
void setGeometry(float3* vertices,unsigned int numofvertices,TriangleMesh* geometryNodes,unsigned int numofnodes);
//update the triangle meshes that have been changed
void updateGeometry(float3* vertices,unsigned int numofvertices,TriangleMesh* geometryNodes,unsigned int numofnodes);
//an array of solar vectors for calculation of daily solar radiation
void setLights(float3* lights,unsigned int numoflights);
//release memory
void destroyGeometry();
//matrix operation
float3 vectorMulMat(float3 v,float* mat);
void executeKernelFrustrumCullingCPU(int width, int height,float3 light,Box frustrum,float* matView,TriangleMesh* geometryNodes,unsigned int numofnodes,TriangleMesh* targetGeometry);
#endif