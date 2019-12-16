//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GFXDEVICE_H_
#define _GFXDEVICE_H_

#include "gfx/gfxShader.h"
#include "gfx/gfxTextureManager.h"
#include "gfx/gfxTextureHandle.h"
#include "gfx/gfxVertexBuffer.h"
#include "gfx/gfxPrimitiveBuffer.h"
#include "gfx/gfxStateFrame.h"
#include "gfx/gfxTarget.h"
#include "gfx/gfxStructs.h"
#include "gfx/gfxAdapter.h"
#include "util/safeDelete.h"
#include "gfx/gfxCubemap.h"
#include "gfx/gfxStateBlock.h"
#include "platform/threads/mutex.h"


#ifndef _SIGNAL_H_
#include "util/tSignal.h"
#endif

class GFont;
class GFXCardProfiler;
class GFXFence;
class FontRenderBatcher;
class GFXDrawUtil;
class GFXEffectMgr;
class GFXStateBlock;
class gfxFlashManager;

// To enable code which will help with state debugging, #define POWER_GFX_STATE_DEBUG

#ifdef POWER_GFX_STATE_DEBUG
#  include "gfx/gfxDebugStateTracker.h"
#endif

// Global macro
#ifdef POWER_GFX_STATE_DEBUG
#  define GFX GFXDevice::get( __FILE__, __LINE__ )
#else
#  define GFX GFXDevice::get()
#endif

#define SFXBBCOPY_SIZE 512
#define MAX_MRT_TARGETS 4 

#define MAX_BACKBUFFER_NUM 3

//-----------------------------------------------------------------------------

/// GFXDevice is the TSE graphics interface layer. This allows the TSE to
/// do many things, such as use multiple render devices for multi-head systems,
/// and allow a game to render in DirectX 9, OpenGL or any other API which has
/// a GFX implementation seamlessly. There are many concepts in GFX device which
/// may not be familiar to you, especially if you have not used DirectX.
/// @n
/// <b>Buffers</b>
/// There are three types of buffers in GFX: vertex, index and primitive. Please
/// note that index buffers are not accessable outside the GFX layer, they are wrapped
/// by primitive buffers. Primitive buffers will be explained in detail later.
/// Buffers are allocated and deallocated using their associated allocXBuffer and
/// freeXBuffer methods on the device. When a buffer is allocated you pass in a
/// pointer to, depending on the buffer, a vertex type pointer or a U16 pointer. 
/// During allocation, this pointer is set to the address of where you should
/// copy in the information for this buffer. You must the tell the GFXDevice
/// that the information is in, and it should prepare the buffer for use by calling
/// the prepare method on it. Dynamic vertex buffer example:
/// @code
/// GFXVertexP *verts;        // Making a buffer containing verticies with only position
///
/// // Allocate a dynamic vertex buffer to hold 3 vertices and use *verts as the location to copy information into
/// GFXVertexBufferHandle vb = GFX->allocVertexBuffer( 3, &verts, true ); 
///
/// // Now set the information, we're making a triangle
/// verts[0].point = Point3F( 200.f, 200.f, 0.f );
/// verts[1].point = Point3F( 200.f, 400.f, 0.f );
/// verts[2].point = Point3F( 400.f, 200.f, 0.f );
///
/// // Tell GFX that the information is in and it should be made ready for use
/// // Note that nothing is done with verts, this should not and MUST NOT be deleted
/// // stored, or otherwise used after prepare is called.
/// GFX->prepare( vb );
///
/// // Because this is a dynamic vertex buffer, it is only assured to be valid until someone 
/// // else allocates a dynamic vertex buffer, so we will render it now
/// GFX->setVertexBuffer( vb );
/// GFX->drawPrimitive( GFXTriangleStrip, 0, 1 );
///
/// // Now because this is a dynamic vertex buffer it MUST NOT BE FREED you are only
/// // given a handle to a vertex buffer which belongs to the device
/// @endcode
/// 
/// To use a static vertex buffer, it is very similar, this is an example using a
/// static primitive buffer:
/// @n
/// This takes place inside a constructor for a class which has a member variable
/// called mPB which is the primitive buffer for the class instance.
/// @code
/// U16 *idx;                          // This is going to be where to write indices
/// GFXPrimitiveInfo *primitiveInfo;   // This will be where to write primitive information
///
/// // Allocate a primitive buffer with 4 indices, and 1 primitive described for use
/// mPB = GFX->allocPrimitiveBuffer( 4, &idx, 1, &primitiveInfo );
///
/// // Write the index information, this is going to be for the outline of a triangle using
/// // a line strip
/// idx[0] = 0;
/// idx[1] = 1;
/// idx[2] = 2;
/// idx[3] = 0;
///
/// // Write the information for the primitive
/// primitiveInfo->indexStart = 0;            // Starting with index 0
/// primitiveInfo->minVertex = 0;             // The minimum vertex index is 0
/// primitiveInfo->maxVertex = 3;             // The maximum vertex index is 3
/// primitiveInfo->primitiveCount = 3;        // There are 3 lines we are drawing
/// primitiveInfo->type = GFXLineStrip;       // This primitive info describes a line strip
/// @endcode
/// The following code takes place in the destructor for the same class
/// @code
/// // Because this is a static buffer it's our responsibility to free it when we are done
/// GFX->freePrimitiveBuffer( mPB );
/// @endcode
/// This last bit takes place in the rendering function for the class
/// @code
/// // You need to set a vertex buffer as well, primitive buffers contain indexing
/// // information, not vertex information. This is so you could have, say, a static
/// // vertex buffer, and a dynamic primitive buffer.
///
/// // This sets the primitive buffer to the static buffer we allocated in the constructor
/// GFX->setPrimitiveBuffer( mPB );
/// 
/// // Draw the first primitive contained in the set primitive buffer, our primitive buffer
/// // has only one primitive, so we could also technically call GFX->drawPrimitives(); and
/// // get the same result. 
/// GFX->drawPrimitive( 0 );
/// @endcode
/// If you need any more examples on how to use these buffers please see the rest of the engine.
/// @n
/// <b>Primitive Buffers</b>
/// @n
/// Primitive buffers wrap and extend the concept of index buffers. The purpose of a primitive
/// buffer is to let objects store all information they have to render their primitives in
/// a central place. Say that a shape is made up of triangle strips and triangle fans, it would
/// still have only one primitive buffer which contained primitive information for each strip
/// and fan. It could then draw itself with one call.
///
/// TO BE FINISHED LATER
struct IDirect3DVolumeTexture9;
class GFXDevice
{
private:
   friend class GFXInit;
   friend class GFXPrimitiveBufferHandle;
   friend class GFXVertexBufferHandleBase;
   friend class GFXTextureObject;
   friend class GFXTexHandle;
   friend class _GFXCanonizer;
   friend class Blender;
   friend class GFXStateFrame;
   friend class GFXTestFullscreenToggle;
   friend class TestGFXTextureCube;
   friend class TestGFXRenderTargetCube;
   friend class TestGFXRenderTargetStack;
   friend class GFXResource;
#ifdef POWER_GFX_STATE_DEBUG
   friend class GFXDebugState::GFXDebugStateWatch;
#endif

   //--------------------------------------------------------------------------
   // Static GFX interface
   //--------------------------------------------------------------------------

public:
	static U32 mDPTimes;

	CCSLock m_mutex;

	Point2I mClipPos;

