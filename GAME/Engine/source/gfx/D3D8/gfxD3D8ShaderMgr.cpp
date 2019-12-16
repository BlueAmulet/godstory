//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include <d3dx8.h>

#include "gfx/gfxDevice.h"
#include "gfx/D3D8/gfxD3D8ShaderMgr.h"
#include "gfx/D3D8/gfxD3D8Shader.h"

//****************************************************************************
// GFX D3D Shader Manager
//****************************************************************************


//----------------------------------------------------------------------------
// Create Shader - for custom shader requests
//----------------------------------------------------------------------------
GFXShader * GFXD3D8ShaderMgr::createShader( const char *vertFile, 
                                           const char *pixFile, 
                                           F32 pixVersion )
{
   GFXD3D8Shader *newShader = new GFXD3D8Shader;
   newShader->registerResourceWithDevice(GFX);
   newShader->init( vertFile, pixFile, pixVersion );
   
   if( newShader )
   {
      mCustShaders.push_back( newShader );
   }
   
   return newShader;
}

//----------------------------------------------------------------------------
// Get shader
//----------------------------------------------------------------------------
GFXShader * GFXD3D8ShaderMgr::getShader( GFXShaderFeatureData &dat,
                                        GFXVertexFlags flags )
{                   
   AssertISV( false, "Why are we here!!?" );

   U32 idx = dat.codify();

   // return shader if exists
   if( mProcShaders[idx] )
   {
      return mProcShaders[idx];
   }

   // if not, then create it
   char vertFile[256];
   char pixFile[256];
   F32  pixVersion = 0.f;
   
//   gShaderGen.generateShader( dat, vertFile, pixFile, &pixVersion, flags );

   GFXD3D8Shader * shader = new GFXD3D8Shader;
   shader->registerResourceWithDevice(GFX);
   shader->init( vertFile, pixFile, pixVersion );
   mProcShaders[idx] = shader;

   return shader;

}

