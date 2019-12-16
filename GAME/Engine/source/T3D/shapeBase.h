//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _SHAPEBASE_H_
#define _SHAPEBASE_H_

#ifndef _GAMEBASE_H_
#include "T3D/gameBase.h"
#endif
#ifndef _MOVEMANAGER_H_
#include "T3D/moveManager.h"
#endif
#ifndef _COLOR_H_
#include "core/color.h"
#endif
#ifndef _CONVEX_H_
#include "collision/convex.h"
#endif
#ifndef _SCENESTATE_H_
#include "sceneGraph/sceneState.h"
#endif
#ifndef _NETSTRINGTABLE_H_
#include "sim/netStringTable.h"
#endif
#ifndef _RENDER_INST_MGR_H_
#include "renderInstance/renderInstMgr.h"
#endif
#include "Effects/EdgeBlur.h"
#include "Gameplay/GameObjects/ShapeStack.h"

#include "util/BackgroundLoadMgr.h"

class GFXCubemap;
class TSShapeInstance;
class SceneState;
class TSShape;
class TSThread;
class GameConnection;
struct CameraScopeQuery;
class ParticleEmitter;
class ParticleEmitterData;
class ProjectileData;
class ExplosionData;
struct DebrisData;
class ShapeBase;
class SFXSource;
class SFXProfile;
class TSShapeInfo;

typedef void* Light;

// <Edit> [3/9/2009 joy] 精简MountedImage
//--------------------------------------------------------------------------

extern void collisionFilter(SceneObject* object,S32 key);
extern void defaultFilter(SceneObject* object,S32 key);


//--------------------------------------------------------------------------
class ShapeBaseConvex : public Convex
{
   typedef Convex Parent;
   friend class ShapeBase;
   friend class Vehicle;
   friend class RigidShape;

  protected:
   ShapeBase*  pShapeBase;
   MatrixF*    nodeTransform;

  public:
   MatrixF*    transform;
   U32         hullId;
   Box3F       box;

  public:
   ShapeBaseConvex() { mType = ShapeBaseConvexType; nodeTransform = 0; }
   ShapeBaseConvex(const ShapeBaseConvex& cv) {
      mObject    = cv.mObject;
      pShapeBase = cv.pShapeBase;
      hullId     = cv.hullId;
      nodeTransform = cv.nodeTransform;
      box        = cv.box;
      transform  = 0;
   }

   void findNodeTransform();
   const MatrixF& getTransform() const;
   Box3F getBoundingBox() const;
   Box3F getBoundingBox(const MatrixF& mat, const Point3F& scale) const;
   Point3F      support(const VectorF& v) const;
   void         getFeatures(const MatrixF& mat,const VectorF& n, ConvexFeature* cf);
   void         getPolyList(AbstractPolyList* list);
};

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/// @nosubgrouping
struct ShapeBaseData : public GameBaseData
{
  private:
   typedef GameBaseData Parent;

public:
   /// Various constants relating to the ShapeBaseData
   enum Constants {
      NumMountPoints = 32,
      NumMountPointBits = 5,
      MaxCollisionShapes = 8,
   };


   bool shadowEnable;
   bool shadowCanMove;
   bool shadowCanAnimate;
   bool shadowSelfShadow;
   U32 shadowSize;
   U32 shadowAnimationFrameSkip;
   F32 shadowMaxVisibleDistance;
   F32 shadowProjectionDistance;
   F32 shadowSphereAdjust;
   F32 shadowBias;
   bool shadowDTSShadow;
   F32 shadowIntensity;


   U32 shapesSetId;
   U32 effectPacketId;
   StringTableEntry  shapeName;
   StringTableEntry  cloakTexName;

   bool dynamicReflection;

   /// @name Physical Properties
   /// @{
   F32 mass;
   F32 drag;
   F32 density;
   F32 maxEnergy;
   F32 maxDamage;
   F32 repairRate;                  ///< Rate per tick.

   F32 disabledLevel;
   F32 destroyedLevel;

   S32 shieldEffectLifetimeMS;
   /// @}

   /// @name 3rd Person Camera
   /// @{
   F32 cameraMaxDist;               ///< Maximum distance from eye
   F32 cameraMinDist;               ///< Minumumistance from eye
   /// @}

   /// @name Camera FOV
   ///
   /// These are specified in degrees.
   /// @{
   F32 cameraDefaultFov;            ///< Default FOV.
   F32 cameraMinFov;                ///< Min FOV allowed.
   F32 cameraMaxFov;                ///< Max FOV allowed.
   /// @}

