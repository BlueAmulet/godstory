//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include "material.h"
#include "console/consoleTypes.h"
#include "core/bitStream.h"
#include SHADER_CONSTANT_INCLUDE_FILE
#include "sceneData.h"
#include "gfx/gfxDevice.h"
#include "gfx/cubemapData.h"
#include "gfx/gfxCubemap.h"
#include "math/mathIO.h"
#include "materialPropertyMap.h"

//--------------------------------------------------------------------------
// getNormalizeCube - static method, returns a normalization cubemap
//--------------------------------------------------------------------------
GFXCubemap * Material::getNormalizeCube()
{
   if(normalizeCube)
      return normalizeCube;
   normalizeCube = GFX->createCubemap();
   normalizeCube->initNormalize(64);
   return normalizeCube;
}


//****************************************************************************
// Material
//****************************************************************************
IMPLEMENT_CONOBJECT(Material);

F32 Material::mDt = 0.0;
U32 Material::mLastTime = 0;
F32 Material::mAccumTime = 0.0;
SimSet *Material::gMatSet = NULL;

static EnumTable::Enums gBlendOpEnums[] =
{
   { Material::None,         "None" },
   { Material::Mul,          "Mul" },
   { Material::Add,          "Add" },
   { Material::AddAlpha,     "AddAlpha" },
   { Material::Sub,          "Sub" },
   { Material::LerpAlpha,    "LerpAlpha" }
};
EnumTable Material::mBlendOpTable( 6, gBlendOpEnums );

static EnumTable::Enums gWaveTypeEnums[] =
{
   { Material::Sin,          "Sin" },
   { Material::Triangle,     "Triangle" },
   { Material::Square,       "Square" },
};
EnumTable Material::mWaveTypeTable( 3, gWaveTypeEnums );

GFXCubemapHandle Material::normalizeCube;

//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------
Material::Material()
{
   for( U32 i=0; i<MAX_STAGES; i++ )
   {
      diffuse[i].set( 1.0, 1.0, 1.0, 1.0 );
      specular[i].set( 1.0, 1.0, 1.0, 1.0 );
      colorMultiply[i].set(0.0f, 0.0f, 0.0f, 0.0f);
      specularPower[i] = 8;
      pixelSpecular[i] = false;
      vertexSpecular[i] = false;
      exposure[i] = 1;
      glow[i] = false;
      emissive[i] = false;
   }

   mType = Base;
   mIsIFL = false;

   noFog = false;
   effectLight = false;
   castsShadow = true;
   breakable = false;
   doubleSided = false;
   enableVertexColor = false;		//Ray: enable vertex color
   attenuateBackFace = false;
   preload = false;

   translucent = false;
   subPassTranslucent = false;
   translucentBlendOp = LerpAlpha;
   translucentZWrite = true;
   translucentZEnable = true;

   enableFilterColor = false;
   filterColor = ColorF(1.0f,1.0f,1.0f);

   alphaTest = true;
   alphaRef = 1;

   planarReflection = false;

   mCubemapData = NULL;
   dynamicCubemap = NULL;
   cubemapName = NULL;

   dMemset( mPath, 0, sizeof(mPath) );

   dMemset( animFlags, 0, sizeof( animFlags ) );
   dMemset( scrollOffset, 0, sizeof( scrollOffset ) );

   dMemset( rotSpeed,   0, sizeof( rotSpeed ) );
   dMemset( rotPivotOffset,  0, sizeof( rotPivotOffset ) );
   dMemset( rotPos,     0, sizeof( rotPos ) );

   dMemset( wavePos,    0, sizeof( wavePos ) );
   dMemset( waveFreq,   0, sizeof( waveFreq ) );
   dMemset( waveAmp,    0, sizeof( waveAmp ) );
   dMemset( waveType,   0, sizeof( waveType ) );

   dMemset( seqFramePerSec,   0, sizeof( seqFramePerSec ) );
   dMemset( seqSegSize,       0, sizeof( seqSegSize ) );

   dMemset( baseTexFilename,  0, sizeof( baseTexFilename ) );
   dMemset( SpecularFilename, 0, sizeof( SpecularFilename ) );
   dMemset( detailFilename,   0, sizeof( detailFilename ) );
   dMemset( bumpFilename,     0, sizeof( bumpFilename ) );
   dMemset( envFilename,      0, sizeof( envFilename ) );

   hasSetStageData = false;
   mLastUpdateTime = 0;
   mapTo = NULL;
}

