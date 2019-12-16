//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _TSSHAPE_H_
#define _TSSHAPE_H_

#ifndef _TSMESH_H_
#include "ts/tsMesh.h"
#endif
#ifndef _TSDECAL_H_
#include "ts/tsDecal.h"
#endif
#ifndef _TSINTEGERSET_H_
#include "ts/tsIntegerSet.h"
#endif
#ifndef _TSTRANSFORM_H_
#include "ts/tsTransform.h"
#endif
#ifndef _TSSHAPEALLOC_H_
#include "ts/tsShapeAlloc.h"
#endif
#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif
#ifndef _RESMANAGER_H_
#include "core/resManager.h"
#endif
#ifndef _MMATH_H_
#include "math/mMath.h"
#endif
#ifndef _STREAM_H_
#include "core/stream.h"
#endif

#include "materials/materialList.h"


#define DTS_EXPORTER_CURRENT_VERSION 124

class TSMaterialList;
class TSLastDetail;


/// TSShape stores generic data for a 3space model.
///
/// TSShape and TSShapeInstance act in conjunction to allow the rendering and
/// manipulation of a three dimensional model.
///
/// @note The material lists are the only thing that is not loaded in TSShape.
/// instead, they are loaded in TSShapeInstance because of a former restriction
/// on the resource manager where only one file could be opened at a time.
/// The resource manager restriction has been resolved, but the material
/// lists are still loaded in TSShapeInstance.
///
/// @see TSShapeInstance for a further discussion of the 3space system.
class TSShape : public ResourceInstance
{
  public:
      enum {
         UniformScale   = BIT(0),
         AlignedScale   = BIT(1),
         ArbitraryScale = BIT(2),
         Blend          = BIT(3),
         Cyclic         = BIT(4),
         MakePath       = BIT(5),
         IflInit        = BIT(6),
         HasTranslucency= BIT(7),
         AnyScale       = UniformScale | AlignedScale | ArbitraryScale
      };

   /// Nodes hold the transforms in the shape's tree.  They are the bones of the skeleton.
   struct Node
   {
      S32 nameIndex;
      S32 parentIndex;

      // computed at runtime
      S32 firstObject;
      S32 firstChild;
      S32 nextSibling;
   };

   /// Objects hold renderable items (in particular meshes).
   ///
   /// Each object has a number of meshes associated with it.
   /// Each mesh corresponds to a different detail level.
   ///
   /// meshIndicesIndex points to numMeshes consecutive indices
   /// into the meshList and meshType vectors.  It indexes the
   /// meshIndexList vector (meshIndexList is merely a clearinghouse
   /// for the object's mesh lists).  Some indices may correspond to
   /// no mesh -- which means no mesh will be drawn for the part for
   /// the given detail level.  See comments on the meshIndexList
   /// for how null meshes are coded.
   ///
   /// @note Things are stored this way so that there are no pointers.
   ///       This makes serialization to disk dramatically simpler.
   struct Object
   {
      S32 nameIndex;
      S32 numMeshes;
      S32 startMeshIndex; ///< Index into meshes array.
      S32 nodeIndex;

      // computed at load
      S32 nextSibling;
      S32 firstDecal; // DEPRECATED
   };

   /// IFL Materials are used to animate material lists -- i.e., run through a series
   /// of frames of a material.
   ///
   /// They work by replacing a material in the material
   /// list so that it is transparent to the rest of the code.
   /// Offset time of each frame is stored in iflFrameOffsets vector, starting at index position
   /// firstFrameOffsetIndex..
   struct IflMaterial
   {
      S32 nameIndex; ///< File name with extension.
      S32 materialSlot;
      S32 firstFrame;
      S32 firstFrameOffTimeIndex;
      S32 numFrames;
   };

