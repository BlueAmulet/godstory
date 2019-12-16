//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _TSMESH_H_
#define _TSMESH_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif
#ifndef _STREAM_H_
#include "core/stream.h"
#endif
#ifndef _MMATH_H_
#include "math/mMath.h"
#endif
#ifndef _TVECTOR_H_
#include "core/tVector.h"
#endif
#ifndef _ABSTRACTPOLYLIST_H_
#include "collision/abstractPolyList.h"
#endif

#ifndef _GFXDEVICE_H_
#include "gfx/gfxDevice.h"
#endif

#ifndef _RENDERABLESCENEOBJECT_H
#include "sceneGraph/renderableSceneObject.h"
#endif

struct shadowParam
{
	Point3F p0;
	Point3F p1;
	Point3F p2;

	Point3F N;
	float	D;

	float x1,x2;
	float y1,y2;

	float tx0,ty0,u0,v0;
	float dpdx;
	float dpdy;
	float dqdx;
	float dqdy;

	U8 type;
};

namespace Opcode
{
   class Model;
}
class Convex;

class TSMeshLightPlugin;
class SceneState;
class SceneObject;

struct RenderInst;

// when working with 3dsmax, we want some things to be vectors that otherwise
// are pointers to non-resizeable blocks of memory
#if defined(POWER_MAX_LIB)
#define ToolVector Vector
#else
template<class A> class ToolVector
{
   public:
      A * addr;
      U32 sz;
      U32 size() const { return sz; }
      bool empty() const { return sz==0; }
      A & operator[](U32 idx) { return addr[idx]; }
      A const & operator[](U32 idx) const { return addr[idx]; }
      A * address() { return addr; }
      void set(void * _addr, U32 _sz) { addr = (A*)_addr; sz = _sz; }
};
#endif

class TSMaterialList;
class TSShapeInstance;
struct RayInfo;
class ConvexFeature;
class ShapeBase;

struct TSDrawPrimitive
{
   enum
   {
      Triangles    = 0 << 30, ///< bits 30 and 31 index element type
      Strip        = 1 << 30, ///< bits 30 and 31 index element type
      Fan          = 2 << 30, ///< bits 30 and 31 index element type
      Indexed      = BIT(29), ///< use glDrawElements if indexed, glDrawArrays o.w.
      NoMaterial   = BIT(28), ///< set if no material (i.e., texture missing)
      MaterialMask = ~(Strip|Fan|Triangles|Indexed|NoMaterial),
      TypeMask     = Strip|Fan|Triangles
   };

   S16 start;
   S16 numElements;
   S32 matIndex;    ///< holds material index & element type (see above enum)
};

#define MAX_BONE_PER_VERTEX 4
#define MAX_VERTEX_TEXTURE_SIZE 1000

struct TSVertBoneIndices
{
	Vector<S32> boneIndices;
	Vector<F32> boneWeight;
};

struct SkinParam;
struct FrameParam;

class TSMesh
{
   friend class TSShape;

protected:
   U32 meshType;
   Box3F mBounds;
   Point3F mCenter;
   F32 mRadius;
   F32 mVisibility;
   bool mDynamic;

   static U32 GameRenderStatus;
   static F32 overrideFadeVal;
   static F32 overrideColorExposure;
   static F32 fogFactor;
   static MatrixF smCamTrans;
   static SceneState * smSceneState;
   static SceneObject * smObject;
   static TSShapeInstance *smShapeIns;
   static GFXCubemap * smCubemap;
   static GFXTextureObject *smLightmap; //Ray:添加lightmap
   static bool smGlowPass;
   static bool smRefractPass;
   static bool smFieldMesh;

   //流光
   static bool smFluidLight; 
   static F32 smFluidLightX;
   static F32 smFluidLightY;

   GFXVertexBufferHandleBase *mpVB;
   GFXPrimitiveBufferHandle mPB;

  public:

   enum
   {
      /// types...
      StandardMeshType = 0,
      SkinMeshType     = 1,
      DecalMeshType    = 2,
      SortedMeshType   = 3,
      NullMeshType     = 4,
      TypeMask = StandardMeshType|SkinMeshType|DecalMeshType|SortedMeshType|NullMeshType,