//--------------------------------------------------------------------------
// Init fields
//--------------------------------------------------------------------------
void Material::initPersistFields()
{
   Parent::initPersistFields();

   addField("diffuse",     TypeColorF, Offset(diffuse,   Material), MAX_STAGES);
   addField("colorMultiply", TypeColorF, Offset(colorMultiply, Material), MAX_STAGES);
   addField("specular",    TypeColorF, Offset(specular,   Material), MAX_STAGES);
   addField("specularPower",  TypeF32, Offset(specularPower,   Material), MAX_STAGES);
   addField("pixelSpecular",  TypeBool, Offset(pixelSpecular,   Material), MAX_STAGES);

   addField("exposure",  TypeS32, Offset(exposure,   Material), MAX_STAGES);

   addField("glow",           TypeBool, Offset(glow,            Material), MAX_STAGES);
   addField("emissive",       TypeBool, Offset(emissive,        Material), MAX_STAGES);

   addField("translucent",          TypeBool, Offset(translucent,          Material));
   addField("translucentBlendOp",   TypeEnum, Offset(translucentBlendOp,   Material), 1, &mBlendOpTable);
   addField("translucentZWrite",    TypeBool, Offset(translucentZWrite,    Material));
   addField("translucentZEnable",    TypeBool, Offset(translucentZEnable,    Material));
   
   addField("alphaTest",    TypeBool, Offset(alphaTest,  Material));
   addField("alphaRef",     TypeS32,  Offset(alphaRef,   Material));

   addField("castsShadow",    TypeBool, Offset(castsShadow,   Material));
   addField("breakable",      TypeBool, Offset(breakable,   Material));
   addField("doubleSided",    TypeBool, Offset(doubleSided,   Material));
   addField("attenuateBackFace",    TypeBool, Offset(attenuateBackFace,   Material));
   addField("preload",        TypeBool, Offset(preload, Material));

   addField("scrollDir",      TypePoint2F,   Offset(scrollDir,   Material),MAX_STAGES);
   addField("scrollSpeed",    TypeF32,       Offset(scrollSpeed,   Material),MAX_STAGES);

   addField("rotSpeed",    TypeF32,       Offset(rotSpeed,  Material),MAX_STAGES);
   addField("rotPivotOffset",   TypePoint2F,   Offset(rotPivotOffset, Material),MAX_STAGES);

   addField("animFlags",   TypeS32,  Offset(animFlags,   Material), MAX_STAGES );

   addField("waveType",    TypeEnum, Offset(waveType,  Material), MAX_STAGES, &mWaveTypeTable );
   addField("waveFreq",    TypeF32, Offset(waveFreq,  Material),MAX_STAGES);
   addField("waveAmp",     TypeF32, Offset(waveAmp,   Material),MAX_STAGES);

   addField("sequenceFramePerSec",  TypeF32, Offset(seqFramePerSec,  Material),MAX_STAGES);
   addField("sequenceSegmentSize",  TypeF32, Offset(seqSegSize,  Material),MAX_STAGES);

   // textures
   addField("baseTex",     TypeFilename, Offset(baseTexFilename, Material),MAX_STAGES);
   addField("specularTex", TypeFilename, Offset(SpecularFilename, Material),MAX_STAGES);
   addField("detailTex",   TypeFilename, Offset(detailFilename, Material),MAX_STAGES);
   addField("bumpTex",     TypeFilename, Offset(bumpFilename, Material),MAX_STAGES);
   addField("envTex",      TypeFilename, Offset(envFilename, Material),MAX_STAGES);

   // cubemap
   addField("cubemap", TypeString, Offset(cubemapName, Material));
   addField("dynamicCubemap", TypeBool, Offset(dynamicCubemap, Material));

   addField("planarReflection", TypeBool, Offset(planarReflection, Material));
   addField("mapTo", TypeString, Offset(mapTo, Material));
}

