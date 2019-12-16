//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include "math/mMath.h"
#include "shaderGen.h"
#include "pixSpecular.h"
#include "bump.h"
#include "featureMgr.h"
#include "gfx/gfxShader.h"
#include "core/filestream.h"
#include "gfx/gfxDevice.h"
#include "shaderGen/manager/shaderGenManager.h"
#include "gfx/gfxCardProfile.h"
#include "core/memstream.h"
#include "zlib/zlib.h"
#define GEN_NEW_SHADERS

//****************************************************************************
// ShaderGen
//****************************************************************************
ShaderGen gShaderGen;


//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------
ShaderGen::ShaderGen()
{
}

//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------
ShaderGen::~ShaderGen()
{
	for (S32 i = 0; i < mComponents.size(); i++)
	{
		if (mComponents[i])
		{
			delete mComponents[i];
			mComponents[i] = NULL;
		}
	}
	mComponents.clear();
}

//----------------------------------------------------------------------------
// Generate shader
//----------------------------------------------------------------------------
void ShaderGen::generateShader( const GFXShaderFeatureData &featureData,
                                char *vertFile, int vertFileSize,
                                char *pixFile, int pixFileSize,
                                F32 *pixVersion,
                                GFXVertexFlags flags )
{
   mFeatureData = featureData;
   mVertFlags = flags;

   AssertFatal(mVertFlags & GFXVertexFlagXYZ, "Vertex has no position data!");

   //uninit();
   //init();

   char vertShaderName[256];
   char pixShaderName[256];
   static U32 shaderNum = 0;

   const char* shaderPath = Con::getVariable( "$Pref::Video::ShaderPath" );
   if( dStrlen(shaderPath) == 0 )
   {
      shaderPath = "temp";//"shaders"
   }
	char buff[512];
	//为防止多开程序时，导致同时写入同一文件而引起的崩溃问题，添加随机文件前缀
	dSprintf(buff, sizeof(buff), "tgb-%08x-%02x-tmp", Platform::getRealMilliseconds(), U32(Platform::getRandom() * 255));
   dSprintf( vertShaderName, sizeof(vertShaderName), "%s/%sfire%03d.gmp", shaderPath, buff, shaderNum );//shaderV
   dSprintf( pixShaderName, sizeof(pixShaderName), "%s/%srain%03d.gmp", shaderPath, buff, shaderNum );//shaderP
   shaderNum++;


#ifdef GEN_NEW_SHADERS
	uninit();
	init();
#ifdef SHADERGEN_COMPRESS//压缩shader文件
   // create vertex shader
   //------------------------
	MemStream *s = dynamic_cast<MemStream*>(ShaderGenManager::openNewShaderStream( vertShaderName, true ));
	AssertFatal( s != NULL, avar("Failed to open Shader MemStream: %s. If the file exists you may need to run PowerEngine with Administrator privileges enabled.", vertShaderName) );

	processVertFeatures();
	printVertShader( *s );

	unsigned char buf[DEFAULT_BUFFER_SIZE];
	unsigned long size = DEFAULT_BUFFER_SIZE;
	int ret = compress(buf, &size, (unsigned char*)s->getBuffer(), s->getPosition());
	ShaderGenManager::closeNewShaderStream( s );
	Stream *os = ShaderGenManager::openNewShaderStream(vertShaderName);//output stream
	AssertFatal( os != NULL, avar("Failed to open Shader Stream: %s. If the file exists you may need to run PowerEngine with Administrator privileges enabled.", vertShaderName) );
	os->write(size, buf);
	ShaderGenManager::closeNewShaderStream( os );
	((ConnectorStruct*)mComponents[C_CONNECTOR])->reset();
	LangElement::deleteElements();
	// create pixel shader
	//------------------------
	//s = ShaderGenManager::openNewShaderStream( pixShaderName );
	s = dynamic_cast<MemStream*>(ShaderGenManager::openNewShaderStream( pixShaderName, true ));
	AssertFatal( s != NULL, avar("Failed to open Shader Stream %s. If the file exists you may need to run PowerEngine with Administrator privileges enabled.", vertShaderName) );

	processPixFeatures();
	printPixShader( *s );
	ret = compress(buf, &size, (unsigned char*)s->getBuffer(), s->getPosition());
	ShaderGenManager::closeNewShaderStream( s );

	os = ShaderGenManager::openNewShaderStream(pixShaderName);
	AssertFatal( os != NULL, avar("Failed to open Shader Stream: %s. If the file exists you may need to run PowerEngine with Administrator privileges enabled.", vertShaderName) );
	os->write(size, buf);
	ShaderGenManager::closeNewShaderStream( os );
	LangElement::deleteElements();
#else//不压缩shader文件
	// create vertex shader
	//------------------------
	Stream *s = ShaderGenManager::openNewShaderStream( vertShaderName );
	AssertFatal( s != NULL, avar("Failed to open Shader Stream: %s. If the file exists you may need to run PowerEngine with Administrator privileges enabled.", vertShaderName) );
	processVertFeatures();
	printVertShader( *s );
	ShaderGenManager::closeNewShaderStream( s );
	((ConnectorStruct*)mComponents[C_CONNECTOR])->reset();
	LangElement::deleteElements();
	// create pixel shader
	//------------------------
	s = ShaderGenManager::openNewShaderStream( pixShaderName );
	AssertFatal( s != NULL, avar("Failed to open Shader Stream %s. If the file exists you may need to run PowerEngine with Administrator privileges enabled.", vertShaderName) );
	processPixFeatures();
	printPixShader( *s );
	ShaderGenManager::closeNewShaderStream( s );
	LangElement::deleteElements();
#endif

#endif

   dStrcpy( vertFile, vertFileSize, vertShaderName );
   dStrcpy( pixFile, pixFileSize, pixShaderName );   
   
   // this needs to change - need to optimize down to ps v.1.1
   *pixVersion = GFX->getPixelShaderVersion();

}