   /// @name Data initialized on preload
   /// @{

   U32 mCRC;
   bool computeCRC;
   bool bInitialized;               ///< 是否已经初始化

   S32 eyeNode;                         ///< Shape's eye node index
   S32 cameraNode;                      ///< Shape's camera node index
   //S32 mountPointNode[NumMountPoints];  ///< Node index of mountPoint
   //S32 debrisDetail;                    ///< Detail level used to generate debris
   //S32 damageSequence;                  ///< Damage level decals
   //S32 hulkSequence;                    ///< Destroyed hulk

   bool              canControl;             // can this object be controlled?
   bool              canObserve;             // may look at object in commander map?
   bool              observeThroughObject;   // observe this object through its camera transform and default fov

   /// @name HUD
   ///
   /// @note This may be only semi-functional.
   /// @{

   enum {
      NumHudRenderImages      = 8,
   };

   StringTableEntry  hudImageNameFriendly[NumHudRenderImages];
   StringTableEntry  hudImageNameEnemy[NumHudRenderImages];
//   TextureHandle     hudImageFriendly[NumHudRenderImages];
//   TextureHandle     hudImageEnemy[NumHudRenderImages];

   bool              hudRenderCenter[NumHudRenderImages];
   bool              hudRenderModulated[NumHudRenderImages];
   bool              hudRenderAlways[NumHudRenderImages];
   bool              hudRenderDistance[NumHudRenderImages];
   bool              hudRenderName[NumHudRenderImages];
   /// @}

   /// @name Collision Data
   /// @{
   Vector<S32>   collisionDetails;  ///< Detail level used to collide with.
                                    ///
                                    /// These are detail IDs, see TSShape::findDetail()
   Vector<Box3F> collisionBounds;   ///< Detail level bounding boxes.

   Vector<S32>   LOSDetails;        ///< Detail level used to perform line-of-sight queries against.
                                    ///
                                    /// These are detail IDs, see TSShape::findDetail()
   /// @}

   /// @name Misc. Settings
   /// @{
   bool emap;                       ///< Enable environment mapping?
   bool firstPersonOnly;            ///< Do we allow only first person view of this image?
   bool useEyePoint;                ///< Do we use this object's eye point to view from?
   bool aiAvoidThis;                ///< If set, the AI's will try to walk around this object...
                                    ///
                                    ///  @note There isn't really any AI code that can take advantage of this.
   bool isInvincible;               ///< If set, object cannot take damage (won't show up with damage bar either)
   bool renderWhenDestroyed;        ///< If set, will not render this object when destroyed.

   bool inheritEnergyFromMount;

   /// @}

   bool preload(bool server, char errorBuffer[256]);
   void computeAccelerator(U32 i);
   S32  findMountPoint(U32 n);

   /// @name Infrastructure
   /// The derived class should provide the following:
   /// @{
   DECLARE_CONOBJECT(ShapeBaseData);
   ShapeBaseData();
   ~ShapeBaseData();
   static void initPersistFields();
   virtual void packData(BitStream* stream);
   virtual void unpackData(BitStream* stream);
   virtual bool initDataBlock();
   /// @}
};

//----------------------------------------------------------------------------

/// ShapeBase is the renderable shape from which most of the scriptable objects
/// are derived, including the player, vehicle and items classes.  ShapeBase
/// provides basic shape loading, audio channels, and animation as well as damage
/// (and damage states), energy, and the ability to mount images and objects.
///
/// @nosubgrouping
class ShapeBase : public GameBase,public CBackgroundBase
{
   typedef GameBase Parent;
   friend class ShapeBaseConvex;
   friend struct ShapeBaseImageData;
   friend void physicalZoneFind(SceneObject*, void *);
    
public:
   enum PublicConstants {
      ThreadSequenceBits = 6,
      MaxSequenceIndex = (1 << ThreadSequenceBits) - 1,
      EnergyLevelBits = 5,
      DamageLevelBits = 6,
      DamageStateBits = 2,
      // The thread and image limits should not be changed without
      // also changing the ShapeBaseMasks enum values declared
      // further down.
      MaxSoundThreads =  4,            ///< Should be a power of 2
      MaxScriptThreads = 4,            ///< Should be a power of 2
	  MaxMountedImages = ShapesSet::MaxMountedImages,            ///< Should be a power of 2
      NumImageBits = 4,
      MountPointBits = 6,
      ShieldNormalBits = 8,
      CollisionTimeoutValue = 250      ///< Timeout in ms.
   };

