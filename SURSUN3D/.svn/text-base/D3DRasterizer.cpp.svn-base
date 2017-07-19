#include "D3DRasterizer.h"
#include "DXUT.h"
#include "cutil_math.h"


//--------------------------------------------------------------------------------------
// Called during device initialization, this code checks the device for some 
// minimum set of capabilities, and rejects those that don't pass by returning E_FAIL.
//--------------------------------------------------------------------------------------
bool CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat,
								 D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
	// No fallback defined by this app, so reject any device that 
	// doesn't support at least ps2.0
	if( pCaps->PixelShaderVersion < D3DPS_VERSION( 2, 0 ) )
		return false;

	// Skip backbuffer formats that don't support alpha blending
	IDirect3D9* pD3D = DXUTGetD3D9Object();
	if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
		AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING,
		D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
		return false;

	return true;
}
static bool isInitilized = false;
D3DRasterizer::D3DRasterizer(void)
{
	if(!isInitilized)
	{

		DXUTSetCallbackD3D9DeviceAcceptable( IsDeviceAcceptable );
		DXUTInit( true, true ); // Parse the command line and show msgboxes
		DXUTSetHotkeyHandling( true, true, true );
		DXUTCreateWindow( L"D3DRasterizer" );
		DXUTCreateDevice( true, 32, 32);
		isInitilized = true;
	}
	g_pd3dDevice = DXUTGetD3D9Device();
	m_TexWidth = 1024;
	m_TexHeight= 1024;
	FlipTexture = false;


	pRenderTexturePos = NULL;
	pRenderSurfacePos = NULL;


	pRenderTextureNormal = NULL;
	pRenderSurfaceNormal = NULL;


	pRenderTexturePosPadding = NULL;
	pRenderSurfacePosPadding = NULL;

	pRenderTextureNormalPadding = NULL;
	pRenderSurfaceNormalPadding = NULL;

	pDestTexturePos = NULL;
	pDestSurfacePos = NULL;



	pDestTextureNormal = NULL;
	pDestSurfaceNormal = NULL;
	g_pRenderToSurface=NULL;

	g_pVB = NULL;
	g_pMyVertices = NULL;
	g_pVertexDeclPadding = NULL;
	g_pVertexDecl = NULL;
	g_pEffect = NULL;


}
D3DRasterizer::D3DRasterizer(int texWidth,int texHeight)
{

	if(!isInitilized)
	{

		DXUTSetCallbackD3D9DeviceAcceptable( IsDeviceAcceptable );
		DXUTInit( true, true ); // Parse the command line and show msgboxes
		DXUTSetHotkeyHandling( true, true, true );
		DXUTCreateWindow( L"BasicHLSL" );
		DXUTCreateDevice( true, 2048, 2048);
		isInitilized = true;
	}
	g_pd3dDevice = DXUTGetD3D9Device();
	m_TexWidth = texWidth;
	m_TexHeight= texHeight;
	FlipTexture = false;

	g_pVB = NULL;
	g_pMyVertices = NULL;
	g_pVertexDeclPadding = NULL;
	g_pVertexDecl = NULL;
	g_pEffect = NULL;


	pRenderTexturePos = NULL;
	pRenderSurfacePos = NULL;


	pRenderTextureNormal = NULL;
	pRenderSurfaceNormal = NULL;


	pRenderTexturePosPadding = NULL;
	pRenderSurfacePosPadding = NULL;

	pRenderTextureNormalPadding = NULL;
	pRenderSurfaceNormalPadding = NULL;

	pDestTexturePos = NULL;
	pDestSurfacePos = NULL;



	pDestTextureNormal = NULL;
	pDestSurfaceNormal = NULL;

	g_pRenderToSurface=NULL;
}

D3DRasterizer::~D3DRasterizer(void)
{
}

void D3DRasterizer::init()
{
  
}
void D3DRasterizer::updateGeometryBuffer(float3* vertices,float3* normals,float2* uvs)
{
	m_pVertices = vertices;
	m_pNormals = normals;
	m_pUVS = uvs;
}

