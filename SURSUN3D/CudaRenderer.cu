
#include "CudaTools.h"
#include "CudaRenderer.h"
#include <vector_types.h>
#include <vector_functions.h>
#include <math_functions.h>
#include <cutil_math.h>
//texture<float4, cudaTextureType2D, cudaReadModeElementType> texMapVertices; 
//texture<float4, cudaTextureType2D, cudaReadModeElementType> texMapNormals;  
//texture<float4, cudaTextureType1D, cudaReadModeElementType> texGeometryVertices; 
TriangleMesh* g_pGeometryNodes = NULL;
unsigned int* g_pValidNodes = NULL;
unsigned int* g_pValidNodes_Host = NULL;

bool* g_pValidNodeMasks = NULL;
bool* g_pValidNodeMasks_Host = NULL;
unsigned int m_numofnodes;
unsigned int m_numofvalidnodes;
//cudaArray* g_pGeometryVertices;
float3* g_pGeometryVertices = NULL;
float3* g_pLights = NULL;
float3* g_pLights_host = NULL;
float* g_pViewMatrices = NULL;
bool* g_pNodeCullInfo = NULL;
unsigned int m_numoflights;
Box* g_pFrustrums = NULL;
TriangleMesh* g_pTargetGeometry = NULL;
TriangleMesh g_pTargetGeometry_Host;
bool* g_pExternalValidNodeMasks = NULL;
void setExternalValidNodeMasks(bool*& masks)
{
	g_pExternalValidNodeMasks = masks;
}

void executeKernelFrustrumCullingCPU(int width, int height,float3 light,Box frustrum,float* matView,TriangleMesh* geometryNodes,unsigned int numofnodes,TriangleMesh* targetGeometry)
{
 


	 unsigned int curoffset = 0;
	 unsigned int* pValidNodes = g_pValidNodes_Host;
	 m_numofvalidnodes = 0;
	 bool* pExternalMask = g_pExternalValidNodeMasks;

	 //for(bool* pMask=g_pValidNodeMasks_Host;pMask<g_pValidNodeMasks_Host+m_numofnodes;pMask++)
	 //{

		// if(*pMask == false && (pExternalMask && *pExternalMask != false))
		// {
		//	 m_numofvalidnodes++;
		//	 *pValidNodes = curoffset;pValidNodes++;
		// }	
		// if(pExternalMask)
		//    pExternalMask++;
		// curoffset++;
	 //}



	unsigned int cullcount = 0;
   for(int i=0;i<numofnodes;i++)
   {
	   TriangleMesh node = geometryNodes[i];
	   Sphere sp;
	   sp.Radius = node.BBRadius;
	   sp.Center = vectorMulMat(node.BBCenter,matView);
	    bool isCulled  = 0;
	      float angle = asinf(light.z);
		//设置最大角度避免阴影无线长,70.0=1.22173,80=1.4835298;89=1.553341722222222
	   if(angle > 1.4835298)
		   angle = 1.4835298;

	   float shadowD = targetGeometry->ZMax / tan(angle); 
		//阴影投射距离小于两对象之间的距离加上包围盒半径，不可能是阴影投射对象
	   float dist = length(targetGeometry->BBCenter - node.BBCenter);
/*	   if( dist - targetGeometry->BBRadius - node.BBRadius > shadowD)
	   {
		      isCulled = 1;
			  cullcount++;
	   }
	   else*/ 
	   if(!intersects(frustrum,sp))
	   {
			   isCulled = 1;
			   cullcount++;
	   }
	   else
	   {
			 m_numofvalidnodes++;
			 *pValidNodes = curoffset;pValidNodes++;
	   }
	   if(pExternalMask)
		   pExternalMask++;
	   curoffset++;
	   geometryNodes[i] = node;

   }
   cudaMemcpy( g_pValidNodes,g_pValidNodes_Host,sizeof(unsigned int) * m_numofvalidnodes ,cudaMemcpyHostToDevice);
   printf("%d/%d\n",cullcount,numofnodes);

} 



