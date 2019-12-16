//-----------------------------------------------------------------------------
// PowerEngine
// 
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GFXD3D8SHADER_H_
#define _GFXD3D8SHADER_H_

#include "gfx/gfxShader.h"

//**************************************************************************
// Shader Manager
//**************************************************************************
class GFXD3D8Shader : public GFXShader
{
   //--------------------------------------------------------------
   // Data
   //--------------------------------------------------------------
   IDirect3DDevice8 *mD3DDevice;

   DWORD vertShader;
   DWORD pixShader;


   //--------------------------------------------------------------
   // Procedures
   //--------------------------------------------------------------
   void initVertShader( const char *vertFile, const char *vertTarget );
   void initPixShader( const char *vertFile, const char *vertTarget );

public:
   GFXD3D8Shader();
   ~GFXD3D8Shader();
   
   bool init( const char *vertFile, const char *pixFile, F32 pixVersion );
   virtual void process();

   // GFXResource interface
   virtual void zombify();
   virtual void resurrect();
};

#endif