   enum GFXDeviceEventType
   {
      /// The device has been created, but not initialized
      deCreate,
      /// The device has been initialized
      deInit,
      /// The device is about to be destroyed.
      deDestroy
   };
   typedef Signal <GFXDeviceEventType> DeviceEventSignal;
   static DeviceEventSignal& getDeviceEventSignal();

#ifdef POWER_GFX_STATE_DEBUG
   GFXDebugState::GFXDebugStateManager *getDebugStateManager() { return mDebugStateManager; }

   inline const U32 &getLastGFXCallLine() const { return mLastGFXCallLine; }
   inline const char *getLastGFXCallFileName() const { return mLastGFXCallFileName; }
   void processStateWatchHistory( const GFXDebugState::GFXDebugStateWatch *watch );

   static GFXDevice *get( const char *fileName, U32 line );
#else
   static GFXDevice *get();
#endif

   static void create();
   static void destroy();
   
   inline static const Vector<GFXDevice *> *getDeviceVector() { return &smGFXDevice; };
   static void setActiveDevice( U32 deviceIndex );
   inline static bool devicePresent(){ return smActiveDeviceIndex > -1 && smGFXDevice[smActiveDeviceIndex] != NULL; }
    
#ifndef NTJ_SERVER
   //获取flash设备
   gfxFlashManager* GetFlashMgr(void) {return m_flashManager;}
#endif

private:
   /// @name Device management variables
   /// @{

   ///
   static Vector<GFXDevice *> smGFXDevice; ///< Global GFXDevice vector
   static S32 smActiveDeviceIndex;         ///< Active GFX Device index, signed so -1 can be uninitalized
   static DeviceEventSignal* smSignalGFXDeviceEvent;

#ifdef POWER_GFX_STATE_DEBUG
   U32 mLastGFXCallLine;
   const char *mLastGFXCallFileName;
   GFXDebugState::GFXDebugStateManager *mDebugStateManager;
#endif

   /// @}

   //--------------------------------------------------------------------------
   // Core GFX interface
   //--------------------------------------------------------------------------
private:
   /// Device ID for this device.
   U32 mDeviceIndex;

   /// Adapter for this device.
   GFXAdapter mAdapter;
    
protected:

#ifndef NTJ_SERVER
   gfxFlashManager* m_flashManager;
#endif

   /// List of valid video modes for this device.
   Vector<GFXVideoMode> mVideoModes;

   /// The CardProfiler for this device.
   GFXCardProfiler *mCardProfiler;

   /// Head of the resource list.
   ///
   /// @see GFXResource
   GFXResource *mResourceListHead;

   /// Set once the device is active.
   bool mCanCurrentlyRender;

   /// Set if we're in a mode where we want rendering to occur.
   bool mAllowRender;

   /// This will allow querying to see if a device is initialized and ready to
   /// have operations performed on it.
   bool mInitialized;

   /// This is called before this, or any other device, is deleted in the global destroy()
   /// method. It allows the device to clean up anything while everything is still valid.
   virtual void preDestroy() =0;

   /// Set the adapter that this device is using.  For use by GFXInit::createDevice only.
   virtual void setAdapter(const GFXAdapter& adapter) { mAdapter = adapter; }

   /// Notify GFXDevice that we are initialized
   virtual void deviceOnInit();
   virtual void  deviceInited();
public:
   GFXDevice();
   virtual ~GFXDevice();

   /// Initialize this GFXDevice, optionally specifying a platform window to
   /// bind to.
   virtual void init( const GFXVideoMode &mode, PlatformWindow *window = NULL ) = 0;

   bool canCurrentlyRender();
   void setAllowRender( bool render );
   bool allowRender();

   inline GFXCardProfiler* getCardProfiler() const { return mCardProfiler; }
   inline const U32 getDeviceIndex() const { return mDeviceIndex; };

   /// Returns active graphics adapter type.
   virtual GFXAdapterType getAdapterType()=0;

   /// Returns the Adapter that was used to create this device
   virtual const GFXAdapter& getAdapter() { return mAdapter; }

   /// @}

   /// @name Debug Methods
   /// @{

   virtual void enterDebugEvent(ColorI color, const char *name) = 0;
   virtual void leaveDebugEvent() = 0;
   virtual void setDebugMarker(ColorI color, const char *name) = 0;

   /// @}

   /// @name Resource debug methods
   /// @{
   
   /// Lists how many of each GFX resource (e.g. textures, texture targets, shaders, etc.) GFX is aware of
   /// @param unflaggedOnly   If true, this method only counts unflagged resources
   virtual void listResources(bool unflaggedOnly);

   /// Flags all resources GFX is currently aware of
   virtual void flagCurrentResources();

   /// Clears the flag on all resources GFX is currently aware of
   virtual void clearResourceFlags();

   virtual bool CreateVolumeTexture(StringTableEntry texname, IDirect3DVolumeTexture9** tex){return true;};
   virtual void SetVolumeTexture(U32 Sampler, IDirect3DVolumeTexture9* tex){};

   /// Dumps a description of the specified resource types to the console
   /// @param resNames     A string of space separated class names (e.g. "GFXTextureObject GFXTextureTarget GFXShader")
   ///                     to describe to the console
   /// @param file         A path to the file to write the descriptions to.  If it is NULL or "", descriptions are
   ///                     written to the console.
   /// @param unflaggedOnly If true, this method only counts unflagged resources
   /// @note resNames is case sensitive because there is no dStristr function.
   virtual void describeResources(const char* resName, const char* file, bool unflaggedOnly);

   /// This method sets up the initial states for the GFXDevice. If state 
   /// debugging is enabled, this method will get called at the beginning of 
   /// every frame, and at the end of the frame, it will generate a report of the
   /// state activity on the states which it sets up. 
   void setInitialGFXState();

protected:
   /// This is a helper method for describeResourcesToFile.  It walks through the
   /// GFXResource list and sorts it by item type, putting the resources into the proper vector.
   /// @see describeResources
   virtual void fillResourceVectors(const char* resNames, bool unflaggedOnly, Vector<GFXTextureObject*> &textureObjects,
      Vector<GFXTextureTarget*> &textureTargets, Vector<GFXWindowTarget*> &windowTargets,
      Vector<GFXVertexBuffer*> &vertexBuffers, Vector<GFXPrimitiveBuffer*> &primitiveBuffers,
      Vector<GFXFence*> &fences, Vector<GFXCubemap*> &cubemaps, Vector<GFXShader*> &shaders);

public:

   /// @}

   /// @name Video Mode Functions
   /// @{
   /// Enumerates the supported video modes of the device
   virtual void enumerateVideoModes() = 0;

   /// Well, this function gets the video mode list!
   const Vector<GFXVideoMode>* const getVideoModeList() const;

   F32 formatByteSize(GFXFormat format);
   virtual GFXFormat selectSupportedFormat(GFXTextureProfile *profile,
	   const Vector<GFXFormat> &formats, bool texture, bool mustblend) = 0;

   /// @}

   //-----------------------------------------------------------------------------

private:

   U32            mStateStackDepth;
   GFXStateFrame  mStateStack[STATE_STACK_SIZE];

   /// @name State-tracking methods
   /// These methods you should not call at all. These are what set the states
   /// for the state caching system.
   /// @{

   ///
   void trackRenderState( U32 state, U32 value );
   void trackTextureStageState( U32 stage, U32 state, U32 value );
   void trackSamplerState( U32 stage, U32 type, U32 value );
   /// @}

public:
   void pushState()
   {
//      Con::printf("state {");
      mStateStackDepth++;
      AssertFatal(mStateStackDepth < STATE_STACK_SIZE, "Too deep a stack!");
      mStateStack[mStateStackDepth].init();
   }