   /// A Sequence holds all the information necessary to perform a particular animation (sequence).
   ///
   /// Sequences index a range of keyframes. Keyframes are assumed to be equally spaced in time.
   ///
   /// Each node and object is either a member of the sequence or not.  If not, they are set to
   /// default values when we switch to the sequence unless they are members of some other active sequence.
   /// Blended sequences "add" a transform to the current transform of a node.  Any object animation of
   /// a blended sequence over-rides any existing object state.  Blended sequences are always
   /// applied after non-blended sequences.
   struct Sequence
   {
      S32 nameIndex;
      S32 numKeyframes;
      F32 duration;
	  S32 defRotation;
	  S32 defTranslation;
      S32 baseRotation;
      S32 baseTranslation;
      S32 baseScale;
      S32 baseObjectState;
      S32 baseDecalState; // DEPRECATED
      S32 firstGroundFrame;
      S32 numGroundFrames;
      S32 firstTrigger;
      S32 numTriggers;
      F32 toolBegin;

	  /// @name Bitsets
      /// These bitsets code whether this sequence cares about certain aspects of animation
      /// e.g., the rotation, translation, or scale of node transforms,
      /// or the visibility, frame or material frame of objects.
      /// @{

      TSIntegerSet rotationMatters;     ///< Set of nodes
      TSIntegerSet translationMatters;  ///< Set of nodes
      TSIntegerSet scaleMatters;        ///< Set of nodes
      TSIntegerSet visMatters;          ///< Set of objects
      TSIntegerSet frameMatters;        ///< Set of objects
      TSIntegerSet matFrameMatters;     ///< Set of objects
      TSIntegerSet decalMatters;        ///< DEPRECATED
      TSIntegerSet iflMatters;          ///< Set of IFLs
      /// @}

      S32 priority;
      U32 flags;
      U32 dirtyFlags; ///< determined at load time

      /// @name Flag Tests
      /// Each of these tests a different flag against the object's flag list
      /// to determine the attributes of the given object.
      /// @{

      inline bool testFlags(U32 comp) const      { return (flags&comp)!=0; }
      inline bool animatesScale() const          { return testFlags(AnyScale); }
      inline bool animatesUniformScale() const   { return testFlags(UniformScale); }
      inline bool animatesAlignedScale() const   { return testFlags(AlignedScale); }
      inline bool animatesArbitraryScale() const { return testFlags(ArbitraryScale); }
      inline bool isBlend() const                { return testFlags(Blend); }
      inline bool isCyclic() const               { return testFlags(Cyclic); }
      inline bool makePath() const               { return testFlags(MakePath); }
      /// @}

      /// @name IO
      /// @{

      void read(Stream *, bool readNameIndex = true);
      void write(Stream *, bool writeNameIndex = true);
      /// @}
   };

   /// Describes state of an individual object.  Includes everything in an object that can be
   /// controlled by animation.
   struct ObjectState
   {
      F32 vis;
      S32 frameIndex;
      S32 matFrameIndex;
   };

   /// When time on a sequence advances past a certain point, a trigger takes effect and changes
   /// one of the state variables to on or off. (State variables found in TSShapeInstance::mTriggerStates)
   struct Trigger
   {
      enum TriggerStates {
         StateOn = BIT(31),
         InvertOnReverse = BIT(30),
         StateMask = BIT(30)-1,

		 State_FootLeft			= 1,
		 State_FootRight		= 2,
		 State_AttackNormal		= 5,
		 State_AttackSpell		= 10,
      };

      U32 state; ///< One of TriggerStates
      F32 pos;
   };

   /// Details are used for render detail selection.
   ///
   /// As the projected size of the shape changes,
   /// a different node structure can be used (subShape) and a different objectDetail can be selected
   /// for each object drawn.   Either of these two parameters can also stay constant, but presumably
   /// not both.  If size is negative then the detail level will never be selected by the standard
   /// detail selection process.  It will have to be selected by name.  Such details are "utility
   /// details" because they exist to hold data (node positions or collision information) but not
   /// normally to be drawn.  By default there will always be a "Ground" utility detail.
   struct Detail
   {
      S32 nameIndex;
      S32 subShapeNum;
      S32 objectDetailNum;
      F32 size;
      F32 averageError;
      F32 maxError;
      S32 polyCount;
   };

   /// @name Collision Accelerators
   ///
   /// For speeding up buildpolylist and support calls.
   /// @{
   struct ConvexHullAccelerator {
      S32      numVerts;
      Point3F* vertexList;
      Point3F* normalList;
      U8**     emitStrings;
   };
   ConvexHullAccelerator* getAccelerator(S32 dl);
   /// @}