      /// flags (stored with meshType)...
      Billboard = BIT(31), HasDetailTexture = BIT(30),
      BillboardZAxis = BIT(29), UseEncodedNormals = BIT(28),
      FlagMask = Billboard|BillboardZAxis|HasDetailTexture|UseEncodedNormals
   };

   inline U32 getMeshType() { return meshType & TypeMask; }
   inline void setFlags(U32 flag) { meshType |= flag; }
   inline void clearFlags(U32 flag) { meshType &= ~flag; }
   inline U32 getFlags(U32 flag = 0xFFFFFFFF) { return meshType & flag; }

   S8 disableZ;  //Ray: 指定忽略Z剔除
   S32 sortOrder;  //Ray: 指定排序
   S32 parentMesh; ///< index into shapes mesh list
   S32 numFrames;
   S32 numMatFrames;
   S32 vertsPerFrame;

   ToolVector<Point3F> verts;
   ToolVector<ColorI>  vcol;	//Ray: export vertex color
   ToolVector<Point3F> norms;
   ToolVector<Point2F> tverts;
   ToolVector<TSDrawPrimitive> primitives;
   ToolVector<U8> encodedNorms;
   ToolVector<U16> indices;

   Vector<Point3F> initialTangents;
   Vector<Point4F> tangents;

   TSVertBoneIndices vertBoneIndices[MAX_BONE_PER_VERTEX];

   /// billboard data
   Point3F billboardAxis;

   //Box3F sortBox;

   /// @name Convex Hull Data
   /// Convex hulls are convex (no angles >= 180? meshes used for collision
   /// @{

   Vector<Point3F> planeNormals;
   Vector<F32>     planeConstants;
   Vector<U32>     planeMaterials;
   S32 planesPerFrame;
   U32 mergeBufferStart;
   /// @}

   bool initShadowTemp;
   Vector< Vector<shadowParam> > shadowTemp;
   void calculateShadowTemp();
   bool fillData(shadowParam &param,const Point3F &p0,const Point3F &p1,const Point3F &p2, const Point2F &t0,const Point2F &t1,const Point2F &t2);

   /// @name Render Methods
   /// @{

   virtual void render(TSMaterialList * materials = NULL);
   virtual void render(S32 frame, S32 matFrame, TSMaterialList *);
   /// @}
   virtual void setVertTexParam(RenderInst *){};


   /// @name Scene state methods
   /// @{
   inline static void setCamTrans( const MatrixF &trans ){ smCamTrans = trans; }
   inline static void setSceneState( SceneState *state ){ smSceneState = state; }
   inline static void setCubemap( GFXCubemap *cm ){ smCubemap = cm; }
   inline static void setLightmap( GFXTextureObject *lm ){ smLightmap = lm; }
   inline static void setShapeIns( TSShapeInstance *shape ){ smShapeIns = shape; }
   inline static void setObject( SceneObject *obj ){ smObject = obj; if(smObject) smObject->onBeginRender();}
   inline static void setGlow( bool glow ){ smGlowPass = glow; }
   inline static void setRefract( bool refract ){ smRefractPass = refract; }
   inline static void setFieldMesh( bool flag ){ smFieldMesh = flag; }
   

   inline static const MatrixF& getCamTrans() { return smCamTrans; }
   /// @}

   /// @name Material Methods
   /// @{
   inline static void setMaterial(S32 matIndex, TSMaterialList *);
   inline void setFade(F32 fade) { mVisibility = fade; }
   inline void clearFade() { setFade(1.0f); }
   inline static void setOverrideFade(F32 fadeValue){ overrideFadeVal = fadeValue; }
   inline static F32  getOverrideFade(){ return overrideFadeVal; }

   inline static void setOverrideColorExposure(F32 colorExposure ){ overrideColorExposure = colorExposure; }
   inline static F32  getOverrideColorExposure(){ return overrideColorExposure; }

   inline static void setFogFactor(F32 fogfactor){ fogFactor = fogfactor; }
   inline static F32  getFogFactor(){ return fogFactor; }

   inline static void setFluidLightParam(bool flag/*,F32 fluidLightX,F32 fluidLightY*/){ smFluidLight = flag; /*smFluidLightX = fluidLightX;smFluidLight = fluidLightY;*/}
   inline static void getFluidLightParam(bool &flag,F32 &fluidLightX,F32 &fluidLightY){ flag = smFluidLight; fluidLightX=smFluidLightX; fluidLightY=smFluidLightY;}