   void popState()
   {
      AssertFatal(mStateStackDepth > 0, "Too shallow a stack!");
      mStateStack[mStateStackDepth].rollback();
      mStateStackDepth--;
//      Con::printf("}");
   }

protected:

   /// @name State tracking variables
   /// @{

   /// Set if ANY state is dirty, including matrices or primitive buffers.
   bool mStateDirty;     

   StateTracker         mStateTracker[GFXRenderState_COUNT];
   U32                  mTrackedState[GFXRenderState_COUNT];
   U32                  mNumDirtyStates;

   StateTracker         mTextureStateTracker[TEXTURE_STAGE_COUNT][GFXTSS_COUNT];
   TextureDirtyTracker  mTextureTrackedState[TEXTURE_STAGE_COUNT * GFXTSS_COUNT];
   U32                  mNumDirtyTextureStates;

   StateTracker         mSamplerStateTracker[TEXTURE_STAGE_COUNT][GFXSAMP_COUNT];
   TextureDirtyTracker  mSamplerTrackedState[TEXTURE_STAGE_COUNT * GFXSAMP_COUNT];
   U32                  mNumDirtySamplerStates;

   enum TexDirtyType
   {
      GFXTDT_Normal,
      GFXTDT_Cube
   };
   
   GFXTexHandle mCurrentTexture[TEXTURE_STAGE_COUNT];
   GFXTexHandle mNewTexture[TEXTURE_STAGE_COUNT];
   GFXCubemapHandle mCurrentCubemap[TEXTURE_STAGE_COUNT];
   GFXCubemapHandle mNewCubemap[TEXTURE_STAGE_COUNT];

   TexDirtyType   mTexType[TEXTURE_STAGE_COUNT];
   bool           mTextureDirty[TEXTURE_STAGE_COUNT];
   bool           mTexturesDirty;

   /// @name Light Tracking
   /// @{

   GFXLightInfo  mCurrentLight[LIGHT_STAGE_COUNT]; 
   bool          mCurrentLightEnable[LIGHT_STAGE_COUNT];
   bool          mLightDirty[LIGHT_STAGE_COUNT];
   bool          mLightsDirty;

   ColorF        mGlobalAmbientColor;
   bool          mGlobalAmbientColorDirty;

   /// @}

   /// @name Fixed function material tracking
   /// @{

   GFXLightMaterial mCurrentLightMaterial;
   bool mLightMaterialDirty;

   /// @}

   /// @name Bitmap modulation and color stack
   /// @{

   ///

   /// @}

   /// @see getDeviceSwizzle32
   Swizzle<U8, 4> *mDeviceSwizzle32;

   /// @see getDeviceSwizzle24
   Swizzle<U8, 3> *mDeviceSwizzle24;


   //-----------------------------------------------------------------------------

   /// @name Matrix managing variables
   /// @{

   ///
   MatrixF mWorldMatrix[WORLD_STACK_MAX];
   bool    mWorldMatrixDirty;
   S32     mWorldStackSize;

   MatrixF mProjectionMatrix;
   bool    mProjectionMatrixDirty;

   MatrixF mViewMatrix;
   bool    mViewMatrixDirty;

   MatrixF mTextureMatrix[TEXTURE_STAGE_COUNT];
   bool    mTextureMatrixDirty[TEXTURE_STAGE_COUNT];
   bool    mTextureMatrixCheckDirty;
   /// @}


	//-----------------------------------------------------------------------------

	/// @name Matrix managing variables
	/// @{

	///
	GFXStateBlock *mCurStateBlock;
	bool mCurStateBlockDirty;
	/// @}

   /// @name Current frustum planes
   /// @{

   ///
   F32 mFrustLeft, mFrustRight;
   F32 mFrustBottom, mFrustTop;
   F32 mFrustNear, mFrustFar;
   bool mFrustOrtho;

   //-----------------------------------------------------------------------------
public:
   /// @name State setting functions
   /// These functions should set the state when called, not
   /// use the state-caching things. The state-tracking
   /// methods will call these functions.
   ///
   /// See their associated enums for more information
   /// @{

   /// Sets states which have to do with general rendering
   virtual void setRenderState( U32 state, U32 value) = 0;

   /// Sets states which have to do with how textures are displayed
   virtual void setTextureStageState( U32 stage, U32 state, U32 value ) = 0;

   /// Sets states which have to do with texture sampling and addressing
   virtual void setSamplerState( U32 stage, U32 type, U32 value ) = 0;
   /// @}
protected:
   virtual void setTextureInternal(U32 textureUnit, const GFXTextureObject*texture) = 0;

   virtual void setLightInternal(U32 lightStage, const GFXLightInfo light, bool lightEnable) = 0;
   virtual void setGlobalAmbientInternal(ColorF color) = 0;
   virtual void setLightMaterialInternal(const GFXLightMaterial mat) = 0;

   virtual void beginSceneInternal() = 0;
   virtual void endSceneInternal() = 0;

   /// @name State Initalization.
   /// @{

   /// State initalization. This MUST BE CALLED in setVideoMode after the device
   /// is created.
   virtual void initStates() = 0;

   /// Helper to init render state
   void initRenderState( U32 state, U32 value );

   /// Helper to init texture state
   void initTextureState( U32 stage, U32 state, U32 value);

   /// Helper to init sampler state
   void initSamplerState( U32 stage, U32 state, U32 value);
   /// @}

   //-----------------------------------------------------------------------------

   /// This function must be implemented differently per
   /// API and it should set ONLY the current matrix.
   /// For example, in OpenGL, there should be NO matrix stack
   /// activity, all the stack stuff is managed in the GFX layer.
   ///
   /// OpenGL does not have separate world and
   /// view matrices. It has ModelView which is world * view.
   /// You must take this into consideration.
   ///
   /// @param   mtype   Which matrix to set, world/view/projection
   /// @param   mat   Matrix to assign
   virtual void setMatrix( GFXMatrixType mtype, const MatrixF &mat ) = 0;

   //-----------------------------------------------------------------------------
protected:


   /// @name Buffer Allocation 
   /// These methods are implemented per-device and are called by the GFX layer
   /// when a user calls an alloc
   ///
   /// @note Primitive Buffers are NOT implemented per device, they wrap index buffers
   /// @{

   /// This allocates a vertex buffer and returns a pointer to the allocated buffer.
   /// This function should not be called directly - rather it should be used by
   /// the GFXVertexBufferHandle class.

   /// @param   numVerts   Number of vertices to allocate
   /// @param   vertexPtr   A pointer to the location where the user can copy in vertex data (out)
   /// @param   vertType   Vertex type
   virtual GFXVertexBuffer *allocVertexBuffer( U32 numVerts, U32 vertFlags, U32 vertSize, GFXBufferType bufferType,U8 streamNum=0 ) = 0;

   RefPtr<GFXVertexBuffer> mCurrentVertexBuffer;
   bool mVertexBufferDirty;
   
   RefPtr<GFXPrimitiveBuffer> mCurrentPrimitiveBuffer;
   bool mPrimitiveBufferDirty;

   /// This allocates a primitive buffer and returns a pointer to the allocated buffer.
   /// A primitive buffer's type argument refers to the index data - the primitive data will
   /// always be preserved from call to call.
   ///
   /// @note All index buffers use 16-bit indices.
   /// @param   numIndices   Number of indices to allocate
   /// @param   indexPtr   Pointer to the location where the user can copy in index data (out)
   virtual GFXPrimitiveBuffer *allocPrimitiveBuffer( U32 numIndices, U32 numPrimitives, GFXBufferType bufferType ) = 0;
   /// @}