   /// @name Shape Vector Data
   /// @{

   ToolVector<Node> nodes;
   ToolVector<Object> objects;
   ToolVector<IflMaterial> iflMaterials;
   ToolVector<S32> subShapeFirstNode;
   ToolVector<S32> subShapeFirstObject;
   ToolVector<S32> detailFirstSkin;
   ToolVector<S32> subShapeNumNodes;
   ToolVector<S32> subShapeNumObjects;
   ToolVector<Detail> details;

   /// @}

   /// These are set up at load time, but memory is allocated along with loaded data
   /// @{

   ToolVector<S32> subShapeFirstTranslucentObject;
   ToolVector<TSMesh*> meshes;

   /// @}

   /// @name Alpha Vectors
   /// these vectors describe how to transition between detail
   /// levels using alpha. "alpha-in" next detail as intraDL goes
   /// from alphaIn+alphaOut to alphaOut. "alpha-out" current
   /// detail level as intraDL goes from alphaOut to 0.
   /// @note
   ///   - intraDL is at 1 when if shape were any closer to us we'd be at dl-1
   ///   - intraDL is at 0 when if shape were any farther away we'd be at dl+1
   /// @{

   ToolVector<F32> alphaIn;
   ToolVector<F32> alphaOut
      ;
   /// @}

   /// @name Resizeable vectors
   /// @{

   Vector<Sequence>                 sequences;
   Vector<Quat16>					defaultRotations;
   Vector<Point3F>					defaultTranslations;
   Vector<Quat16>                   nodeRotations;
   Vector<Point3F>                  nodeTranslations;
   Vector<F32>                      nodeUniformScales;
   Vector<Point3F>                  nodeAlignedScales;
   Vector<Quat16>                   nodeArbitraryScaleRots;
   Vector<Point3F>                  nodeArbitraryScaleFactors;
   Vector<Quat16>                   groundRotations;
   Vector<Point3F>                  groundTranslations;
   Vector<Trigger>                  triggers;
   Vector<F32>                      iflFrameOffTimes;
   Vector<TSLastDetail*>            billboardDetails;
   Vector<ConvexHullAccelerator*>   detailCollisionAccelerators;
   Vector<const char *>             names;

   Vector<ObjectState>				objectStates;
   /// @}

   #pragma message(ENGINE(修改了上下部动作的叠加))
   TSIntegerSet WholeNodesMatters;    ///< 所有结点
   TSIntegerSet UpperNodesMatters;    ///< 上部结点（中间点本身属于上部）
   TSIntegerSet LowerNodesMatters;    ///< 下部结点

   /// Memory block for data storage.
   ///
   /// Most vectors are stored in a single memory block
   /// except when compiled using POWER_MAX_LIB defined.
   ///
   /// in that case, ToolVector becomes Vector<> and the
   /// vectors are resizeable
   S8 * mMemoryBlock;

   TSMaterialList * materialList;

   /// @name Bounding
   /// @{

   F32 radius;
   F32 tubeRadius;
   Point3F center;
   Box3F bounds;

   /// @}

   // various...
   U32 mExporterVersion;
   F32 mSmallestVisibleSize;  ///< Computed at load time from details vector.
   S32 mSmallestVisibleDL;    ///< @see mSmallestVisibleSize
   S32 mReadVersion;          ///< File version that this shape was read from.
   U32 mFlags;                ///< hasTranslucancy, iflInit
   U32 data;                  ///< User-defined data storage.

   bool mSequencesConstructed;

   // shape class has few methods --
   // just constructor/destructor, io, and lookup methods

   // constructor/destructor
   TSShape();
   ~TSShape();
   bool CanRender(void) {return true;}
   void init();
   void initMaterialList();    ///< you can swap in a new material list, but call this if you do
   bool preloadMaterialList(); ///< called to preload and validate the materials in the mat list

   void setupBillboardDetails(TSShapeInstance *shape);

   inline bool getSequencesConstructed() const { return mSequencesConstructed; }
   inline void setSequencesConstructed(const bool c) { mSequencesConstructed = c; }

   /// @name Lookup Animation Info
   /// indexed by keyframe number and offset (which objecct/node/decal
   /// of the animated objects/nodes/decals you want information for).
   /// @{