   /// This enum indexes into the sDamageStateName array
   enum DamageState {
      Enabled,
      Disabled,
      NumDamageStates,
      NumDamageStateBits = 2,   ///< Should be log2 of the number of states.
   };

   enum EdgeBlur
   {
	   EB_Left_A = 0,
	   EB_Left_B,
	   EB_Right_A,
	   EB_Right_B,
	   MaxEdgeBlur,
	   EdgeLinkpoints = MaxEdgeBlur<<1,

	   EdgeBlurPts = 16,
   };

private:
   ShapeBaseData*    mDataBlock;                ///< Datablock
   //GameConnection*   mControllingClient;        ///< Controlling client
   ShapeBase*        mControllingObject;        ///< Controlling object
   bool              mTrigger[MaxTriggerKeys];  ///< What triggers are set, if any.
   StringTableEntry  mObjectName;				// 名称
   NetStringHandle   mEdgeBlurTextureName;		// 拖尾贴图名称

protected:
	CEdgeBlur* mEdgeBlur[MaxEdgeBlur]; // <Edit> [10/23/2009 joy] 拖尾特效
	S32 mEdgePts[EdgeLinkpoints];

	// effects
	U32 mShapeEPItem;

	// FluidLight effects
	S32 mFluidLight;

	// ShapeShifting
	bool mShapeShifting;

	SphereF mShadowSphere;

   /// @name Mounted Images
   /// @{

   /// An image mounted on a shapebase.
   struct MountedImage 
   {
	  StringTableEntry  shapeName;      ///< Name of shape to render.
	  StringTableEntry  mountPoint;     ///< Mount point for the image.
	  StringTableEntry  mountPointSelf; ///< Mount point for the image itself.
      NetStringHandle   skinNameHandle;

	  //Resource<TSShape> shape;        ///< Shape handle
	  U32     mCRC;                     ///< Checksum of shape.
	  bool    computeCRC;               ///< Should the shape's CRC be checked?
	  S32     mountNode;                ///< Mount node ID. 这是image自身上的链接点
	  MatrixF mountTransform;           ///< Transformation to get to the mountNode.

      /// @name 3space
      ///
      /// Handles to threads and shapeinstances in the 3space system.
      /// @{
      TSShapeInstance* shapeInstance;
      TSThread *animThread;
      /// @}
      bool isAnimated;                 ///< This image contains at least one animated states
      bool isUpper;                    ///< This image bounds Upper Animation
      bool isRender;                   ///< This image need render

	  // effects
	  U32 imageEPItem;

      //
      MountedImage();
      ~MountedImage();
   };

   MountedImage mMountedImageList[MaxMountedImages];

   /// @}

   /// @name Render settings
   /// @{

   TSShapeInfo*     mTSSahpeInfo;
   TSShapeInstance* mShapeInstance;
   Convex *         mConvexList;
   char				mSkinTag;	//用来标注是换第几套贴图
   F32				mLastZ;

   NetStringHandle mShapeNameHandle;   ///< Name sent to client
   /// @}

   /// @name Physical Properties
   /// @{
   F32 mMass;                       ///< Mass.
   F32 mOneOverMass;                ///< Inverse of mass.
                                    /// @note This is used to optimize certain physics calculations.
   /// @}

   /// @name Physical Properties
   ///
   /// Properties for the current object, which are calculated
   /// based on the container we are in.
   ///
   /// @see ShapeBase::updateContainer()
   /// @see ShapeBase::mContainer
   /// @{
   F32 mDrag;                       ///< Drag.
   F32 mBuoyancy;                   ///< Buoyancy factor.
   U32 mLiquidType;                 ///< Type of liquid (if any) we are in.
   F32 mLiquidHeight;               ///< Height of liquid around us (from 0..1).
   F32 mWaterCoverage;              ///< Percent of this object covered by water

   Point3F mAppliedForce;
   F32 mGravityMod;
   /// @}

   bool mFlipFadeVal;
   U32 mLightTime;

   /// Mounted objects
   struct MountInfo 
   {
      ShapeBase* list;              ///< Objects mounted on this object
      ShapeBase* object;            ///< Object this object is mounted on.
      ShapeBase* link;              ///< Link to next object mounted to this object's mount
      StringTableEntry node;        ///< Node point we are mounted to.
      StringTableEntry myNode;
   } mMount;
    
private:
   //记录当前正在加载的模型
   ShapesSet    m_loadingModels;