__global__ void kernelFrustrumCulling(int width, int height,float3 light,unsigned int lightindex,Box* frustrum,float* matView,TriangleMesh* geometryNodes,bool* masks,unsigned int numofnodes,TriangleMesh* targetGeometry)
{
    unsigned int i = blockIdx.y * blockDim.y + threadIdx.y; // row number 
    unsigned int j = blockIdx.x * blockDim.x + threadIdx.x; // col number

   unsigned int index = i*width+j;
   if(index >= numofnodes)
	   return;	
   TriangleMesh node = geometryNodes[index];
   Sphere sp;
   sp.Radius = node.BBRadius;
   bool isCulled = 0;
   //float3 light = lights[ilight];
   //求阴影投射长度
   float angle = asinf(light.z);
	//设置最大角度避免阴影无线长,70.0=1.22173,80=1.4835298;89=1.553341722222222
   if(angle > 1.4835298)
	   angle = 1.4835298;

   float shadowD = targetGeometry->ZMax / tan(angle); 
	//阴影投射距离小于两对象之间的距离加上包围盒半径，不可能是阴影投射对象
   float dist = length(targetGeometry->BBCenter - node.BBCenter);
   //if( dist - targetGeometry->BBRadius - node.BBRadius > shadowD)
   //{
   //     isCulled = 1;
   //}
   //else
   //{
	   sp.Center = vectorMulMat(node.BBCenter,matView + lightindex*16);
	   if(!intersects(frustrum[lightindex],sp))
		   isCulled = 1;
   //}

  //if( node.ID == targetGeometry->ID)
	 // isCulled = 0;
   
   masks[index]  = isCulled;
} 

bool* getNodeMasks()
{
   return g_pValidNodeMasks_Host;
}

