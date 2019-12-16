//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _SCENEOBJECT_H_
#define _SCENEOBJECT_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif
#ifndef _NETOBJECT_H_
#include "sim/netObject.h"
#endif
#ifndef _COLLISION_H_
#include "collision/collision.h"
#endif
#ifndef _POLYHEDRON_H_
#include "collision/polyhedron.h"
#endif
#ifndef _ABSTRACTPOLYLIST_H_
#include "collision/abstractPolyList.h"
#endif
#ifndef _OBJECTTYPES_H_
#include "T3D/objectTypes.h"
#endif
#ifndef _COLOR_H_
#include "core/color.h"
#endif
#ifndef _TVECTOR_H_
#include "core/tVector.h"
#endif
#include "platform/profiler.h"
#include <hash_map>
#include <hash_set>
#include "core/fileStream.h"
//-------------------------------------- Forward declarations...
class SceneObject;
class SceneGraph;
class SceneState;
class SceneRenderImage;
class Box3F;
class Point3F;
class Convex;
struct RenderInst;
class LightInfo;

enum CAST_TYPE
{
	CAST_RAY_TYPE,
	CAST_RAY_FADE_TYPE,
	CAST_SHADOW_TYPE
};

// 所有场景对象之间最大的交互距离
#define INTERACTIONDISTANCE  3.00f

//----------------------------------------------------------------------------
/// Extension of the collision structure to allow use with raycasting.
///
/// @see Collision
struct RayInfo: public Collision 
{
	// The collision struct has object, point, normal & material.
	F32 t;
};

//--------------------------------------------------------------------------

// CodeReview - old note which was posing as documentation!
//    There are two indiscretions here.  First is the name, which refers rather
//    blatantly to the container bin system.  A hygiene issue.  Next is the
//    user defined U32, which is added solely for the zoning system.  This should
//    properly be split up into two structures, for the disparate purposes, especially
//    since it's not nice to force the container bin to use 20 bytes structures when
//    it could get away with a 16 byte version.

/// Reference to a scene object.
class SceneObjectRef
{
public:
	SceneObjectRef() : object(NULL), nextInBin(NULL), prevInBin(NULL), nextInObj(NULL), zone(0) { }

public:
	SceneObject*    object;
	SceneObjectRef* nextInBin;
	SceneObjectRef* prevInBin;
	SceneObjectRef* nextInObj;

	U32             zone;
};

#define BinPow  6
//----------------------------------------------------------------------------
class Container
{
public:
	typedef Vector<SimObjectPtr<SceneObject>*> SearchListType;
	struct Link
	{
		Link* next;
		Link* prev;
		Link();
		void unlink();
		void linkAfter(Link* ptr);
	};

	struct CallbackInfo 
	{
		AbstractPolyList* polyList;
		Box3F boundingBox;
		SphereF boundingSphere;
		void *key;
	};

	static const U32 csmNumBins;
	static const F32 csmBinSize;
	static const F32 csmTotalBinSize;
	static const U32 csmRefPoolBlockSize;
	static U32    smCurrSeqKey;

private:
	Link mStart,mEnd;

	SceneObjectRef*         mFreeRefPool;
	Vector<SceneObjectRef*> mRefPoolBlocks;

	stdext::hash_map < U32, SceneObjectRef *> mBinArrayVec;
	//SceneObjectRef* mBinArray;
	SceneObjectRef  mOverflowBin;

public:
	Container();
	~Container();

	void getObjectsInLayer(stdext::hash_set<SceneObject*>& objectSet, int nLayerId);

	void clearObjects(int nLayerId);

	/// @name Basic database operations
	/// @{
	///
	typedef void (*FindCallback)(SceneObject*,void *key);
	void findObjects(U32 mask, FindCallback, void *key = NULL, U32 layerID = 0);
	void findObjects(const Box3F& box, U32 mask, FindCallback, void *key = NULL, U32 layerID = 0);
	void polyhedronFindObjects(const Polyhedron& polyhedron, U32 mask,
		FindCallback, void *key = NULL, U32 layerID = 0);



	/// @}

	/// @name Line intersection
	/// @{
	// 注意，callback返回值是true为符合要求的对象，返回值为false为不符合要求的对象
	typedef bool (*castCallback)(SceneObject* obj);

	inline bool castRayType(const Point3F &start, const Point3F &end, U32 mask, RayInfo* info, U32 layerID, CAST_TYPE casttype, castCallback callback = NULL);
	inline bool castRayChain(const Point3F &start, const Point3F &end, U32 mask, RayInfo* info, CAST_TYPE casttype, F32 &currentT, SceneObjectRef* chain, castCallback callback);