   //是否已经全部加载完成
   bool         m_isLoading;
    
   void __LoadModel(const char* model);
   bool __CheckModelLoadings(void);
 public:
   ShapeStack mShapeStack;

   /// @name Collision Notification
   /// This is used to keep us from spamming collision notifications. When
   /// a collision occurs, we add to this list; then we don't notify anyone
   /// of the collision until the CollisionTimeout expires (which by default
   /// occurs in 1/10 of a second).
   ///
   /// @see notifyCollision(), queueCollision()
   /// @{
   struct CollisionTimeout {
      CollisionTimeout* next;
      ShapeBase* object;
      U32 objectNumber;
      SimTime expireTime;
      VectorF vector;
   };
   CollisionTimeout* mTimeoutList;
   static CollisionTimeout* sFreeTimeoutList;

   /// Go through all the items in the collision queue and call onCollision on them all
   /// @see onCollision
   void notifyCollision();

   /// This gets called when an object collides with this object
   /// @param   object   Object colliding with this object
   /// @param   vec   Vector along which collision occured
   virtual void onCollision(ShapeBase* object, VectorF vec);

   /// Add a collision to the queue of collisions waiting to be handled @see onCollision
   /// @param   object   Object collision occurs with
   /// @param   vec      Vector along which collision occurs
   void queueCollision(ShapeBase* object, const VectorF& vec);

   /// @}
 protected:

   /// @name Damage
   /// @{
   DamageState mDamageState;
   /// @}

   /// @name Cloaking
   /// @{
   bool mCloaked;
   F32  mCloakLevel;
//   TextureHandle mCloakTexture;
   bool mHidden; ///< in/out of world

   /// @}

protected:

   /// @name Control info
   /// @{
   F32  mCameraFov;           ///< Camera FOV(in degrees)
   bool mIsControlled;        ///< Client side controlled flag

   /// @}
protected:

   U32 mLastRenderFrame;
   F32 mLastRenderDistance;
   U32 mSkinHash;


   /// This recalculates the total mass of the object, and all mounted objects
   void updateMass();

   /// @name Image Manipulation
   /// @{

   /// Assign a ShapeBaseImage to an image slot
   /// @param   imageSlot   Image Slot ID
   /// @param   shapeName   TSShape Data Id
   /// @param   skinNameHandle Skin texture name
   /// @param   mountPoint  Mount point for the image.
   /// @param   mountPointSelf Mount point for the image itself.
   virtual void setImage(U32 imageSlot, StringTableEntry shapeName, NetStringHandle &skinNameHandle,
                         StringTableEntry mountPoint, StringTableEntry mountPointSelf);

   virtual void refreshImageAction(U32 imageSlot) {}

   /// Clear out an image slot
   /// @param   imageSlot   Image slot id
   void resetImageSlot(U32 imageSlot);

   /// Advance animation on a image
   /// @param   imageSlot   Image slot id
   /// @param   dt          Change in time since last animation update
   void updateImageAnimation(U32 imageSlot, F32 dt);

   /// @}

   /// Prune out non looping sounds from the sound manager which have expired
   void updateServerAudio();

   /// Recalculates the spacial sound based on the current position of the object
   /// emitting the sound.
   void updateAudioPos();

   /// Update bouyency and drag properties
   void updateContainer();

   /// @name Events
   /// @{
   virtual void onDeleteNotify(SimObject*);
   virtual void blowUp();
   virtual void onMount(ShapeBase* obj,StringTableEntry node);
   virtual void onUnmount(ShapeBase* obj,StringTableEntry node);
   virtual void onImpact(SceneObject* obj, VectorF vec);
   virtual void onImpact(VectorF vec);
   /// @}

   virtual void updateEffectObjects(){};

public:
   ShapeBase();
   ~ShapeBase();

#ifdef USE_MULTITHREAD_ANIMATE
   void deleteObject();
#endif

   TSShapeInstance* getShapeInstance() { return mShapeInstance; }
   const inline TSShapeInfo* getTSSahpeInfo() { return mTSSahpeInfo;}

   /// @name Network state masks
   /// @{

   /// updataMask
   const static U64 NameMask        = Parent::NextFreeMask;
   const static U64 DamageMask      = Parent::NextFreeMask << 1;
   const static U64 NoWarpMask      = Parent::NextFreeMask << 2;
   const static U64 MountedMask     = Parent::NextFreeMask << 3;
   const static U64 CloakMask       = Parent::NextFreeMask << 4;
   const static U64 SkinMask        = Parent::NextFreeMask << 5;
   const static U64 ImageMaskN      = Parent::NextFreeMask << 6;  ///< Extends + MaxMountedImage bits
   const static U64 NextFreeMask    = ImageMaskN  << MaxMountedImages;

