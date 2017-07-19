#pragma once
//currently only DirectX3D is used for geometry rasterization (render the position and normal vectors from 3D meshes to 2D texture maps)
//the codes might be ported to OpenGL or OpenSceneGraph
#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include "CudaRenderer.h"
#include "TexturedDrawable.h"
//#include "GeometryVisitor.h"
// A structure for our custom vertex type. We added texture coordinates
struct MY_VERTEX
{
	D3DXVECTOR4 position; // The position
	D3DXVECTOR4 mapnormal; // The position
	D3DXVECTOR3 mappos;   // The texture coordinates
};
#define MY_VERTEX_FVF (D3DFVF_XYZRHW | D3DFVF_TEX0 | D3DFVF_TEX1)

struct MY_VERTEX_PADDING
{
	float x,y,z,rhw; // The position
	float u,v;
};
#define MY_VERTEX_PADDING_FVF (D3DFVF_XYZRHW | D3DFVF_TEX0)

// A structure for our custom vertex type
struct CUSTOMVERTEX
{
	FLOAT x, y, z, rhw; // The transformed position for the vertex
	DWORD color;        // The vertex color
};

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)


class D3DRasterizer
{
public:
	D3DRasterizer(void);	
	D3DRasterizer(int texWidth,int texHeight);
	~D3DRasterizer(void);	
	virtual void createBuffer(float3* vertices,float3* normals,float2* uvs,unsigned int numofvertices);	
	LPDIRECT3DTEXTURE9 pRenderTexturePos,pRenderTextureNormal;
	LPDIRECT3DSURFACE9 pRenderSurfacePos,pRenderSurfaceNormal;

	LPDIRECT3DTEXTURE9 pRenderTexturePosPadding,pRenderTextureNormalPadding;
	LPDIRECT3DSURFACE9 pRenderSurfacePosPadding,pRenderSurfaceNormalPadding;

	LPDIRECT3DTEXTURE9 pDestTexturePos,pDestTextureNormal;
	LPDIRECT3DSURFACE9 pDestSurfacePos,pDestSurfaceNormal;

     LPD3DXRENDERTOSURFACE  g_pRenderToSurface;
	void render(TriangleMesh* nodes,int numofnodes);
    bool FlipTexture;	
	void setTextureSize(int width,int height);
	int getTextureWidth(){return m_TexWidth;}
	int getTextureHeight(){return m_TexHeight;}
    //static void rasterizeVertices(GeometryVisitor* geometryVisitor,osg::Node* scene,std::string outdir,bool flip);
	void updateGeometryBuffer(float3* vertices,float3* normals,float2* uvs);
private:

	int m_TexWidth;
   	int m_TexHeight;
    float3* m_pVertices;
	float3* m_pNormals;
	float2* m_pUVS;
	LPDIRECT3DDEVICE9    g_pd3dDevice;
	LPDIRECT3DVERTEXBUFFER9 g_pVB; // Buffer to hold vertices
	ID3DXEffect* g_pEffect;
	int m_NumberOfPrimitives;
	int m_NumberOfVertices;
	int m_NumberOfIndices;
	IDirect3DVertexDeclaration9*    g_pVertexDecl;
	IDirect3DVertexDeclaration9*    g_pVertexDeclPadding;

protected:
	virtual void init();
	virtual void cleanup();
	void createVertices(unsigned int startFace,unsigned int faceCount);
	void createTestVertices();
	void cleanupTexture();
	MY_VERTEX* g_pMyVertices;
};
//void RasterizeVertices(GeometryVisitor* geometryVisitor,osg::Node* scene,std::string outdir,bool flip);