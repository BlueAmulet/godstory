//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "gfx/D3D9/gfxD3D9Device.h"
#include "gfx/gfxD3D9OcclusionQuery.h"

GFXD3D9OcclusionQuery::~GFXD3D9OcclusionQuery()
{
   SAFE_RELEASE( d3dQuery );
   SAFE_RELEASE( occlusionTexture );
   SAFE_DELETE(rect);
}

//------------------------------------------------------------------------------

bool GFXD3D9OcclusionQuery::initialize()
{
	SAFE_RELEASE( d3dQuery );
	SAFE_RELEASE( occlusionTexture );

	d3dDevice = dynamic_cast<GFXD3D9Device *>(pGFXDevice)->getDevice();

	// Create the query
	d3dDevice->CreateQuery( D3DQUERYTYPE_OCCLUSION, &d3dQuery );

	//IDirect3DSurface9 *buffer;
	//D3DSURFACE_DESC desc;

	//d3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &buffer );
	//buffer->GetDesc(&desc);
	//SAFE_RELEASE(buffer);

	//// Create the texture first, so we can get access to it's surface
	//if( FAILED( D3DXCreateTexture( d3dDevice, desc.Width,desc.Height, 1, D3DUSAGE_RENDERTARGET,
	//	desc.Format, D3DPOOL_DEFAULT, &occlusionTexture ) ) )
	//{
	//	return false;
	//}

	//if(!rect)
	//{
	//	rect = new RectI;
	//}

	return true;
}

bool GFXD3D9OcclusionQuery::begin()
{
	//*rect = GFX->getViewport();

	//d3dDevice->GetRenderTarget(0,&backBuff);

	//IDirect3DSurface9 *buffer;
	//occlusionTexture->GetSurfaceLevel(0,&buffer);
	//d3dDevice->SetRenderTarget(0,buffer);
	//SAFE_RELEASE(buffer);

	////d3dDevice->SetRenderTarget(0,NULL);

	//GFX->setViewport(*rect);

	d3dQuery->Issue( D3DISSUE_BEGIN );
	return true;
}

bool GFXD3D9OcclusionQuery::end()
{
	d3dQuery->Issue( D3DISSUE_END );
	canQuery = true;
	return true;
}

int GFXD3D9OcclusionQuery::query()
{
	DWORD pixelsVisible = 0;

	if(canQuery)
	{
		while (d3dQuery->GetData((void *) &pixelsVisible, 
			sizeof(DWORD), D3DGETDATA_FLUSH) == S_FALSE);

		//d3dDevice->SetRenderTarget(0,backBuff);
		//SAFE_RELEASE(backBuff);

		//GFX->setViewport(*rect);
		canQuery = false;
	}

	return pixelsVisible;
}

void GFXD3D9OcclusionQuery::zombify()
{
	SAFE_RELEASE( d3dQuery );
	SAFE_RELEASE( occlusionTexture );
}

void GFXD3D9OcclusionQuery::resurrect()
{
	// Recreate the query
	if( d3dQuery == NULL )
	{
		initialize();
	}
}