   //---------------------------------------
   // Dmap flag
   //---------------------------------------
protected:
	bool mDmapSet;
public:
	bool isDmapSet() {return mDmapSet;}
	void setDmap(bool flag) {mDmapSet=flag;}

   //---------------------------------------
   // SFX buffer
   //---------------------------------------
protected:
   GFXTexHandle mSfxBackBuffer[MAX_BACKBUFFER_NUM];
   bool         mUseSfxBackBuffer;
   static S32   smSfxBackBufferSize;

   GFXTexHandle mFrontBuffer[2];
   U32 mCurrentFrontBufferIdx;

   //---------------------------------------
   // Render target related
   //---------------------------------------
   Vector<GFXTargetRef> mRTStack;
   GFXTargetRef mCurrentRT;

   virtual void _updateRenderTargets();
public:

   /// @name Special FX Back Buffer functions
   /// @{

   ///
   virtual GFXTexHandle &getSfxBackBuffer(int Idx=0){ return mSfxBackBuffer[Idx]; }
   virtual void copyBBToSfxBuff(int Idx=0) = 0;
   virtual void copyBBToBuff(GFXTexHandle &){ return; };

   /// @}

   /// @name Texture functions
   /// @{
protected:
   GFXTextureManager * mTextureManager;

public:
   virtual void zombifyTextureManager()=0;
   virtual void resurrectTextureManager()=0;
   
   void registerTexCallback( GFXTexEventCallback callback, void *userData, S32 &handle );
   void unregisterTexCallback( S32 handle );
   
   void reloadTextureResource( const char *filename ){ mTextureManager->reloadTextureResource( filename ); }
   virtual GFXCubemap * createCubemap() = 0;

   inline GFXTextureManager *getTextureManager()
   {
      return mTextureManager;
   }

   ///@}

   /// Swizzle to convert 32bpp bitmaps from RGBA to the native device format.
   const Swizzle<U8, 4> *getDeviceSwizzle32() const 
   { 
      return mDeviceSwizzle32;
   }

   /// Swizzle to convert 24bpp bitmaps from RGB to the native device format.
   const Swizzle<U8, 3> *getDeviceSwizzle24() const 
   { 
      return mDeviceSwizzle24;
   }

   /// @name Render Target functions
   /// @{

   /// Allocate a target for doing render to texture operations, with no
   /// depth/stencil buffer.
   virtual GFXTextureTarget *allocRenderToTextureTarget()=0;

   /// Allocate a target for a given window.
   virtual GFXWindowTarget *allocWindowTarget(PlatformWindow *window)=0;

   /// Save current render target states - note this works with MRT's
   virtual void pushActiveRenderTarget();

   /// Restore all render targets - supports MRT's
   virtual void popActiveRenderTarget();

   /// Start rendering to to a specified render target.
   virtual void setActiveRenderTarget( GFXTarget *target )=0;

   /// Return a pointer to the current active render target.
   virtual GFXTarget *getActiveRenderTarget();

   ///@}

   /// @name Shader functions
   /// @{
   virtual F32   getPixelShaderVersion() const = 0;
   virtual void  setPixelShaderVersion( F32 version ) = 0;

   /// Returns the number of texture samplers that can be used in a shader rendering pass
   virtual U32 getNumSamplers() const = 0;

   virtual void setShader( GFXShader *shader ){};
   virtual void setVertexShaderConstF( U32 reg, const float *data, U32 size, U32 realCntFloat = 4 ){};
   virtual void setPixelShaderConstF( U32 reg, const float *data, U32 size, U32 realCntFloat = 4 ){};
   virtual void disableShaders(){};
   
   /// Creates a shader
   ///
   /// @param  vertFile       Vertex shader filename
   /// @param  pixFile        Pixel shader filename
   /// @param  pixVersion     Pixel shader version
   virtual GFXShader * createShader( const char *vertFile, const char *pixFile, F32 pixVersion ) = 0;
   
   /// Generates a shader based on the passed in features
   virtual GFXShader * createShader( GFXShaderFeatureData &featureData, GFXVertexFlags vertFlags ) = 0;

   /// Destroys shader
   virtual void destroyShader( GFXShader *shader ){};

   // This is called by MatInstance::reinitInstances to cause the shaders to be regenerated.
   virtual void flushProceduralShaders() = 0;

   /// @}

   /// @name Effect functions
   /// @{

   virtual GFXEffectMgr* getEffectManager() { return NULL; }

   /// @}

   //-----------------------------------------------------------------------------

   /// @name Rendering methods
   /// @{

   ///
   virtual void clear( U32 flags, ColorI color, F32 z, U32 stencil ) = 0;
   virtual void beginScene();
   virtual void endScene();

   virtual GFXTexHandle & getFrontBuffer(){ return mFrontBuffer[mCurrentFrontBufferIdx]; }

   void setPrimitiveBuffer( GFXPrimitiveBuffer *buffer );
   void setVertexBuffer( GFXVertexBuffer *buffer );
   
   virtual void drawPrimitive( GFXPrimitiveType primType, U32 vertexStart, U32 primitiveCount ) = 0;

   /// The parameters to drawIndexedPrimitive are somewhat complicated. From a raw-data stand point
   /// they evaluate to something like the following:
   /// @code
   /// U16 indicies[] = { 0, 1, 2, 1, 0, 0, 2 }; 
   /// Point3F verts[] = { Point3F( 0.0f, 0.0f, 0.0f ), Point3F( 0.0f, 1.0f, 0.0f ), Point3F( 0.0f, 0.0f, 1.0f ) };
   /// 
   /// GFX->drawIndexedPrimitive( GFXLineList, // Drawing a list of lines, each line is two verts
   ///                            0, // vertex 0 will be referenced so minIndex = 0
   ///                            3, // 3 verticies will be used for this draw call
   ///                            1, // We want index 1 to be the first index used, so indicies 1-6 will be used
   ///                            3  // Drawing 3 LineList primitives, meaning 6 verts will be drawn
   ///                             );
   ///
   /// U16 *idxPtr = &indicies[1];  // 1 = startIndex, so the pointer is offset such that:
   ///                              //    idxPtr[0] is the same as indicies[1]
   ///
   /// U32 numVertsToDrawFromBuffer = primitiveCount * 2; // 2 verts define a line in the GFXLineList primitive type (6)
   /// @endcode
   ///
   /// @param  primType    Type of primitive to draw
   /// @param  minIndex    The smallest index into the vertex stream which will be used for this draw call.
   ///                     This parameter is a performance hint for implementations. No vertex below minIndex
   ///                     will be referenced by this draw call. For device implementors, this should _not_
   ///                     be used to offset the vertex buffer, or index buffer.
   /// @param  numVerts    The number of verticies which will be referenced in this draw call. This is not
   ///                     the number of verticies which will be drawn. That is a function of 'primType' and 
   ///                     'primitiveCount'.
   /// @param  startIndex  An offset from the start of the index buffer to specify where to start. If
   ///                     'idxBuffer' is a pointer to an array of integers, this could be written as
   ///                     int *offsetIdx = idxBuffer + startIndex;
   /// @param  primitiveCount The number of primitives of type 'primType' to draw.
   virtual void drawIndexedPrimitive( GFXPrimitiveType primType, U32 minIndex, U32 numVerts, U32 startIndex, U32 primitiveCount ) = 0;

   void drawPrimitive( U32 primitiveIndex );
   void drawPrimitives();
   void drawPrimitiveBuffer( GFXPrimitiveBuffer *buffer );
   /// @}

   //-----------------------------------------------------------------------------

   /// Allocate a fence. The API specific implementation of GFXDevice is responsible
   /// to make sure that the proper type is used. GFXGeneralFence should work in
   /// all cases. 
   virtual GFXFence *createFence() = 0;
   