//----------------------------------------------------------------------------
// Init
//----------------------------------------------------------------------------
void ShaderGen::init()
{
   createComponents();
   initVertexDef();

}

//----------------------------------------------------------------------------
// Uninit
//----------------------------------------------------------------------------
void ShaderGen::uninit()
{
   for( U32 i=0; i<mComponents.size(); i++ )
   {
      delete mComponents[i];
      mComponents[i] = NULL;
   }
   mComponents.setSize(0);

   LangElement::deleteElements();

   Var::reset();
}


//----------------------------------------------------------------------------
// Create shader components
//----------------------------------------------------------------------------
void ShaderGen::createComponents()
{

   // Make sure this order matches the "Components" enum!!
   ConnectorStruct *vertComp = new ConnectorStruct;
   mComponents.push_back( vertComp );
   vertComp->setName( "VertData" );



   // Set up connector structure
   mComponents.push_back( new ConnectorStruct );
   ((ConnectorStruct*)mComponents[1])->setName( "ConnectData" );

   mComponents.push_back( new VertexMainDef );
   mComponents.push_back( new PixelMainDef );
}

//----------------------------------------------------------------------------
// Allocate vertex variables
//----------------------------------------------------------------------------
void ShaderGen::initVertexDef()
{
   // Grab tex coord 0 as default for base texture - also used
   //    for bumpmap and detail map coords
   ConnectorStruct *vertComp = dynamic_cast<ConnectorStruct *>( mComponents[C_VERT_STRUCT] );
   
   Var *inTex = vertComp->getElement( RT_TEXCOORD );
   inTex->setName( "texCoord" );
   inTex->setStructName( "IN" );
   inTex->setType( "float2" );
   
   // set up lightmap coords
   Var *lmCoord = vertComp->getElement( RT_TEXCOORD );
   lmCoord->setName( "lmCoord" );
   lmCoord->setStructName( "IN" );
   lmCoord->setType( "float2" );

   if(GFX->getCardProfiler()->queryProfile("allowRGBA32FBitVertexTextures", false))
   {
	   Var *inTex0= vertComp->getElement( RT_TEXCOORD );
	   inTex0->setType( "float2" );
	   inTex0->setName( "inTex0" );
	   inTex0->setStructName( "IN" );
	   Var *inTex1= vertComp->getElement( RT_TEXCOORD );
	   inTex1->setType( "float2" );
	   inTex1->setName( "inTex1" );
	   inTex1->setStructName( "IN" );
	   Var *inTex2= vertComp->getElement( RT_TEXCOORD );
	   inTex2->setType( "float2" );
	   inTex2->setName( "inTex2" );
	   inTex2->setStructName( "IN" );
	   Var *inTex3= vertComp->getElement( RT_TEXCOORD );
	   inTex3->setType( "float2" );
	   inTex3->setName( "inTex3" );
	   inTex3->setStructName( "IN" );
   }

   // grab incoming tex space matrix
   Var *T = vertComp->getElement( RT_TEXCOORD );
   T->setName( "T4" );
   T->setStructName( "IN" );
   T->setType( "float4" );

   //Ray: 目前B是在引擎中求得的，如果ind物件需要填充B，还需要单独修改。 下面的N不知道对应那个值

   // grab incoming tex space matrix
   Var *N = vertComp->getElement( RT_TEXCOORD );
   N->setName( "N" );
   N->setStructName( "IN" );
   N->setType( "float3" );
   
   // handle normal
   if( mVertFlags & GFXVertexFlagNormal )
   {
      Var *inNormal = vertComp->getElement( RT_NORMAL );
      inNormal->setName( "org_normal" );
      inNormal->setStructName( "IN" );
      inNormal->setType( "float3" );
   }

   //Ray: handle vertex color
   if( mVertFlags & GFXVertexFlagDiffuse )
   {
	   Var *inVColor = vertComp->getElement( RT_COLOR );
	   inVColor->setName( "vcolor" );
	   inVColor->setStructName( "IN" );
	   inVColor->setType( "float4" );
   }

}