void D3DRasterizer::cleanupTexture()
{

	if(pRenderTexturePos != NULL){
		pRenderTexturePos->Release(); pRenderTexturePos = NULL;
	}

	if(pRenderSurfacePos != NULL){
		pRenderSurfacePos->Release(); pRenderSurfacePos = NULL;
	}

	if(pRenderTextureNormal != NULL){
		pRenderTextureNormal->Release(); pRenderTextureNormal = NULL;
	}

	if(pRenderSurfaceNormal != NULL){
		pRenderSurfaceNormal->Release(); pRenderSurfaceNormal = NULL;
	}

	if(pRenderTexturePosPadding != NULL){
		pRenderTexturePosPadding->Release(); pRenderTexturePosPadding = NULL;
	}

	if(pRenderSurfacePosPadding != NULL){
		pRenderSurfacePosPadding->Release(); pRenderSurfacePosPadding = NULL;
	}

	if(pRenderTextureNormalPadding != NULL){
		pRenderTextureNormalPadding->Release(); pRenderTextureNormalPadding = NULL;
	}

	if(pRenderSurfaceNormalPadding != NULL){
		pRenderSurfaceNormalPadding->Release(); pRenderSurfaceNormalPadding = NULL;
	}

	if(pDestTexturePos != NULL){
		pDestTexturePos->Release(); pDestTexturePos = NULL;
	}

	if(pDestSurfacePos != NULL){
		pDestSurfacePos->Release(); pDestSurfacePos = NULL;
	}

	if(pDestTextureNormal != NULL){
		pDestTextureNormal->Release(); pDestTextureNormal = NULL;
	}

	if(pDestSurfaceNormal != NULL){
		pDestSurfaceNormal->Release(); pDestSurfaceNormal = NULL;
	}


	if(g_pRenderToSurface != NULL){
		g_pRenderToSurface->Release(); g_pRenderToSurface = NULL;
	}
}

