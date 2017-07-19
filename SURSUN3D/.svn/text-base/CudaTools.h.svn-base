#include "CudaRenderer.h"
#include <cutil_math.h>



#define SIM_COLS_PER_BLOCK 16
#define SIM_ROWS_PER_BLOCK 16

bool isNan(float a)
{
	return isnan(a);
}
//compute the grid and block layout for CUDA kernel execution
//this is not yet optimized for CUDA parallelization
bool computeKernelDims(unsigned int rows,unsigned int cols,dim3& gridDim,dim3& blockDim)
{

	unsigned count = 0;
	unsigned int gridDimX = 0;
	while(count < cols)
	{
		count += SIM_COLS_PER_BLOCK;
		gridDimX++;
	}

    count = 0;
	unsigned int gridDimY = 0;
	while(count < rows)
	{
		count += SIM_ROWS_PER_BLOCK;
		gridDimY++;
	}
	blockDim = dim3(SIM_COLS_PER_BLOCK,SIM_ROWS_PER_BLOCK,1);
    gridDim =  dim3(gridDimX,gridDimY,1);
	 //printf("%d,%d\n",SIM_COLS_PER_BLOCK,SIM_ROWS_PER_BLOCK);
	return true;
}

//CUDA ray-triangle intersection
__host__ __device__
bool intersectTriangle(float3 orig, float3 dir, float3* vertices)
{
    float3 u, v, n; // triangle vectors
    float3 w0, w;  // ray vectors
    float r, a, b; // params to calc ray-plane intersect

    // get triangle edge vectors and plane normal
    u = vertices[1] - vertices[0];
    v = vertices[2] - vertices[0];
    n = cross(u, v);

    w0 = orig - vertices[0];
    a = -dot(n, w0);
    b = dot(n, dir);
    if (abs(b) < EPSILON)
    {
        // ray is parallel to triangle plane, and thus can never intersect.
        return false;
    }

    // get intersect point of ray with triangle plane
    r = a / b;
    if (r < 0.0)
        return false;

    float3 I = orig + r * dir;
    float uu, uv, vv, wu, wv, D;
    uu = dot(u, u);
    uv = dot(u, v);
    vv = dot(v, v);
    w = I - vertices[0];
    wu = dot(w, u);
    wv = dot(w, v);
    D = uv * uv - uu * vv;

    // get and test parametric coords
    float s, t;
    s = (uv * wv - vv * wu) / D;
    if (s < 0.0 || s > 1.0)
        return false;
    t = (uv * wu - uu * wv) / D;
    if (t < 0.0 || (s + t) > 1.0)
        return false;

    return (r > EPSILON) ? true : false;
}
//CPU code for ray-triangle intersection,for test only
inline float intersectTriangle2_CPU(float3 org,float3 dir,
      float3 v0,float3 v1,float3 v2)
{



		//triangles.push_back(make_float4(v0.x,v0.y,v0.z,0));
		//triangles.push_back(make_float4(v1.x-v0.x, v1.y-v0.y, v1.z-v0.z,0)); // notice we store the edges instead of vertex points, to save some calculations in the 
		//triangles.push_back(make_float4(v2.x-v0.x, v2.y-v0.y, v2.z-v0.z,0)); // ray triangle intersection test.

	float3 edge1 = v1 - v0;
	float3 edge2 = v2 - v0;
	float3 tvec = org - v0;  
	float3 pvec = cross(dir, edge2);  
	float  det  = dot(edge1, pvec);  

	det = 1.0f / det;//__fdividef(1.0f, det);  
	float u = dot(tvec, pvec) * det;  

	if (u < 0.0f || u > 1.0f)  
		return -1.0f;  

	float3 qvec = cross(tvec, edge1);  

	float v = dot(dir, qvec) * det;  

	if (v < 0.0f || (u + v) > 1.0f)  
		return -1.0f;  

    float dist = dot(edge2, qvec) * det;
	if(isnan(dist))
	{
        dist = -1;
	}
	return dist;
}

//CUDA ray-triangle intersection
inline __device__
float intersectTriangle2(float3 org,float3 dir,
      float3 v0,float3 v1,float3 v2)
{

	float3 edge1 = v1 - v0;
	float3 edge2 = v2 - v0;
	float3 tvec = org - v0;  
	float3 pvec = cross(dir, edge2);  
	float  det  = dot(edge1, pvec);  

	det = __fdividef(1.0f, det);  

	//det = 1.0f / det;//__fdividef(1.0f, det);  
	float u = dot(tvec, pvec) * det;  

	if (u < 0.0f || u > 1.0f)  
		return -1.0f;  

	float3 qvec = cross(tvec, edge1);  

	float v = dot(dir, qvec) * det;  

	if (v < 0.0f || (u + v) > 1.0f)  
		return -1.0f; 

	return dot(edge2, qvec) * det;
}
 
inline __host__ __device__
float3 getFloat3(float4 val)
{
   return make_float3(val.x,val.y,val.z);
}

 __host__ __device__ 
float getMatrixElement(int row,int col,float* mat)
{
     return mat[col + row*4];
}