   QuatF & getRotation(const Sequence & seq, S32 keyframeNum, S32 rotNum, QuatF *) const;
   const Point3F & getTranslation(const Sequence & seq, S32 keyframeNum, S32 tranNum) const;
   F32 getUniformScale(const Sequence & seq, S32 keyframeNum, S32 scaleNum) const;
   const Point3F & getAlignedScale(const Sequence & seq, S32 keyframeNum, S32 scaleNum) const;
   TSScale & getArbitraryScale(const Sequence & seq, S32 keyframeNum, S32 scaleNum, TSScale *) const;
   const ObjectState & getObjectState(const Sequence & seq, S32 keyframeNum, S32 objectNum) const;
   /// @}

   /// build LOS collision detail
   void computeAccelerator(S32 dl);
   bool buildConvexHull(S32 dl) const;
   void computeBounds(S32 dl, Box3F & bounds) const; // uses default transforms to compute bounding box around a detail level
                                                     // see like named method on shapeInstance if you want to use animated transforms

   bool isUpper(S32 nodeIndex, S32 nameIndex);
   void initNodesMatters(StringTableEntry midNode);
   TSIntegerSet& getNodesMatters(S32 val);
   /// @name Lookup Methods
   /// @{

   S32 findName(const char *) const;
   const char * getName(S32) const;

   S32 findNode(S32 nameIndex) const;
   inline S32 findNode(const char * name) const { return findNode(findName(name)); }

   S32 findObject(S32 nameIndex) const;
   inline S32 findObject(const char * name) const { return findObject(findName(name)); }

   S32 findIflMaterial(S32 nameIndex) const;
   inline S32 findIflMaterial(const char * name) const { return findIflMaterial(findName(name)); }

   S32 findDetail(S32 nameIndex) const;
   inline S32 findDetail(const char * name) const { return findDetail(findName(name)); }

   S32 findSequence(S32 nameIndex) const;
   inline S32 findSequence(const char * name) const { return findSequence(findName(name)); }

   inline bool hasTranslucency() const { return (mFlags & HasTranslucency)!=0; }
   /// @}

   /// @name Alpha Transitions
   /// These control default values for alpha transitions between detail levels
   /// @{

   static F32 smAlphaOutLastDetail;
   static F32 smAlphaInBillboard;
   static F32 smAlphaOutBillboard;
   static F32 smAlphaInDefault;
   static F32 smAlphaOutDefault;
   /// @}

   /// don't load this many of the highest detail levels (although we always
   /// load one renderable detail if there is one)
   static S32 smNumSkipLoadDetails;

   /// by default we initialize shape when we read...
   static bool smInitOnRead;

   /// @name Version Info
   /// @{

   /// Most recent version...the one we write
   static S32 smVersion;
   /// Version currently being read, only valid during read
   static S32 smReadVersion;
   static const U32 smMostRecentExporterVersion;
   ///@}

   /// @name Persist Methods
   /// Methods for saving/loading shapes to/from streams
   /// @{

   void write(Stream *);
   bool read(Stream *);
   void readOldShape(Stream * s, S32 * &, S16 * &, S8 * &, S32 &, S32 &, S32 &);
   void writeName(Stream *, S32 nameIndex);
   S32  readName(Stream *, bool addName);

   /// Initializes our TSShape to be ready to receive put mesh data
   void createEmptyShape();
   bool readMD2(Stream *);

   void exportSequences(Stream *);
   bool importSequences(Stream *);

   void readIflMaterials(const char* shapePath);
   /// @}

   /// @name Persist Helper Functions
   /// @{

   static TSShapeAlloc alloc;
   void fixEndian(S32 *, S16 *, S8 *, S32, S32, S32);
   /// @}

   /// @name Memory Buffer Transfer Methods
   /// uses TSShape::Alloc structure
   /// @{

   void assembleShape();
   void disassembleShape();
   ///@}

   /// mem buffer transfer helper (indicate when we don't want to include a particular mesh/decal)
   bool checkSkip(S32 meshNum, S32 & curObject, S32 skipDL);