   inline static void setGameRenderStatus(U32 Value){ GameRenderStatus = Value; }
   inline static U32  getGameRenderStatus(){ return GameRenderStatus; }

   /// @}

   /// @name Lighting plugin methods
   /// @{
protected:
   static TSMeshLightPlugin* smLightPlugin;
public:
   static void registerLightPlugin(TSMeshLightPlugin* mlp);
   static void unregisterLightPlugin(TSMeshLightPlugin* mlp);
   /// @}

   /// @name Collision Methods
   /// @{

   bool LineTrianglesCollision(const Point3F & start, const Point3F & end,const shadowParam &param,GFXTexHandle &tex,GFXLockedRect *&pRect,RayInfo * rayInfo);
   //bool LineTrianglesCollision(const Point3F & start, const Point3F & end,const Point3F &v0,const Point3F &v1,const Point3F &v2, const Point2F &t0,const Point2F &t1,const Point2F &t2,
   //	   GFXTexHandle &tex,GFXLockedRect *&pRect,RayInfo * rayInfo);

   virtual bool buildPolyList(S32 frame, AbstractPolyList * polyList, U32 & surfaceKey);
   virtual bool getFeatures(S32 frame, const MatrixF&, const VectorF&, ConvexFeature*, U32& surfaceKey);
   virtual void support(S32 frame, const Point3F& v, F32* currMaxDP, Point3F* currSupport);
   virtual bool castShadow(S32 frame, const Point3F & start, const Point3F & end, TSMaterialList*,RayInfo * rayInfo);
   virtual bool castRay(S32 frame, const Point3F & start, const Point3F & end, RayInfo * rayInfo);
   virtual bool buildConvexHull(); ///< returns false if not convex (still builds planes)
   bool addToHull(U32 idx0, U32 idx1, U32 idx2);
   /// @}

   /// @name Bounding Methods
   /// calculate and get bounding information
   /// @{

   void computeBounds();
   virtual void computeBounds(MatrixF & transform, Box3F & bounds, S32 frame = 0, Point3F * center = NULL, F32 * radius = NULL);
   void computeBounds(Point3F *, S32 numVerts, MatrixF & transform, Box3F & bounds, Point3F * center, F32 * radius);
   Box3F & getBounds() { return mBounds; }
   Point3F & getCenter() { return mCenter; }
   F32 getRadius() { return mRadius; }
   virtual S32 getNumPolys();

   U8 encodeNormal(const Point3F & normal);
   const Point3F & decodeNormal(U8 ncode);
   /// @}

   void UpdateFrame(S32 frame, S32 matFrame);
   void doUpdateFrame(FrameParam *pParam);
   static void frameCallback(void* param);

   /// persist methods...
   virtual void assemble(bool skip);
   static TSMesh * assembleMesh(U32 meshType, bool skip);
   virtual void disassemble();