   /// This resets a number of basic rendering states.  Insures that a forgotten
   /// state like a disable Z buffer doesn't affect other bits of rendering code.
   void setBaseRenderState();
	void setBaseDrawState();

   /// @name Light Settings
   /// NONE of these should be overridden by API implementations
   /// because of the state caching stuff.
   /// @{
   void setLight(U32 stage, GFXLightInfo* light);
   void setLightMaterial(GFXLightMaterial mat);
   void setGlobalAmbientColor(ColorF color);

   /// @}
   
   /// @name Texture State Settings
   /// NONE of these should be overridden by API implementations
   /// because of the state caching stuff.
   /// @{

   ///
   void setTexture(U32 stage, GFXTextureObject*texture);
   void setCubeTexture( U32 stage, GFXCubemap *cubemap );
//protected:
   void setTextureStageColorOp( U32 stage, GFXTextureOp op );
   void setTextureStageColorArg1( U32 stage, U32 argFlags );
   void setTextureStageColorArg2( U32 stage, U32 argFlags );
   void setTextureStageColorArg3( U32 stage, U32 argFlags );
   void setTextureStageResultArg( U32 stage, GFXTextureArgument arg );
   void setTextureStageAlphaOp( U32 stage, GFXTextureOp op );
   void setTextureStageAlphaArg1( U32 stage, U32 argFlags );
   void setTextureStageAlphaArg2( U32 stage, U32 argFlags );
   void setTextureStageBumpEnvMat( U32 stage, F32 mat[2][2] );
   void setTextureStageTransform( U32 stage, U32 texTransFlags );
   /// @}

   //-----------------------------------------------------------------------------

   /// @name Sampler State Settings
   /// NONE of these should be overridden by API implementations
   /// because of the state caching stuff.
   /// @{

   /// 
   void setTextureStageAddressModeU( U32 stage, GFXTextureAddressMode mode );
   void setTextureStageAddressModeV( U32 stage, GFXTextureAddressMode mode );
   void setTextureStageAddressModeW( U32 stage, GFXTextureAddressMode mode );
   void setTextureStageBorderColor( U32 stage, ColorI color );
   void setTextureStageMagFilter( U32 stage, GFXTextureFilterType filter );
   void setTextureStageMinFilter( U32 stage, GFXTextureFilterType filter );
   void setTextureStageMipFilter( U32 stage, GFXTextureFilterType filter );
   void setTextureStageLODBias( U32 stage, F32 bias );
   void setTextureStageMaxMipLevel( U32 stage, U32 maxMiplevel );
   void setTextureStageMaxAnisotropy( U32 stage, F32 maxAnisotropy );
   /// @}

   //-----------------------------------------------------------------------------

   /// @name Render State Settings
   ///
   /// NONE of these should be overridden by API implementations
   /// because of the state caching stuff.
   ///
   /// @note Might be a good idea to have a render state that sets
   /// texturing on or off (openGL style).  Right now the proper way
   /// turn off texturing is to call "setTextureStageColorOp( 0, GFXTOPDisable )"
   /// which is not intuitive.  Even the interns are calling setTexture( 0, NULL)
   /// which is wrong. -bramage
   ///
   /// @{

   ///
   void setCullMode( GFXCullMode mode );
   GFXCullMode getCullMode();

   void setNormalizeNormals( bool doNormalize );
   void setZEnable( bool enable );
   void setZWriteEnable( bool enable );
   void setZFunc( GFXCmpFunc func );
   void setZBias( U32 zBias );
   void setSlopeScaleDepthBias(U32 slopebias)
   {
	   trackRenderState(GFXRSSlopeScaleDepthBias, slopebias);
   }
   void enableColorWrites( bool red, bool green, bool blue, bool alpha );
   void setFillMode( GFXFillMode mode );
   void setShadeMode( GFXShadeMode mode );
   void setSpecularEnable( bool enable );
   void setAmbientLightColor( ColorI color );
   void setVertexColorEnable( bool enable );
   void setDiffuseMaterialSource( GFXMaterialColorSource src );
   void setSpecularMaterialSource( GFXMaterialColorSource src );
   void setAmbientMaterialSource( GFXMaterialColorSource src );
   void setEmissiveMaterialSource( GFXMaterialColorSource src );
   void setLightingEnable( bool enable );
   void setAlphaTestEnable( bool enable );
   void setSrcBlend( GFXBlend blend );
   void setDestBlend( GFXBlend blend );
   void setBlendOp( GFXBlendOp blendOp );
   void setSeparateAlphaBlendEnable( bool enable);
   void setBlendFactor(U32 color);
   void setSrcBlendAlpha( GFXBlend blend );
   void setDestBlendAlpha( GFXBlend blend );
   void setAlphaRef( U8 alphaVal );
   void setAlphaFunc( GFXCmpFunc func );
   void setAlphaBlendEnable( bool enable );
   void setStencilEnable( bool enable );
   void setStencilFailOp( GFXStencilOp op );
   void setStencilZFailOp( GFXStencilOp op );
   void setStencilPassOp( GFXStencilOp op );
   void setStencilFunc( GFXCmpFunc func );
   void setStencilRef( U32 ref );
   void setStencilMask( U32 mask );
   void setStencilWriteMask( U32 mask );
   void setTextureFactor( ColorI color );
   void setPointSize(U32 mask);
   /// @}
public:
   //-----------------------------------------------------------------------------

   /// @name State-tracker interface
   /// @{

   /// Sets the dirty Render/Texture/Sampler states from the caching system
   void updateStates(bool forceSetAll = false);

   /// Returns the setting for a particular render state
   /// @param   state   Render state to get status of
   U32 getRenderState( U32 state ) const;

   /// Returns the texture stage state
   /// @param   stage   Texture unit to query
   /// @param   state   State to get status of
   U32 getTextureStageState( U32 stage, U32 state ) const;

   /// Returns the sampler state
   /// @param   stage   Texture unit to query
   /// @param   state   State to get status of  
   U32 getSamplerState( U32 stage, U32 type ) const;
   /// @}

   //-----------------------------------------------------------------------------

   /// @name Matrix interface
   /// @{

   /// Sets the top of the world matrix stack
   /// @param   newWorld   New world matrix to set
   void setWorldMatrix( const MatrixF &newWorld );

   /// Gets the matrix on the top of the world matrix stack
   /// @param   out   Where to store the matrix
   const MatrixF &getWorldMatrix() const;

   /// Pushes the world matrix stack and copies the current top
   /// matrix to the new top of the stack
   void pushWorldMatrix();

   /// Pops the world matrix stack
   void popWorldMatrix();

   /// Sets the projection matrix
   /// @param   newProj   New projection matrix to set
   void setProjectionMatrix( const MatrixF &newProj );

   /// Gets the projection matrix
   /// @param   out   Where to store the projection matrix
   const MatrixF &getProjectionMatrix() const;

   /// Sets the view matrix
   /// @param   newView   New view matrix to set
   void setViewMatrix( const MatrixF &newView );

   /// Gets the view matrix
   /// @param   out   Where to store the view matrix
   const MatrixF &getViewMatrix() const;

   /// Multiplies the matrix at the top of the world matrix stack by a matrix
   /// and replaces the top of the matrix stack with the result
   /// @param   mat   Matrix to multiply
   void multWorld( const MatrixF &mat );

   /// Set texture matrix for a sampler
   void setTextureMatrix( const U32 stage, const MatrixF &texMat );

   virtual void setViewport( const RectI &rect ) = 0;
   virtual const RectI &getViewport() const = 0;

