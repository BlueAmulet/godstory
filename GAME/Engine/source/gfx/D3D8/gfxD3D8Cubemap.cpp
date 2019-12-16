//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include <d3dx8.h>
#include "gfx/D3D8/gfxD3D8Cubemap.h"
#include "gfx/D3D8/gfxD3D8Device.h"

// Set to make dynamic cubemaps only update once.
//#define INIT_HACK

_D3DCUBEMAP_FACES GFXD3D8Cubemap::faceList[6] = 
{ 
   D3DCUBEMAP_FACE_POSITIVE_X, D3DCUBEMAP_FACE_NEGATIVE_X,
   D3DCUBEMAP_FACE_POSITIVE_Y, D3DCUBEMAP_FACE_NEGATIVE_Y,
   D3DCUBEMAP_FACE_POSITIVE_Z, D3DCUBEMAP_FACE_NEGATIVE_Z
};

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
GFXD3D8Cubemap::GFXD3D8Cubemap()
{
   mCubeTex = NULL;
   mDepthBuff = NULL;
   mDynamic = false;
   mCallbackHandle = -1;

   #ifdef INIT_HACK
   mInit = false;
   #endif
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
GFXD3D8Cubemap::~GFXD3D8Cubemap()
{
   releaseSurfaces();
   
   if( mDynamic )
   {
      GFX->unregisterTexCallback( mCallbackHandle );
   }
}

//-----------------------------------------------------------------------------
// Release D3D surfaces
//-----------------------------------------------------------------------------
void GFXD3D8Cubemap::releaseSurfaces()
{
   if( mCubeTex )
   {
      mCubeTex->Release();
      mCubeTex = NULL;
   }
   if( mDepthBuff )
   {
      mDepthBuff->Release();
      mDepthBuff = NULL;
   }
}

//-----------------------------------------------------------------------------
// Texture manager callback - for resetting textures on video change
//-----------------------------------------------------------------------------
void GFXD3D8Cubemap::texManagerCallback( GFXTexCallbackCode code, void *userData )
{
   GFXD3D8Cubemap *cubemap = (GFXD3D8Cubemap *) userData;
   
   if( !cubemap->mDynamic ) return;
   
   if( code == GFXZombify )
   {
      cubemap->releaseSurfaces();
      return;
   }

   if( code == GFXResurrect )
   {
      cubemap->initDynamic( cubemap->mTexSize );
      return;
   }
}

//-----------------------------------------------------------------------------
// Init Static
//-----------------------------------------------------------------------------
void GFXD3D8Cubemap::initStatic( GFXTexHandle *faces )
{
   if( mCubeTex )
   {
      return;
   }

   if( faces )
   {
      if( !faces[0] )
      {
         AssertFatal( false, "empty texture passed to CubeMap::create" );
      }

      LPDIRECT3DDEVICE8 D3DDevice = dynamic_cast<GFXD3D8Device *>(GFX)->getDevice();
      
      
      // NOTE - check tex sizes on all faces - they MUST be all same size
      U32 texSize = faces[0].getWidth();

      D3D8Assert( D3DDevice->CreateCubeTexture( texSize, 1, 0, D3DFMT_A8R8G8B8,
                 D3DPOOL_MANAGED, &mCubeTex ), NULL );


      fillCubeTextures( faces, D3DDevice );
//      mCubeTex->GenerateMipSubLevels();
   }
   
}


//-----------------------------------------------------------------------------
// Init Dynamic
//-----------------------------------------------------------------------------
void GFXD3D8Cubemap::initDynamic( U32 texSize )
{
   if( mCubeTex )
   {
      return;
   }

   if( mCallbackHandle == -1 )  // make sure it hasn't already registered.
   {
      GFX->registerTexCallback( texManagerCallback, (void*) this, mCallbackHandle );
   }
   
   mInit = false;
   mDynamic = true;
   mTexSize = texSize;
   

   LPDIRECT3DDEVICE8 D3DDevice = dynamic_cast<GFXD3D8Device *>(GFX)->getDevice();

   // might want to try this as a 16 bit texture...
   D3D8Assert( D3DDevice->CreateCubeTexture( texSize,
                                            1, 
                                            D3DUSAGE_RENDERTARGET, 
                                            D3DFMT_A8R8G8B8,
                                            D3DPOOL_DEFAULT, 
                                            &mCubeTex ), NULL );
      
   D3DDevice->CreateDepthStencilSurface( texSize, 
                                         texSize, 
                                         D3DFMT_D24S8, 
                                         D3DMULTISAMPLE_NONE, 
                                         &mDepthBuff );
   
}

//-----------------------------------------------------------------------------
// Fills in face textures of cube map from existing textures
//-----------------------------------------------------------------------------
void GFXD3D8Cubemap::fillCubeTextures( GFXTexHandle *faces, LPDIRECT3DDEVICE8 D3DDevice )
{
   for( U32 i=0; i<6; i++ )
   {
      // get cube face surface
      IDirect3DSurface8 *cubeSurf = NULL;
      D3D8Assert( mCubeTex->GetCubeMapSurface( faceList[i], 0, &cubeSurf ), NULL );

      // get incoming texture surface
      GFXD3D8TextureObject *texObj = dynamic_cast<GFXD3D8TextureObject*>( (GFXTextureObject*)faces[i] );
      IDirect3DSurface8 *inSurf;
      D3D8Assert( texObj->get2DTex()->GetSurfaceLevel( 0, &inSurf ), NULL );
      
      // copy incoming texture into cube face
      D3D8Assert( GFXD3D8X.GFXD3D8XLoadSurfaceFromSurface( cubeSurf, NULL, NULL, inSurf, NULL, 
                                  NULL, D3DX_FILTER_NONE, 0 ), NULL );
      cubeSurf->Release();
      inSurf->Release();
   }
}

//-----------------------------------------------------------------------------
// Set the cubemap to the specified texture unit num
//-----------------------------------------------------------------------------
void GFXD3D8Cubemap::setToTexUnit( U32 tuNum )
{
   LPDIRECT3DDEVICE8 D3DDevice = static_cast<GFXD3D8Device *>(GFX)->getDevice();
   D3DDevice->SetTexture( tuNum, mCubeTex );
}

//-----------------------------------------------------------------------------
// Update dynamic cubemap
//-----------------------------------------------------------------------------
void GFXD3D8Cubemap::updateDynamic( const Point3F &pos )
{
   if( !mCubeTex ) return;

#ifdef INIT_HACK
   if( mInit ) return;
   mInit = true;
#endif

   LPDIRECT3DDEVICE8 D3DDevice = dynamic_cast<GFXD3D8Device *>(GFX)->getDevice();

   GFX->pushActiveRenderTarget();

   // store current matrices
   GFX->pushWorldMatrix();
   MatrixF proj = GFX->getProjectionMatrix();

   // store previous depth buffer
   IDirect3DSurface8 *prevDepthSurface;
   D3DDevice->GetDepthStencilSurface( &prevDepthSurface );

   // set projection to 90 degrees vertical and horizontal
   MatrixF matProj;
   GFXD3D8X.GFXD3D8XMatrixPerspectiveFovRH( (D3DXMATRIX *) &matProj, D3DX_PI/2, 1.0f, 0.1f, 1000.0f );
   matProj.transpose();

   // set projection to match PowerEngine space
   MatrixF rotMat(EulerF( F32(M_PI / 2.f), 0.0, 0.0));

   matProj.mul(rotMat);
   GFX->setProjectionMatrix(matProj);
   
   // Taken out for TGB -pw
   //gClientSceneGraph->setReflectPass( true );
   
   // Loop through the six faces of the cube map.
   for( DWORD i=0; i<6; i++ )
   {
      // Standard view that will be overridden below.
      VectorF vEnvEyePt = VectorF( 0.0f, 0.0f, 0.0f );
      VectorF vLookatPt, vUpVec, vRight;

      switch( i )
      {
         case D3DCUBEMAP_FACE_POSITIVE_X:
             vLookatPt = VectorF( 1.0f, 0.0f, 0.0f );
             vUpVec    = VectorF( 0.0f, 1.0f, 0.0f );
             break;
         case D3DCUBEMAP_FACE_NEGATIVE_X:
             vLookatPt = VectorF( -1.0f, 0.0f, 0.0f );
             vUpVec    = VectorF( 0.0f, 1.0f, 0.0f );
             break;
         case D3DCUBEMAP_FACE_POSITIVE_Y:
             vLookatPt = VectorF( 0.0f, 1.0f, 0.0f );
             vUpVec    = VectorF( 0.0f, 0.0f,-1.0f );
             break;
         case D3DCUBEMAP_FACE_NEGATIVE_Y:
             vLookatPt = VectorF( 0.0f, -1.0f, 0.0f );
             vUpVec    = VectorF( 0.0f, 0.0f, 1.0f );
             break;
         case D3DCUBEMAP_FACE_POSITIVE_Z:
             vLookatPt = VectorF( 0.0f, 0.0f, 1.0f );
             vUpVec    = VectorF( 0.0f, 1.0f, 0.0f );
             break;
         case D3DCUBEMAP_FACE_NEGATIVE_Z:
             vLookatPt = VectorF( 0.0f, 0.0f, -1.0f );
             vUpVec    = VectorF( 0.0f, 1.0f, 0.0f );
             break;
      }

      // create camera matrix
      VectorF cross = mCross( vUpVec, vLookatPt );
      cross.normalizeSafe();

      MatrixF matView(true);
      matView.setColumn( 0, cross );
      matView.setColumn( 1, vLookatPt );
      matView.setColumn( 2, vUpVec );
      matView.setPosition( pos );
      matView.inverse();

      GFX->setWorldMatrix(matView);


      // set new render target / zbuffer , clear buffers
      LPDIRECT3DSURFACE8 pFace;
      mCubeTex->GetCubeMapSurface( (D3DCUBEMAP_FACES)i, 0, &pFace );
      D3DDevice->SetRenderTarget( pFace, mDepthBuff );
      pFace->Release();

      D3DDevice->Clear( 0, NULL, D3DCLEAR_STENCIL | D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );

      // render scene
      // Not for TGB -pw
      //gClientSceneGraph->renderScene( InteriorObjectType );
   }
   // Not for TGB -pw
   //gClientSceneGraph->setReflectPass( false );
   
   // restore render surface and depth buffer
   D3DDevice->SetRenderTarget( NULL, prevDepthSurface ); // MUST Call this before popActiveRenderTarget!!! -pw
   GFX->popActiveRenderTarget();
   prevDepthSurface->Release();
   
   // restore matrices
   GFX->popWorldMatrix();
   GFX->setProjectionMatrix( proj );
}

void GFXD3D8Cubemap::zombify()
{
   // Static cubemaps are handled by D3D
   if( mDynamic )
   {
      releaseSurfaces();
   }
}

void GFXD3D8Cubemap::resurrect()
{
   // Static cubemaps are handled by D3D
   if( mDynamic )
   {
      initDynamic(mTexSize);
   }
}