   enum BaseMaskConstants {
      ImageMask      = (ImageMaskN << MaxMountedImages) - ImageMaskN
   };

   /// @}

   static bool gRenderEnvMaps; ///< Global flag which turns on or off all environment maps
   static U32  sLastCtrlSimId;

   virtual GFXCubemap * getDynamicCubemap(){return mDynamicCubemap;}
   GFXCubemap * mDynamicCubemap;

   /// @name Initialization
   /// @{

   bool onAdd();
   void onRemove();
   void onSceneRemove();
   static void consoleInit();
   bool onNewDataBlock(GameBaseData* dptr);
   
   bool onChangeDataBlock(GameBaseData* dptr);
	void setEdgeBlurTexture(StringTableEntry textName);
	void setEdgeBlurTexture(NetStringHandle& newtext);
	bool addEPItems(S32 slot, U32 id);
	bool addEPItems(S32 slot, U32 id, U32 index);
	void activeEPItems();
	void addFluidLight(S32 count);

   /// @}
   void __CreateShapeInstance(void);
   void __InitShapeInstance();
   void updateShapes();
   virtual bool updateShape();
   virtual bool setShapeShifting(bool val);
   inline bool getShapeShifting() { return mShapeShifting;}
   /// @name Name & Skin tags
   /// @{
   // ObjectName可作为固定名称，ShapeName是可变名称
   void setObjectName(const char*);
   const char* getObjectName();
   void setShapeName(const char*);
   const char* getShapeName();
   void setSkinTag(const char);
   const char getSkinTag();
   /// @}

   /// @name Basic attributes
   /// @{
   virtual void onEnabled() {}
   virtual void onDisabled() {}

   /// Changes the object's damage state.
   /// @param   state   New state of the object
   void setDamageState(DamageState state);

   /// Changes the object's damage state, based on a named state.
   /// @see setDamageState
   /// @param   state   New state of the object as a string.
   bool setDamageState(const char* state);

   /// Returns the name of the current damage state as a string.
   const char* getDamageStateName();

   /// Returns the current damage state.
   DamageState getDamageState() { return mDamageState; }

   inline bool isEnabled() { return mDamageState == Enabled; }
   inline bool isDisabled() { return mDamageState != Enabled; }
   /// @}

   void    resetWorldBox()
   {   
	   Parent::resetWorldBox();
#ifdef NTJ_CLIENT
	   resetShadowSphere();
#endif
   }

   void resetShadowSphere();
   const SphereF& getShadowSphere() const;

   /// @name Cloaking
   /// @{

   /// Force uncloaking of object
   /// @param   reason   Reason this is being forced to uncloak, this is passed directly to script control
   void forceUncloak(const char *reason);

   /// Set cloaked state of object
   /// @param   cloaked   True if object is cloaked
   void setCloakedState(bool cloaked);

   /// Returns true if object is cloaked
   bool getCloakedState();

   /// Returns level of cloaking, as it's not an instant "now you see it, now you don't"
   F32 getCloakLevel();
   /// @}

   /// @name Mounted objects
   /// @{

   /// Mount an object to a mount point
   /// @param   obj   Object to mount
   /// @param   node   Mount node ID
   virtual void mountObject(ShapeBase* obj,StringTableEntry node, StringTableEntry myNode = 0);

   /// Remove an object mounting
   /// @param   obj   Object to unmount
   void unmountObject(ShapeBase *obj);

   /// Unmount this object from it's mount
   void unmount();

   /// Return the object that this object is mounted to
   ShapeBase* getObjectMount()  { return mMount.object; }
   inline ShapeBase* getRootMount() const;

   /// Return object link of next object mounted to this object's mount
   ShapeBase* getMountLink()  { return mMount.link; }

   /// Returns the list of things mounted along with this object
   ShapeBase* getMountList()  { return mMount.list; }

   /// Returns the mount id that this is mounted to
   StringTableEntry getMountNode()  { return mMount.node; }
   StringTableEntry getMountMyNode()  { return mMount.myNode; }

   /// Returns true if this object is mounted to anything at all
   bool isMounted() { return mMount.object != 0; }

   /// Returns the number of object mounted along with this
   S32 getMountedObjectCount();