   // this function allows subclasses to override where there vertex and primitive buffers come from.
   // note that this function returns a reference, allowing the caller to modify the buffers.
   virtual GFXTextureObject *getRVertexTexture(void *);
   virtual GFXTextureObject *getCVertexTexture(void *);
   virtual void setCVertexTexture(U32 , U32 , GFXFormat , GFXTextureProfile *){};
   virtual void switchVertexTexture(void *){};
   virtual GFXVertexBufferHandleBase *getVertexBuffer();
   virtual void setVertexBuffer(GFXVertexBufferHandleBase *pVB);
   template<class T>   bool setVertexData(T *&,S32,S32,U32,ToolVector<Point3F> *,ToolVector<Point3F> *) {return false;  }
   //--------------------------------- Ray: support vertex color
   template<> bool setVertexData(GFXVertexBufferHandle<GFXVertexPNTTT>* &pVB,S32 frame,S32 matFrame,U32 numVerts,ToolVector<Point3F> *pVt,ToolVector<Point3F> *pNm) 
   {
	   if(!pVB)
	   {
		   pVB = (GFXVertexBufferHandle<GFXVertexPNTTT>*)getVertexBuffer();
		   if(!pVB)
		   {
			   pVB = new GFXVertexBufferHandle<GFXVertexPNTTT>;
			   setVertexBuffer(pVB);
		   }
	   }

	   GFXVertexBufferHandle<GFXVertexPNTTT> &vb = *pVB;

	   // Number of verts can change in LOD skinned mesh
	   bool vertsChanged = ( vb && vb->mNumVerts < numVerts );

	   if( vertsChanged || vb == NULL )
	   {
		   vb.set(GFX, numVerts, (mDynamic)  ? GFXBufferTypeDynamic : GFXBufferTypeStatic);
	   }

	   const S32 firstVert  = vertsPerFrame * frame;
	   const S32 firstTVert = vertsPerFrame * matFrame;

	   GFXVertexPNTTT *vbVerts = vb.lock();
	   GFXVertexPNTTT *pEnd = vbVerts+vertsPerFrame;

	   Point3F *pVerts = &(*pVt)[firstVert];
	   Point3F *pNorms = &(*pNm)[firstVert];
	   Point2F *pTVerts = &tverts[firstTVert];
	   Point4F *pTangents = &tangents[firstVert];

	   while(vbVerts!=pEnd)
	   {
		   vbVerts->point = *pVerts;
		   ++pVerts;

		   vbVerts->normal = *pNorms;        // gets dereferenced 3 times
		   ++pNorms;

		   vbVerts->texCoord = *pTVerts;
		   vbVerts->texCoord2 = *pTVerts;         // need to write this or write will be slower
		   ++pTVerts;

		   vbVerts->T = *pTangents;
		   ++pTangents;

		   vbVerts++;
	   }

	   vb.unlock();
	   return vertsChanged;
   }

   template<> bool setVertexData(GFXVertexBufferHandle<GFXVertexPCNTTT>* &pVB,S32 frame,S32 matFrame,U32 numVerts,ToolVector<Point3F> *pVt,ToolVector<Point3F> *pNm) 
   {
	   if(!pVB)
	   {
		   pVB = (GFXVertexBufferHandle<GFXVertexPCNTTT>*)getVertexBuffer();
		   if(!pVB)
		   {
			   pVB = new GFXVertexBufferHandle<GFXVertexPCNTTT>;
			   setVertexBuffer(pVB);
		   }
	   }

	   GFXVertexBufferHandle<GFXVertexPCNTTT> &vb = *pVB;

	   // Number of verts can change in LOD skinned mesh
	   bool vertsChanged = ( vb && vb->mNumVerts < numVerts );

	   if( vertsChanged || vb == NULL )
	   {
		   vb.set(GFX, numVerts, (mDynamic) ? GFXBufferTypeDynamic : GFXBufferTypeStatic);
	   }

	   S32 firstVert  = vertsPerFrame * frame;
	   S32 firstTVert = vertsPerFrame * matFrame;

	   GFXVertexPCNTTT *vbVerts = vb.lock();
	   GFXVertexPCNTTT *pEnd = vbVerts+vertsPerFrame;

	   Point3F *pVerts = &(*pVt)[firstVert];
	   Point3F *pNorms = &(*pNm)[firstVert];
	   Point2F *pTVerts = &tverts[firstTVert];
	   Point4F *pTangents = &tangents[firstVert];
	   ColorI *pColors = &vcol[firstVert];

	   while(vbVerts!=pEnd)
	   {
		   vbVerts->point = *pVerts;
		   ++pVerts;

		   vbVerts->color.set(*pColors);
		   ++pColors;

		   vbVerts->normal = *pNorms;        // gets dereferenced 3 times
		   ++pNorms;

		   vbVerts->texCoord = *pTVerts;
		   vbVerts->texCoord2 = *pTVerts;         // need to write this or write will be slower
		   ++pTVerts;

		   vbVerts->T = *pTangents;
		   ++pTangents;

		   vbVerts++;
	   }

	   vb.unlock();
	   return vertsChanged;
   }

