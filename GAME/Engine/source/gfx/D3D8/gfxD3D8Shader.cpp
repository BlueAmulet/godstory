//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include <d3dx8.h>
#include "gfx/D3D8/gfxD3D8Shader.h"
#include "platform/platform.h"
#include "console/console.h"
#include "gfx/D3D8/gfxD3D8Device.h"

//****************************************************************************
// GFX D3D Shader
//****************************************************************************
//#define PRINT_SHADER_WARNINGS

//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------
GFXD3D8Shader::GFXD3D8Shader()
{
   mD3DDevice = dynamic_cast<GFXD3D8Device *>(GFX)->getDevice();
   vertShader = NULL;
   pixShader = NULL;
}

//----------------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------------
GFXD3D8Shader::~GFXD3D8Shader()
{
   if( vertShader )
   {
      mD3DDevice->DeleteVertexShader( vertShader );
      vertShader = NULL;
   }
   if( pixShader )
   {
      mD3DDevice->DeletePixelShader( pixShader );
      pixShader = NULL;
   }
}

//----------------------------------------------------------------------------
// Init
//----------------------------------------------------------------------------
bool GFXD3D8Shader::init( const char *vertFile, const char *pixFile, F32 pixVersion  )
{
   
   if( pixVersion > GFX->getPixelShaderVersion() )
   {
      return false;
   }

   if( pixVersion < 1.0 && pixFile && pixFile[0] )
   {
      return false;
   }


   char vertTarget[32];
   char pixTarget[32];

   U32 mjVer = (U32)floor( pixVersion );
   U32 mnVer = (U32)( (pixVersion - F32(mjVer)) * 10.01 ); // 10.01 instead of 10.0 because of floating point issues

   dSprintf( vertTarget, sizeof(vertTarget), "vs_%d_%d", mjVer, mnVer );
   dSprintf( pixTarget, sizeof(pixTarget), "ps_%d_%d", mjVer, mnVer );

   if( (pixVersion < 2.0) && (pixVersion > 1.101) )
   {
      dStrcpy( vertTarget, sizeof(vertTarget), "vs_1_1" );
   }
 
   initVertShader( vertFile, vertTarget );
   initPixShader( pixFile, pixTarget );
   
   return true;
}

//--------------------------------------------------------------------------
// Init vertex shader
//--------------------------------------------------------------------------
void GFXD3D8Shader::initVertShader( const char *vertFile, const char *vertTarget )
{
   // CodeReview: There is no shader compiling support in D3D8, AFAIK.
   // we will need to either compile with D3D9, if it is available, or
   // ship with compiled shaders. [5/10/2007 Pat]
}


//--------------------------------------------------------------------------
// Init pixel shader
//--------------------------------------------------------------------------
void GFXD3D8Shader::initPixShader( const char *pixFile, const char *pixTarget )
{
   // CodeReview: There is no shader compiling support in D3D8, AFAIK.
   // we will need to either compile with D3D9, if it is available, or
   // ship with compiled shaders. [5/10/2007 Pat]
}

//--------------------------------------------------------------------------
// Process
//--------------------------------------------------------------------------
void GFXD3D8Shader::process()
{
   GFX->setShader( this );
}

void GFXD3D8Shader::zombify()
{
   // Shaders don't need zombification
}

void GFXD3D8Shader::resurrect()
{
   // Shaders are never zombies, and therefore don't have to be brought back
}