	bool castShadow(const Point3F &start, const Point3F &end, U32 mask, RayInfo* info, U32 layerID = 0, castCallback callback = NULL);
	bool castRay(const Point3F &start, const Point3F &end, U32 mask, RayInfo* info, U32 layerID = 0, castCallback callback = NULL);
	bool castRayFade(const Point3F &start, const Point3F &end, U32 mask, U32 layerID = 0, castCallback callback = NULL);
	bool collideBox(const Point3F &start, const Point3F &end, U32 mask, RayInfo* info, U32 layerID = 0, castCallback callback = NULL);
	/// @}

	/// @name Poly list
	/// @{

	///
	bool buildPolyList(const Box3F& box, U32 mask, AbstractPolyList*,FindCallback=0,void *key = NULL);
	bool buildCollisionList(const Box3F& box, const Point3F& start, const Point3F& end, const VectorF& velocity,
		U32 mask,CollisionList* collisionList,FindCallback = 0,void *key = NULL,const Box3F *queryExpansion = 0);
	bool buildCollisionList(const Polyhedron& polyhedron,
		const Point3F& start, const Point3F& end,
		const VectorF& velocity,
		U32 mask, CollisionList* collisionList,
		FindCallback callback = 0, void *key = NULL);
	/// @}

	///
	bool addObject(SceneObject*);
	bool removeObject(SceneObject*);

	void addRefPoolBlock();
	SceneObjectRef* allocateObjectRef();
	void freeObjectRef(SceneObjectRef*);
	void insertIntoBins(SceneObject*);
	void removeFromBins(SceneObject*);

	/// Checkbins makes sure that we're not just sticking the object right back
	/// where it came from.  The overloaded insertInto is so we don't calculate
	/// the ranges twice.
	void checkBins(SceneObject*);
	void insertIntoBins(SceneObject*, U32, U32, U32, U32);

	static bool checkGlobal(SceneObject *);

private:
	void insertIntoBinsLayer(SceneObject*, U32, U32, U32, U32, U32);
	void findObjectsInBin(const Box3F &box, U32 mask, FindCallback callback, void *key, SceneObjectRef* chain);
	void findObjectsInOverflowBin(const Box3F& box, U32 mask, FindCallback callback, void *key);
	void findObjectsInLayer(const Box3F& box, U32 mask, FindCallback callback, void *key, U32 layerID, U32 minX, U32 maxX, U32 minY, U32 maxY);

private:
	SearchListType                      mSearchList;///< Object searches to support console querying of the database.  ONLY WORKS ON SERVER
	S32                                 mCurrSearchPos;
	Point3F                             mSearchReferencePoint;
	void cleanupSearchVectors();

public:
	void initRadiusSearch(const Point3F& searchPoint,
		const F32      searchRadius,
		const U32      searchMask,
		const U32      layerId = 0);
	inline SearchListType& getSearch() { return mSearchList;}
	void initTypeSearch(const U32      searchMask);
	U32  containerSearchNext();
	F32  containerSearchCurrDist();
	F32  containerSearchCurrRadiusDist();
};