   template<> bool setVertexData(GFXVertexBufferHandle<GFXVertexPCNTTBBBBT>* &pVB,S32 frame,S32 matFrame,U32 numVerts,ToolVector<Point3F> *pVt,ToolVector<Point3F> *pNm) 
   {
	   if(!pVB)
	   {
		   pVB = (GFXVertexBufferHandle<GFXVertexPCNTTBBBBT>*)getVertexBuffer();
		   if(!pVB)
		   {
			   pVB = new GFXVertexBufferHandle<GFXVertexPCNTTBBBBT>;
			   setVertexBuffer(pVB);
		   }
	   }

	   GFXVertexBufferHandle<GFXVertexPCNTTBBBBT> &vb = *pVB;

	   // Number of verts can change in LOD skinned mesh
	   bool vertsChanged = ( vb && vb->mNumVerts < numVerts );

	   if( vertsChanged || vb == NULL )
	   {
		   vb.set(GFX, numVerts, GFXBufferTypeStatic);
	   }

	   S32 firstVert  = vertsPerFrame * frame;
	   S32 firstTVert = vertsPerFrame * matFrame;

	   GFXVertexPCNTTBBBBT *vbVerts = vb.lock();
	   GFXVertexPCNTTBBBBT *pEnd = vbVerts+vertsPerFrame;
	   register Point3F temp;

	   Point3F *pVerts = &(*pVt)[firstVert];
	   Point3F *pNorms = &(*pNm)[firstVert];
	   Point2F *pTVerts = &tverts[firstTVert];
	   Point4F *pTangents = &tangents[firstVert];
	   ColorI *pColors = &vcol[firstVert];

	   int count = 0;
	   while(vbVerts!=pEnd)
	   {
		   vbVerts->point = *pVerts;
		   ++pVerts;

		   vbVerts->color.set(*pColors);
		   ++pColors;

		   vbVerts->normal = *pNorms;        // gets dereferenced 3 times
		   ++pNorms;

		   vbVerts->texCoord = *pTVerts;
		   vbVerts->texCoord2 = *pTVerts;         // need to write this or write will be slower
		   ++pTVerts;

		   vbVerts->T = *pTangents;
		   ++pTangents;

		   vbVerts->Bone0.x = (F32)vertBoneIndices[0].boneIndices[count];
		   vbVerts->Bone0.y = vertBoneIndices[0].boneWeight[count];
		   vbVerts->Bone1.x = (F32)vertBoneIndices[1].boneIndices[count];
		   vbVerts->Bone1.y = vertBoneIndices[1].boneWeight[count];
		   vbVerts->Bone2.x = (F32)vertBoneIndices[2].boneIndices[count];
		   vbVerts->Bone2.y = vertBoneIndices[2].boneWeight[count];
		   vbVerts->Bone3.x = (F32)vertBoneIndices[3].boneIndices[count];
		   vbVerts->Bone3.y = vertBoneIndices[3].boneWeight[count];

		   vbVerts++;
		   count++;
	   }

	   vb.unlock();
	   return vertsChanged;
   }

   template<> bool setVertexData(GFXVertexBufferHandle<GFXVertexPNTTBBBBT>* &pVB,S32 frame,S32 matFrame,U32 numVerts,ToolVector<Point3F> *pVt,ToolVector<Point3F> *pNm) 
   {
	   if(!pVB)
	   {
		   pVB = (GFXVertexBufferHandle<GFXVertexPNTTBBBBT>*)getVertexBuffer();
		   if(!pVB)
		   {
			   pVB = new GFXVertexBufferHandle<GFXVertexPNTTBBBBT>;
			   setVertexBuffer(pVB);
		   }
	   }

	   GFXVertexBufferHandle<GFXVertexPNTTBBBBT> &vb = *pVB;

	   // Number of verts can change in LOD skinned mesh
	   bool vertsChanged = ( vb && vb->mNumVerts < numVerts );

	   if( vertsChanged || vb == NULL )
	   {
		   vb.set(GFX, numVerts, GFXBufferTypeStatic);
	   }

	   S32 firstVert  = vertsPerFrame * frame;
	   S32 firstTVert = vertsPerFrame * matFrame;

	   GFXVertexPNTTBBBBT *vbVerts = vb.lock();
	   GFXVertexPNTTBBBBT *pEnd = vbVerts+vertsPerFrame;
	   register Point3F temp;

	   Point3F *pVerts = &(*pVt)[firstVert];
	   Point3F *pNorms = &(*pNm)[firstVert];
	   Point2F *pTVerts = &tverts[firstTVert];
	   Point4F *pTangents = &tangents[firstVert];
	  
	   int count = 0;
	   while(vbVerts!=pEnd)
	   {
		   vbVerts->point = *pVerts;
		   ++pVerts;

		   vbVerts->normal = *pNorms;        // gets dereferenced 3 times
		   ++pNorms;

		   vbVerts->texCoord = *pTVerts;
		   vbVerts->texCoord2 = *pTVerts;         // need to write this or write will be slower
		   ++pTVerts;

		   vbVerts->T = *pTangents;
		   ++pTangents;

		   vbVerts->Bone0.x = (F32)vertBoneIndices[0].boneIndices[count];
		   vbVerts->Bone0.y = vertBoneIndices[0].boneWeight[count];
		   vbVerts->Bone1.x = (F32)vertBoneIndices[1].boneIndices[count];
		   vbVerts->Bone1.y = vertBoneIndices[1].boneWeight[count];
		   vbVerts->Bone2.x = (F32)vertBoneIndices[2].boneIndices[count];
		   vbVerts->Bone2.y = vertBoneIndices[2].boneWeight[count];
		   vbVerts->Bone3.x = (F32)vertBoneIndices[3].boneIndices[count];
		   vbVerts->Bone3.y = vertBoneIndices[3].boneWeight[count];

		   vbVerts++;
		   count++;
	   }

	   vb.unlock();
	   return vertsChanged;
   }