void executeKernelFrustrumCulling(unsigned int lightnum)
{
	 cudaError_t error;
	 dim3 gridDim3,blockDim3;
	 unsigned int height,width;
	 height = 16;
	 width  = m_numofnodes / height + 1;
     computeKernelDims(height,width,gridDim3,blockDim3);
 	 kernelFrustrumCulling<<< gridDim3, blockDim3 >>>(   
width, 
height,
g_pLights_host[lightnum],
lightnum,
g_pFrustrums, 
g_pViewMatrices,
g_pGeometryNodes,
g_pValidNodeMasks,
m_numofnodes,
g_pTargetGeometry);
 	 cudaThreadSynchronize();

	 m_numofvalidnodes = 0;
     cudaMemcpy(g_pValidNodeMasks_Host,g_pValidNodeMasks,sizeof(bool) * m_numofnodes,cudaMemcpyDeviceToHost);
	 unsigned int curoffset = 0;
	 unsigned int* pValidNodes = g_pValidNodes_Host;
	 m_numofvalidnodes = 0;
	 bool* pExternalMask = g_pExternalValidNodeMasks;

	 for(bool* pMask=g_pValidNodeMasks_Host;pMask<g_pValidNodeMasks_Host+m_numofnodes;pMask++)
	 {

		 if(*pMask == false)
		 {
			 if(!pExternalMask || *pExternalMask != false)
			 {
			     m_numofvalidnodes++;
			    *pValidNodes = curoffset;pValidNodes++;
			 }
		 }	
		 if(pExternalMask)
		    pExternalMask++;
		 curoffset++;
	 }
    cudaMemcpy( g_pValidNodes,g_pValidNodes_Host,sizeof(unsigned int) * m_numofvalidnodes ,cudaMemcpyHostToDevice);

	//printf("remaining: %d/%d\n",m_numofvalidnodes,m_numofnodes);

}
void setTargetGeometry(TriangleMesh* targetGeometry)
{
	 g_pTargetGeometry_Host = *targetGeometry;
	 size_t sizeInBytes = sizeof(TriangleMesh);
     cudaMemcpy(g_pTargetGeometry,targetGeometry,sizeInBytes,cudaMemcpyHostToDevice);
}
void setGeometry(float3* vertices,unsigned int numofvertices,TriangleMesh* geometryNodes,unsigned int numofnodes)
{
	cudaError error;
	size_t sizeInBytes ;
    sizeInBytes = numofvertices * sizeof(float3);
	//cudaChannelFormatDesc channelDesc = cudaCreateChannelDesc(32, 32, 32, 32,cudaChannelFormatKindFloat);
 //   cudaMallocArray( &g_pGeometryVertices, &channelDesc, numofvertices, 1 ); 
 //   cudaMemcpyToArray( g_pGeometryVertices, 0, 0, vertices, numofvertices, cudaMemcpyHostToDevice);


    error  = cudaMalloc((void**)&g_pGeometryVertices, sizeInBytes);
	cudaMemcpy(g_pGeometryVertices,vertices,sizeInBytes,cudaMemcpyHostToDevice);
    //// set texture parameters
    //texGeometryVertices.addressMode[0] = cudaAddressModeWrap;
    //texGeometryVertices.addressMode[1] = cudaAddressModeWrap;
    //texGeometryVertices.filterMode = cudaFilterModePoint;
    //texGeometryVertices.normalized = false;    
    //cutilSafeCall(cudaBindTextureToArray(texGeometryVertices, g_pGeometryVertices, channelDesc));

	 m_numofnodes = numofnodes;
     sizeInBytes = m_numofnodes * sizeof(TriangleMesh);
     error  = cudaMalloc((void**)&g_pGeometryNodes, sizeInBytes);
     cudaMemcpy(g_pGeometryNodes,geometryNodes,sizeInBytes,cudaMemcpyHostToDevice);

     error  = cudaMalloc((void**)&g_pValidNodes, sizeof(unsigned int) * m_numofnodes);
     g_pValidNodes_Host  = (unsigned int*)malloc(sizeof(unsigned int) * m_numofnodes);
	 
     error  = cudaMalloc((void**)&g_pValidNodeMasks, sizeof(bool) * m_numofnodes);
     g_pValidNodeMasks_Host  = (bool*)malloc(sizeof(bool) * m_numofnodes);



     sizeInBytes = sizeof(TriangleMesh);
     error  = cudaMalloc((void**)&g_pTargetGeometry, sizeInBytes);

}

void updateGeometry(float3* vertices,unsigned int numofvertices,TriangleMesh* geometryNodes,unsigned int numofnodes)
{
	cudaError error;
	size_t sizeInBytes;
    sizeInBytes = numofvertices * sizeof(float3);
	cudaMemcpy(g_pGeometryVertices,vertices,sizeInBytes,cudaMemcpyHostToDevice);

	 m_numofnodes = numofnodes;
     sizeInBytes = m_numofnodes * sizeof(TriangleMesh);
     cudaMemcpy(g_pGeometryNodes,geometryNodes,sizeInBytes,cudaMemcpyHostToDevice);
}
//void setGeometryNode(TriangleMesh* node,unsigned int index)
//{
//     cudaMemcpy(g_pGeometryNodes+index,node,sizeof(TriangleMesh),cudaMemcpyHostToDevice);
//}





////float3* g_pLights;
////float* g_pViewMatrices;
////int* g_pNodeCullInfo;

void setLights(float3* lights,unsigned int numoflights)
{
     
	if(g_pViewMatrices)
       cudaFree(g_pViewMatrices);
	g_pViewMatrices = NULL;

	if(g_pLights)
       cudaFree(g_pLights);
	g_pLights = NULL;

	if(g_pFrustrums)
       cudaFree(g_pFrustrums);
	g_pFrustrums = NULL;


	 size_t sizeInBytes;
	 cudaError_t error;
	 m_numoflights = numoflights;
     sizeInBytes = 16 * sizeof(float) * numoflights;
     error  = cudaMalloc((void**)&g_pViewMatrices, sizeInBytes);
     //cudaMemcpy(g_pViewMatrices,viewMatrices,sizeInBytes,cudaMemcpyHostToDevice);

     sizeInBytes = numoflights * sizeof(float3);
     error  = cudaMalloc((void**)&g_pLights, sizeInBytes);
     cudaMemcpy(g_pLights,lights,sizeInBytes,cudaMemcpyHostToDevice);
	 //g_pLights = lights;//(float3*)malloc(numoflights * sizeof(float3));
	 //memcpy(g_pLights,lights,numoflights * sizeof(float3));
     g_pLights_host = lights;


	 sizeInBytes = sizeof(Box) * m_numoflights;
     error  = cudaMalloc((void**)&g_pFrustrums, sizeInBytes);

}