// Utility method for bin insertion
inline void getBinRange(const F32 min,
						const F32 max,
						U32&      minBin,
						U32&      maxBin)
{
	AssertFatal(max >= min, "Error, bad range! in getBinRange");

	if ((max - min) >= (Container::csmTotalBinSize - Container::csmBinSize))
	{
		F32 minCoord = mFmod(min, Container::csmTotalBinSize);
		if (minCoord < 0.0f) 
		{
			minCoord += Container::csmTotalBinSize;

			// This is truly lame, but it can happen.  There must be a better way to
			//  deal with this.
			if (minCoord == Container::csmTotalBinSize)
				minCoord = Container::csmTotalBinSize - 0.01;
		}

		AssertFatal(minCoord >= 0.0 && minCoord < Container::csmTotalBinSize, "Bad minCoord");

		minBin = U32(minCoord / Container::csmBinSize);
		AssertFatal(minBin < Container::csmNumBins, avar("Error, bad clipping! (%g, %d)", minCoord, minBin));

		maxBin = minBin + (Container::csmNumBins - 1);
		return;
	}
	else 
	{

		F32 minCoord = mFmod(min, Container::csmTotalBinSize);

		if (minCoord < 0.0f) 
		{
			minCoord += Container::csmTotalBinSize;

			// This is truly lame, but it can happen.  There must be a better way to
			//  deal with this.
			if (minCoord == Container::csmTotalBinSize)
				minCoord = Container::csmTotalBinSize - 0.01;
		}
		AssertFatal(minCoord >= 0.0 && minCoord < Container::csmTotalBinSize, "Bad minCoord");

		F32 maxCoord = mFmod(max, Container::csmTotalBinSize);
		if (maxCoord < 0.0f) {
			maxCoord += Container::csmTotalBinSize;

			// This is truly lame, but it can happen.  There must be a better way to
			//  deal with this.
			if (maxCoord == Container::csmTotalBinSize)
				maxCoord = Container::csmTotalBinSize - 0.01; 
		}
		AssertFatal(maxCoord >= 0.0 && maxCoord < Container::csmTotalBinSize, "Bad maxCoord");

		minBin = U32(minCoord / Container::csmBinSize);
		maxBin = U32(maxCoord / Container::csmBinSize);
		AssertFatal(minBin < Container::csmNumBins, avar("Error, bad clipping(min)! (%g, %d)", maxCoord, minBin));
		AssertFatal(minBin < Container::csmNumBins, avar("Error, bad clipping(max)! (%g, %d)", maxCoord, maxBin));

		// MSVC6 seems to be generating some bad floating point code around
		// here when full optimizations are on.  The min != max test should
		// not be needed, but it clears up the VC issue.
		if (min != max && minCoord > maxCoord)
			maxBin += Container::csmNumBins;

		AssertFatal(maxBin >= minBin, "Error, min should always be less than max!");
	}
}
//----------------------------------------------------------------------------
/// For simple queries.  Simply creates a vector of the objects
class SimpleQueryList
{
public:
	Vector<SceneObject*> mList;

public:
	SimpleQueryList() 
	{
		VECTOR_SET_ASSOCIATION(mList);
	}

	void insertObject(SceneObject* obj) { mList.push_back(obj); }
	static void insertionCallback(SceneObject* obj, void *key);
};

class SceneObjectLightingPlugin 
{
public:
	virtual ~SceneObjectLightingPlugin() { }
	// Called by Shapebase and descendents 
	virtual void renderShadow(SceneObject* obj, SceneState *state) = 0;
	// Called by statics
	virtual U64  packUpdate(SceneObject* obj, U64 checkMask, NetConnection *conn, U64 mask, BitStream *stream) = 0;
	virtual void unpackUpdate(SceneObject* obj, NetConnection *conn, BitStream *stream) = 0; 
	virtual void saveDynamicShadowData(SceneObject* obj,Stream *stream){};
	virtual void setDynamicShadowData(SceneObject* obj,void *,void *){};
	virtual void loadResource(){};
	virtual void freeResource(){};
};

//----------------------------------------------------------------------------

/// A 3D object.
///
/// @section SceneObject_intro Introduction
///
/// SceneObject exists as a foundation for 3D objects in PowerEngine. It provides the
/// basic functionality for:
///      - A scene graph (in the Zones and Portals sections), allowing efficient
///        and robust rendering of the game scene.
///      - Various helper functions, including functions to get bounding information
///        and momentum/velocity.
///      - Collision detection, as well as ray casting.
///      - Lighting. SceneObjects can register lights both at lightmap generation time,
///        and dynamic lights at runtime (for special effects, such as from flame or
///        a projectile, or from an explosion).
///      - Manipulating scene objects, for instance varying scale.
///
/// @section SceneObject_example An Example
///
/// Melv May has written a most marvelous example object deriving from SceneObject.
/// Unfortunately this page is too small to contain it.
///
/// @see http://www.FairRain.com/index.php?sec=mg&mod=resource&page=view&qid=3217
///      for a copy of Melv's example.


//SceneObject修改：将renderObject和renderShadow渲染操作移到RenderableSceneObject中 -> 08.12.22 by Jacky
class SceneObject : public NetObject, public Container::Link
{
	typedef NetObject Parent;
	friend class Container;
	friend class SceneGraph;
	friend class SceneState;

	//-------------------------------------- Public constants
public:
	enum 
	{
		MaxObjectZones = 128
	};

	enum TraversalState 
	{
		Pending = 0,
		Working = 1,
		Done = 2
	};

	enum SceneObjectMasks 
	{
		ScaleMask    = BIT(0),
		NextFreeMask = BIT(1)
	};

	//-------------------------------------- Public interfaces
	// C'tors and D'tors
private:
	SceneObject(const SceneObject&); ///< @deprecated disallowed

public:
	SceneObject();
	virtual ~SceneObject();

	/// Returns a value representing this object which can be passed to script functions.
	const char* scriptThis();