   /// used when reading old shapes/sequences
   void rearrangeKeyframeData(Sequence &, S32 keyframeStart, U8 * pns32 = NULL, U8 * pns16 = NULL, U8 * pos = NULL, U8 * pds = NULL, 
      S32 szNS32=-1, S32 szNS16=-1, S32 szOS32=-1);
   void rearrangeStates(S32 start, S32 rows, S32 cols, U8 * data, S32 size);

   void fixupOldSkins(S32 numMeshes, S32 numSkins, S32 numDetails, S32 * detailFirstSkin, S32 * detailNumSkins);
};

/// Specialized material list for 3space objects
///
/// @note Reflectance amounts on 3space objects are determined by
///       the alpha channel of the base material texture

class TSMaterialList : public MaterialList
{
   typedef MaterialList Parent;

   Vector<U32> mFlags;
   // Additional textures
   Vector<U32> mReflectanceMaps;
   //Ray: 高光贴图
   Vector<U32> mSpecularMaps;
   Vector<U32> mBumpMaps;
   Vector<U32> mDetailMaps;
   Vector<U32> mLightMaps;
   // Additional texture info
   Vector<F32> mDetailScales;
   Vector<F32> mReflectionAmounts;
   Vector<ColorF> mMaterialFilterColors;
   Vector<S32> mSortedOrders;

   bool mNamesTransformed;

   void allocate(U32 sz);
	
  public:
   char mPath[128]; // 当前路径,用于纹理加载 [7/2/2009 LivenHotch]

  public:

   enum
   {
      S_Wrap             = BIT(0),
      T_Wrap             = BIT(1),
      Translucent        = BIT(2),
      Additive           = BIT(3),
      Subtractive        = BIT(4),
      SelfIlluminating   = BIT(5),
      NeverEnvMap        = BIT(6),
      NoMipMap           = BIT(7),
      MipMap_ZeroBorder  = BIT(8),
	  NoCullBack		 = BIT(9),	//Ray: fix doubleSide bug
	  VertexColor		 = BIT(10), //Ray: enable vertex color
	  EffectFlag		 = BIT(11), //Ray: 特效光照
	  NoAlphaBland		 = BIT(12), //Ray: BlendOp::None
	  MulBland			 = BIT(13), //Ray: BlendOp::Mul
	  AddBland			 = BIT(14), //Ray: BlendOp::Add
	  FilterBlend	     = BIT(16),	//Ray: Filter
	  NoFog				 = BIT(17),	//Ray: No fog


	  SpecularMapOnly    = BIT(26),
      IflMaterial        = BIT(27),
      IflFrame           = BIT(28),
      AuxiliaryMap       = BIT(29) | BIT(30) | BIT(31) | BIT(26) // DEPRECATED
   };

   TSMaterialList(U32 materialCount, const char **materialNames, const U32 * materialFlags,
	   const U32 * reflectanceMaps, const U32 * specMaps,const U32 * bumpMaps, const U32 * detailMaps,
	   const F32 * detailScales, const F32 * reflectionAmounts,const F32 * filterColors, const U32 * sortedOrders,bool);
   TSMaterialList(U32 materialCount, const char **materialNames, const U32 * materialFlags,
	   const U32 * reflectanceMaps, const U32 * bumpMaps, const U32 * detailMaps, const U32 * lightMaps,
	   const F32 * detailScales, const F32 * reflectionAmounts,const F32 * filterColors, const U32 * sortedOrders);
   TSMaterialList();
   TSMaterialList(const TSMaterialList*);
   ~TSMaterialList();
   void free();

   void load(U32 index, const char* path = 0);
   inline bool load(TextureHandleType type, const char* path = 0,bool clampToEdge = false) { return Parent::load(type,path,clampToEdge); }

   /// @name Lookups
   /// @{