void D3DRasterizer::cleanup()
{
	if(g_pMyVertices)
	{
		delete[] g_pMyVertices;
		g_pMyVertices = NULL;
	}

	if(g_pVB)
	{
		g_pVB->Release();
		g_pVB = NULL;
	}


	if(g_pEffect)
	{
		g_pEffect->Release();
		g_pEffect = NULL;
	}

	if(g_pEffect)
	{
		g_pEffect->Release();
		g_pEffect = NULL;
	}

	if(g_pVertexDecl)
	{
		g_pVertexDecl->Release();
		g_pVertexDecl = NULL;
	}

	if(g_pVertexDeclPadding)
	{
		g_pVertexDeclPadding->Release();
		g_pVertexDeclPadding = NULL;
	}
	
	cleanupTexture();

}
void D3DRasterizer::setTextureSize(int width,int height)
{

  if(m_TexHeight == width && m_TexWidth == height)
	  return;
   m_TexHeight = height;
   m_TexWidth = width;

   /* if(pRenderSurfacePos != NULL)
   {

   pRenderTexturePos->Release(); pRenderTexturePos = NULL;
   if(pRenderSurfacePos != NULL)
   pRenderSurfacePos->Release(); pRenderSurfacePos = NULL;


   pRenderTextureNormal->Release(); pRenderTextureNormal = NULL;
   if(pRenderSurfaceNormal != NULL)
   pRenderSurfaceNormal->Release(); pRenderSurfaceNormal = NULL;


   pRenderTexturePosPadding->Release(); pRenderTexturePosPadding = NULL;
   if(pRenderSurfacePosPadding != NULL)
   pRenderSurfacePosPadding->Release(); pRenderSurfacePosPadding = NULL;

   pRenderTextureNormalPadding->Release(); pRenderTextureNormalPadding = NULL;
   if(pRenderSurfaceNormalPadding != NULL)
   pRenderSurfaceNormalPadding->Release(); pRenderSurfaceNormalPadding = NULL;

   pDestTexturePos->Release(); pDestTexturePos = NULL;
   if(pDestSurfacePos != NULL)
   pDestSurfacePos->Release(); pDestSurfacePos = NULL;



   pDestTextureNormal->Release(); pDestTextureNormal = NULL;
   if(pDestSurfaceNormal != NULL)
   pDestSurfaceNormal->Release(); pDestSurfaceNormal = NULL;

   g_pRenderToSurface->Release();g_pRenderToSurface=NULL;

   }*/

   cleanupTexture();

   D3DFORMAT fmt = D3DFMT_A32B32G32R32F;
   //D3DFORMAT fmt = fmt;
   g_pd3dDevice->CreateTexture(m_TexWidth,m_TexHeight,1,D3DUSAGE_RENDERTARGET,fmt,D3DPOOL_DEFAULT,&pRenderTexturePos,NULL);
   pRenderTexturePos->GetSurfaceLevel(0,&pRenderSurfacePos);


   g_pd3dDevice->CreateTexture(m_TexWidth,m_TexHeight,1,D3DUSAGE_RENDERTARGET,fmt,D3DPOOL_DEFAULT,&pRenderTextureNormal,NULL);
   pRenderTextureNormal->GetSurfaceLevel(0,&pRenderSurfaceNormal);


   g_pd3dDevice->CreateTexture(m_TexWidth,m_TexHeight,1,D3DUSAGE_RENDERTARGET,fmt,D3DPOOL_DEFAULT,&pRenderTexturePosPadding,NULL);
   pRenderTexturePosPadding->GetSurfaceLevel(0,&pRenderSurfacePosPadding);


   g_pd3dDevice->CreateTexture(m_TexWidth,m_TexHeight,1,D3DUSAGE_RENDERTARGET,fmt,D3DPOOL_DEFAULT,&pRenderTextureNormalPadding,NULL);
   pRenderTextureNormalPadding->GetSurfaceLevel(0,&pRenderSurfaceNormalPadding);


   g_pd3dDevice->CreateTexture(m_TexWidth,m_TexHeight,1,D3DUSAGE_DYNAMIC,fmt, D3DPOOL_SYSTEMMEM,&pDestTexturePos,NULL);
   pDestTexturePos->GetSurfaceLevel(0,&pDestSurfacePos);


   g_pd3dDevice->CreateTexture(m_TexWidth,m_TexHeight,1,D3DUSAGE_DYNAMIC,fmt, D3DPOOL_SYSTEMMEM,&pDestTextureNormal,NULL);
   pDestTextureNormal->GetSurfaceLevel(0,&pDestSurfaceNormal);


   // Create a ID3DXRenderToSurface to help render to a texture on cards 
   // that don't support render targets
   D3DXCreateRenderToSurface(g_pd3dDevice, m_TexWidth,m_TexHeight,
	   fmt, TRUE, D3DFMT_D16, &g_pRenderToSurface );

}
void D3DRasterizer::createBuffer(float3* vertices,float3* normals,float2* uvs,unsigned int numofvertices)
{
	g_pd3dDevice = DXUTGetD3D9Device();
	m_pVertices = vertices;
	m_pNormals = normals;
	m_pUVS = uvs;

	
	cleanup();

    g_pMyVertices = new MY_VERTEX[numofvertices];
	// Create the vertex buffer.
	if( FAILED( g_pd3dDevice->CreateVertexBuffer( numofvertices * sizeof( MY_VERTEX ),
		0, MY_VERTEX_FVF,
		D3DPOOL_DEFAULT, &g_pVB, NULL ) ) )
	{
		return;
	}
	// Fill the vertex buffer. We are setting the tu and tv texture
	// coordinates, which range from 0.0 to 1.0
	MY_VERTEX* pVertices;
	if( FAILED( g_pVB->Lock( 0, 0, ( void** )&pVertices, 0 ) ) )
		return;
	for (int i=0;i<numofvertices;i++)
	{

		//MY_VERTEX p;
		float3 pos = vertices[i];
		float3 normal = normals[i];
		float2 uv = uvs[i];
		pVertices[i].position.x = uv.x * m_TexWidth;
		pVertices[i].position.y = uv.y * m_TexHeight;
		pVertices[i].position.z = 0.5;
		pVertices[i].position.w = 1;

		pVertices[i].mapnormal.x = normal.x;
		pVertices[i].mapnormal.y = normal.y;
		pVertices[i].mapnormal.z = normal.z;

		pVertices[i].mappos.x = pos.x;
		pVertices[i].mappos.y = pos.y;
	    pVertices[i].mappos.z = pos.z;

	} 
    //赋予三角形索引
	for (int i=0;i<numofvertices/3;i++)
	{
		pVertices[i*3].mapnormal.w = i;
		pVertices[i*3+1].mapnormal.w = i;
		pVertices[i*3+2].mapnormal.w = i;
	}

	g_pVB->Unlock();

	m_NumberOfVertices = numofvertices;
	DWORD dwShaderFlags;
#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DXSHADER_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DXSHADER_DEBUG;
#endif

#ifdef DEBUG_VS
	dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
#endif
#ifdef DEBUG_PS
	dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
#endif
	LPD3DXBUFFER error_buffer;

	D3DXCreateEffectFromFileA(g_pd3dDevice, "rasterizer.fx", NULL, NULL, NULL, NULL, &g_pEffect, &error_buffer);
	//D3DXCreateEffectFromFileA(g_pd3dDevice, "sectionslice.fx", NULL, NULL, NULL, NULL, &g_pEffect, &buf);
	if(error_buffer)
	{
		//MessageBoxA(NULL, (char *) (error_buffer->GetBufferPointer()), "ERROR", MB_OK);
		printf("error:%s\n",(char *) (error_buffer->GetBufferPointer()));
		std::string str = (char *) (error_buffer->GetBufferPointer());
		error_buffer->Release();

	}


	LPDIRECT3DTEXTURE9 tex;
	//pd3dDevice->CreateTexture(m_TexWidth,m_TexHeight,1,D3DUSAGE_RENDERTARGET,D3DFMT_R5G6B5,D3DPOOL_DEFAULT,&pRenderTexture,NULL);


    D3DFORMAT fmt = D3DFMT_A32B32G32R32F;
    //D3DFORMAT fmt = fmt;
	g_pd3dDevice->CreateTexture(m_TexWidth,m_TexHeight,1,D3DUSAGE_RENDERTARGET,fmt,D3DPOOL_DEFAULT,&pRenderTexturePos,NULL);
	pRenderTexturePos->GetSurfaceLevel(0,&pRenderSurfacePos);


	g_pd3dDevice->CreateTexture(m_TexWidth,m_TexHeight,1,D3DUSAGE_RENDERTARGET,fmt,D3DPOOL_DEFAULT,&pRenderTextureNormal,NULL);
	pRenderTextureNormal->GetSurfaceLevel(0,&pRenderSurfaceNormal);


	g_pd3dDevice->CreateTexture(m_TexWidth,m_TexHeight,1,D3DUSAGE_RENDERTARGET,fmt,D3DPOOL_DEFAULT,&pRenderTexturePosPadding,NULL);
	pRenderTexturePosPadding->GetSurfaceLevel(0,&pRenderSurfacePosPadding);


	g_pd3dDevice->CreateTexture(m_TexWidth,m_TexHeight,1,D3DUSAGE_RENDERTARGET,fmt,D3DPOOL_DEFAULT,&pRenderTextureNormalPadding,NULL);
	pRenderTextureNormalPadding->GetSurfaceLevel(0,&pRenderSurfaceNormalPadding);


	g_pd3dDevice->CreateTexture(m_TexWidth,m_TexHeight,1,D3DUSAGE_DYNAMIC,fmt, D3DPOOL_SYSTEMMEM,&pDestTexturePos,NULL);
	pDestTexturePos->GetSurfaceLevel(0,&pDestSurfacePos);


	g_pd3dDevice->CreateTexture(m_TexWidth,m_TexHeight,1,D3DUSAGE_DYNAMIC,fmt, D3DPOOL_SYSTEMMEM,&pDestTextureNormal,NULL);
	pDestTextureNormal->GetSurfaceLevel(0,&pDestSurfaceNormal);


	// Create a ID3DXRenderToSurface to help render to a texture on cards 
	// that don't support render targets
	D3DXCreateRenderToSurface(g_pd3dDevice, m_TexWidth,m_TexHeight,
		fmt, TRUE, D3DFMT_D16, &g_pRenderToSurface );


	D3DVERTEXELEMENT9 VE[] =
	{
		{ 0, 0,  D3DDECLTYPE_FLOAT4,     D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITIONT,  0 },
		{ 0, 16, D3DDECLTYPE_FLOAT4,     D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD,  0 },
		{ 0, 32, D3DDECLTYPE_FLOAT3,     D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD,  1 },
		D3DDECL_END()
	};

	g_pd3dDevice->CreateVertexDeclaration(VE, &g_pVertexDecl);

	D3DVERTEXELEMENT9 VEPadding[] =
	{
		{ 0, 0,  D3DDECLTYPE_FLOAT4,     D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITIONT,  0 },
		{ 0, 16, D3DDECLTYPE_FLOAT2,     D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD,  0 },
		D3DDECL_END()
	};

	g_pd3dDevice->CreateVertexDeclaration(VEPadding, &g_pVertexDeclPadding);

	//D3DLOCKED_RECT lockedRect;
	//RECT rc = {0, 0, 1024, 1024};
	//HRESULT result = pRenderTextureNormal->LockRect(0,&lockedRect, &rc, D3DLOCK_READONLY);
}