	void OnSerialize( CTinyStream& stream );
	void OnUnserialize( CTinyStream& stream );

public:
	virtual void onBeginRender(){}
	/// @name Collision and transform related interface
	///
	/// The Render Transform is the interpolated transform with respect to the
	/// frame rate. The Render Transform will differ from the object transform
	/// because the simulation is updated in fixed intervals, which controls the
	/// object transform. The framerate is, most likely, higher than this rate,
	/// so that is why the render transform is interpolated and will differ slightly
	/// from the object transform.
	///
	/// @{
	virtual bool canAddToScope(NetConnection *con) { return true; }

	/// Disables collisions for this object including raycasts
	virtual void disableCollision();

	/// Enables collisions for this object
	virtual void enableCollision();

	/// Returns true if collisions are enabled
	bool isCollisionEnabled() const { return mCollisionCount == 0; }

	virtual bool isFunctionEnabled(U32 type){return true;}

	/// Returns true if this object allows itself to be displaced
	/// @see displaceObject
	virtual bool isDisplacable() const;

	/// Returns the momentum of this object
	virtual Point3F getMomentum() const;

	/// Sets the momentum of this object
	/// @param   momentum   Momentum
	virtual void    setMomentum(const Point3F &momentum);

	/// Returns the mass of this object
	virtual F32 getMass() const;

	/// Displaces this object by a vector
	/// @param   displaceVector   Displacement vector
	virtual bool displaceObject(const Point3F& displaceVector);

	/// Returns the transform which can be used to convert object space
	/// to world space
	virtual const MatrixF& getTransform() const      { return mObjToWorld; }

	/// Returns the transform which can be used to convert world space
	/// into object space
	const MatrixF& getWorldTransform() const { return mWorldToObj; }

	/// Returns the scale of the object
	const VectorF& getScale() const          { return mObjScale;   }

	/// Returns the bounding box for this object in local coordinates
	const Box3F&   getObjBox() const        { return mObjBox;      }

	/// Returns the bounding box for this object in world coordinates
	const Box3F&   getWorldBox() const      { return mWorldBox;    }

	/// Returns the bounding sphere for this object in world coordinates
	const SphereF& getWorldSphere() const   { return mWorldSphere; }

	/// Returns the center of the bounding box in world coordinates
	Point3F        getBoxCenter() const     { return (mWorldBox.min + mWorldBox.max) * 0.5f; }

	/// Sets the Object -> World transform
	///
	/// @param   mat   New transform matrix
	virtual void setTransform(const MatrixF & mat);

	/// Sets the scale for the object
	/// @param   scale   Scaling values
	virtual void setScale(const VectorF & scale);

	/// This sets the render transform for this object
	/// @param   mat   New render transform
	virtual void   setRenderTransform(const MatrixF &mat);

	/// Returns the render transform
	const MatrixF& getRenderTransform() const      { return mRenderObjToWorld; }

	/// Returns the render transform to convert world to local coordinates
	const MatrixF& getRenderWorldTransform() const { return mRenderWorldToObj; }

	/// Returns the render world box
	const Box3F&   getRenderWorldBox()  const      { return mRenderWorldBox;   }

	/// Builds a convex hull for this object.
	///
	/// Think of a convex hull as a low-res mesh which covers, as tightly as
	/// possible, the object mesh, and is used as a collision mesh.
	/// @param   box
	/// @param   convex   Convex mesh generated (out)
	virtual void     buildConvex(const Box3F& box,Convex* convex);

	/// Builds a list of polygons which intersect a bounding volume.
	///
	/// This will use either the sphere or the box, not both, the
	/// SceneObject implimentation ignores sphere.
	///
	/// @see AbstractPolyList
	/// @param   polyList   Poly list build (out)
	/// @param   box        Box bounding volume
	/// @param   sphere     Sphere bounding volume
	virtual bool     buildPolyList(AbstractPolyList* polyList, const Box3F &box, const SphereF &sphere);

	/// Builds a collision tree of all the polygons which collide with a bounding volume.
	///
	/// @note Not implemented in SceneObject. @see TerrainBlock::buildCollisionBSP
	/// @param   tree     BSP tree built (out)
	/// @param   box      Box bounding volume
	/// @param   sphere   Sphere bounding volume
	virtual BSPNode* buildCollisionBSP(BSPTree *tree, const Box3F &box, const SphereF &sphere);

	/// Casts a ray and obtain collision information, returns true if RayInfo is modified.
	///
	/// @param   start   Start point of ray
	/// @param   end   End point of ray
	/// @param   info   Collision information obtained (out)
	virtual bool castShadow(const Point3F &start, const Point3F &end,RayInfo* info){return castRay(start,end,info);}
	virtual bool castRay(const Point3F &start, const Point3F &end, RayInfo* info);