   template<> bool setVertexData(GFXVertexBufferHandle<GFXVertexTTT>* &pVB,S32 frame,S32 matFrame,U32 numVerts,ToolVector<Point3F> *pVt,ToolVector<Point3F> *pNm) 
   {
	   if(!pVB)
	   {
		   pVB = (GFXVertexBufferHandle<GFXVertexTTT>*)getVertexBuffer();
		   if(!pVB)
		   {
			   pVB = new GFXVertexBufferHandle<GFXVertexTTT>;
			   setVertexBuffer(pVB);
		   }
	   }

	   GFXVertexBufferHandle<GFXVertexTTT> &vb = *pVB;

	   // Number of verts can change in LOD skinned mesh
	   bool vertsChanged = ( vb && vb->mNumVerts < numVerts );

	   if( vertsChanged || vb == NULL )
	   {
		   vb.set(GFX, numVerts, GFXBufferTypeStatic);
	   }

	   const S32 firstVert  = vertsPerFrame * frame;
	   const S32 firstTVert = vertsPerFrame * matFrame;

	   GFXVertexTTT *vbVerts = vb.lock();
	   GFXVertexTTT *pEnd = vbVerts+vertsPerFrame;
	   register Point3F temp;

	   Point3F *pVerts = &(*pVt)[firstVert];
	   Point3F *pNorms = &(*pNm)[firstVert];
	   Point2F *pTVerts = &tverts[firstTVert];
	   Point4F *pTangents = &tangents[firstVert];

	   while(vbVerts!=pEnd)
	   {
		   vbVerts->texCoord = *pTVerts;
		   vbVerts->texCoord2 = *pTVerts;         // need to write this or write will be slower
		   ++pTVerts;

		   vbVerts->T = *pTangents;
		   ++pTangents;

		   vbVerts++;
	   }

	   vb.unlock();
	   return vertsChanged;
   }

   template<> bool setVertexData(GFXVertexBufferHandle<GFXVertexCTTT>* &pVB,S32 frame,S32 matFrame,U32 numVerts,ToolVector<Point3F> *pVt,ToolVector<Point3F> *pNm) 
   {
	   if(!pVB)
	   {
		   pVB = (GFXVertexBufferHandle<GFXVertexCTTT>*)getVertexBuffer();
		   if(!pVB)
		   {
			   pVB = new GFXVertexBufferHandle<GFXVertexCTTT>;
			   setVertexBuffer(pVB);
		   }
	   }

	   GFXVertexBufferHandle<GFXVertexCTTT> &vb = *pVB;

	   // Number of verts can change in LOD skinned mesh
	   bool vertsChanged = ( vb && vb->mNumVerts < numVerts );

	   if( vertsChanged || vb == NULL )
	   {
		   vb.set(GFX, numVerts, GFXBufferTypeStatic);
	   }

	   const S32 firstVert  = vertsPerFrame * frame;
	   const S32 firstTVert = vertsPerFrame * matFrame;

	   GFXVertexCTTT *vbVerts = vb.lock();
	   GFXVertexCTTT *pEnd = vbVerts+vertsPerFrame;
	   register Point3F temp;

	   Point3F *pVerts = &(*pVt)[firstVert];
	   Point3F *pNorms = &(*pNm)[firstVert];
	   Point2F *pTVerts = &tverts[firstTVert];
	   Point4F *pTangents = &tangents[firstVert];
	   ColorI *pColors = &vcol[firstVert];

	   while(vbVerts!=pEnd)
	   {
		   vbVerts->color = *pColors;
		   pColors++;

		   vbVerts->texCoord = *pTVerts;
		   vbVerts->texCoord2 = *pTVerts;         // need to write this or write will be slower
		   ++pTVerts;

		   vbVerts->T = *pTangents;
		   ++pTangents;

		   vbVerts++;
	   }

	   vb.unlock();
	   return vertsChanged;
   }