   /// Returns the position of object mounted along with this
   S32 getMountedObjectIndex(ShapeBase* pObj);

   /// Return the position that this object is mounted to
   S32 getMountIndex() const;

   /// Returns the object mounted at a position in the mount list
   /// @param   idx   Position on the mount list
   ShapeBase* getMountedObject(S32 idx);

   /// Returns the node the object at idx is mounted to
   /// @param   idx   Index
   StringTableEntry getMountedObjectNode(S32 idx);

   /// Returns the object a object on the mount list is mounted to
   /// @param   node
   ShapeBase* getMountNodeObject(StringTableEntry node);

   /// @}

   /// @name Mounted Images
   /// @{

   /// Mount an image (ShapeBaseImage) onto an image slot
   /// @param   shapeName TSShape Data Id
   /// @param   imageSlot Image mount point
   /// @param   skinNameHandle   Skin name for object
   /// @param   mountPoint     Mount point for the image.
   /// @param   mountPointSelf Mount point for the image itself.
   virtual bool mountImage(StringTableEntry shapeName,U32 imageSlot,NetStringHandle &skinNameHandle,StringTableEntry mountPoint, StringTableEntry mountPointSelf);

   /// Unmount an image from a slot
   /// @param   imageSlot   Mount point
   virtual bool unmountImage(U32 imageSlot);

   /// Gets the information on the image mounted in a slot
   /// @param   imageSlot   Mount point
   StringTableEntry getMountedImage(U32 imageSlot);

   /// Gets the mounted image on on a slot
   /// @param   imageSlot   Mount Point
   MountedImage* getImageStruct(U32 imageSlot);

   TSShapeInstance* getImageShapeInstance(U32 imageSlot)
   {
      const MountedImage &image = mMountedImageList[imageSlot];
      if(image.shapeName && image.shapeInstance)
         return image.shapeInstance;
      return NULL;
   }

   /// Returns true if the specified image is mounted
   /// @param   image   ShapeBase image
   bool isImageMounted(StringTableEntry shapeName);

   /// Returns the slot which the image specified is mounted on
   /// @param   image   Image to test for
   S32 getMountSlot(StringTableEntry shapeName);

   /// Returns the skin for the image in a slot
   /// @param   imageSlot   Image slot to get the skin from
   NetStringHandle getImageSkinTag(U32 imageSlot);

   /// Returns true if the image need render
   /// @param   imageSlot   Image slot
   virtual bool isRenderImage(U32 imageSlot) { return true;}
   virtual bool isEdgeBlurPause() { return true;}
   void hideImage();

   /// @}

   /// @name Transforms
   /// @{
   Point3F getPosition() const;

   /// Gets the minimum viewing distance, maximum viewing distance, camera offsetand rotation
   /// for this object, if the world were to be viewed through its eyes
   /// @param   min   Minimum viewing distance
   /// @param   max   Maximum viewing distance
   /// @param   offset Offset of the camera from the origin in local space
   /// @param   rot   Rotation matrix
   virtual void getCameraParameters(F32 *min,F32* max,Point3F* offset,MatrixF* rot);

   /// Gets the camera transform
   /// @todo Find out what pos does
   /// @param   pos   TODO: Find out what this does
   /// @param   mat   Camera transform (out)
   virtual void getCameraTransform(F32* pos,MatrixF* mat);

   /// Gets the index of a node inside a mounted image given the name
   /// @param   imageSlot   Image slot
   /// @param   nodeName    Node name
   S32 getNodeIndex(U32 imageSlot,StringTableEntry nodeName);

   /// @}

   /// @name Object Transforms
   /// @{

   /// Returns the eye transform of this shape, IE the eyes of a player
   /// @param   mat   Eye transform (out)
   virtual void getEyeTransform(MatrixF* mat);

   /// Mount point to world space transform
   /// @param   mountPoint   Mount point
   /// @param   mat    mount point transform (out)
   virtual void getMountTransform(StringTableEntry mountPoint,MatrixF* mat);

   /// Mount point to local space transform
   /// @param   mountPoint   Mount point
   /// @param   mat    mount point transform (out)
   virtual void getLocalMountTransform(StringTableEntry mountPoint,MatrixF* mat);

   /// Gets the transform of a mounted image in world space
   /// @param   imageSlot   Image slot
   /// @param   mat    Transform (out)
   virtual void getImageTransform(U32 imageSlot,MatrixF* mat);

