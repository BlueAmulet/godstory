//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _RENDER_INST_MGR_H_
#define _RENDER_INST_MGR_H_

#ifndef _GFXDEVICE_H_
#include "gfx/gfxDevice.h"
#endif

#ifndef _SCENEGRAPH_H_
#include "sceneGraph/sceneGraph.h"
#endif

#ifndef _SCENESTATE_H_
#include "sceneGraph/sceneState.h"
#endif

#ifndef _SHADERTDATA_H_
#include "materials/shaderData.h"
#endif


class MatInstance;
struct SceneGraphData;
class ShaderData;
class Sky;
class RenderElemMgr;
class LightInfo;
class RenderableSceneObject;

//**************************************************************************
// Render Instance
//**************************************************************************
struct RenderInst
{
   GFXVertexBufferHandleBase * vertBuff;
   GFXPrimitiveBufferHandle  * primBuff;

   GFXPrimitive *prim;

   U32 primBuffIndex;
   MatInstance * matInst;

   S32 SortedIndex; // Ray: for SortedMesh
   S8 fadeFlag;

   // transforms
   MatrixF *worldXform;     // model/View/projection transform
   MatrixF *objXform;       // model space transform

   SceneState *state;               // need this for straight up object renders
   RenderableSceneObject *obj;                // need this for straight up object renders

   // sorting
   U8 type;                   // sort overrides (draw sky last, etc.)
   Point3F sortPoint;         // just points for now - nothing uses plane

   // misc render states
   bool  translucent;  // this could be passed into addInst()?
   S32   particles;
   U8    transFlags;
   bool  reflective;
   F32   visibility;
   bool   enableBlendColor;
   ColorF maxBlendColor;
   U8  meshDisableZ;
   F32 overrideColorExposure;

   //Ray: 控制特效渲染
   bool needRender;
   RenderInst *pInstNext;

   //流光
   bool enableFluidLight;
   F32 FluidLightX;
   F32 FluidLightY;

   // mesh related
   S32   mountedObjIdx;  // for debug rendering on ShapeBase objects
   U32   texWrapFlags;

   U32   GameRenderStatus;		//Ray: 游戏中用来控制的标识

   // lighting...
   bool *primitiveFirstPass;
   LightInfo* light;
   LightInfo* lightSecondary;
   GFXTextureObject * dynamicLight;
   GFXTextureObject * dynamicLightSecondary;

   F32 fogFactor;

   // textures
   GFXTextureObject *lightmap;
   GFXTextureObject *normLightmap;
   GFXTextureObject *fogTex;
   GFXTextureObject *backBuffTex;
   GFXTextureObject *reflectTex;
   GFXTextureObject *miscTex;
   GFXCubemap   *cubemap;
   GFXTextureObject *vertexTex;
   F32   boneNum;

   // methods
   void clear();
   void calcSortPoint( SceneObject *obj, const Point3F &camPosition );
};


//**************************************************************************
// Render Instance Manager
//**************************************************************************
class RenderInstManager
{
   //-------------------------------------
   // structs / enum
   //-------------------------------------
public:

   enum RenderInstType
   {
      RIT_First = 0,
      RIT_Interior,
      RIT_InteriorDynamicLighting,
	  RIT_MeshField,
      RIT_Mesh,
      RIT_Shadow,
      RIT_Sky,
      RIT_Object,      // terrain, water, etc. objects that do their own rendering
      RIT_ObjectTranslucent,// self rendering, but sorted with RIT_Translucent
      RIT_Water,
      RIT_Foliage,
      RIT_Translucent,
	  RIT_Decal,
      RIT_Begin,
      RIT_NumTypes
   };

private:

   //-------------------------------------
   // data
   //-------------------------------------
   Chunker< RenderInst >      mRIAllocator;
   Chunker< MatrixF >         mXformAllocator;
   Vector< RenderElemMgr * >  mRenderBins;

   // for lighting passes...
   Chunker<bool>         mPrimitiveFirstPassAllocator;

   bool mInitialized;   
   MatInstance *mWarningMat;
   Point3F mCamPos;

   void handleGFXEvent(GFXDevice::GFXDeviceEventType event);
   void initBins();
   void uninitBins();
   void initWarnMat();

   void init();
   void uninit();
public:

   //-------------------------------------
   // main interface
   //-------------------------------------
   RenderInstManager();
   ~RenderInstManager();

   inline RenderInst * allocInst( bool clear = true )
   {
      RenderInst *inst = mRIAllocator.alloc();
      if ( clear )
          inst->clear();
      return inst;
   }

   void doQuery();
   void addQueryInst( RenderInst *inst );
   void addShadowQueryInst( RenderInst *inst );
   void addInst( RenderInst *inst );
   inline MatrixF * allocXform(){ return mXformAllocator.alloc(); }

   // for lighting...
   inline bool *allocPrimitiveFirstPass(){ return mPrimitiveFirstPassAllocator.alloc(); }

   void clear();  // clear instances, matrices
   void sort();
   void render();

   inline void setCamPos( Point3F &camPos ){ mCamPos = camPos; }
   inline Point3F getCamPos() const { return mCamPos; }
   inline MatInstance *getWarningMat() { initWarnMat(); return mWarningMat; }
};

extern RenderInstManager gRenderInstManager;



#endif