   inline GFXTexHandle * getReflectionMap(U32 index) { return mReflectanceMaps[index] == 0xFFFFFFFF ? NULL : &getMaterial(mReflectanceMaps[index]); }
   inline F32 getReflectionAmount(U32 index) { return mReflectionAmounts[index]; }
   inline GFXTexHandle * getBumpMap(U32 index) { return mBumpMaps[index] == 0xFFFFFFFF ? NULL : &getMaterial(mBumpMaps[index]); }
   inline GFXTexHandle * getDetailMap(U32 index) { return mDetailMaps[index] == 0xFFFFFFFF ? NULL : &getMaterial(mDetailMaps[index]); }
   inline GFXTexHandle * getLightMap(U32 index) { return mLightMaps[index] == 0xFFFFFFFF ? NULL : &getMaterial(mLightMaps[index]); }
   inline F32 getDetailMapScale(U32 index) { return mDetailScales[index]; }
   inline ColorF getFilterColor(U32 index) { return mMaterialFilterColors[index]; }
   inline U32 getSortedOrder(U32 index) { return mSortedOrders[index]; }
   inline bool reflectionInAlpha(U32 index) { return mReflectanceMaps[index] == index; }
   inline bool isIFL( U32 index )
   { 
      if( index < mFlags.size() )
      {
         return mFlags[index] & IflMaterial;
      }
      else
      {
         return false;
      }
   }

   bool compareMaterialParam(int Index,Material *currentMat);
   void prepareMaterial(int ,Material *);
   void mapMaterials();


   /// @}

   inline U32 getFlags(U32 index)
   {
       AssertFatal(index < getMaterialCount(),"TSMaterialList::getFlags: index out of range");
       return mFlags[index];
   }
   inline void setFlags(U32 index, U32 value)
   {
       AssertFatal(index < getMaterialCount(),"TSMaterialList::getFlags: index out of range");
       mFlags[index] = value;
   }

   void remap(U32 toIndex, U32 fromIndex); ///< support for ifl sequences

   bool setMaterial(U32 index, const char* texPath); // use to support reskinning

   /// pre-load only ... support for ifl sequences

   //Ray: 支持高光贴图的导出
   void push_back(const char * name, U32 flags,
	   U32 a=0xFFFFFFFF, U32 b=0xFFFFFFFF, U32 c=0xFFFFFFFF,U32 d=0xFFFFFFFF,
	   F32 dm=1.0f, F32 em=1.0f,
	   U32 l=0xFFFFFFFF,ColorF filterColor=ColorF(1.0f,1.0f,1.0f),U32 sortOrder=0);

   /// @name IO
   /// Functions for reading/writing to/from streams
   /// @{

   bool write(Stream &);
   bool read(Stream &);
   /// @}
};


extern ResourceInstance *constructTSShape(Stream &stream, ResourceObject *);
extern ResourceInstance *constructTSShapeMD2(Stream &stream, ResourceObject *);

#define TSNode TSShape::Node
#define TSObject TSShape::Object
#define TSSequence TSShape::Sequence
#define TSDetail TSShape::Detail

inline QuatF & TSShape::getRotation(const Sequence & seq, S32 keyframeNum, S32 rotNum, QuatF * quat) const
{
   return nodeRotations[seq.baseRotation + rotNum*seq.numKeyframes + keyframeNum].getQuatF(quat);
}

inline const Point3F & TSShape::getTranslation(const Sequence & seq, S32 keyframeNum, S32 tranNum) const
{
   return nodeTranslations[seq.baseTranslation + tranNum*seq.numKeyframes + keyframeNum];
}

inline F32 TSShape::getUniformScale(const Sequence & seq, S32 keyframeNum, S32 scaleNum) const
{
   return nodeUniformScales[seq.baseScale + scaleNum*seq.numKeyframes + keyframeNum];
}

inline const Point3F & TSShape::getAlignedScale(const Sequence & seq, S32 keyframeNum, S32 scaleNum) const
{
   return nodeAlignedScales[seq.baseScale + scaleNum*seq.numKeyframes + keyframeNum];
}

inline TSScale & TSShape::getArbitraryScale(const Sequence & seq, S32 keyframeNum, S32 scaleNum, TSScale * scale) const
{
   nodeArbitraryScaleRots[seq.baseScale + scaleNum*seq.numKeyframes + keyframeNum].getQuatF(&scale->mRotate);
   scale->mScale = nodeArbitraryScaleFactors[seq.baseScale + scaleNum*seq.numKeyframes + keyframeNum];
   return *scale;
}

inline const TSShape::ObjectState & TSShape::getObjectState(const Sequence & seq, S32 keyframeNum, S32 objectNum) const
{
   return objectStates[seq.baseObjectState + objectNum*seq.numKeyframes + keyframeNum];
}

#endif