   /// Gets the transform of a node on a mounted image in world space
   /// @param   imageSlot   Image Slot
   /// @param   node    node on image
   /// @param   mat   Transform (out)
   virtual void getImageTransform(U32 imageSlot,S32 node, MatrixF* mat);

   /// Gets the transform of a node on a mounted image in world space
   /// @param   imageSlot   Image Slot
   /// @param   nodeName    Name of node on image
   /// @param   mat         Transform (out)
   virtual void getImageTransform(U32 imageSlot, StringTableEntry nodeName, MatrixF* mat);

   ///@}

   /// @name Render transforms
   /// Render transforms are different from object transforms in that the render transform of an object
   /// is where, in world space, the object is actually rendered. The object transform is the
   /// absolute position of the object, as in where it should be.
   ///
   /// The render transforms typically vary from object transforms due to client side prediction.
   ///
   /// Other than that, these functions are identical to their object-transform counterparts
   ///
   /// @note These are meaningless on the server.
   /// @{
   virtual void getRenderMountTransform(StringTableEntry index,MatrixF* mat);
   virtual void getRenderMountTransform_(StringTableEntry index,MatrixF* mat);
   virtual void getRenderNodeTransform(StringTableEntry nodeName,MatrixF* mat);
   virtual void getRenderNodeTransform(S32 node,MatrixF* mat);
   virtual void getRenderImageTransform(U32 imageSlot,MatrixF* mat);
   virtual void getRenderImageTransform(U32 index,S32 node, MatrixF* mat);
   virtual void getRenderImageTransform(U32 index, StringTableEntry nodeName, MatrixF* mat);
   virtual void getRenderEyeTransform(MatrixF* mat);
   /// @}


   /// @name Movement & velocity
   /// @{

   /// Sets the velocity of this object
   /// @param   vel   Velocity vector
   virtual void setVelocity(const VectorF& vel);

   /// Applies an impulse force to this object
   /// @param   pos   Position where impulse came from in world space
   /// @param   vec   Velocity vector (Impulse force F = m * v)
   virtual void applyImpulse(const Point3F& pos,const VectorF& vec);

   /// @}

   /// @name Cameras and Control
   /// @{

   /// Assigns this object a controling client
   /// @param   client   New controling client
   //virtual void setControllingClient(GameConnection* client);

   /// Returns the client controling this object
   //GameConnection* getControllingClient() { return mControllingClient; }

   /// Returns the object controling this object
   ShapeBase* getControllingObject()   { return mControllingObject; }

   /// Sets the controling object
   /// @param   obj   New controling object
   virtual void setControllingObject(ShapeBase* obj);

   /// Returns the object this is controling
   virtual ShapeBase* getControlObject();

   /// sets the object this is controling
   /// @param   obj   New controlled object
   virtual void setControlObject(ShapeBase *obj);

   /// Returns true if this object is controled by something
   bool isControlled() { return(mIsControlled); }

   /// Returns true if this object is being used as a camera in first person
   bool isFirstPerson();

   /// Returns true if the camera uses this objects eye point (defined by modeler)
   bool useObjsEyePoint() const;

   /// Returns true if this object can only be used as a first person camera
   bool onlyFirstPerson() const;

   /// Returns the Field of Vision for this object if used as a camera
   virtual F32 getCameraFov();

   /// Returns the default FOV if this object is used as a camera
   virtual F32 getDefaultCameraFov();

   /// Sets the FOV for this object if used as a camera
   virtual void setCameraFov(F32 fov);

   /// Returns true if the FOV supplied is within allowable parameters
   /// @param   fov   FOV to test
   virtual bool isValidCameraFov(F32 fov);
   /// @}


   void processTick(const Move *move);
   void advanceTime(F32 dt);

   /// @name Rendering
   /// @{

   /// Returns the renderable shape of this object
   TSShape const* getShape();

   bool prepRenderImage(SceneState* state, const U32 stateKey, const U32 startZone, const bool modifyBaseZoneState);
   virtual void prepBatchRender( SceneState *state, S32 mountedImageIndex );
   void renderObject( SceneState *state, RenderInst * );
   void renderShadow( SceneState *state);

   void setShadowVariables(sgShadowProjector * );
   void renderShadowBuff(sgShadowProjector *pShadow ,MatrixF &prj,MatrixF &lightSpace,MatrixF &worldToLightY,MatrixF &shadowscale);
   bool shadowTest(SceneState *);