//multiply a 3d vector by a matrix
__host__ __device__
float3 vectorMulMat(float3 v,float _mat[4][4])
{

	float d = 1.0f/(_mat[0][3]*v.x+_mat[1][3]*v.y+_mat[2][3]*v.z+_mat[3][3]);
	return make_float3( (_mat[0][0]*v.x + _mat[1][0]*v.y + _mat[2][0]*v.z + _mat[3][0])*d,
		(_mat[0][1]*v.x + _mat[1][1]*v.y + _mat[2][1]*v.z + _mat[3][1])*d,
		(_mat[0][2]*v.x + _mat[1][2]*v.y + _mat[2][2]*v.z + _mat[3][2])*d);
};

//multiply a 3d vector by a matrix
__host__ __device__
float3 vectorMulMat(float3 v,float* mat)
{

	float d = 1.0f/( getMatrixElement(0,3,mat)*v.x+getMatrixElement(1,3,mat)*v.y+getMatrixElement(2,3,mat)*v.z+getMatrixElement(3,3,mat));
	return make_float3(
		(getMatrixElement(0,0,mat)*v.x + getMatrixElement(1,0,mat)*v.y + getMatrixElement(2,0,mat)*v.z + getMatrixElement(3,0,mat))*d,
		(getMatrixElement(0,1,mat)*v.x + getMatrixElement(1,1,mat)*v.y + getMatrixElement(2,1,mat)*v.z + getMatrixElement(3,1,mat))*d,
		(getMatrixElement(0,2,mat)*v.x + getMatrixElement(1,2,mat)*v.y + getMatrixElement(2,2,mat)*v.z + getMatrixElement(3,2,mat))*d);
};

/** Return true if this bounding box intersects the specified bounding box. */
inline __host__ __device__
bool intersects(Box bb1,Box bb2)
{    return MAX(bb1.MinMax[0].x,bb2.MinMax[0].x) <= MIN(bb1.MinMax[1].x,bb2.MinMax[1].x) &&
MAX(bb1.MinMax[0].y,bb2.MinMax[0].y) <= MIN(bb1.MinMax[1].y,bb2.MinMax[1].y) &&
MAX(bb1.MinMax[0].z,bb2.MinMax[0].z) <= MIN(bb1.MinMax[1].z,bb2.MinMax[1].z);

};

//determines if sp1 intersects sp2
__host__ __device__
bool intersects(Sphere sp1,Sphere sp2)
{
     return length(sp1.Center - sp2.Center) <= (sp1.Radius + sp2.Radius)*(sp1.Radius + sp2.Radius);
}

//determines if a box intersects a sphere
__host__ __device__
bool intersects(Box bb,Sphere bs)
{
	if(bs.Center.x + bs.Radius < bb.MinMax[0].x || 
bs.Center.x - bs.Radius > bb.MinMax[1].x || 
bs.Center.y + bs.Radius < bb.MinMax[0].y || 
bs.Center.y - bs.Radius > bb.MinMax[1].y || 
bs.Center.z + bs.Radius < bb.MinMax[0].z || 
bs.Center.z - bs.Radius > bb.MinMax[1].z)
	{
        return false;
	}
return true;
}
//determine if a ray (with origin 'orig' and direction 'dir' ) is parallel to a face (with a vertex array 'vertices')
__host__ __device__
bool isParallel(float3 orig, float3 dir, float4* vertices)
{
	float3 u, v, n; // triangle vectors
	float3 w0, w;  // ray vectors
	float r, a, b; // params to calc ray-plane intersect
	// get triangle edge vectors and plane normal
	u = make_float3(vertices[1]) - make_float3(vertices[0]);
	v = make_float3(vertices[2]) - make_float3(vertices[0]);
	n = cross(u, v);

	w0 = orig - make_float3(vertices[0]);
	a = -dot(n, w0);
	b = dot(n, dir);
	return (abs(b) < EPSILON) ? true:false;
}

//determine if a ray (with origin 'orig' and direction 'dir' ) is parallel to a face (with three vertices 'f0', 'f1' and 'f2')
__host__ __device__
bool isParallel(float3 orig, float3 dir, float3 f0,float3 f1,float3 f2)
{
	float3 u, v, n; // triangle vectors
	float3 w0, w;  // ray vectors
	float r, a, b; // params to calc ray-plane intersect
	// get triangle edge vectors and plane normal
	u = f1 - f0;
	v = f2 - f0;
	n = cross(u, v);

	w0 = orig - f0;
	a = -dot(n, w0);
	b = dot(n, dir);
	float c = dot(-n, dir);
	//return (abs(b) < EPSILON) ? true:false;

	float minf = abs(c);
	if(abs(b) < abs(c))
		minf = abs(b);
	return (minf < 0.001) ? true:false;
}
//determine if vector 'dir' is parallel to a face (with a normal vector 'n')
__host__ __device__
bool isParallel(float3 dir, float3 n)
{

	float a = acos(dot(n, dir)) - 0.5 * 3.1415926;
	float b = acos(dot(-n, dir)) - 0.5 * 3.1415926;

	float minf = abs(a);
	if(abs(b) < abs(a))
		minf = abs(b);
	return (minf < 0.001) ? true:false;
}