	virtual bool haveDynamicShadow(){return false;}
	virtual void saveDynamicShadowData(Stream *stream){};
	virtual void setDynamicShadowData(void *,void *){};

	virtual bool collideBox(const Point3F &start, const Point3F &end, RayInfo* info);

	/// Returns the position of the object.
	virtual Point3F  getPosition() const;
	virtual U32  getCRC(){return 0;}


	/// Returns the render-position of the object.
	///
	/// @see getRenderTransform
	Point3F  getRenderPosition() const;

	/// Sets the position of the object
	void     setPosition(const Point3F &pos);

	/// Gets the velocity of the object
	virtual Point3F getVelocity() const;

	/// Sets the velocity of the object
	/// @param  v  Velocity
	virtual void setVelocity(const Point3F &v);

	F32 getDistance(const SceneObject* obj);
	F32 getDistance(const Point3F* pos);

	bool canInteraction(SceneObject* obj) { return getDistance(obj) < INTERACTIONDISTANCE;}

	/// @}

	// <Edit> [7/14/2009 joy] 可是距离等级
	virtual inline S32 getVisibleDistanceLevel() { return 0;}
	bool IsUseClosestPoint() {return mIsUseClosestPoint;}
	void useClosestPoint(bool flag) {mIsUseClosestPoint = flag;}

public:
	/// @name Zones
	///
	/// A zone is a portalized section of an InteriorInstance, and an InteriorInstance can manage more than one zone.
	/// There is always at least one zone in the world, zone 0, which represens the whole world. Any
	/// other zone will be in an InteriorInstance. PowerEngine keeps track of the zones containing an object
	/// as it moves throughout the world. An object can exists in multiple zones at once.
	/// @{

	/// Returns true if this object is managing zones.
	///
	/// This is only true in the case of InteriorInstances which have zones in them.
	bool isManagingZones() const;

	/// Gets the index of the first zone this object manages in the collection of zones.
	U32  getZoneRangeStart() const                         { return mZoneRangeStart;   }

	/// Gets the number of zones containing this object.
	U32  getNumCurrZones() const                           { return mNumCurrZones;     }

	/// Returns the nth zone containing this object.
	U32  getCurrZone(const U32 index) const;

	/// If an object exists in multiple zones, this method will give you the
	/// number and indices of these zones (storing them in the provided variables).
	///
	/// @param   obj      Object in question.
	/// @param   zones    Indices of zones containing the object. (out)
	/// @param   numZones Number of elements in the returned array.  (out)
	virtual bool getOverlappingZones(SceneObject* obj, U32* zones, U32* numZones);

	/// Returns the zone containing p.
	///
	/// @param   p   Point to test.
	virtual U32  getPointZone(const Point3F& p);

	/// This is called on a zone managing object to scope all the zones managed.
	///
	/// @param   rootPosition   Camera position
	/// @param   rootDistance   Camera visible distance
	/// @param   zoneScopeState Array of booleans which line up with the collection of zones, marked true if that zone is scoped (out)
	virtual bool scopeObject(const Point3F&        rootPosition,
		const F32             rootDistance,
		bool*                 zoneScopeState);
	/// @}

	/// Called when the SceneGraph is ready for the registration of RenderImages.
	///
	/// @see SceneState
	///
	/// @param   state               SceneState
	/// @param   stateKey            State key of the current SceneState
	/// @param   startZone           Base zone index
	/// @param   modifyBaseZoneState If true, the object needs to modify the zone state.
	virtual bool prepRenderImage(SceneState *state, const U32 stateKey, const U32 startZone,
		const bool modifyBaseZoneState = false);

	/// Adds object to the client or server container depending on the object
	void addToScene();

	/// Removes the object from the client/server container
	void removeFromScene();

	/// This is a callback for objects that have reflections and are added to
	/// the "reflectiveSet" SimSet.
	virtual void updateReflection(){}
	virtual void updateWaterQuery(){}
	//-------------------------------------- Derived class interface
	// Overrides
protected:
	bool onAdd();
	void onRemove();

	// Overrideables
protected:
	/// Called when this is added to the SceneGraph.
	///
	/// @param   graph   SceneGraph this is getting added to
	virtual bool onSceneAdd(SceneGraph *graph);

	/// Called when this is removed from the SceneGraph
	virtual void onSceneRemove();

	/// Called when the size of the object changes
	virtual void onScaleChanged();

	/// @name Portals
	/// @{