   inline void setClipPos( const Point2I &pos){ mClipPos = pos;}
   inline Point2I getClipPos(){ return mClipPos;}

   virtual void setClipRect( const RectI &rect ) = 0;
   virtual void setClipRectOrtho( const RectI &rect, const RectI &orthoRect ) = 0;
   virtual const RectI &getClipRect() const = 0;

   virtual	void       setFrustum(	F32 left, F32 right,
												F32 bottom, F32 top,
												F32 nearPlane, F32 farPlane,
												bool bRotate	= true);
   void getFrustum(F32 *left, F32 *right, F32 *bottom, F32 *top, F32 *nearPlane, F32 *farPlane );

   virtual void setFrustum( F32 FOV, F32 aspectRatio, F32 nearPlane, F32 farPlane );

   /// This will construct and apply an orthographic projection matrix with the provided parameters
   /// @param doRotate If set to true, the resulting matrix will be rotated PI/2 around the X axis
   //                  for support in tsShapeInstance. You probably want to leave this as 'false'.
   void setOrtho(F32 left, F32 right, F32 bottom, F32 top, F32 nearPlane, F32 farPlane, bool doRotate = false);

   /// should this even be in GFX layer? -bramage
   F32 projectRadius( F32 dist, F32 radius );

   /// @}

   /// These methods are designed to be overridden by specific GFX implementations
   /// for optimizations.
   /// @{
protected:
   GFXRenderBinTypes mCurrentBinType;
   virtual void _beginBin() {};
   virtual void _endBin() {};

   GFXDrawUtil *mDrawer;

public:
   void beginBinNotify( GFXRenderBinTypes type ) 
   {
      mCurrentBinType = type; 
      _beginBin(); 
   }
   
   void endBinNotify() 
   {
      _endBin(); 
      mCurrentBinType = GFXBin_NumRenderBins; 
   }

   /// @}

   enum GenericShaderType
   {
      GSColor = 0,
      GSTexture,
      GSModColorTexture,
      GSAddColorTexture,
      GS_COUNT
   };

   /// This is a helper function to set a default shader for rendering GUI elements
   /// on systems which do not support fixed-function operations as well as for
   /// things which need just generic position/texture/color shaders
   ///
   /// @param  type  Type of generic shader, add your own if you need
   virtual void setupGenericShaders( GenericShaderType type = GSColor ) {};

   /// Get the fill convention for this device
   virtual F32 getFillConventionOffset() const = 0;

   virtual U32 getMaxDynamicVerts() = 0;
   virtual U32 getMaxDynamicIndices() = 0;

   virtual U32 getMultisampleLevel() { return 0; }

   virtual void doParanoidStateCheck(){};

   /// Get access to this device's drawing utility class.
   inline GFXDrawUtil *getDrawUtil()
   {
      return mDrawer;
   }

	/// @State Block
	/// @{
	void setCurStateBlock(GFXStateBlock *sb);
	GFXStateBlock* getCurStateBlock();
	void applyCurStateBlock();
	/// @}
	

	virtual void beginStateBlock(){};
	virtual void endStateBlock(GFXStateBlock *sb){};

#ifndef POWER_SHIPPING
   /// This is a method designed for debugging. It will allow you to dump the states
   /// in the render manager out to a file so that it can be diffed and examined.
   void _dumpStatesToFile( const char *fileName ) const;
#else
   void _dumpStatesToFile( const char *fileName ) const {};
#endif
private:
	static GFXStateBlock* mBaseRenderSB;
	static GFXStateBlock* mBaseDrawSB;
	//static GFXStateBlock* mDefaultSB;
public:
	static void initsb();
	static void shutdown();
	//
	//设备丢失时调用
	//
	static void releaseStateBlock();
	//
	//设备重置时调用
	//
	static void resetStateBlock();
}; 

//-----------------------------------------------------------------------------
// Texture Stage States

inline void GFXDevice::setTextureStageColorOp( U32 stage, GFXTextureOp op ) 
{
   trackTextureStageState( stage, GFXTSSColorOp, op );
}

inline void GFXDevice::setTextureStageColorArg1( U32 stage, U32 argFlags )
{
   trackTextureStageState( stage, GFXTSSColorArg1, argFlags );
}

inline void GFXDevice::setTextureStageColorArg2( U32 stage, U32 argFlags )
{
   trackTextureStageState( stage, GFXTSSColorArg2, argFlags );
}

inline void GFXDevice::setTextureStageColorArg3( U32 stage, U32 argFlags )
{
   trackTextureStageState( stage, GFXTSSColorArg0, argFlags );
}

inline void GFXDevice::setTextureStageAlphaOp( U32 stage, GFXTextureOp op )
{
   trackTextureStageState( stage, GFXTSSAlphaOp, op );
}

inline void GFXDevice::setTextureStageResultArg( U32 stage, GFXTextureArgument arg )
{
   trackTextureStageState( stage, GFXTSSResultArg, arg );
}

inline void GFXDevice::setTextureStageAlphaArg1( U32 stage, U32 argFlags )
{
   trackTextureStageState( stage, GFXTSSAlphaArg1, argFlags );
}

inline void GFXDevice::setTextureStageAlphaArg2( U32 stage, U32 argFlags )
{
   trackTextureStageState( stage, GFXTSSAlphaArg2, argFlags );
}

inline void GFXDevice::setTextureStageTransform( U32 stage, U32 flags )
{
   trackTextureStageState( stage, GFXTSSTextureTransformFlags, flags );
}


// Utility function
inline U32 F32toU32( F32 f ) { return *((U32*)&f); }

inline void GFXDevice::setTextureStageBumpEnvMat( U32 stage, F32 mat[2][2] )
{
   trackTextureStageState( stage, GFXTSSBumpEnvMat00, F32toU32( mat[0][0] ) );
   trackTextureStageState( stage, GFXTSSBumpEnvMat01, F32toU32( mat[0][1] ) );
   trackTextureStageState( stage, GFXTSSBumpEnvMat10, F32toU32( mat[1][0] ) );
   trackTextureStageState( stage, GFXTSSBumpEnvMat11, F32toU32( mat[1][1] ) );
}

//-----------------------------------------------------------------------------
// Sampler States

inline void GFXDevice::setTextureStageAddressModeU( U32 stage, GFXTextureAddressMode mode )
{
   trackSamplerState( stage, GFXSAMPAddressU, mode );
}

inline void GFXDevice::setTextureStageAddressModeV( U32 stage, GFXTextureAddressMode mode )
{
   trackSamplerState( stage, GFXSAMPAddressV, mode );
}

inline void GFXDevice::setTextureStageAddressModeW( U32 stage, GFXTextureAddressMode mode )
{
   trackSamplerState( stage, GFXSAMPAddressW, mode );
}

inline void GFXDevice::setTextureStageBorderColor( U32 stage, ColorI color )
{
   trackSamplerState( stage, GFXSAMPBorderColor,   42/*color*/ ); // ToDo: Needs proper cast
}

inline void GFXDevice::setTextureStageMagFilter( U32 stage, GFXTextureFilterType filter )
{
   trackSamplerState( stage, GFXSAMPMagFilter, filter ); 
}

inline void GFXDevice::setTextureStageMinFilter( U32 stage, GFXTextureFilterType filter )
{
   trackSamplerState( stage, GFXSAMPMinFilter, filter );
}

inline void GFXDevice::setTextureStageMipFilter( U32 stage, GFXTextureFilterType filter )
{
   trackSamplerState( stage, GFXSAMPMipFilter, filter );
}