void D3DRasterizer::createTestVertices()
{
	g_pMyVertices[0].position = D3DXVECTOR4(0.1 * m_TexWidth,0.6 * m_TexHeight,0.5,1);
	g_pMyVertices[1].position = D3DXVECTOR4(0.1 * m_TexWidth,0.1 * m_TexHeight,0.5,1);
	g_pMyVertices[2].position = D3DXVECTOR4(0.6 * m_TexWidth,0.6 * m_TexHeight,0.5,1);

	g_pMyVertices[0].mapnormal = D3DXVECTOR4(1,0,0,1);
	g_pMyVertices[1].mapnormal = D3DXVECTOR4(1,0,0,1);
	g_pMyVertices[2].mapnormal = D3DXVECTOR4(1,0,0,1);

	g_pMyVertices[0].mappos = D3DXVECTOR3(-41.3099, 41.3099,1);
	g_pMyVertices[1].mappos = D3DXVECTOR3(-41.3099,-41.3099,1);
	g_pMyVertices[2].mappos = D3DXVECTOR3( 41.3099, 41.3099,1);
}


void D3DRasterizer::createVertices(unsigned int startFace,unsigned int faceCount)
{
	//MY_VERTEX* pVertices = new MY_VERTEX[faceCount*3];

	unsigned int startvertex = startFace*3;
	for (unsigned int i=0;i<faceCount*3;i++)
	{

		//MY_VERTEX p;
		float3 pos = m_pVertices[startvertex + i];
		float3 normal = m_pNormals[startvertex + i];
		float2 uv = m_pUVS[startvertex + i];
		g_pMyVertices[i].position.x = uv.x * m_TexWidth;
		g_pMyVertices[i].position.y = uv.y * m_TexHeight;
		g_pMyVertices[i].position.z = 0.5;
		g_pMyVertices[i].position.w = 1;

		g_pMyVertices[i].mapnormal.x = normal.x;
		g_pMyVertices[i].mapnormal.y = normal.y;
		g_pMyVertices[i].mapnormal.z = normal.z;
		g_pMyVertices[i].mapnormal.w = 1;

		g_pMyVertices[i].mappos.x = pos.x;
		g_pMyVertices[i].mappos.y = pos.y;
		g_pMyVertices[i].mappos.z = pos.z;

	} 
	//赋予三角形索引
	for (unsigned int i=0;i<faceCount;i++)
	{

		if(abs(g_pMyVertices[i*3].mapnormal.x) > 0.001 || abs(g_pMyVertices[i*3].mapnormal.y) > 0.001 || abs(g_pMyVertices[i*3].mapnormal.z) > 0.001)
		{
			g_pMyVertices[i*3].mapnormal.w = startFace + i;
			g_pMyVertices[i*3+1].mapnormal.w = startFace + i;
			g_pMyVertices[i*3+2].mapnormal.w = startFace + i;

		}
		//m_pMyVertices[i*3].mapnormal.w = startFace + i;
		//m_pMyVertices[i*3+1].mapnormal.w = startFace + i;
		//m_pMyVertices[i*3+2].mapnormal.w = startFace + i;
	}
	//return m_pMyVertices;

}
void D3DRasterizer::render(TriangleMesh* nodes,int numofnodes)
{    


	HRESULT result;
	D3DFORMAT fmt = D3DFMT_A32B32G32R32F;
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,false);
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	D3DXVECTOR4 one_over_pixel(1.0f/m_TexWidth,1.0f/m_TexHeight,1.0f/m_TexWidth,1.0f/m_TexHeight);
	g_pEffect->SetVector("one_over_pixel",&one_over_pixel);
	if( SUCCEEDED( g_pRenderToSurface->BeginScene( pRenderSurfacePos, NULL ) ) )
	{

		g_pd3dDevice->SetRenderTarget(0,pRenderSurfacePos);
		g_pd3dDevice->SetRenderTarget(1,pRenderSurfaceNormal);

		result = g_pd3dDevice->Clear(0,
			NULL,
			D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
			D3DCOLOR_RGBA(0,0,0,0),
			1.0f,
			0);
		result = g_pd3dDevice->Clear(1,
			NULL,
			D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
			D3DCOLOR_RGBA(0,0,0,0),
			1.0f,
			0);
		g_pd3dDevice->SetVertexDeclaration(g_pVertexDecl);
		UINT iPass, cPasses;
		// render the vertex buffer contents
		//g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof( MY_VERTEX ) );

		g_pEffect->SetTechnique("RenderTechnique");
		g_pEffect->Begin( &cPasses, 0 );

		for( iPass = 0; iPass < cPasses; iPass++ )
		{
			g_pEffect->BeginPass( iPass );
			for (int j=0;j<numofnodes;j++)
			{
				TriangleMesh node = nodes[j];
				createVertices(node.FaceStart,node.FaceCount);
		        result = g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST,node.FaceCount,g_pMyVertices,sizeof(MY_VERTEX));
			}
			g_pEffect->EndPass();

		}
		g_pEffect->End();
		
		result = g_pRenderToSurface->EndScene( 0 );
		g_pd3dDevice->GetRenderTargetData(pRenderSurfacePos,pDestSurfacePos);
		g_pd3dDevice->GetRenderTargetData(pRenderSurfaceNormal,pDestSurfaceNormal);
	}

	if( SUCCEEDED( g_pRenderToSurface->BeginScene( pRenderSurfacePosPadding, NULL ) ) )
	{

		g_pd3dDevice->SetRenderTarget(0,pRenderSurfacePosPadding);
		g_pd3dDevice->SetRenderTarget(1,pRenderSurfaceNormalPadding);

		result = g_pd3dDevice->Clear(0,
			NULL,
			D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
			D3DCOLOR_RGBA(0,0,0,0),
			1.0f,
			0);
		result = g_pd3dDevice->Clear(1,
			NULL,
			D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
			D3DCOLOR_RGBA(0,0,0,0),
			1.0f,
			0);

		UINT iPass, cPasses;
		// render the vertex buffer contents
		//g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof( MY_VERTEX ) );
		g_pEffect->SetTexture("Texture0",pRenderTexturePos);
		g_pEffect->SetTexture("Texture1",pRenderTextureNormal);
		g_pEffect->SetBool("flip",FlipTexture);
		g_pd3dDevice->SetVertexDeclaration(g_pVertexDeclPadding);
		//g_pd3dDevice->SetFVF(MY_VERTEX_PADDING_FVF);


		result = g_pEffect->SetTechnique("RenderTechnique_PADDING");
		result = g_pEffect->Begin( &cPasses, 0 );
		result = g_pEffect->BeginPass( 0 );



		float top = 0;
		float left = 0;
		float bottom = m_TexHeight;
		float right = m_TexWidth;
		MY_VERTEX_PADDING quad[] = {
		{0.0f,0.0f,0.5f,1.0f,0.0f,0.0f},
		{m_TexWidth,0.0f,0.5f,1.0f,1.0f,0.0f},
		{0.0f,m_TexHeight,0.5f,1.0f,0.0f,1.0f},
		{m_TexWidth,m_TexHeight,0.5f,1.0f,1.0f,1.0f}
		};

		result = g_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP,2,quad,sizeof(MY_VERTEX_PADDING));


		g_pEffect->EndPass();

		g_pEffect->End();

		result = g_pRenderToSurface->EndScene( 0 );

		/*g_pd3dDevice->CreateOffscreenPlainSurface(
			m_TexWidth, m_TexHeight, fmt, D3DPOOL_SYSTEMMEM, &pDestSurfacePos, NULL);

		g_pd3dDevice->CreateOffscreenPlainSurface(
			m_TexWidth, m_TexHeight, fmt, D3DPOOL_SYSTEMMEM, &pDestSurfaceNormal, NULL);*/
		g_pd3dDevice->GetRenderTargetData(pRenderSurfacePosPadding,pDestSurfacePos);
		g_pd3dDevice->GetRenderTargetData(pRenderSurfaceNormalPadding,pDestSurfaceNormal);
		//D3DXSaveSurfaceToFileA("test.bmp",D3DXIMAGE_FILEFORMAT::D3DXIFF_BMP,pRenderSurfaceNormal,NULL,NULL);

	}
	//g_pd3dDevice->Present(NULL,NULL,NULL,NULL);



}