//----------------------------------------------------------------------------
// Process features
//----------------------------------------------------------------------------
void ShaderGen::processVertFeatures()
{
   // process auxiliary features
   for( U32 i=0; i<FeatureMgr::NumAuxFeatures; i++ )
   {
      gFeatureMgr.getAux(i)->processVert( mComponents, mFeatureData );
   }

   // process the other features
   for( U32 i=0; i<GFXShaderFeatureData::NumFeatures; i++ )
   {
      if( mFeatureData.features[i] )
      {
         gFeatureMgr.get(i)->processVert( mComponents, mFeatureData );
      }
   }

   ConnectorStruct *connect = dynamic_cast<ConnectorStruct *>( mComponents[C_CONNECTOR] );
   connect->sortVars();
   Var::reset();
}

//----------------------------------------------------------------------------
// Process features
//----------------------------------------------------------------------------
void ShaderGen::processPixFeatures()
{
   // process auxiliary features
   for( U32 i=0; i<FeatureMgr::NumAuxFeatures; i++ )
   {
      gFeatureMgr.getAux(i)->processPix( mComponents, mFeatureData );
   }
   
   // process the other features
   for( U32 i=0; i<GFXShaderFeatureData::NumFeatures; i++ )
   {
      if( mFeatureData.features[i] )
      {
         gFeatureMgr.get(i)->processPix( mComponents, mFeatureData );
      }
   }
   
   ConnectorStruct *connect = dynamic_cast<ConnectorStruct *>( mComponents[C_CONNECTOR] );
   connect->sortVars();
}

//----------------------------------------------------------------------------
// Print vertex OR pixel shader features
//----------------------------------------------------------------------------
void ShaderGen::printFeatures( Stream &stream )
{
   // process auxiliary features
   for( U32 i=0; i<FeatureMgr::NumAuxFeatures; i++ )
   {
      LangElement *le = gFeatureMgr.getAux(i)->getOutput();
      if( le )
      {
         le->print( stream );
      }
   }

   // process the other features
   for( U32 i=0; i<GFXShaderFeatureData::NumFeatures; i++ )
   {
      if( mFeatureData.features[i] )
      {
         LangElement *le = gFeatureMgr.get(i)->getOutput();
         if( le )
         {
            le->print( stream );
         }

      }
   }
}

//----------------------------------------------------------------------------
// Print header
//----------------------------------------------------------------------------
void ShaderGen::printShaderHeader( Stream &stream )
{

   const char *header1 = "//*****************************************************************************\r\n";
   const char *header2 = "// PowerEngine -- HLSL procedural shader                                              \r\n";
   
   stream.write( dStrlen(header1), header1 );
   stream.write( dStrlen(header2), header2 );
   stream.write( dStrlen(header1), header1 );


   const char * header3 = "//-----------------------------------------------------------------------------\r\n";
   const char * header4 = "// Structures                                                                  \r\n";

   stream.write( dStrlen(header3), header3 );
   stream.write( dStrlen(header4), header4 );
   stream.write( dStrlen(header3), header3 );
}

//----------------------------------------------------------------------------
// Print components to stream
//----------------------------------------------------------------------------
void ShaderGen::printVertShader( Stream &stream )
{
   printShaderHeader( stream );

   // print out structures
   mComponents[C_VERT_STRUCT]->print( stream );
   mComponents[C_CONNECTOR]->print( stream );


   // Print out main function definition
   const char * header5 = "// Main                                                                        \r\n";
   const char * line    = "//-----------------------------------------------------------------------------\r\n";

   stream.write( dStrlen(line), line );
   stream.write( dStrlen(header5), header5 );
   stream.write( dStrlen(line), line );

   mComponents[C_VERT_MAIN]->print( stream );


   // print out the function
   printFeatures( stream );

   const char *closer = "   return OUT;\r\n}\r\n";
   stream.write( dStrlen(closer), closer );

}

//----------------------------------------------------------------------------
// Print components to stream
//----------------------------------------------------------------------------
void ShaderGen::printPixShader( Stream &stream )
{
   printShaderHeader( stream );

   mComponents[C_CONNECTOR]->print( stream );

   WRITESTR( "struct Fragout\r\n" );
   WRITESTR( "{\r\n" );
   WRITESTR( "   float4 col : COLOR0;\r\n" );
   WRITESTR( "};\r\n" );
   WRITESTR( "\r\n" );
   WRITESTR( "\r\n" );


   // Print out main function definition
   const char * header5 = "// Main                                                                        \r\n";
   const char * line    = "//-----------------------------------------------------------------------------\r\n";

   WRITESTR( line );
   WRITESTR( header5 );
   WRITESTR( line );

   mComponents[C_PIX_MAIN]->print( stream );

   // print out the function
   printFeatures( stream );

   
   WRITESTR( "\r\n   return OUT;\r\n}\r\n" );

}