   void createVBIB(S32 frame=0, S32 matFrame=0,GFXVertexBufferHandleBase *pVB=NULL,ToolVector<Point3F> *pVt=NULL,ToolVector<Point3F> *pNm=NULL,bool isNormal=false);
   void createTangents();
   void findTangent( U32 index1, 
                     U32 index2, 
                     U32 index3, 
                     Point3F *tan0, 
                     Point3F *tan1 );

   /// on load...optionally convert primitives to other form
   static bool smUseTriangles;
   static bool smUseOneStrip;
   static S32  smMinStripSize;
   static bool smUseEncodedNormals;

   /// convert primitives on load...
   void convertToTris(S16 * primitiveDataIn, S32 * primitiveMatIn, S16 * indicesIn,
                      S32 numPrimIn, S32 & numPrimOut, S32 & numIndicesOut,
                      S32 * primitivesOut, S16 * indicesOut);
   void convertToSingleStrip(S16 * primitiveDataIn, S32 * primitiveMatIn, S16 * indicesIn,
                             S32 numPrimIn, S32 & numPrimOut, S32 & numIndicesOut,
                             S32 * primitivesOut, S16 * indicesOut);
   void leaveAsMultipleStrips(S16 * primitiveDataIn, S32 * primitiveMatIn, S16 * indicesIn,
                              S32 numPrimIn, S32 & numPrimOut, S32 & numIndicesOut,
                              S32 * primitivesOut, S16 * indicesOut);

   /// methods used during assembly to share vertexand other info
   /// between meshes (and for skipping detail levels on load)
   S32 * getSharedData32(S32 parentMesh, S32 size, S32 ** source, bool skip);
   S8  * getSharedData8 (S32 parentMesh, S32 size, S8  ** source, bool skip);

   /// @name Assembly Variables
   /// variables used during assembly (for skipping mesh detail levels
   /// on load and for sharing verts between meshes)
   /// @{

   static Vector<Point3F*> smVertsList;
   static Vector<ColorI*>  smVColList;  //Ray: export vertex color
   static Vector<Point3F*> smNormsList;
   static Vector<U8*>      smEncodedNormsList;
   static Vector<Point2F*> smTVertsList;
   static Vector<bool>     smDataCopied;

   static const Point3F smU8ToNormalTable[];
   /// @}


   TSMesh() : meshType(StandardMeshType) {
      VECTOR_SET_ASSOCIATION(planeNormals);
      VECTOR_SET_ASSOCIATION(planeConstants);
      VECTOR_SET_ASSOCIATION(planeMaterials);
      parentMesh = -1;

      mOptTree = NULL;
      mDynamic = false;
      mVisibility = 1.0f;

	  mpVB	=	NULL;
	  initShadowTemp = false;
	  
	  sortOrder = 0;
	  disableZ = false;
   }
   virtual ~TSMesh();

   Opcode::Model *mOptTree;

   void           prepOpcodeCollision();
   virtual bool   buildPolyListOpcode(const S32 od, AbstractPolyList * polyList, const Box3F &nodeBox);
   bool           buildConvexOpcode(const MatrixF &mat, const Box3F &bounds, Convex *c, Convex *list);
   virtual bool   castRayOpcode(const Point3F & start, const Point3F & end, RayInfo * rayInfo);

   static const F32 VISIBILITY_EPSILON; 
};

inline const Point3F & TSMesh::decodeNormal(U8 ncode) { return smU8ToNormalTable[ncode]; }