__device__ unsigned int cast_Ray(unsigned int index,float3 lightDir,
									 float4 pos4,float4 normal4,
									 TriangleMesh* geometryNodes,unsigned int numofnodes,
									 unsigned int* validNodes,unsigned int numofvalidnodes,
									 float3* geometryVertices)
{

   float3 pos = make_float3(pos4);
   if(normal4.w < -1)
   {
	   return 0;
   }

   float3 normal = make_float3(normal4);
   //unsigned int faceindex = normal4.w;
   //float3 f0 = geometryVertices[faceindex*3];
   //float3 f1 = geometryVertices[faceindex*3 + 1];
   //float3 f2 = geometryVertices[faceindex*3 + 2];

   if(isParallel(lightDir,normal)) 
   {  
	   return 1;
   }

   float b = dot(normal, lightDir);  
   if (b <= 0)
   {
        return 1;
   }

   int isShadowed = 0;    
   for(int k=0;k<numofvalidnodes;k++)
   {	
	   TriangleMesh node = geometryNodes[validNodes[k]]; 
	   for(unsigned int l=0;l<node.FaceCount;l++)
	   {

	     //  if((unsigned int)normal4.w ==  node.FaceStart + l)
			   //continue;
           float3 v0,v1,v2;

		   v0 = geometryVertices[(node.FaceStart + l) * 3 + 0];
		   v1 = geometryVertices[(node.FaceStart + l) * 3 + 1];
		   v2 = geometryVertices[(node.FaceStart + l) * 3 + 2];
		   float3 n = cross(v1 - v0, v2 - v0);
		   b = dot(n, lightDir);  

		   if (b <= 0)
		   {
				continue;
		   }
	
		   float dist = intersectTriangle2(pos, lightDir, v0,v1,v2);
		   if(dist > 0.01 && dist < 1000000)
		   {
               isShadowed = 1;//node.ID + 2;
			   break;
		   }


	   }
	   if(isShadowed > 0)
	   {
		   break;
	   }	 		
   }

   return isShadowed;
    
} 


__global__ void kernelShadowCasting(bool* output, int width, int height, unsigned int startIndex,unsigned int offset,float3* lightDir,unsigned int lightindex,
									 float4* mapVertices,float4* mapNormals,
									 TriangleMesh* geometryNodes,unsigned int numofnodes,
									 unsigned int* validNodes,unsigned int numofvalidnodes,
									 float3* geometryVertices)
{
    int i = blockIdx.y * blockDim.y + threadIdx.y; // row number 
    int j = blockIdx.x * blockDim.x + threadIdx.x; // col number

    if(i >= height || j >= width)
		return;

   unsigned int index = i*width+j + startIndex;
   float4 pos4 = mapVertices[index];
   float4 normal4 = mapNormals[index];
   int shadow = cast_Ray(index,lightDir[lightindex],pos4,normal4,geometryNodes,numofnodes,validNodes,numofvalidnodes,geometryVertices);
   if(shadow > 0)
   {
       output[index + offset] = true;
   }
   else
   {
	   output[index + offset] = false;
   }

} 

void printfFloat3(float3 v)
{
	printf("(%f,%f,%f)\n",v.x,v.y,v.z);
}