   /// Renders a mounted object
   /// @param   state   State of scene
   /// @param   ri      RenderInst data
   virtual void renderMountedImage(SceneState* state, RenderInst *ri){};

   virtual void renderImage(SceneState* state){};

   /// Renders the shadow for this object
   /// @param   dist   Distance away from object shadow is rendering on
   /// @param   fogAmount  Amount of fog present
   //void renderShadow(F32 dist, F32 fogAmount);

   /// Draws a wire cube at any point in space with specified parameters
   /// @param   size   Length, width, depth
   /// @param   pos    xyz position in world space
   static void wireCube(const Point3F& size, const Point3F& pos);

   /// This is a callback for objects that have reflections and are added to
   /// the "reflectiveSet" SimSet.
   virtual void updateReflection();

   /// Preprender logic
   virtual void calcClassRenderData();
   /// @}

   /// Control object scoping
   void onCameraScopeQuery(NetConnection *cr, CameraScopeQuery *camInfo);

   /// @name Collision
   /// @{

   /// Casts a ray from start to end, stores gathered information in 'info' returns true if successful
   /// @param   start   Start point for ray
   /// @param   end     End point for ray
   /// @param   info    Information from raycast (out)
   bool castRay(const Point3F &start, const Point3F &end, RayInfo* info);

   /// Builds a polylist of the polygons in this object returns true if successful
   /// @param   polyList   Returned polylist (out)
   /// @param   box        Not used
   /// @param   sphere     Not used
   bool buildPolyList(AbstractPolyList* polyList, const Box3F &box, const SphereF& sphere);

   /// Builds a convex hull for this object @see Convex
   /// @param   box   Bounding box
   /// @param   convex  New convex hull (out)
   void buildConvex(const Box3F& box, Convex* convex);

   /// @}

   /// @name Rendering
   /// @{

   /// Returns true if the last frame calculated rendered
   bool didRenderLastRender() { return mLastRenderFrame == SceneGraph::smFrameKey; }

   /// Sets the state of this object as hidden or not. If an object is hidden
   /// it is removed entirely from collisions, it is not ghosted and is
   /// essentially "non existant" as far as simulation is concerned.
   /// @param   hidden   True if object is to be hidden
   //virtual void setHidden(bool hidden);

   /// Returns true if this object is hidden
   /// @see setHidden
   //bool isHidden()   { return mHidden; }

   /// Returns true if this object can be damaged
   bool isInvincible();

   /// Start fade of object in/out
   /// @param   fadeTime Time fade should take
   /// @param   fadeDelay Delay before starting fade
   /// @param   fadeOut   True if object is fading out, false if fading in.
   //void startFade( F32 fadeTime, F32 fadeDelay = 0.0, bool fadeOut = true );

   /// Returns true if the point specified is in the water
   /// @param   point    Point to test in world space
   bool pointInWater( Point3F &point );

   /// Returns the percentage of this object covered by water
   F32 getWaterCoverage()  { return mWaterCoverage; }

   /// Returns the height of the liquid on this object
   F32 getLiquidHeight()  { return mLiquidHeight; }

   /// @name Network
   /// @{

   F32 getUpdatePriority(CameraScopeQuery *focusObject, U32 updateMask, S32 updateSkips);
   U64  packUpdate(NetConnection *conn, U64 mask, BitStream *stream);
   void unpackUpdate(NetConnection *conn, BitStream *stream);
   void writePacketData(GameConnection *conn, BitStream *stream);
   void readPacketData(GameConnection *conn, BitStream *stream);

   /// @}

#ifdef NTJ_CLIENT
   //Ray: 客户端游戏对象管理入口
   virtual void onAddToClient(){};
   virtual void onRemoveFromClient(){};
#endif

   DECLARE_CONOBJECT(ShapeBase);
};


//------------------------------------------------------------------------------
// inlines
//------------------------------------------------------------------------------

inline bool ShapeBase::getCloakedState()
{
   return(mCloaked);
}

inline F32 ShapeBase::getCloakLevel()
{
   return(mCloakLevel);
}

inline const char* ShapeBase::getShapeName()
{
   return mShapeNameHandle.getString();
}

inline const char ShapeBase::getSkinTag()
{
   return mSkinTag;
}

inline void ShapeBase::setObjectName(const char* name)
{
	mObjectName = StringTable->insert(name);
}

inline const char* ShapeBase::getObjectName()
{
	if(mShapeNameHandle.isValidString())
		return getShapeName();
	else
		return mObjectName;
}

#endif  // _H_SHAPEBASE_