class TSSkinMesh : public TSMesh
{
public:
   typedef TSMesh Parent;

   /// vectors that define the vertex, weight, bone tuples
   ToolVector<F32> weight;
   ToolVector<S32> boneIndex;
   ToolVector<S32> vertexIndex;

   /// vectors indexed by bone number
   ToolVector<S32> nodeIndex;
   ToolVector<MatrixF> initialTransforms;

   /// initial values of verts and normals
   /// these get transformed into initial bone space,
   /// from there into world space relative to current bone
   /// pos, and then weighted by bone weights...
   ToolVector<Point3F> initialVerts;
   ToolVector<Point3F> initialNorms;

   bool bFirstUpdate;

   /// set verts and normals...
   void updateSkin(S32 frame, S32 matFrame);
   void doUpdateSkin(SkinParam *pParam);
   static void skinCallback(void* param);

   // overrides from TSMesh
   GFXVertexBufferHandleBase *getVertexBuffer();
   void setVertexBuffer(GFXVertexBufferHandleBase *pVB);

   GFXTextureObject *getRVertexTexture(void *);
   void setRVertexTexture(void *,U32 width, U32 height, GFXFormat format, GFXTextureProfile *profile);
   GFXTextureObject *getCVertexTexture(void *);
   void setCVertexTexture(void *,U32 width, U32 height, GFXFormat format, GFXTextureProfile *profile);
   void switchVertexTexture(void *);

   // render methods..
   //void render(TSMaterialList * materials = NULL);
   void render(S32 frame, S32 matFrame, TSMaterialList *);
   void setVertTexParam(RenderInst *);

   // collision methods...
   //bool buildPolyList(S32 frame, AbstractPolyList * polyList, U32 & surfaceKey);
   bool castRay(S32 frame, const Point3F & start, const Point3F & end, RayInfo * rayInfo);
   bool buildConvexHull(); // does nothing, skins don't use this

   void computeBounds(MatrixF & transform, Box3F & bounds, S32 frame, Point3F * center, F32 * radius);

   /// persist methods...
   void assemble(bool skip);
   void disassemble();

   /// variables used during assembly (for skipping mesh detail levels
   /// on load and for sharing verts between meshes)
   static Vector<MatrixF*> smInitTransformList;
   static Vector<S32*>     smVertexIndexList;
   static Vector<S32*>     smBoneIndexList;
   static Vector<F32*>     smWeightList;
   static Vector<S32*>     smNodeIndexList;

   GFXTexHandle mSourceVNTexture;
   GFXTexHandle mBoneIdxTexture;
   GFXTexHandle mWeigthTexture;

   TSSkinMesh()
   {
      meshType = SkinMeshType;
      mDynamic = true;
	  bFirstUpdate = true;

	  mSourceVNTexture		= NULL;
	  mBoneIdxTexture	= NULL;
	  mWeigthTexture	= NULL;
   }

   ~TSSkinMesh()
   {
	   mSourceVNTexture		= NULL;
	   mBoneIdxTexture	= NULL;
	   mWeigthTexture	= NULL;
   }
};

//
// Allows the lighting system to process the mesh before submission to the Render Instance manager
//
class TSMeshLightPlugin
{
public:   
   // Is passed the current mesh and a list of render instances corresponding to each primitive. 
   // The plugin is responible for sumbitting the render instances to the render instance manager.
    virtual void processRI(TSMesh* mesh, Vector<RenderInst*>& list) = 0;
	virtual U32 prepareLight(){return 0;}
};

//
// DefaultMeshLightPlugin - submits mesh with no lighting
//
class DefaultMeshLightPlugin : public TSMeshLightPlugin
{
public:   
	virtual void processRI(TSMesh* mesh, Vector<RenderInst*>& list);
};

struct SkinParam
{
	bool isNormal;
	S32 frame;
	S32 matFrame;
	GFXVertexBufferHandleBase *pVB;
	const MatrixF * nodeTransforms;
	TSSkinMesh *pMesh;
	void * pMeshObj;
};

struct FrameParam : public CVirtualAllocBase
{
	S32 frame;
	S32 matFrame;
	GFXVertexBufferHandleBase *pVB;
	TSMesh *pMesh;
};
#endif