//--------------------------------------------------------------------------
// On add - verify data settings
//--------------------------------------------------------------------------
bool Material::onAdd()
{
   if (Parent::onAdd() == false)
      return false;

   if( cubemapName )
   {
      mCubemapData = static_cast<CubemapData*>( Sim::findObject( cubemapName ) );
   }

   if( translucentBlendOp >= NumBlendTypes || translucentBlendOp < 0 )
   {
      Con::errorf( "Invalid blend op in material: %s", getName() );
      translucentBlendOp = LerpAlpha;
   }

   SimSet *matSet = getMaterialSet();
   if( matSet )
   {
      matSet->addObject( (SimObject*)this );
   }

   // 当前路径由外部直接赋值,不再从脚本路径读取,修正多线程加载问题 [7/2/2009 LivenHotch]

   if( mPath[0] == 0 )
   {  
	   // save the current script path for texture lookup later
	   const char *curScriptFile = Con::getVariable("Con::File");  // current script file - local materials.cs
	   const char *path = dStrrchr( curScriptFile, '/' );
	   U32 fileStrLen = path - curScriptFile + 1;
	   dStrncpy( mPath, sizeof(mPath), curScriptFile, fileStrLen );
	   mPath[fileStrLen] = '\0';
   }

   mapMaterial();


   return true;
}

//--------------------------------------------------------------------------
// On remove
//--------------------------------------------------------------------------
void Material::onRemove()
{
   SAFE_DELETE( normalizeCube );

   Parent::onRemove();
}

//--------------------------------------------------------------------------
// Update time
//--------------------------------------------------------------------------
void Material::updateTime()
{
   U32 curTime = Platform::getVirtualMilliseconds();
   if(curTime > mLastTime)
   {
      mDt =  (curTime - mLastTime) / 1000.0f;
      mLastTime = curTime;
      mAccumTime += mDt;
   }
}

void Material::updateTimeBasedParams()
{
   if (mLastUpdateTime != mLastTime)
   {
      for (U32 i = 0; i < MAX_STAGES; i++)
      {
         scrollOffset[i] += scrollDir[i] * scrollSpeed[i] * mDt;
         rotPos[i] += rotSpeed[i] * mDt;         
         wavePos[i] += waveFreq[i] * mDt;
      }      
      mLastUpdateTime = mLastTime;
   }
}

//--------------------------------------------------------------------------
// Map this material to the texture specified in the "mapTo" data variable
//--------------------------------------------------------------------------
void Material::mapMaterial()
{
   if( !getName() )
   {
      Con::warnf( "Unnamed Material!  Could not map to: %s", mapTo );
      return;
   }

   if( mapTo && dStrstr( mapTo, ".ifl" ) )
   {
      mIsIFL = true;
   }

   // If mapTo not defined in script, try to use the base texture name instead
   if( !mapTo )
   {
      if( !baseTexFilename[0] )
      {
         Con::warnf( "Could not map material: %s to a texture name, need to specify 'mapTo' or 'baseTex[0]' parameter", getName() );
         return;
      }
      else
      {
         // extract filename from base texture
         mapTo = (char *) dStrrchr( baseTexFilename[0], '/') + 1;
         if( mapTo == (char*) 0x001 )
         {
            // no '/' character, must be no path, just the filename
            mapTo = baseTexFilename[0];
         }
      }
   }

   // add mapping to MaterialPropertyMap
   MaterialPropertyMap* pMap = MaterialPropertyMap::get();
   if( pMap == NULL )
   {
      Con::errorf(ConsoleLogEntry::General, "Error, cannot find the global material map object");
   }
   else
   {
      char *namePtrs[2];
      char matName[128] = { "material: " };

      U32 strLen = dStrlen( matName );
      dStrcpy( &matName[strLen], sizeof(matName) - strLen, getName() );
      namePtrs[0] = (char*) mapTo;
      namePtrs[1] = matName;
      pMap->addMapping( 2, (const char**) namePtrs );
   }
}
//--------------------------------------------------------------------------
// getMaterialSet - returns "MaterialSet" SimSet
//--------------------------------------------------------------------------
SimSet * Material::getMaterialSet()
{
   if( gMatSet )
   {
      return gMatSet;
   }

   gMatSet = static_cast<SimSet*>( Sim::findObject( "MaterialSet" ) );
   AssertFatal( gMatSet, "MaterialSet not found" );

   return gMatSet;
}


