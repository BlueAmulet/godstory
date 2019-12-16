//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------

#ifndef _SGLIGHTMAP_H_
#define _SGLIGHTMAP_H_

#ifndef _TVECTOR_H_
#include "core/tVector.h"
#endif

#ifndef _COLOR_H_
#include "core/color.h"
#endif

#ifndef _MBOX_H_
#include "math/mBox.h"
#endif

#ifndef _SGLIGHTING_H_
#include "lightingSystem/synapseGaming/sgLighting.h"
#endif

#ifndef HASHMAP_H_
#include "lightingSystem/synapseGaming/sgHashMap.h"
#endif

#ifndef BVPT_H_
#include "lightingSystem/synapseGaming/sgBinaryVolumePartitionTree.h"
#endif

class SceneObject;
class TerrainBlock;
class sgLightInfo;
class InteriorInstance;
class Interior;
class InteriorSimpleMesh;

class sgShadowObjects
{
public:
   /// used for storing static mesh geometry for fast shadow detection...
   struct sgStaticMeshTri
   {
      Point3D sgVert[3];
      PlaneF sgPlane;
      Box3F sgBox;
   };
   /// BVPT to static mesh geometry mapping typedef...
   typedef BVPT<sgStaticMeshTri *> sgStaticMeshBVPT;
   /// object info...
   struct sgObjectInfo
   {
      MatrixF sgInverseTransform;
      sgStaticMeshBVPT sgBVPT;
      Vector<sgStaticMeshTri> sgTris;
   };
   /// static mesh to object mapping typedef...
   typedef hash_multimap<void *, sgObjectInfo *> sgStaticMeshBVPTEntry;

   /// s and e are in interior space, not static mesh space...
   static bool sgCastRayStaticMesh(Point3F s, Point3F e, InteriorSimpleMesh *staticmesh);
   static void sgClearStaticMeshBVPTData();

private:
   /// master object info storage...
   static Vector<sgObjectInfo *> sgObjectInfoStorage;
   /// static mesh to BVPT mapping...
   static sgStaticMeshBVPTEntry sgStaticMeshBVPTMap;

public:
	static VectorPtr<SceneObject *> sgObjects;
	static void sgGetObjects(SceneObject *obj);
};

class sgColorMap
{
public:
	U32 sgWidth;
	U32 sgHeight;
	ColorF *sgData;
	sgColorMap(U32 width, U32 height)
	{
		sgWidth = width;
		sgHeight = height;
		sgData = new ColorF[(width * height)];
		dMemset(sgData, 0, (width * height * sizeof(ColorF)));
	}
	~sgColorMap() {delete[] sgData;}
	void sgFillInLighting();
	void sgBlur();
	//void sgMerge(GBitmap *lightmap, U32 xoffset, U32 yoffset, bool normalblend);
};

/**
 * The base class for generating mission level or real-time light maps
 * for any sceneObject.  All actual work is performed in the descendent
 * class' sgLightMap::sgCalculateLighting method.
 */
class sgLightMap
{
protected:
	U32 sgWidth;
	U32 sgHeight;
	/// The light map color buffer.
	sgColorMap *sgTexels;
   sgColorMap *sgNexels;
public:
	/// The world space position that the texture space coord (0, 0) represents.
   Point3D sgWorldPosition;
	/// Defines the world space directional change
	/// corresponding to a change of (+1, 0) in the light map texture space.
	/// Similar to the tangent vector in dot3 bump mapping.
   Point3D sgLightMapSVector;
	/// Defines the world space directional change
	/// corresponding to a change of (0, +1) in the light map texture space.
	/// Similar to the binormal vector in dot3 bump mapping.
   Point3D sgLightMapTVector;
	sgLightMap(U32 width, U32 height)
	{
		sgWidth = width;
		sgHeight = height;
      sgWorldPosition = Point3D(0.0f, 0.0f, 0.0f);
      sgLightMapSVector = Point3D(0.0f, 0.0f, 0.0f);
      sgLightMapTVector = Point3D(0.0f, 0.0f, 0.0f);
		sgTexels = new sgColorMap(width, height);
      sgNexels = new sgColorMap(width, height);
	}
	~sgLightMap()
	{
      delete sgNexels;
		delete sgTexels;
	}
	/// Object specific light mapping calculations are done here.
	virtual void sgSetupLighting() {}// default...
	virtual void sgCalculateLighting(sgLightInfo *light) = 0;
protected:
   struct sgStaticMeshInfo
   {
      Box3F sgWorldBounds;
      InteriorSimpleMesh *sgStaticMesh;
      InteriorInstance *sgInteriorInstance;
   };
   typedef BVPT<sgStaticMeshInfo> sgStaticMeshBVPT;

   Vector<SceneObject *> sgIntersectingSceneObjects;
   //Vector<sgStaticMeshInfo> sgIntersectingStaticMeshObjects;
   sgStaticMeshBVPT sgIntersectingStaticMeshObjects;
   void sgGetIntersectingObjects(const Box3F &surfacebox, const SceneObject *skipobject);
};

#endif