inline void GFXDevice::setTextureStageLODBias( U32 stage, F32 bias )
{
   trackSamplerState( stage, GFXSAMPMipMapLODBias, F32toU32( bias ) );
}

inline void GFXDevice::setTextureStageMaxMipLevel( U32 stage, U32 maxMiplevel )
{
   trackSamplerState( stage, GFXSAMPMaxMipLevel, maxMiplevel );
}

inline void GFXDevice::setTextureStageMaxAnisotropy( U32 stage, F32 maxAnisotropy )
{
   trackSamplerState( stage, GFXSAMPMaxAnisotropy, (U32)maxAnisotropy );
}

//-----------------------------------------------------------------------------
// Render States

inline void GFXDevice::enableColorWrites( bool red, bool green, bool blue, bool alpha )
{
   U32 mask = 0;

   mask |= ( red   ? GFXCOLORWRITEENABLE_RED   : 0 );
   mask |= ( green ? GFXCOLORWRITEENABLE_GREEN : 0 );
   mask |= ( blue  ? GFXCOLORWRITEENABLE_BLUE  : 0 );
   mask |= ( alpha ? GFXCOLORWRITEENABLE_ALPHA : 0 );

   trackRenderState( GFXRSColorWriteEnable, mask );
}

inline void GFXDevice::setAlphaBlendEnable( bool enable )
{
   trackRenderState( GFXRSAlphaBlendEnable, enable );
}

inline void GFXDevice::setAlphaFunc( GFXCmpFunc func )
{
   trackRenderState( GFXRSAlphaFunc, func );
}

inline void GFXDevice::setAlphaRef( U8 alphaVal )
{
   trackRenderState( GFXRSAlphaRef, alphaVal );
}

inline void GFXDevice::setAlphaTestEnable( bool enable )
{
   trackRenderState( GFXRSAlphaTestEnable, enable );
}

inline void GFXDevice::setAmbientLightColor( ColorI color )
{
   trackRenderState( GFXRSAmbient, color.getARGBPack() );
}

inline void GFXDevice::setVertexColorEnable( bool enable )
{
   trackRenderState( GFXRSColorVertex, enable );
}

inline void GFXDevice::setAmbientMaterialSource( GFXMaterialColorSource src )
{
   trackRenderState( GFXRSAmbientMaterialSource, src );
}

inline void GFXDevice::setBlendOp( GFXBlendOp blendOp )
{
   trackRenderState( GFXRSBlendOp, blendOp );
}

inline void GFXDevice::setCullMode( GFXCullMode mode )
{
   trackRenderState( GFXRSCullMode, mode );
}

inline GFXCullMode GFXDevice::getCullMode()
{
   return (GFXCullMode) mStateTracker[GFXRSCullMode].currentValue;
}

inline void GFXDevice::setDestBlend( GFXBlend blend )
{
   trackRenderState( GFXRSDestBlend, blend );
}

inline void GFXDevice::setDestBlendAlpha( GFXBlend blend )
{
	trackRenderState( GFXRSDestBlendAlpha, blend );
}

inline void GFXDevice::setDiffuseMaterialSource( GFXMaterialColorSource src )
{
   trackRenderState( GFXRSDiffuseMaterialSource, src );
}

inline void GFXDevice::setEmissiveMaterialSource( GFXMaterialColorSource src )
{
   trackRenderState( GFXRSEmissiveMaterialSource, src );
}

inline void GFXDevice::setFillMode( GFXFillMode mode )
{
   trackRenderState( GFXRSFillMode, mode ); 
}

inline void GFXDevice::setLightingEnable( bool enable )
{
   trackRenderState( GFXRSLighting, enable );
}

inline void GFXDevice::setNormalizeNormals( bool doNormalize )
{
   trackRenderState( GFXRSNormalizeNormals, doNormalize );
}

inline void GFXDevice::setShadeMode( GFXShadeMode mode )
{
   trackRenderState( GFXRSShadeMode, mode );
}

inline void GFXDevice::setSpecularEnable( bool enable )
{
   trackRenderState( GFXRSSpecularEnable, enable );
}

inline void GFXDevice::setSpecularMaterialSource( GFXMaterialColorSource src )
{
   trackRenderState( GFXRSSpecularMaterialSource, src );
}

inline void GFXDevice::setBlendFactor( U32 color)
{
	trackRenderState( GFXRSBlendFactor, color );
}

inline void GFXDevice::setSrcBlend( GFXBlend blend )
{
   trackRenderState( GFXRSSrcBlend, blend );
}

inline void GFXDevice::setSrcBlendAlpha( GFXBlend blend )
{
	trackRenderState( GFXRSSrcBlendAlpha, blend );
}

inline void GFXDevice::setSeparateAlphaBlendEnable( bool enable)
{
	trackRenderState( GFXRSSeparateAlphaBlendEnable, enable?1:0 );
}

inline void GFXDevice::setStencilEnable( bool enable )
{
   trackRenderState( GFXRSStencilEnable, enable );
}

inline void GFXDevice::setStencilFailOp( GFXStencilOp op )
{
   trackRenderState( GFXRSStencilFail, op );
}

inline void GFXDevice::setStencilFunc( GFXCmpFunc func )
{
   trackRenderState( GFXRSStencilFunc, func );
}

inline void GFXDevice::setStencilMask( U32 mask )
{
   trackRenderState( GFXRSStencilMask, mask );
}

inline void GFXDevice::setStencilPassOp( GFXStencilOp op )
{
   trackRenderState( GFXRSStencilPass, op );
}

inline void GFXDevice::setStencilRef( U32 ref )
{
   trackRenderState( GFXRSStencilRef, ref );
}

inline void GFXDevice::setStencilWriteMask( U32 mask )
{
   trackRenderState( GFXRSStencilWriteMask, mask );
}

inline void GFXDevice::setStencilZFailOp( GFXStencilOp op )
{
   trackRenderState( GFXRSStencilZFail, op );
}

inline void GFXDevice::setTextureFactor( ColorI color )
{
   trackRenderState( GFXRSTextureFactor, color.getARGBPack() );
}


inline void GFXDevice::setZBias( U32 zBias )
{
   trackRenderState( GFXRSDepthBias, zBias );
}

inline void GFXDevice::setZEnable( bool enable )
{
   trackRenderState( GFXRSZEnable, enable );
}

inline void GFXDevice::setZFunc( GFXCmpFunc func )
{
   trackRenderState( GFXRSZFunc, func );
}

inline void GFXDevice::setZWriteEnable( bool enable )
{
   trackRenderState( GFXRSZWriteEnable, enable );
}

inline void GFXDevice::setPointSize( U32 mask )
{
	trackRenderState( GFXRSPointSize, mask );
}


//-----------------------------------------------------------------------------
// State caching methods

inline void GFXDevice::trackRenderState( U32 state, U32 value )
{
#ifdef POWER_GFX_STATE_DEBUG
   U32 prevValue = mStateTracker[state].newValue;
#endif

   if( !mStateTracker[state].dirty )
   {
      if( mStateTracker[state].currentValue == value ) 
         return;

      // Update our internal data.
      mStateTracker[state].dirty = true;
      mTrackedState[mNumDirtyStates] = state;
      mNumDirtyStates++;
      mStateDirty = true;

#ifdef POWER_GFX_STATE_DEBUG
      prevValue = mStateTracker[state].currentValue;
#endif
   }

   // Update the state stack.
   if(mStateStackDepth)
      mStateStack[mStateStackDepth].trackRenderState(state, mStateTracker[state].newValue);

   mStateTracker[state].newValue = value;

#ifdef POWER_GFX_STATE_DEBUG
   mDebugStateManager->submitStateChange( GFXDebugState::GFXRenderStateEvent, state, prevValue, value );
#endif
}

