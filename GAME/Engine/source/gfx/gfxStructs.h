//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GFXSTRUCTS_H_
#define _GFXSTRUCTS_H_

#include "gfx/gfxVertexColor.h"
#include "gfx/gfxEnums.h"
#include "math/mMath.h"
#include "platform/profiler.h"
#include "gfx/gfxResource.h"

#ifndef _REFBASE_H_
#include "core/refBase.h"
#endif

struct StateTracker 
{
   StateTracker()
   {
      currentValue = newValue = 0;
      dirty = false;
   }

   U32 currentValue;
   U32 newValue;
   bool dirty;
};

struct TextureDirtyTracker 
{
   TextureDirtyTracker()
   {
      stage = state = 0;
   }

   U32 stage;
   U32 state;
};

//-----------------------------------------------------------------------------
// This class is used to interact with an API's fixed function lights.  See GFX->setLight
class GFXLightInfo 
{
public:
   enum Type {
      Point    = 0,
      Spot     = 1,
      Vector   = 2,
      Ambient  = 3,
   };
   Type        mType;

   Point3F     mPos;
   VectorF     mDirection;
   ColorF      mColor;
   ColorF      mAmbient;
   F32         mRadius;
   F32         sgSpotAngle;

   // TODO: Revisit below (currently unused by fixed function lights)
	Point3F position;
	ColorF ambient;
	ColorF diffuse;
	ColorF specular;
	VectorF spotDirection;
	F32 spotExponent;
	F32 spotCutoff;
	F32 constantAttenuation;
	F32 linearAttenuation;
	F32 quadraticAttenuation;
};

//-----------------------------------------------------------------------------

// Material definition for FF lighting
struct GFXLightMaterial
{
   ColorF ambient;
   ColorF diffuse;
   ColorF specular;
   ColorF emissive;
   F32 shininess;
};

//-----------------------------------------------------------------------------

struct GFXVideoMode 
{
   GFXVideoMode();

   Point2I resolution;
   U32 bitDepth;
   U32 refreshRate;
   bool fullScreen;
   bool fullWindow;		// 窗口最大化模式
   bool wideScreen;
   // When this is returned from GFX, it's the max, otherwise it's the desired AA level.
   U32 antialiasLevel;

   inline bool operator == ( GFXVideoMode &otherMode ) const 
   {
      if( otherMode.fullScreen != fullScreen )
         return false;
      if( otherMode.fullWindow != fullWindow )
         return false;
      if( otherMode.resolution != resolution )
         return false;
      if( otherMode.bitDepth != bitDepth )
         return false;
      if( otherMode.refreshRate != refreshRate )
         return false;
      if( otherMode.wideScreen != wideScreen )
         return false;
      if( otherMode.antialiasLevel != antialiasLevel)
         return false;

      return true;
   }

   /// Fill whatever fields we can from the passed string, which should be 
   /// of form "width height [bitDepth [refreshRate] [antialiasLevel]]" Unspecified fields
   /// aren't modified, so you may want to set defaults before parsing.
   void parseFromString( const char *str );

   /// Gets a string representation of the object as
   /// "resolution.x resolution.y fullScreen bitDepth refreshRate antialiasLevel"
   ///
   /// \return (string) A string representation of the object.
   const char * toString();
};

//-----------------------------------------------------------------------------
// GFXShaderFeatureData
//-----------------------------------------------------------------------------
struct GFXShaderFeatureData
{

   // WARNING - if the number of features here grows past 15 or so, then
   //    ShaderManager should implement a hash table or binary tree for lookup
   // WARNING - if this enum is modified - be sure to also change
   //    FeatureMgr::init() which maps features to this enum!
   enum
   {
      RTLighting = 0,   // realtime lighting
	  VertexColor,
      TexAnim,
      BaseTex,
      ColorMultiply,
      DynamicLight,
      DynamicLightDual,
      DynamicLightMask,
      DynamicLightAttenuateBackFace,
      SelfIllumination,
      LightMap,
      LightNormMap,
      BumpMap,
      DetailMap,
      ExposureX2,
      ExposureX4,
      EnvMap,
      CubeMap,
      // BumpCubeMap,
      // Refraction,
      PixSpecular,
      VertSpecular,      
	  SpecularMap,		// Ray: 高光贴图
      Translucent,      // Not a feature with shader code behind it, but needed for pixSpecular.
      Visibility,
	  EffectLight,		//Ray: 特效光照
      ColorExposure,    // 颜色倍乘
      Fog,              // keep fog last feature      
      NumFeatures,
   };
   
   // lighting info - this will probably get a lot more complex
   bool useLightDir; // use light direction instead of position

   // General feature data for a pass or for other purposes.
   bool features[NumFeatures];
   
   // This is to give hints to shader creation code.  It contains
   //    all the features that are in a material stage instead of just
   //    the current pass.
   bool materialFeatures[NumFeatures];

public:

   GFXShaderFeatureData();
   U32 codify();

};

struct GFXStreamVertexAttribute {
	int stream;             /**< Stream mapping */
	GFXAttribType type;        /**< Specifies the vertex type. */
	GFXAttribFormat format;    /**< Specifies the vertex format (float, bool, int, etc). */
	int size;               /**< Specifies the vertex format size (3 for <type>3, for example, where type is float and size is 3, the attribute is a float3). */
};

//-----------------------------------------------------------------------------
// GFX vertices
//-----------------------------------------------------------------------------
template<class T> inline U32 getGFXVertFlags(T *vertexPtr) { return 0; }

#ifdef POWER_OS_XENON
// For the Xbox360, we want to be sure that verts are on aligned boundaries. 
#define DEFINE_VERT(name,flags) struct name; \
   template<> inline U32 getGFXVertFlags(name *ptr) { return flags; }\
   __declspec(align(16)) struct name 

#else
#define DEFINE_VERT(name,flags) struct name; \
            template<> inline U32 getGFXVertFlags(name *ptr) { return flags; }\
            struct name 

#define DECL_VERT(name) \
	const GFXStreamVertexAttribute name []=
#endif

#define ELEMENTOF(x) (sizeof(x) / sizeof(x[0])) 

// Vertex types

// At some point I'd like HLSL and this to include the same file, and use some
// kind of macro magic to create C++ structs and vertex declarations, and HLSL
// structs as well.
#include "gfx/gfxVertexTypes.h"

//-----------------------------------------------------------------------------

struct GFXPrimitive
{
   GFXPrimitiveType type;

   U32 minIndex;       /// minimal value we will see in the indices
   U32 startIndex;     /// start of indices in buffer
   U32 numPrimitives;  /// how many prims to render
   U32 numVertices;    /// how many vertices... (used for locking, we lock from minIndex to minIndex + numVertices)

   GFXPrimitive()
   {
      dMemset( this, 0, sizeof( GFXPrimitive ) );
   }
};

/// Helper class to deal with OS specific debug functionality.
class GFXDebugMarker
{
private:
   ColorF mColor;
   const char *mName;

   bool mActive;

public:
   GFXDebugMarker(ColorF color, const char *name) 
      : mColor(color), mName(name)
   {
      //
      mActive = false;
   }

   /// We automatically mark the marker as exited if we leave scope.
   ~GFXDebugMarker()
   {
      if(mActive)
         leave();
   }

   void enter();
   void leave();
};

#endif