void executeKernelShadowCasting(bool* output,float4* map_vertices,float4* map_normals,
				   int width, int height,float* matViews,Box* frustrums)
{   
	 size_t sizeInBytes;
     size_t grid_pitch;
	 size_t count = width * height;
     cudaError_t error;
     //cudaChannelFormatDesc channeldesc = cudaCreateChannelDesc<float4>();
     float4* mapVertices_device;
     float4* mapNormals_device;
     sizeInBytes = sizeof(float4)*width*height;
     error = cudaMalloc((void **)&mapVertices_device, sizeInBytes);
     cudaMemcpy(mapVertices_device,map_vertices,sizeInBytes,cudaMemcpyHostToDevice);

     error = cudaMalloc((void **)&mapNormals_device, sizeInBytes);
     cudaMemcpy(mapNormals_device,map_normals,sizeInBytes,cudaMemcpyHostToDevice);


     sizeInBytes = 16 * sizeof(float) * m_numoflights;
     cudaMemcpy(g_pViewMatrices,matViews,sizeInBytes,cudaMemcpyHostToDevice);


     sizeInBytes = sizeof(Box) * m_numoflights;
     cudaMemcpy(g_pFrustrums,frustrums,sizeInBytes,cudaMemcpyHostToDevice);

	 //executeKernelFrustrumCulling();
	 bool* output_device;
     sizeInBytes = sizeof(bool)*width*height*m_numoflights;
     error = cudaMalloc(&output_device, sizeInBytes);
	 dim3 gridDim3,blockDim3;



	 for(int i=0;i<m_numoflights;i++)
	 {
		 unsigned int startIndex = 0;
		 int rowstep = 100;
		 int currows = 0;
		 int rows;
	     unsigned int offset = i * height * width;
		 executeKernelFrustrumCulling(i);
		 while(startIndex < height * width)
		 {
			 rows = rowstep;
			 rows = min(rows,height-currows);

			 computeKernelDims(rows,width,gridDim3,blockDim3);
			 kernelShadowCasting<<< gridDim3, blockDim3 >>>
				(   
		output_device,  
		width, 
		rows,
		startIndex,
		offset,
		g_pLights,
		i,
		mapVertices_device,
		mapNormals_device,
		g_pGeometryNodes,
		m_numofnodes,
		g_pValidNodes,
		m_numofvalidnodes,
		g_pGeometryVertices
				);
			 startIndex+=rows*width;
			 currows+=rows;
			 error = cudaThreadSynchronize();	
			 //printf ("Cuda error -- %s\n", cudaGetErrorString(cudaGetLastError()));
	 }
	 }
 	


     cudaMemcpy(output,output_device,sizeInBytes,cudaMemcpyDeviceToHost);
	 cudaFree(mapVertices_device);
	 cudaFree(mapNormals_device);
	 cudaFree(output_device);

}


void destroyGeometry()
{
    //cudaUnbindTexture(texGeometryVertices);
	if(g_pGeometryVertices)
       cudaFree(g_pGeometryVertices);
	g_pGeometryVertices = NULL;
	if(g_pViewMatrices)
       cudaFree(g_pViewMatrices);
	g_pViewMatrices = NULL;
	if(g_pFrustrums)
       cudaFree(g_pFrustrums);
	g_pFrustrums = NULL;
	if(g_pTargetGeometry)
       cudaFree(g_pTargetGeometry);
	g_pTargetGeometry = NULL;
	if(g_pGeometryNodes)
       cudaFree(g_pGeometryNodes);
	g_pGeometryNodes = NULL;
	if(g_pValidNodes)
        cudaFree(g_pValidNodes);
	g_pValidNodes = NULL;
	if(g_pValidNodeMasks)
       cudaFree(g_pValidNodeMasks);
	g_pValidNodeMasks = NULL;
	if(g_pValidNodes_Host)
       free(g_pValidNodes_Host);
	g_pValidNodes_Host = NULL;
	if(g_pValidNodeMasks_Host)
       free(g_pValidNodeMasks_Host); 
	g_pValidNodeMasks_Host = NULL;

	if(g_pLights)
       cudaFree(g_pLights);
	g_pLights = NULL;

}