	/// This is used by a portal controling object to transform the base-modelview
	/// used by the scenegraph for rendering to the modelview it needs to render correctly.
	///
	/// @see MirrorSubObject
	///
	/// @param   portalIndex   Index of portal in the list of portals controlled by the object.
	/// @param   oldMV         Current modelview matrix used by the SceneGraph (in)
	/// @param   newMV         New modelview to be used by the SceneGraph (out)
	virtual void transformModelview(const U32 portalIndex, const MatrixF& oldMV, MatrixF* newMV);

	/// Used to tranform the position of a point based on a portal.
	///
	/// @param   portalIndex   Index of a portal to transform by.
	/// @param   point         Point to transform.
	virtual void transformPosition(const U32 portalIndex, Point3F& point);

	/// Returns a new view frustum for the portal.
	///
	/// @param   portalIndex   Which portal in the list of portals the object controls
	/// @param   oldFrustum    Current frustum.
	/// @param   nearPlane     Near clipping plane.
	/// @param   farPlane      Far clipping plane.
	/// @param   oldViewport   Current viewport.
	/// @param   newFrustum    New view frustum to use. (out)
	/// @param   newViewport   New viewport to use. (out)
	/// @param   flippedMatrix Should the object should use a flipped matrix to calculate viewport and frustum?
	virtual bool computeNewFrustum(const U32      portalIndex,
		const F64*     oldFrustum,
		const F64      nearPlane,
		const F64      farPlane,
		const RectI&   oldViewport,
		F64*           newFrustum,
		RectI&         newViewport,
		const bool     flippedMatrix);

	/// Called before things are to be rendered from the portals point of view, to set up
	/// everything the portal needs to render correctly.
	///
	/// @param   portalIndex   Index of portal to use.
	/// @param   pCurrState    Current SceneState
	/// @param   pParentState  SceneState used before this portal was activated
	virtual void openPortal(const U32   portalIndex,
		SceneState* pCurrState,
		SceneState* pParentState);

	/// Called after rendering of a portal is complete, this resets the states
	/// the previous call to openPortal() changed.
	///
	/// @param   portalIndex    Index of portal to use.
	/// @param   pCurrState     Current SceneState
	/// @param   pParentState   SceneState used before this portal was activated
	virtual void closePortal(const U32   portalIndex,
		SceneState* pCurrState,
		SceneState* pParentState);
public:

	/// Returns the plane of the portal in world space.
	///
	/// @param   portalIndex   Index of portal to use.
	/// @param   plane         Plane of the portal in world space (out)
	virtual void getWSPortalPlane(const U32 portalIndex, PlaneF *plane);

	/// @}

protected:
	/// Sets the mLastState and mLastStateKey.
	///
	/// @param   state   SceneState to set as the last state
	/// @param   key     Key to set as the last state key
	void          setLastState(SceneState *state, U32 key);

	/// Returns true if the provided SceneState and key are set as this object's
	/// last state and key.
	///
	/// @param   state    SceneState in question
	/// @param   key      State key in question
	bool          isLastState(SceneState *state, U32 key) const;


	/// @name Traversal State
	///
	/// The SceneGraph traversal is recursive and the traversal state of an object
	/// can be one of three things:
	///           - Pending - The object has not yet been examined for zone traversal.
	///           - Working - The object is currently having its zones traversed.
	///           - Done    - The object has had all of its zones traversed or doesn't manage zones.
	///
	/// @note These states were formerly referred to as TraverseColor, with White, Black, and
	///       Gray; this was changed in PowerEngine 1.2 by Pat "KillerBunny" Wilson. This code is
	///       only used internal to this class
	/// @{

	// These two replaced by TraversalState because that makes more sense -KB
	//void          setTraverseColor(TraverseColor);
	//TraverseColor getTraverseColor() const;
	// ph34r teh muskrat! - Travis Colure

	/// This sets the traversal state of the object.
	///
	/// @note This is used internally; you should not normally need to call it.
	/// @param   s   Traversal state to assign
	void           setTraversalState( TraversalState s );

	/// Returns the traversal state of this object
	TraversalState getTraversalState() const;

	/// @}

	/// @name Lighting
	/// @{
protected:
	SceneObjectLightingPlugin* mLightPlugin;
public:
	void setLightingPlugin(SceneObjectLightingPlugin* plugin) { mLightPlugin = plugin; }
	SceneObjectLightingPlugin* getLightingPlugin() { return mLightPlugin; }

	// These fields below should be moved into a lighting component at somepoint.  

	// forward declared for TSStatic and StaticShape
	// not used or transmited in any way by other classes
	// don't worry this is light-weight and network aware
	//
	// we need to override the options in all but the
	// objects that expose these to script - this is
	// static and NOT transmitted across the network...
	bool overrideOptions;
	bool receiveLMLighting;
	bool receiveSunLight;
	bool useAdaptiveSelfIllumination;
	bool useCustomAmbientLighting;
	bool customAmbientForSelfIllumination;
	ColorF customAmbientLighting;
	StringTableEntry lightGroupName;
protected:

	/// @name Transform and Collision Members
	/// @{

	///
	Container* mContainer;
	U32	  mLayerID;

	MatrixF mObjToWorld;   ///< Transform from object space to world space
	MatrixF mWorldToObj;   ///< Transform from world space to object space (inverse)
	Point3F mObjScale;     ///< Object scale

	const static Box3F csObjBox; ///< default bounding box in object space
	Box3F   mObjBox;       ///< Bounding box in object space
	Box3F   mWorldBox;     ///< Bounding box in world space
	SphereF mWorldSphere;  ///< Bounding sphere in world space

	MatrixF mRenderObjToWorld;    ///< Render matrix to transform object space to world space
	MatrixF mRenderWorldToObj;    ///< Render matrix to transform world space to object space
	Box3F   mRenderWorldBox;      ///< Render bounding box in world space
	SphereF mRenderWorldSphere;   ///< Render bounxing sphere in world space

	bool mIsUseClosestPoint;

	/// Regenerates the world-space bounding box and bounding sphere
	virtual void    resetWorldBox();

	/// Regenerates the render-world-space bounding box and sphere
	void    resetRenderWorldBox();

	SceneObjectRef* mZoneRefHead;
	SceneObjectRef* mBinRefHead;

	U32 mBinMinX;
	U32 mBinMaxX;
	U32 mBinMinY;
	U32 mBinMaxY;

	/// @}

	/// @name Container Interface
	///
	/// When objects are searched, we go through all the zones and ask them for
	/// all of their objects. Because an object can exist in multiple zones, the
	/// container sequence key is set to the id of the current search. Then, while
	/// searching, we check to see if an object's sequence key is the same as the
	/// current search key. If it is, it will NOT be added to the list of returns
	/// since it has already been processed.
	///
	/// @{

	U32  mContainerSeqKey;  ///< Container sequence key

	/// Returns the container sequence key
	U32  getContainerSeqKey() const        { return mContainerSeqKey; }

	/// Sets the container sequence key
	void setContainerSeqKey(const U32 key) { mContainerSeqKey = key;  }
	/// @}

public:

	/// Returns a pointer to the container that contains this object
	Container* getContainer()         { return mContainer;       }
	U32 getLayerID() { return mLayerID; }
	void setLayerID(U32 layerID);

	virtual bool isSelfPlayer(){return false;}
protected:
	S32 mCollisionCount;

	bool mGlobalBounds;

public:
	const bool isGlobalBounds() const
	{
		return mGlobalBounds;
	}

	/// If global bounds are set to be true, then the object is assumed to
	/// have an infinitely large bounding box for collision and rendering
	/// purposes.
	///
	/// They can't be toggled currently.
	void setGlobalFlag()
	{
		mGlobalBounds = true;
	}

	void setGlobalBounds()
	{ 
		if(mContainer)
			mContainer->removeFromBins(this);

		mGlobalBounds = true;
		mObjBox.min.set(-1e10, -1e10, -1e10);
		mObjBox.max.set( 1e10,  1e10,  1e10);

		if(mContainer)
			mContainer->insertIntoBins(this);
	}


	/// @name Rendering Members
	/// @{
protected:
	SceneGraph* mSceneManager;      ///< SceneGraph that controls this object
	U32         mZoneRangeStart;    ///< Start of range of zones this object controls, 0xFFFFFFFF == no zones

	U32         mNumCurrZones;     ///< Number of zones this object exists in

private:
	TraversalState mTraversalState;  ///< State of this object in the SceneGraph traversal - DON'T MESS WITH THIS
	SceneState*    mLastState;       ///< Last SceneState that was used to render this object.
	U32            mLastStateKey;    ///< Last state key that was used to render this object.

	/// @}

	/// @name Persist and console
	/// @{
public:
	static void initPersistFields();
	void inspectPostApply();
	DECLARE_CONOBJECT(SceneObject);

	/// @}

	/// Triggered when a scene object onAdd is called.  Allows other systems to plug into SceneObject
	static Signal<SceneObject*> smSceneObjectAdd;
};


//--------------------------------------------------------------------------
extern Container gServerContainer;
extern Container gClientContainer;

//--------------------------------------------------------------------------
//-------------------------------------- Inlines
//
inline bool SceneObject::isManagingZones() const
{
	return mZoneRangeStart != 0xFFFFFFFF;
}