inline void GFXDevice::trackTextureStageState( U32 stage, U32 state, U32 value )
{
#ifdef POWER_GFX_STATE_DEBUG
   U32 prevValue = mTextureStateTracker[stage][state].newValue;
#endif

   if( !mTextureStateTracker[stage][state].dirty )
   {
      if( mTextureStateTracker[stage][state].currentValue == value ) 
         return;

      // Update our internal data.
      mTextureStateTracker[stage][state].dirty = true;
      mStateDirty = true;
      mTextureTrackedState[mNumDirtyTextureStates].state = state;
      mTextureTrackedState[mNumDirtyTextureStates].stage = stage;
      mNumDirtyTextureStates++;

#ifdef POWER_GFX_STATE_DEBUG
      prevValue = mTextureStateTracker[stage][state].currentValue;
#endif
   }

   // Update the state stack.
   if(mStateStackDepth)
      mStateStack[mStateStackDepth].trackTextureStageState(stage, state, mTextureStateTracker[stage][state].newValue);

   mTextureStateTracker[stage][state].newValue = value;

#ifdef POWER_GFX_STATE_DEBUG
   mDebugStateManager->submitStateChange( GFXDebugState::GFXTextureStageStateEvent, state, prevValue, value, stage );
#endif 
}


inline void GFXDevice::trackSamplerState( U32 stage, U32 type, U32 value )
{
#ifdef POWER_GFX_STATE_DEBUG
   U32 prevValue = mSamplerStateTracker[stage][type].newValue;
#endif

   if(!mSamplerStateTracker[stage][type].dirty)
   {
      if( mSamplerStateTracker[stage][type].currentValue == value ) 
         return;

      // Update our internal data.
      mSamplerStateTracker[stage][type].dirty = true;
      mStateDirty = true;
      mSamplerTrackedState[mNumDirtySamplerStates].state = type;
      mSamplerTrackedState[mNumDirtySamplerStates].stage = stage;
      mNumDirtySamplerStates++;

#ifdef POWER_GFX_STATE_DEBUG
      prevValue = mSamplerStateTracker[stage][type].currentValue;
#endif
   }

   // Update the state stack.
   if(mStateStackDepth)
      mStateStack[mStateStackDepth].trackSamplerState(stage, type, mSamplerStateTracker[stage][type].newValue);

   mSamplerStateTracker[stage][type].newValue = value;

#ifdef POWER_GFX_STATE_DEBUG
   mDebugStateManager->submitStateChange( GFXDebugState::GFXSamplerStateEvent, type, prevValue, value, stage );
#endif
}


//-----------------------------------------------------------------------------
// State tracker interface

inline U32 GFXDevice::getRenderState( U32 state ) const 
{
   return mStateTracker[state].newValue;
}

inline U32 GFXDevice::getTextureStageState( U32 stage, U32 state ) const 
{
   return mTextureStateTracker[stage][state].newValue;
}

inline U32 GFXDevice::getSamplerState( U32 stage, U32 type ) const 
{
   return mSamplerStateTracker[stage][type].newValue;
}

inline void GFXDevice::initRenderState( U32 state, U32 value ) 
{
   mStateTracker[state].dirty = false; 
   mStateTracker[state].newValue = value;
   mStateTracker[state].currentValue = value;
}

inline void GFXDevice::initTextureState( U32 stage, U32 state, U32 value) 
{
   mTextureStateTracker[stage][state].dirty = false; 
   mTextureStateTracker[stage][state].newValue = value;
   mTextureStateTracker[stage][state].currentValue = value;
}

inline void GFXDevice::initSamplerState( U32 stage, U32 state, U32 value ) 
{
   mSamplerStateTracker[stage][state].dirty = false; 
   mSamplerStateTracker[stage][state].newValue = value;
   mSamplerStateTracker[stage][state].currentValue = value;
}

//-----------------------------------------------------------------------------
// Matrix interface

inline void GFXDevice::setWorldMatrix( const MatrixF &newWorld )
{
   mWorldMatrixDirty = true;
   mStateDirty = true;
   mWorldMatrix[mWorldStackSize] = newWorld;
}

inline const MatrixF &GFXDevice::getWorldMatrix() const 
{
   return mWorldMatrix[mWorldStackSize];
}

inline void GFXDevice::pushWorldMatrix()
{
   mWorldMatrixDirty = true;
   mStateDirty = true;
   mWorldStackSize++;
   if( mWorldStackSize >= WORLD_STACK_MAX )
   {
      AssertFatal( false, "GFX: Exceeded world matrix stack size" );
   }
   mWorldMatrix[mWorldStackSize] = mWorldMatrix[mWorldStackSize - 1];
}

inline void GFXDevice::popWorldMatrix()
{
   mWorldMatrixDirty = true;
   mStateDirty = true;
   mWorldStackSize--;
   if( mWorldStackSize < 0 )
   {
      AssertFatal( false, "GFX: Negative WorldStackSize!" );
   }
}

inline void GFXDevice::multWorld( const MatrixF &mat )
{
   mWorldMatrixDirty = true;
   mStateDirty = true;
   mWorldMatrix[mWorldStackSize].mul(mat);
}

inline void GFXDevice::setProjectionMatrix( const MatrixF &newProj )
{
   mProjectionMatrixDirty = true;
   mStateDirty = true;
   mProjectionMatrix = newProj;
}

inline const MatrixF &GFXDevice::getProjectionMatrix() const 
{
   return mProjectionMatrix;
}

inline void GFXDevice::setViewMatrix( const MatrixF &newView )
{
   mStateDirty = true;
   mViewMatrixDirty = true;
   mViewMatrix = newView;
}

inline const MatrixF &GFXDevice::getViewMatrix() const 
{
   return mViewMatrix;
}

inline void GFXDevice::setTextureMatrix( const U32 stage, const MatrixF &texMat )
{
   AssertFatal( stage < TEXTURE_STAGE_COUNT, "Out of range texture sampler" );
   mStateDirty = true;
   mTextureMatrixDirty[stage] = true;
   mTextureMatrix[stage] = texMat;
   mTextureMatrixCheckDirty = true;
}

//-----------------------------------------------------------------------------
// Misc
inline const Vector<GFXVideoMode> *const GFXDevice::getVideoModeList() const 
{
   return &mVideoModes;
}

//-----------------------------------------------------------------------------
// Buffer management

inline void GFXDevice::setVertexBuffer( GFXVertexBuffer *buffer )
{
   if( buffer == mCurrentVertexBuffer )
      return;

   mCurrentVertexBuffer = buffer;
   mVertexBufferDirty = true;
   mStateDirty = true;
}

//-----------------------------------------------------------------------------
// Immediate mode


inline bool GFXDevice::canCurrentlyRender() 
{
   return mCanCurrentlyRender;
}

inline bool GFXDevice::allowRender()
{
   return mAllowRender;
}

inline void GFXDevice::setAllowRender( bool render )
{
   mAllowRender = render;
}

//------------------------------------------------------------------------------
// State Block

inline void GFXDevice::setCurStateBlock(GFXStateBlock *sb)
{
	if (mCurStateBlock != sb)
	{
		mCurStateBlock = sb;
		mCurStateBlockDirty = true;
	}
}

inline GFXStateBlock* GFXDevice::getCurStateBlock()
{
	return mCurStateBlock;
}

inline void GFXDevice::applyCurStateBlock()
{
	if (mCurStateBlock)
	{
		mCurStateBlock->apply();
	}
}
#endif