//void RasterizeVertices(GeometryVisitor* geometryVisitor,osg::Node* scene,std::string outdir,bool flip)
//{
//
//	std::map<osg::Texture2D*,int>::iterator iter = geometryVisitor->TextureList.begin();	
//	int count = 0;
//	QString dirname = outdir.data();
//	QDir dir(dirname);
//	if(!dir.exists())
//		dir.mkdir(dirname);
//
//	D3DRasterizer renderer(1024,1024);
//	renderer.createBuffer(geometryVisitor->getVertices(),geometryVisitor->getNormals(),geometryVisitor->getUVs(),geometryVisitor->getNumberOfFaces() * 3);
//	renderer.FlipTexture = flip;
//	std::vector<TexturedDrawable> drawables = geometryVisitor->getDrawablesByDescendingOrder();
//	int oldwidth = -1;
//	int oldheight = -1;
//	for (int n=0;n<drawables.size();n++)
//	{
//
//		TexturedDrawable tdrawable = drawables[n];
//
//		int Width = tdrawable.Width;
//		int Height = tdrawable.Height;
//
//		if(Width != oldwidth || Height != oldheight)
//		{
//			renderer.setTextureSize(Width,Height);
//			oldheight = Height;
//			oldwidth = Width;
//		}
//
//		std::string filename = tdrawable.FileName;
//		if(filename.length() < 3)
//			continue;
//
//		QFileInfo fileInfo(filename.data());
//		/*QString texturefilename = dirname + QString("/") + fileInfo.baseName()  + QString(".") +  fileInfo.completeSuffix();
//		QString normalfilename = dirname + QString("/") + fileInfo.baseName() + QString("_normal.dds");
//		QString posfilename = dirname + QString("/") + fileInfo.baseName() + QString("_position.dds");
//		QString tesfilename = dirname + QString("/") + fileInfo.baseName() + QString(".bmp");*/
//		//if(QFileInfo(posfilename).exists() && QFileInfo(normalfilename).exists())
//		//	continue;
//		QString ext = fileInfo.suffix();
//		printf("%s\n",filename.data());
//
//		TriangleMesh* nodes = new TriangleMesh[tdrawable.NodeIndices.size()];
//		for (int i=0;i < tdrawable.NodeIndices.size();i++)
//		{
//
//			int nodeindex = tdrawable.NodeIndices[i];
//			TriangleMesh node = geometryVisitor->getGeometryNodes()[nodeindex];
//			nodes[i]=node;
//
//		}
//		renderer.render(nodes,tdrawable.NodeIndices.size());
//
//
//		delete[] nodes; 
//
//		std::string normalfilename = tdrawable.getNormalFileName();
//		std::string posfilename =  tdrawable.getPosFileName();
//		D3DXSaveSurfaceToFileA(posfilename.data(),D3DXIMAGE_FILEFORMAT::D3DXIFF_DDS,renderer.pDestSurfacePos,NULL,NULL);
//		D3DXSaveSurfaceToFileA(normalfilename.data(),D3DXIMAGE_FILEFORMAT::D3DXIFF_DDS,renderer.pDestSurfaceNormal,NULL,NULL);
//
//		continue;
//
//
//		D3DLOCKED_RECT lockedRectNormal,lockedRectPos;
//		HRESULT result = renderer.pDestSurfaceNormal->LockRect(&lockedRectNormal, NULL, D3DLOCK_READONLY);
//		result = renderer.pDestSurfacePos->LockRect(&lockedRectPos, NULL, D3DLOCK_READONLY);
//
//
//		osg::ref_ptr<osg::Image> img = new osg::Image;
//		img->allocateImage(Width,Height,1,GL_RGB,GL_UNSIGNED_BYTE);
//		unsigned char* data = img->data();
//
//		//
//		//std::map<unsigned int,unsigned int> triangleids;
//		//ofstream ofs("test.txt");
//		//ostringstream oss;
//		float miny = FLT_MAX;
//		float maxy = -FLT_MAX;
//		for (int i=0;i<Height;i++)
//		{
//
//			//if(i != (int)(0.2 * Height))
//			//{
//			//	continue;
//			//}
//			for (int j=0;j<Width;j++)
//			{
//
//				/*	if(j > 50)
//				continue;*/
//				float3 normal = make_float3(0,0,0);
//				float3 pos = make_float3(0,0,0);
//
//
//				unsigned char* pNormalSurfaceData = (unsigned char*)(lockedRectNormal.pBits) + i*lockedRectNormal.Pitch+ j * 16;
//
//				unsigned char* pPosSurfaceData =    (unsigned char*)(lockedRectPos.pBits) + i*lockedRectPos.Pitch+ j * 16;
//
//				float* pNormal = (float*)pNormalSurfaceData;
//				float* pPos = (float*)pPosSurfaceData;
//
//
//				normal.x = pNormal[3]; // Blue
//				normal.y = pNormal[2]; // Green
//				normal.z = pNormal[1]; // Red
//
//
//				pos.x = pPos[3]; // Blue
//				pos.y = pPos[2]; // Green
//				pos.z = pPos[1]; // Red
//
//				*data = (char)(abs(normal.x * 255));data++;
//				*data = (char)(abs(normal.y * 255));data++;
//				*data = (char)(abs(normal.z * 255));data++;
//				//oss << pos.x << " ";
//
//			}
//			//oss << endl;
//		}
//		//ofs.close();
//		osgDB::writeImageFile(*img,(fileInfo.baseName()  + QString(".") +  fileInfo.completeSuffix()).toStdString());
//		//printf("%s\n",oss.str().data());
//		renderer.pDestSurfaceNormal->UnlockRect();
//		renderer.pDestSurfacePos->UnlockRect();
//
//
//		continue;
//	}
//
//}
		

	