inline void SceneObject::setLastState(SceneState* state, U32 key)
{
	mLastState    = state;
	mLastStateKey = key;
}

inline bool SceneObject::isLastState(SceneState* state, U32 key) const
{
	return (mLastState == state && mLastStateKey == key);
}

inline void SceneObject::setTraversalState( TraversalState s ) {
	mTraversalState = s;
}

inline SceneObject::TraversalState SceneObject::getTraversalState() const {
	return mTraversalState;
}

inline U32 SceneObject::getCurrZone(const U32 index) const
{
	// Not the most efficient way to do this, walking the list,
	//  but it's an uncommon call...
	SceneObjectRef* walk = mZoneRefHead;
	for (U32 i = 0; i < index; i++) 
	{
		walk = walk->nextInObj;
		AssertFatal(walk!=NULL, "Error, too few object refs!");
	}
	AssertFatal(walk!=NULL, "Error, too few object refs!");

	return walk->zone;
}

//--------------------------------------------------------------------------
inline SceneObjectRef* Container::allocateObjectRef()
{
	if (mFreeRefPool == NULL) 
	{
		addRefPoolBlock();
	}
	AssertFatal(mFreeRefPool!=NULL, "Error, should always have a free reference here!");

	SceneObjectRef* ret = mFreeRefPool;
	mFreeRefPool = mFreeRefPool->nextInObj;

	ret->nextInObj = NULL;
	return ret;
}

inline void Container::freeObjectRef(SceneObjectRef* trash)
{
	trash->object = NULL;
	trash->nextInBin = NULL;
	trash->prevInBin = NULL;
	trash->nextInObj = mFreeRefPool;
	mFreeRefPool     = trash;
}

inline void Container::findObjects(U32 mask, FindCallback callback, void *key, U32 layerID)
{
	for (Link* itr = mStart.next; itr != &mEnd; itr = itr->next) {
		SceneObject* ptr = static_cast<SceneObject*>(itr);
		//if((ptr->getType() & TerrainObjectType))
		//{
		//	int a = 0;
		//}

		if ((ptr->getType() & mask) != 0 && !ptr->mCollisionCount && ( ptr->getLayerID() == layerID || ptr->getLayerID() == 0 ) )
			(*callback)(ptr,key);
	}
}

inline void Container::findObjectsInBin(const Box3F &box, U32 mask, FindCallback callback, void *key, SceneObjectRef* chain)
{
	while (chain && chain->object)
	{
		if (chain->object->getContainerSeqKey() != smCurrSeqKey)
		{
			chain->object->setContainerSeqKey(smCurrSeqKey);

			if ((chain->object->getType() & mask)  && chain->object->isCollisionEnabled())
			{
				if (chain->object->getWorldBox().isOverlapped(box) || chain->object->isGlobalBounds())
				{
					(*callback)(chain->object,key);
				}
			}
		}
		chain = chain->nextInBin;
	}
}

inline void Container::findObjectsInOverflowBin(const Box3F& box, U32 mask, FindCallback callback, void *key)
{
	findObjectsInBin(box, mask, callback, key, mOverflowBin.nextInBin);
}

inline void Container::findObjects(const Box3F& box, U32 mask, FindCallback callback, void *key, U32 layerID)
{
	PROFILE_START(findObjects_box);
	U32 minX, maxX, minY, maxY;
	getBinRange(box.min.x, box.max.x, minX, maxX);
	getBinRange(box.min.y, box.max.y, minY, maxY);
	smCurrSeqKey++;

	//layerID = 0;

	findObjectsInLayer(box, mask, callback, key, layerID, minX, maxX, minY, maxY);
	if (layerID != 0)
		findObjectsInLayer(box, mask, callback, key, 0, minX, maxX, minY, maxY);

	findObjectsInOverflowBin(box, mask, callback, key);

	PROFILE_END(findObjects_box);
}

inline void Container::findObjectsInLayer(const Box3F &box, U32 mask, FindCallback callback, void *key, U32 layerID, U32 minX, U32 maxX, U32 minY, U32 maxY)
{
	if(mBinArrayVec.find(layerID) == mBinArrayVec.end())
		return;
	SceneObjectRef *refInLayerID = mBinArrayVec[layerID];

	for (U32 i = minY; i <= maxY; i++)
	{
		U32 insertY = i % csmNumBins;
		U32 base    = insertY << BinPow;
		for (U32 j = minX; j <= maxX; j++)
		{
			U32 insertX = j % csmNumBins;
			findObjectsInBin(box, mask, callback, key, refInLayerID[base + insertX].nextInBin);
		}
	}
}
#endif  // _H_SCENEOBJECT_

