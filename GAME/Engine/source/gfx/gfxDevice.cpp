//-----------------------------------------------------------------------------
// PowerEngine
// 
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include "gfx/gfxFlashManager.h"
#include "gfx/gfxDevice.h"

#include "console/console.h"
#include "gfx/gfxInit.h"
#include "core/frameAllocator.h"
#include "gfx/gFont.h"
#include "gfx/gfxTextureHandle.h"
#include "gfx/gfxCubemap.h"
#include "gfx/debugDraw.h"
#include "platform/profiler.h"
#include "core/unicode.h"
#include "gfx/gfxFence.h"
#include "gfx/gfxFontRenderBatcher.h"
#include "gfx/gfxDrawUtil.h"
#include "console/consoleTypes.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
GFXStateBlock* GFXDevice::mBaseRenderSB = NULL;
GFXStateBlock* GFXDevice::mBaseDrawSB = NULL;
//#define STATEBLOCK

Vector<GFXDevice *> GFXDevice::smGFXDevice;
S32 GFXDevice::smActiveDeviceIndex = -1;
GFXDevice::DeviceEventSignal* GFXDevice::smSignalGFXDeviceEvent = NULL;
S32 GFXDevice::smSfxBackBufferSize = 64;
U32 GFXDevice::mDPTimes = 0;

//-----------------------------------------------------------------------------

// Static method
#ifndef POWER_GFX_STATE_DEBUG
GFXDevice *GFXDevice::get()
#else
GFXDevice *GFXDevice::get( const char *fileName, U32 line ) 
#endif
{
   AssertFatal( smActiveDeviceIndex > -1 && smActiveDeviceIndex < smGFXDevice.size() 
               && smGFXDevice[smActiveDeviceIndex] != NULL, 
      "Attempting to get invalid GFX device. GFX may not have been initialized!" );

#ifdef POWER_GFX_STATE_DEBUG
   smGFXDevice[smActiveDeviceIndex]->mLastGFXCallFileName = fileName;
   smGFXDevice[smActiveDeviceIndex]->mLastGFXCallLine = line;
#endif

   return smGFXDevice[smActiveDeviceIndex];
}

GFXDevice::DeviceEventSignal& GFXDevice::getDeviceEventSignal()
{
   if (smSignalGFXDeviceEvent == NULL)
   {
      smSignalGFXDeviceEvent = new GFXDevice::DeviceEventSignal();
   }
   return *smSignalGFXDeviceEvent;
}

//-----------------------------------------------------------------------------

GFXDevice::GFXDevice() 
{ 
   mNumDirtyStates = 0;
   mNumDirtyTextureStates = 0;
   mNumDirtySamplerStates = 0;
   
   mWorldMatrixDirty = false;
   mWorldStackSize = 0;
   mProjectionMatrixDirty = false;
   mViewMatrixDirty = false;
   mTextureMatrixCheckDirty = false;

	mCurStateBlockDirty = false;
	mCurStateBlock = NULL;

   mViewMatrix.identity();
   mProjectionMatrix.identity();
   
   for( int i = 0; i < WORLD_STACK_MAX; i++ )
      mWorldMatrix[i].identity();
   
   mDeviceIndex = smGFXDevice.size();
   smGFXDevice.push_back( this );
   
   if( smActiveDeviceIndex == -1 )
      smActiveDeviceIndex = 0;
      
   // Vertex buffer cache
   mVertexBufferDirty = false;
   
   // Primitive buffer cache
   mPrimitiveBufferDirty = false;
   mTexturesDirty = false;
   
   // Use of TEXTURE_STAGE_COUNT in initialization is okay [7/2/2007 Pat]
   for(U32 i = 0; i < TEXTURE_STAGE_COUNT; i++)
   {
      mTextureDirty[i] = false;
      mCurrentTexture[i] = NULL;
      mNewTexture[i] = NULL;
      mCurrentCubemap[i] = NULL;
      mNewCubemap[i] = NULL;
      mTexType[i] = GFXTDT_Normal;

      mTextureMatrix[i].identity();
      mTextureMatrixDirty[i] = false;
   }

   mLightsDirty = false;
   for(U32 i = 0; i < LIGHT_STAGE_COUNT; i++)
   {
      mLightDirty[i] = false;
      mCurrentLightEnable[i] = false;
   }

   mGlobalAmbientColorDirty = false;
   mGlobalAmbientColor = ColorF(0.0f, 0.0f, 0.0f, 1.0f);

   mLightMaterialDirty = false;
   dMemset(&mCurrentLightMaterial, NULL, sizeof(GFXLightMaterial));

   // Initialize the state stack.
   mStateStackDepth = 0;
   mStateStack[0].init();

   // misc
   mAllowRender = true;

   mInitialized = false;

   mCurrentFrontBufferIdx = 0;

   mCurrentBinType = GFXBin_NumRenderBins;

   mDeviceSwizzle32 = NULL;
   mDeviceSwizzle24 = NULL;

   mResourceListHead = NULL;

   for(int i=0;i<MAX_BACKBUFFER_NUM;i++)
	   mSfxBackBuffer[i] = NULL;

#ifdef POWER_GFX_STATE_DEBUG
   mDebugStateManager = new GFXDebugState::GFXDebugStateManager( this );
#endif

   mClipPos = Point2I(0,0);

   // Initialize our drawing utility.
   mDrawer = new GFXDrawUtil(this);

#ifndef NTJ_SERVER
   m_flashManager = new gfxFlashManager;
#endif

   mDmapSet = false;
}

//-----------------------------------------------------------------------------
void GFXDevice::deviceOnInit()
{
#ifndef NTJ_SERVER
    if (m_flashManager)
        m_flashManager->Reset();
#endif
}

void GFXDevice::deviceInited()
{
    getDeviceEventSignal().trigger(deInit);

	//初始化状态块
	GFXStateBlock::init();

#ifndef NTJ_SERVER
    //初始化flash 设备
    if (m_flashManager)
        m_flashManager->Init();
#endif
}

// Static method
void GFXDevice::create()
{
   Con::addVariable("pref::Video::sfxBackBufferSize", TypeS32, &GFXDevice::smSfxBackBufferSize);
}

//-----------------------------------------------------------------------------

// Static method
void GFXDevice::destroy()
{
   // Make this release its buffer.
   PrimBuild::shutdown();

   // Let people know we are shutting down
   if (smSignalGFXDeviceEvent)
   {
      smSignalGFXDeviceEvent->trigger(deDestroy);
      delete smSignalGFXDeviceEvent;
      smSignalGFXDeviceEvent = NULL;
   }

   // Destroy this way otherwise we are modifying the loop end case
   U32 arraySize = smGFXDevice.size();

   // Call preDestroy on them all
   for( U32 i = 0; i < arraySize; i++ ) 
   {
      GFXDevice *curr = smGFXDevice[i];

      curr->preDestroy();
   }    

   // Delete all the devices - make sure to have the device be active
   // when it is deleted, just in case it needs to reference itself.
   for( U32 i = 0; i < arraySize; i++ )
   {
      GFXDevice *curr = smGFXDevice[0];
      smActiveDeviceIndex = 0;
      SAFE_DELETE( curr );       
   }
	//释放状态块
	GFXStateBlock::shutdown();
}

//-----------------------------------------------------------------------------

// Static method
void GFXDevice::setActiveDevice( U32 deviceIndex )
{
   AssertFatal( deviceIndex < smGFXDevice.size(), "Device non-existant" );
   
   smActiveDeviceIndex = deviceIndex;
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

GFXDevice::~GFXDevice()
{ 
   SAFE_DELETE( mDrawer );

#ifndef NTJ_SERVER
   SAFE_DELETE(m_flashManager);
#endif

   if( smActiveDeviceIndex == mDeviceIndex )
      smActiveDeviceIndex = 0;

   // Loop through all the GFX devices to find ourself.  If we find ourself, 
   // remove ourself from the vector and decrease the device index of all devices 
   // which came after us.  
   for(Vector<GFXDevice *>::iterator i = smGFXDevice.begin(); i != smGFXDevice.end(); i++)
   {
      if( (*i)->mDeviceIndex == mDeviceIndex )
      {
         for (Vector<GFXDevice *>::iterator j = i + 1; j != smGFXDevice.end(); j++)
            (*j)->mDeviceIndex--;
         smGFXDevice.erase( i );
         break;
      }
   }

   /// In the future, we may need to separate out this code block that clears out the GFXDevice refptrs, so that
   /// derived classes can clear them out before releasing the device or something. BTR
   for(int i=0;i<MAX_BACKBUFFER_NUM;i++)
		mSfxBackBuffer[i] = NULL;

   // Clean up our current PB, if any.
   mCurrentPrimitiveBuffer = NULL;
   mCurrentVertexBuffer = NULL;

   // Clear out our current texture references
   for (U32 i = 0; i < TEXTURE_STAGE_COUNT; i++)
   {
      mCurrentTexture[i] = NULL;
      mNewTexture[i] = NULL;
      mCurrentCubemap[i] = NULL;
      mNewCubemap[i] = NULL;
   }

   // Check for resource leaks
#ifdef POWER_DEBUG
   GFXTextureObject::dumpActiveTOs();
   GFXPrimitiveBuffer::dumpActivePBs();
#endif

   SAFE_DELETE( mTextureManager );

#ifdef POWER_GFX_STATE_DEBUG
   SAFE_DELETE( mDebugStateManager );
#endif

   /// End Block above BTR

   // -- Clear out resource list
   // Note: our derived class destructor will have already released resources.
   // Clearing this list saves us from having our resources (which are not deleted
   // just released) turn around and try to remove themselves from this list.
   while (mResourceListHead)
   {
      GFXResource * head = mResourceListHead;
      mResourceListHead = head->mNextResource;
      
      head->mPrevResource = NULL;
      head->mNextResource = NULL;
      head->mOwningDevice = NULL;
   }

   // mark with bad device index so we can detect already deleted device
   mDeviceIndex = -1;
}

//-----------------------------------------------------------------------------

F32 GFXDevice::formatByteSize(GFXFormat format)
{
   if(format < GFXFormat_16BIT)
      return 1.0f;// 8 bit...
   else if(format < GFXFormat_24BIT)
      return 2.0f;// 16 bit...
   else if(format < GFXFormat_32BIT)
      return 3.0f;// 24 bit...
   else if(format < GFXFormat_64BIT)
      return 4.0f;// 32 bit...
   else if(format < GFXFormat_128BIT)
      return 8.0f;// 64 bit...
   else if(format < GFXFormat_UNKNOWNSIZE)
      return 16.0f;// 128 bit...
   return 4.0f;// default...
}

//-----------------------------------------------------------------------------

void GFXDevice::updateStates(bool forceSetAll /*=false*/)
{
   PROFILE_SCOPE(GFXDevice_updateStates);

   if(forceSetAll)
   {
      bool rememberToEndScene = false;
      if(!canCurrentlyRender())
      {
         beginScene();
         rememberToEndScene = true;
      }
		
		//applyCurStateBlock();

      setMatrix( GFXMatrixProjection, mProjectionMatrix );
      setMatrix( GFXMatrixWorld, mWorldMatrix[mWorldStackSize] );
      setMatrix( GFXMatrixView, mViewMatrix );

      if(mCurrentVertexBuffer.isValid())
         mCurrentVertexBuffer->prepare();

      if( mCurrentPrimitiveBuffer.isValid() ) // This could be NULL when the device is initalizing
         mCurrentPrimitiveBuffer->prepare();

      for(U32 i = 0; i < getNumSamplers(); i++)
      {
         switch (mTexType[i])
         {
            case GFXTDT_Normal :
               {
                  mCurrentTexture[i] = mNewTexture[i];
                  setTextureInternal(i, mCurrentTexture[i]);
               }  
               break;
            case GFXTDT_Cube :
               {
                  mCurrentCubemap[i] = mNewCubemap[i];
                  if (mCurrentCubemap[i])
                     mCurrentCubemap[i]->setToTexUnit(i);
                  else
                     setTextureInternal(i, NULL);
               }
               break;
            default:
               AssertFatal(false, "Unknown texture type!");
               break;
         }
      }
      
      for(S32 i=0; i<GFXRenderState_COUNT; i++)
      {
         setRenderState(i, mStateTracker[i].newValue);
         mStateTracker[i].currentValue = mStateTracker[i].newValue;
      }

      // Why is this 8 and not 16 like the others?
      for(S32 i=0; i<8; i++)
      {
         for(S32 j=0; j<GFXTSS_COUNT; j++)
         {
            StateTracker &st = mTextureStateTracker[i][j];
            setTextureStageState(i, j, st.newValue);
            st.currentValue = st.newValue;
         }
      }

      for(S32 i=0; i<8; i++)
      {
         for(S32 j=0; j<GFXSAMP_COUNT; j++)
         {
            StateTracker &st = mSamplerStateTracker[i][j];
            setSamplerState(i, j, st.newValue);
            st.currentValue = st.newValue;
         }
      }
      // Set our material
      setLightMaterialInternal(mCurrentLightMaterial);

      // Set our lights
      for(U32 i = 0; i < LIGHT_STAGE_COUNT; i++)
      {
         setLightInternal(i, mCurrentLight[i], mCurrentLightEnable[i]);
      }

       _updateRenderTargets();

      if(rememberToEndScene)
         endScene();

      return;
   }

   // Normal update logic begins here.
   mStateDirty = false;


   // Update Projection Matrix
   if( mProjectionMatrixDirty )
   {
      setMatrix( GFXMatrixProjection, mProjectionMatrix );
      mProjectionMatrixDirty = false;
   }
   
   // Update World Matrix
   if( mWorldMatrixDirty )
   {
      setMatrix( GFXMatrixWorld, mWorldMatrix[mWorldStackSize] );
      mWorldMatrixDirty = false;
   }
   
   // Update View Matrix
   if( mViewMatrixDirty )
   {
      setMatrix( GFXMatrixView, mViewMatrix );
      mViewMatrixDirty = false;
   }


   if( mTextureMatrixCheckDirty )
   {
      for( int i = 0; i < getNumSamplers(); i++ )
      {
         if( mTextureMatrixDirty[i] )
         {
            mTextureMatrixDirty[i] = false;
            setMatrix( (GFXMatrixType)(GFXMatrixTexture + i), mTextureMatrix[i] );
         }
      }

      mTextureMatrixCheckDirty = false;
   }
   
   // Update vertex buffer
   if( mVertexBufferDirty )
   {
      if(mCurrentVertexBuffer.isValid())
         mCurrentVertexBuffer->prepare();
      mVertexBufferDirty = false;
   }
   
   // Update primitive buffer
   //
   // NOTE: It is very important to set the primitive buffer AFTER the vertex buffer
   // because in order to draw indexed primitives in DX8, the call to SetIndicies
   // needs to include the base vertex offset, and the DX8 GFXDevice relies on
   // having mCurrentVB properly assigned before the call to setIndices -patw
   if( mPrimitiveBufferDirty )
   {
      if( mCurrentPrimitiveBuffer.isValid() ) // This could be NULL when the device is initalizing
         mCurrentPrimitiveBuffer->prepare();
      mPrimitiveBufferDirty = false;
   }

   // NOTE: it is important that textures are set before texture states since
   // some devices (e.g., OpenGL) set certain states on the texture.
   if( mTexturesDirty )
   {
      mTexturesDirty = false;
      for(U32 i = 0; i < TEXTURE_STAGE_COUNT; i++)
      {
         if(!mTextureDirty[i])
            continue;
         mTextureDirty[i] = false;

         switch (mTexType[i])
         {
         case GFXTDT_Normal :
            {
               mCurrentTexture[i] = mNewTexture[i];
               setTextureInternal(i, mCurrentTexture[i]);
            }  
            break;
         case GFXTDT_Cube :
            {
               mCurrentCubemap[i] = mNewCubemap[i];
               if (mCurrentCubemap[i])
                  mCurrentCubemap[i]->setToTexUnit(i);
               else
                  setTextureInternal(i, NULL);
            }
            break;
         default:
            AssertFatal(false, "Unknown texture type!");
            break;
         }
      }
   }
   
#ifndef STATEBLOCK
   // Set render states
   while( mNumDirtyStates )
   {
      mNumDirtyStates--;
      U32 state = mTrackedState[mNumDirtyStates];
      
      // Added so that an unsupported state can be set to > Max render state
      // and thus be ignored, could possibly put in a call to the gfx device
      // to handle an unsupported call so it could log it, or do some workaround
      // or something? -pw
      if( state > GFXRenderState_COUNT )
         continue;

      if( mStateTracker[state].currentValue != mStateTracker[state].newValue )
      {
         setRenderState(state, mStateTracker[state].newValue);
         mStateTracker[state].currentValue = mStateTracker[state].newValue;
      }
      mStateTracker[state].dirty = false;
   }
   
   // Set texture states
   while( mNumDirtyTextureStates )
   {
      mNumDirtyTextureStates--;
      U32 state = mTextureTrackedState[mNumDirtyTextureStates].state;
      U32 stage = mTextureTrackedState[mNumDirtyTextureStates].stage;
      StateTracker &st = mTextureStateTracker[stage][state];
      st.dirty = false;
      if( st.currentValue != st.newValue )
      {
         setTextureStageState(stage, state, st.newValue);
         st.currentValue = st.newValue;
      }
   }
   
   // Set sampler states
   while( mNumDirtySamplerStates )
   {
      mNumDirtySamplerStates--;
      U32 state = mSamplerTrackedState[mNumDirtySamplerStates].state;
      U32 stage = mSamplerTrackedState[mNumDirtySamplerStates].stage;
      StateTracker &st = mSamplerStateTracker[stage][state];
      st.dirty = false;
      if( st.currentValue != st.newValue )
      {
         setSamplerState(stage, state, st.newValue);
         st.currentValue = st.newValue;
      }
   }
#endif
   // Set light material
   if(mLightMaterialDirty)
   {
      setLightMaterialInternal(mCurrentLightMaterial);
      mLightMaterialDirty = false;
   }

   // Set our lights
   if(mLightsDirty)
   {
      mLightsDirty = false;
      for(U32 i = 0; i < LIGHT_STAGE_COUNT; i++)
      {
         if(!mLightDirty[i])
            continue;

         mLightDirty[i] = false;
         setLightInternal(i, mCurrentLight[i], mCurrentLightEnable[i]);
      }
   }

	if (mCurStateBlockDirty)
	{
		//applyCurStateBlock();
		mCurStateBlockDirty = false;
	}
   _updateRenderTargets();

#ifdef POWER_DEBUG_RENDER
   doParanoidStateCheck();
#endif
}

//-----------------------------------------------------------------------------

void GFXDevice::setPrimitiveBuffer( GFXPrimitiveBuffer *buffer )
{
   if( buffer == mCurrentPrimitiveBuffer )
      return;
   
   mCurrentPrimitiveBuffer = buffer;
   mPrimitiveBufferDirty = true;
   mStateDirty = true;
}

//-----------------------------------------------------------------------------

void GFXDevice::drawPrimitive( U32 primitiveIndex )
{
   if( mStateDirty )
      updateStates();
   
   AssertFatal( mCurrentPrimitiveBuffer.isValid(), "Trying to call drawPrimitive with no current primitive buffer, call setPrimitiveBuffer()" );
   AssertFatal( primitiveIndex < mCurrentPrimitiveBuffer->mPrimitiveCount, "Out of range primitive index.");
   GFXPrimitive *info = &mCurrentPrimitiveBuffer->mPrimitiveArray[primitiveIndex];
   
   // Do NOT add index buffer offset to this call, it will be added by drawIndexedPrimitive
   drawIndexedPrimitive( info->type, info->minIndex, info->numVertices, info->startIndex, info->numPrimitives );
}

//-----------------------------------------------------------------------------

void GFXDevice::drawPrimitives()
{
   if( mStateDirty )
      updateStates();
   
   AssertFatal( mCurrentPrimitiveBuffer.isValid(), "Trying to call drawPrimitive with no current primitive buffer, call setPrimitiveBuffer()" );

   GFXPrimitive *info = NULL;
   
   for( U32 i = 0; i < mCurrentPrimitiveBuffer->mPrimitiveCount; i++ ) {
      info = &mCurrentPrimitiveBuffer->mPrimitiveArray[i];

      // Do NOT add index buffer offset to this call, it will be added by drawIndexedPrimitive
      drawIndexedPrimitive( info->type, info->minIndex, info->numVertices, info->startIndex, info->numPrimitives );
   }
}

//-------------------------------------------------------------
// Console functions
//-------------------------------------------------------------
ConsoleFunction( getDisplayDeviceList, const char*, 1, 1, "Returns a tab-seperated string of the detected devices.")
{
   Vector<GFXAdapter*> adapters;
   GFXInit::getAdapters(&adapters);
   
   U32 deviceCount = adapters.size();
   if (deviceCount < 1)
      return NULL;
   
   U32 strLen = 0, i;
   for ( i = 0; i < deviceCount; i++ )
      strLen += ( dStrlen( adapters[i]->mName ) + 1 );

   char* returnString = Con::getReturnBuffer( strLen );
   dStrcpy( returnString, strLen, adapters[0]->mName );
   for ( i = 1; i < deviceCount; i++ )
   {
      dStrcat( returnString, strLen, "\t" );
      dStrcat( returnString, strLen, adapters[i]->mName );
   }
   
   return( returnString );
}

ConsoleFunction( getDisplayDeviceInformation, const char*, 1, 1, "Get a string describing the current GFX device")
{
   if (!GFXDevice::devicePresent())
      return StringTable->insert("(no device)");

   const GFXAdapter& adapter = GFX->getAdapter();

   U32 bufSize = 4096;
   char* ret = Con::getReturnBuffer(bufSize);
   dSprintf(ret, bufSize, "%s", adapter.getName());
   return ret;
}

//-----------------------------------------------------------------------------
// Set projection frustum
//-----------------------------------------------------------------------------
void GFXDevice::setFrustum(F32 left, 
                           F32 right, 
                           F32 bottom, 
                           F32 top, 
                           F32 nearPlane, 
                           F32 farPlane,
                           bool bRotate)
{
   // store values
   mFrustLeft = left;
   mFrustRight = right;
   mFrustBottom = bottom;
   mFrustTop = top;
   mFrustNear = nearPlane;
   mFrustFar = farPlane;
   mFrustOrtho = false;

   // compute matrix
   MatrixF projection;

   Point4F row;
   row.x = 2.0*nearPlane / (right-left);
   row.y = 0.0;
   row.z = 0.0;
   row.w = 0.0;
   projection.setRow( 0, row );

   row.x = 0.0;
   row.y = 2.0 * nearPlane / (top-bottom);
   row.z = 0.0;
   row.w = 0.0;
   projection.setRow( 1, row );

   row.x = (left+right) / (right-left);
   row.y = (top+bottom) / (top-bottom);
   row.z = farPlane / (nearPlane-farPlane);
   row.w = -1.0;
   projection.setRow( 2, row );

   row.x = 0.0;
   row.y = 0.0;
   row.z = nearPlane * farPlane / (nearPlane-farPlane);
   row.w = 0.0;
   projection.setRow( 3, row );

   projection.transpose();

   if (bRotate)
   {
      static MatrixF rotMat(EulerF( (M_PI_F / 2.0f), 0.0f, 0.0f));
      projection.mul( rotMat );
   }
   
   setProjectionMatrix( projection );
}


//-----------------------------------------------------------------------------
// Get projection frustum
//-----------------------------------------------------------------------------
void GFXDevice::getFrustum(F32 *left, F32 *right, F32 *bottom, F32 *top, F32 *nearPlane, F32 *farPlane )
{
   *left       = mFrustLeft;
   *right      = mFrustRight;
   *bottom     = mFrustBottom;
   *top        = mFrustTop;
   *nearPlane  = mFrustNear;
   *farPlane   = mFrustFar;
}

//-----------------------------------------------------------------------------
// Set frustum using FOV (Field of view) in degrees along the horizontal axis
//-----------------------------------------------------------------------------
void GFXDevice::setFrustum( F32 FOVx, F32 aspectRatio, F32 nearPlane, F32 farPlane )
{
   // Figure our planes and pass it up.

   //b = a tan D
   F32 left    = -nearPlane * mTan( mDegToRad(FOVx) / 2.0 );
   F32 right   = -left;
   F32 top     = left / aspectRatio;
   F32 bottom  = -top;

   setFrustum(left, right, top, bottom, nearPlane, farPlane);

   return;
}

//-----------------------------------------------------------------------------
// Set projection matrix to ortho transform
//-----------------------------------------------------------------------------
void GFXDevice::setOrtho(F32 left, 
                         F32 right, 
                         F32 bottom, 
                         F32 top, 
                         F32 nearPlane, 
                         F32 farPlane,
                         bool doRotate)
{
   // store values
   mFrustLeft = left;
   mFrustRight = right;
   mFrustBottom = bottom;
   mFrustTop = top;
   mFrustNear = nearPlane;
   mFrustFar = farPlane;
   mFrustOrtho = true;

   // compute matrix
   MatrixF projection;

   Point4F row;

   row.x = 2.0f / (right - left);
   row.y = 0.0;
   row.z = 0.0;
   row.w = 0.0;
   projection.setRow( 0, row );

   row.x = 0.0;
   row.y = 2.0f / (top - bottom);
   row.z = 0.0;
   row.w = 0.0;
   projection.setRow( 1, row );

   row.x = 0.0;
   row.y = 0.0;
   // This may need be modified to work with OpenGL (d3d has 0..1 projection for z, vs -1..1 in OpenGL)
   row.z = 1.0f / (nearPlane - farPlane); 
   row.w = 0.0;
   projection.setRow( 2, row );

   row.x = (left + right) / (left - right);
   row.y = (top + bottom) / (bottom - top);
   row.z = nearPlane / (nearPlane - farPlane);
   row.w = 1;
   projection.setRow( 3, row );

   projection.transpose();

   static MatrixF sRotMat(EulerF( (M_PI_F / 2.0f), 0.0f, 0.0f));

   if( doRotate )
      projection.mul( sRotMat );
   
   
   setProjectionMatrix( projection );
}


//-----------------------------------------------------------------------------
// Project radius
//-----------------------------------------------------------------------------
F32 GFXDevice::projectRadius( F32 dist, F32 radius )
{
   RectI viewPort = getViewport();
   F32 worldToScreenScale;
   if (!mFrustOrtho)
      worldToScreenScale = (mFrustNear * viewPort.extent.x) / (mFrustRight - mFrustLeft);
   else
      worldToScreenScale = viewPort.extent.x / (mFrustRight - mFrustLeft);
   return (radius / dist) * worldToScreenScale;
}

//-----------------------------------------------------------------------------
// Set base state
//-----------------------------------------------------------------------------
void GFXDevice::setBaseRenderState()
{
#ifdef STATEBLOCK
	AssertFatal(mBaseRenderSB, "GFXDevice::setBaseRenderState -- mBaseRenderSB cannot be NULL.");
	mBaseRenderSB->apply();
#else
   setAlphaBlendEnable( false );
   setCullMode( GFXCullNone );
   setLightingEnable( false );
   setZWriteEnable( true );
   setTextureStageColorOp( 0, GFXTOPDisable );  
#endif
   disableShaders();
}

void GFXDevice::setBaseDrawState()
{
#ifdef STATEBLOCK
	AssertFatal(mBaseDrawSB, "GFXDevice::setBaseDrawState -- mBaseDrawSB cannot be NULL.");
	mBaseDrawSB->apply();
#else
	setZEnable( true );
	setZWriteEnable( false );
	setAlphaBlendEnable( false );
	setCullMode( GFXCullNone );
	setLightingEnable( false );
	setTextureStageColorOp( 0, GFXTOPDisable );  
#endif
	disableShaders();
}

//-----------------------------------------------------------------------------
// Set Light
//-----------------------------------------------------------------------------
void GFXDevice::setLight(U32 stage, GFXLightInfo* light)
{
   AssertFatal(stage < LIGHT_STAGE_COUNT, "GFXDevice::setLight - out of range stage!");

   if(!mLightDirty[stage])
   {
      mStateDirty = true;
      mLightsDirty = true;
      mLightDirty[stage] = true;
   }
   mCurrentLightEnable[stage] = (light != NULL);
   if(mCurrentLightEnable[stage])
      mCurrentLight[stage] = *light;
}

//-----------------------------------------------------------------------------
// Set Light Material
//-----------------------------------------------------------------------------
void GFXDevice::setLightMaterial(GFXLightMaterial mat)
{
   mCurrentLightMaterial = mat;
   mLightMaterialDirty = true;
   mStateDirty = true;
}

void GFXDevice::setGlobalAmbientColor(ColorF color)
{
   if(mGlobalAmbientColor != color)
   {
      mGlobalAmbientColor = color;
      mGlobalAmbientColorDirty = true;
   }
}

//-----------------------------------------------------------------------------
// Set texture
//-----------------------------------------------------------------------------
void GFXDevice::setTexture( U32 stage, GFXTextureObject *texture )
{
   AssertFatal(stage<TEXTURE_STAGE_COUNT, "GFXDevice::setTexture - out of range stage!");
   
   if( mCurrentTexture[stage].getPointer() == texture )
   {
      mTextureDirty[stage] = false;
      return;
   }

   if( !mTextureDirty[stage] )
   {
      mStateDirty = true;
      mTexturesDirty = true;
      mTextureDirty[stage] = true;
   }
   mNewTexture[stage] = texture;
   mTexType[stage] = GFXTDT_Normal;

   // Clear out the cubemaps
   mNewCubemap[stage] = NULL;
   mCurrentCubemap[stage] = NULL;
}

//-----------------------------------------------------------------------------
// Set cube texture
//-----------------------------------------------------------------------------
void GFXDevice::setCubeTexture( U32 stage, GFXCubemap *texture )
{
   AssertFatal(stage < getNumSamplers(), "GFXDevice::setTexture - out of range stage!");

   if( mCurrentCubemap[stage].getPointer() == texture )
   {
      mTextureDirty[stage] = false;
      return;
   }

   if( !mTextureDirty[stage] )
   {
      mStateDirty = true;
      mTexturesDirty = true;
      mTextureDirty[stage] = true;
   }
   mNewCubemap[stage] = texture;
   mTexType[stage] = GFXTDT_Cube;

   // Clear out the normal textures
   mNewTexture[stage] = NULL;
   mCurrentTexture[stage] = NULL;
}

//-----------------------------------------------------------------------------
// Register texture callback
//-----------------------------------------------------------------------------
void GFXDevice::registerTexCallback( GFXTexEventCallback callback, 
                                     void *userData, 
                                     S32 &handle )
{
   mTextureManager->registerTexCallback( callback, userData, handle );
}

//-----------------------------------------------------------------------------
// unregister texture callback
//-----------------------------------------------------------------------------
void GFXDevice::unregisterTexCallback( S32 handle )
{
   mTextureManager->unregisterTexCallback( handle );
}
//------------------------------------------------------------------------------

void GFXDevice::setInitialGFXState()
{
   // Implementation plan:
   // Create macro which respects debug state #define, sets the proper render state
   // and creates a no-break watch on that state so that a report may be run at
   // end of frame. Repeat process for sampler/texture stage states.

   // Future work:
   // Some simple "red flag" logic should be pretty trivial to implement here that
   // could warn about a file-name setting a state and not restoring it's value

   // CodeReview: Should there be some way to reset value of a state back to its
   // 'initial' value? Not like a dglSetCanonicalState, but more like a:
   //    GFX->setSomeRenderState(); // No args means default value
   //
   // When we switch back to state blocks, does a canonical state actually make
   // *more* sense now?
   // [6/5/2007 Pat]
   //
   // Another approach is to be able to "mark" the beginning of a new render setup,
   // and from that point until a draw primitive track anything which was dirty
   // before the mark  and not touched after the mark.  That way we can rely on a 
   // clean rendering slate without slamming all the states.  Something like this: 
   //    Default value of state A=a.  Assume previous render left A=a'.
   //    If after calling markInitState() we never touch state A, it will get set to value a on first draw.
   //    If we touch it, it will not get set back to default.
   //    If, OTOH, the previous draw had left it in default state, it would not get reset.
   // So algo would be that when markInitState() is called all the dirty states would get
   // added to a linked list.  As states are touched they would be removed from this list.  When draw
   // call is made, all remaining states would be slammed.  This works out better than the dglCannonical
   // approach because we don't end up setting A=a' then A=a (on dglCannonical), then A=a' because
   // next render wants the same state.  Particularly important for things like shader constants which
   // our state cacher isn't cacheing.
   // Note: markInitState() not meant to be actual name of method, just for illustration purposes.
   // [6/30/2007 Clark]

   // This macro will help this function out, because we don't actually want to
   // use the GFX device methods to set the states. This is a forced-state set
   // that, while it will notify the state-cache, it occurs on the graphics device
   // immediately. If state debugging is enabled, then the macro will also set up
   // a watch on each state and provide state history at end of frame. This will
   // be very useful for figuring out what code is not cleaning up states.
#ifdef POWER_GFX_STATE_DEBUG
#  define SET_INITIAL_RENDER_STATE( state, val ) \
   initRenderState( state, val ); \
   setRenderState( state, val ); \
   mDebugStateManager->WatchState( GFXDebugState::GFXRenderStateEvent, state )
#else
#  define SET_INITIAL_RENDER_STATE( state, val ) \
   initRenderState( state, val ); \
   setRenderState( state, val )
#endif

   // Set up all render states with the macro
   SET_INITIAL_RENDER_STATE( GFXRSZEnable,          false );
   SET_INITIAL_RENDER_STATE( GFXRSZWriteEnable,     true );
   SET_INITIAL_RENDER_STATE( GFXRSLighting,         false );
   SET_INITIAL_RENDER_STATE( GFXRSAlphaBlendEnable, false );
   SET_INITIAL_RENDER_STATE( GFXRSAlphaTestEnable,  false );
   SET_INITIAL_RENDER_STATE( GFXRSZFunc,            GFXCmpAlways );

   SET_INITIAL_RENDER_STATE( GFXRSSrcBlend,         GFXBlendOne );
   SET_INITIAL_RENDER_STATE( GFXRSDestBlend,        GFXBlendZero );

   SET_INITIAL_RENDER_STATE( GFXRSAlphaFunc,        GFXCmpAlways );
   SET_INITIAL_RENDER_STATE( GFXRSAlphaRef,         0 );

   SET_INITIAL_RENDER_STATE( GFXRSStencilFail,      GFXStencilOpKeep );
   SET_INITIAL_RENDER_STATE( GFXRSStencilZFail,     GFXStencilOpKeep );
   SET_INITIAL_RENDER_STATE( GFXRSStencilPass,      GFXStencilOpKeep );

   SET_INITIAL_RENDER_STATE( GFXRSStencilFunc,      GFXCmpAlways );
   SET_INITIAL_RENDER_STATE( GFXRSStencilRef,       0 );
   SET_INITIAL_RENDER_STATE( GFXRSStencilMask,      0xFFFFFFFF );

   SET_INITIAL_RENDER_STATE( GFXRSStencilWriteMask, 0xFFFFFFFF );

   SET_INITIAL_RENDER_STATE( GFXRSStencilEnable,    false );

   SET_INITIAL_RENDER_STATE( GFXRSCullMode,         GFXCullCCW );

   SET_INITIAL_RENDER_STATE( GFXRSColorVertex,      true );

   SET_INITIAL_RENDER_STATE( GFXRSDepthBias,        0 );

   // Add more here using the macro
   
#undef SET_INITIAL_RENDER_STATE

   // Do the same thing for Texture-Stage states. In this case, all color-op's
   // will get set to disable. This is actually the proper method for disabling
   // texturing on a texture-stage. Setting the texture to NULL, is not.
#ifdef POWER_GFX_STATE_DEBUG
#  define SET_INITIAL_TS_STATE( state, val, stage ) \
   initTextureState( stage, state, val ); \
   setTextureStageState( stage, state, val ); \
   mDebugStateManager->WatchState( GFXDebugState::GFXTextureStageStateEvent, state, stage )
#else
#  define SET_INITIAL_TS_STATE( state, val, stage ) \
   initTextureState( stage, state, val ); \
   setTextureStageState( stage, state, val )
#endif

   // Init these states across all texture stages
   for( int i = 0; i < getNumSamplers(); i++ )
   {
      SET_INITIAL_TS_STATE( GFXTSSColorOp, GFXTOPDisable, i );

      // Add more here using the macro
   }
#undef SET_INITIAL_TS_STATE

#ifdef POWER_GFX_STATE_DEBUG
#  define SET_INITIAL_SAMPLER_STATE( state, val, stage ) \
   initSamplerState( stage, state, val ); \
   setSamplerState( stage, state, val ); \
   mDebugStateManager->WatchState( GFXDebugState::GFXSamplerStateEvent, state, stage )
#else
#  define SET_INITIAL_SAMPLER_STATE( state, val, stage ) \
   initSamplerState( stage, state, val ); \
   setSamplerState( stage, state, val )
#endif

   // Init these states across all texture stages
   for( int i = 0; i < getNumSamplers(); i++ )
   {
      // Set all samplers to clamp tex coordinates outside of their range
      SET_INITIAL_SAMPLER_STATE( GFXSAMPAddressU, GFXAddressClamp, i );
      SET_INITIAL_SAMPLER_STATE( GFXSAMPAddressV, GFXAddressClamp, i );
      SET_INITIAL_SAMPLER_STATE( GFXSAMPAddressW, GFXAddressClamp, i );

      // And point sampling for everyone
      SET_INITIAL_SAMPLER_STATE( GFXSAMPMagFilter, GFXTextureFilterPoint, i );
      SET_INITIAL_SAMPLER_STATE( GFXSAMPMinFilter, GFXTextureFilterPoint, i );
      SET_INITIAL_SAMPLER_STATE( GFXSAMPMipFilter, GFXTextureFilterPoint, i );

      // Add more here using the macro
   }
#undef SET_INITIAL_SAMPLER_STATE
}

//------------------------------------------------------------------------------

#ifdef POWER_GFX_STATE_DEBUG
void GFXDevice::processStateWatchHistory( const GFXDebugState::GFXDebugStateWatch *watch )
{
   // This method will get called once per no-break watch, at end of frame, for 
   // every frame that the watch is active. The accessors on the state watch
   // should provide all of the information needed to look at the history of
   // the state that was being tracked.
}
#endif

//------------------------------------------------------------------------------

// These two methods I modified because I may want to catch these events for state
// debugging. [6/7/2007 Pat]
inline void GFXDevice::beginScene()
{
   beginSceneInternal();
#ifdef DEBUG
   mDPTimes = 0;
#endif
}

//------------------------------------------------------------------------------

inline void GFXDevice::endScene()
{
   endSceneInternal();
}
 
//------------------------------------------------------------------------------

void GFXDevice::_updateRenderTargets()
{
   // Re-set the RT if needed.
   GFXTarget *targ = getActiveRenderTarget();

   if( !targ )
      return;

   if( targ->isPendingState() )
      setActiveRenderTarget( targ );
}

void GFXDevice::pushActiveRenderTarget()
{
   // Duplicate last item on the stack.
   mRTStack.push_back(mCurrentRT);
}

void GFXDevice::popActiveRenderTarget()
{
   // Pop the last item on the stack, set next item down.
   AssertFatal(mRTStack.size() > 0, "GFXD3D9Device::popActiveRenderTarget - stack is empty!");
   setActiveRenderTarget(mRTStack.last());
   mRTStack.pop_back();
}

inline GFXTarget *GFXDevice::getActiveRenderTarget()
{
   return mCurrentRT;
}

#ifndef POWER_SHIPPING
void GFXDevice::_dumpStatesToFile( const char *fileName ) const
{
#ifdef POWER_GFX_STATE_DEBUG
   static int stateDumpNum = 0;
   static char nameBuffer[256];
   
   dSprintf( nameBuffer, sizeof(nameBuffer), "demo/%s_%d.gfxstate", fileName, stateDumpNum );
   

   FileStream stream;

   if( ResourceManager->openFileForWrite( stream, nameBuffer ) ) 
   {
      // Report
      Con::printf( "--Dumping GFX States to file: %s", nameBuffer );

      // Increment state dump #
      stateDumpNum++;

      // Dump render states
      stream.writeLine( (U8 *)"Render States" );
      for( U32 state = GFXRenderState_FIRST; state < GFXRenderState_COUNT; state++ )
         stream.writeLine( (U8 *)avar( "%s - %s", GFXStringRenderState[state], GFXStringRenderStateValueLookup[state]( mStateTracker[state].newValue ) ) );

      // Dump texture stage states
      for( U32 stage = 0; stage < getNumSamplers(); stage++ )
      {
         stream.writeLine( (U8 *)avar( "Texture Stage: %d", stage ) );
         for( U32 state = GFXTSS_FIRST; state < GFXTSS_COUNT; state++ ) 
            stream.writeLine( (U8 *)avar( "::%s - %s", GFXStringTextureStageState[state], GFXStringTextureStageStateValueLookup[state]( mTextureStateTracker[stage][state].newValue ) ) );
      }

      // Dump sampler states
      for( U32 stage = 0; stage < getNumSamplers(); stage++ )
      {
         stream.writeLine( (U8 *)avar( "Sampler Stage: %d\n", stage ) );
         for( U32 state = GFXSAMP_FIRST; state < GFXSAMP_COUNT; state++ ) 
            stream.writeLine( (U8 *)avar( "::%s - %s", GFXStringSamplerState[state], GFXStringSamplerStateValueLookup[state]( mSamplerStateTracker[stage][state].newValue ) ) );
      }
   }
#endif
}
#endif

void GFXDevice::listResources(bool unflaggedOnly)
{
   U32 numTextures = 0, numShaders = 0, numRenderToTextureTargs = 0, numWindowTargs = 0;
   U32 numCubemaps = 0, numVertexBuffers = 0, numPrimitiveBuffers = 0, numFences = 0;
   GFXResource* walk = mResourceListHead;
   while(walk)
   {
      if(unflaggedOnly && walk->isFlagged())
      {
         walk = walk->getNextResource();
         continue;
      }

      if(dynamic_cast<GFXTextureObject*>(walk))
         numTextures++;
      else if(dynamic_cast<GFXShader*>(walk))
         numShaders++;
      else if(dynamic_cast<GFXTextureTarget*>(walk))
         numRenderToTextureTargs++;
      else if(dynamic_cast<GFXWindowTarget*>(walk))
         numWindowTargs++;
      else if(dynamic_cast<GFXCubemap*>(walk))
         numCubemaps++;
      else if(dynamic_cast<GFXVertexBuffer*>(walk))
         numVertexBuffers++;
      else if(dynamic_cast<GFXPrimitiveBuffer*>(walk))
         numPrimitiveBuffers++;
      else if(dynamic_cast<GFXFence*>(walk))
         numFences++;
      else
         Con::warnf("Unknown resource: %x", walk);

      walk = walk->getNextResource();
   }
   const char* flag = unflaggedOnly ? "unflagged" : "allocated";

   Con::printf("GFX currently has:");
   Con::printf("   %i %s textures", numTextures, flag);
   Con::printf("   %i %s shaders", numShaders, flag);
   Con::printf("   %i %s texture targets", numRenderToTextureTargs, flag);
   Con::printf("   %i %s window targets", numWindowTargs, flag);
   Con::printf("   %i %s cubemaps", numCubemaps, flag);
   Con::printf("   %i %s vertex buffers", numVertexBuffers, flag);
   Con::printf("   %i %s primitive buffers", numPrimitiveBuffers, flag);
   Con::printf("   %i %s fences", numFences, flag);
}

void GFXDevice::fillResourceVectors(const char* resNames, bool unflaggedOnly, Vector<GFXTextureObject*> &textureObjects,
                                    Vector<GFXTextureTarget*> &textureTargets, Vector<GFXWindowTarget*> &windowTargets,
                                    Vector<GFXVertexBuffer*> &vertexBuffers, Vector<GFXPrimitiveBuffer*> &primitiveBuffers,
                                    Vector<GFXFence*> &fences, Vector<GFXCubemap*> &cubemaps, Vector<GFXShader*> &shaders)
{
   bool describeTexture = true, describeTextureTarget = true, describeWindowTarget = true, describeVertexBuffer = true, 
      describePrimitiveBuffer = true, describeFence = true, describeCubemap = true, describeShader = true;

   // If we didn't specify a string of names, we'll print all of them
   if(resNames && resNames[0] != '\0')
   {
      // If we did specify a string of names, determine which names
      describeTexture =          (dStrstr(resNames, "GFXTextureObject")    != NULL);
      describeTextureTarget =    (dStrstr(resNames, "GFXTextureTarget")    != NULL);
      describeWindowTarget =     (dStrstr(resNames, "GFXWindowTarget")     != NULL);
      describeVertexBuffer =     (dStrstr(resNames, "GFXVertexBuffer")     != NULL);
      describePrimitiveBuffer =  (dStrstr(resNames, "GFXPrimitiveBuffer")   != NULL);
      describeFence =            (dStrstr(resNames, "GFXFence")            != NULL);
      describeCubemap =          (dStrstr(resNames, "GFXCubemap")          != NULL);
      describeShader =           (dStrstr(resNames, "GFXShader")           != NULL);
   }

   // Start going through the list
   GFXResource* walk = mResourceListHead;
   while(walk)
   {
      // If we only want unflagged resources, skip all flagged resources
      if(unflaggedOnly && walk->isFlagged())
      {
         walk = walk->getNextResource();
         continue;
      }

      // All of the following checks go through the same logic.
      // if(describingThisResource) 
      // {
      //    ResourceType* type = dynamic_cast<ResourceType*>(walk)
      //    if(type)
      //    {
      //       typeVector.push_back(type);
      //       walk = walk->getNextResource();
      //       continue;
      //    }
      // }

      if(describeTexture)
      {
         GFXTextureObject* tex = dynamic_cast<GFXTextureObject*>(walk);
         {
            if(tex)
            {
               textureObjects.push_back(tex);
               walk = walk->getNextResource();
               continue;
            }
         }
      }
      if(describeShader)
      {
         GFXShader* shd = dynamic_cast<GFXShader*>(walk);
         if(shd)
         {
            shaders.push_back(shd);
            walk = walk->getNextResource();
            continue;
         }
      }
      if(describeVertexBuffer)
      {
         GFXVertexBuffer* buf = dynamic_cast<GFXVertexBuffer*>(walk);
         if(buf)
         {
            vertexBuffers.push_back(buf);
            walk = walk->getNextResource();
            continue;
         }
      }
      if(describePrimitiveBuffer)
      {
         GFXPrimitiveBuffer* buf = dynamic_cast<GFXPrimitiveBuffer*>(walk);
         if(buf)
         {
            primitiveBuffers.push_back(buf);
            walk = walk->getNextResource();
            continue;
         }
      }
      if(describeTextureTarget)
      {
         GFXTextureTarget* targ = dynamic_cast<GFXTextureTarget*>(walk);
         if(targ)
         {
            textureTargets.push_back(targ);
            walk = walk->getNextResource();
            continue;
         }
      }
      if(describeWindowTarget)
      {
         GFXWindowTarget* targ = dynamic_cast<GFXWindowTarget*>(walk);
         if(targ)
         {
            windowTargets.push_back(targ);
            walk = walk->getNextResource();
            continue;
         }
      }
      if(describeCubemap)
      {
         GFXCubemap* cube = dynamic_cast<GFXCubemap*>(walk);
         if(cube)
         {
            cubemaps.push_back(cube);
            walk = walk->getNextResource();
            continue;
         }
      }
      if(describeFence)
      {
         GFXFence* fence = dynamic_cast<GFXFence*>(walk);
         if(fence)
         {
            fences.push_back(fence);
            walk = walk->getNextResource();
            continue;
         }
      }
      // Wasn't something we were looking for
      walk = walk->getNextResource();
   }
}

/// Helper class for GFXDevice::describeResources.
class DescriptionOutputter
{
   /// Are we writing to a file?
   bool mWriteToFile;

   /// File if we are writing to a file
   FileStream mFile;
public:
   DescriptionOutputter(const char* file)
   {
      mWriteToFile = false;
      // If we've been given what could be a valid file path, open it.
      if(file && file[0] != '\0')
      {
         mWriteToFile = mFile.open(file, FileStream::Write);

         // Note that it is safe to retry.  If this is hit, we'll just write to the console instead of to the file.
         AssertFatal(mWriteToFile, avar("DescriptionOutputter::DescriptionOutputter - could not open file %s", file));
      }
   }

   ~DescriptionOutputter()
   {
      // Close the file
      if(mWriteToFile)
         mFile.close();
   }

   /// Writes line to the file or to the console, depending on what we want.
   void write(const char* line)
   {
      if(mWriteToFile)
         mFile.writeLine((U8*)line);
      else
         Con::printf(line);
   }
};

void GFXDevice::describeResources(const char* resNames, const char* filePath, bool unflaggedOnly)
{
   // Vectors of objects.  By having these we can go through the list once.
   Vector<GFXTextureObject*> textureObjects(__FILE__, __LINE__);
   Vector<GFXTextureTarget*> textureTargets(__FILE__, __LINE__);
   Vector<GFXWindowTarget*> windowTargets(__FILE__, __LINE__);
   Vector<GFXVertexBuffer*> vertexBuffers(__FILE__, __LINE__);
   Vector<GFXPrimitiveBuffer*> primitiveBuffers(__FILE__, __LINE__);
   Vector<GFXFence*> fences(__FILE__, __LINE__);
   Vector<GFXCubemap*> cubemaps(__FILE__, __LINE__);
   Vector<GFXShader*> shaders(__FILE__, __LINE__);

   // Fill the vectors with the right resources
   fillResourceVectors(resNames, unflaggedOnly, textureObjects, textureTargets, windowTargets, vertexBuffers, primitiveBuffers,
      fences, cubemaps, shaders);

   // Helper object
   DescriptionOutputter output(filePath);

   // Print the info to the file
   // Note that we check if we have any objects of that type.
   char descriptionBuffer[996];
   char writeBuffer[1024];
   if(textureObjects.size())
   {
      output.write("--------Dumping GFX texture descriptions...----------");
      for(U32 i = 0; i < textureObjects.size(); i++)
      {
         textureObjects[i]->describeSelf(descriptionBuffer, sizeof(descriptionBuffer));
         dSprintf(writeBuffer, sizeof(writeBuffer), "Addr: %x %s", textureObjects[i], descriptionBuffer);
         output.write(writeBuffer);
      }
      output.write("--------------------Done---------------------");
      output.write("");
   }

   if(textureTargets.size())
   {
      output.write("--------Dumping GFX texture target descriptions...----------");
      for(U32 i = 0; i < textureTargets.size(); i++)
      {
         textureTargets[i]->describeSelf(descriptionBuffer, sizeof(descriptionBuffer));
         dSprintf(writeBuffer, sizeof(writeBuffer), "Addr: %x %s", textureTargets[i], descriptionBuffer);
         output.write(writeBuffer);
      }
      output.write("--------------------Done---------------------");
      output.write("");
   }

   if(windowTargets.size())
   {
      output.write("--------Dumping GFX window target descriptions...----------");
      for(U32 i = 0; i < windowTargets.size(); i++)
      {
         windowTargets[i]->describeSelf(descriptionBuffer, sizeof(descriptionBuffer));
         dSprintf(writeBuffer, sizeof(writeBuffer), "Addr: %x %s", windowTargets[i], descriptionBuffer);
         output.write(writeBuffer);
      }
      output.write("--------------------Done---------------------");
      output.write("");
   }

   if(vertexBuffers.size())
   {
      output.write("--------Dumping GFX vertex buffer descriptions...----------");
      for(U32 i = 0; i < vertexBuffers.size(); i++)
      {
         vertexBuffers[i]->describeSelf(descriptionBuffer, sizeof(descriptionBuffer));
         dSprintf(writeBuffer, sizeof(writeBuffer), "Addr: %x %s", vertexBuffers[i], descriptionBuffer);
         output.write(writeBuffer);
      }
      output.write("--------------------Done---------------------");
      output.write("");
   }

   if(primitiveBuffers.size())
   {
      output.write("--------Dumping GFX primitive buffer descriptions...----------");
      for(U32 i = 0; i < primitiveBuffers.size(); i++)
      {
         primitiveBuffers[i]->describeSelf(descriptionBuffer, sizeof(descriptionBuffer));
         dSprintf(writeBuffer, sizeof(writeBuffer), "Addr: %x %s", primitiveBuffers[i], descriptionBuffer);
         output.write(writeBuffer);
      }
      output.write("--------------------Done---------------------");
      output.write("");
   }

   if(fences.size())
   {
      output.write("--------Dumping GFX fence descriptions...----------");
      for(U32 i = 0; i < fences.size(); i++)
      {
         fences[i]->describeSelf(descriptionBuffer, sizeof(descriptionBuffer));
         dSprintf(writeBuffer, sizeof(writeBuffer), "Addr: %x %s", fences[i], descriptionBuffer);
         output.write(writeBuffer);
      }
      output.write("--------------------Done---------------------");
      output.write("");
   }

   if(cubemaps.size())
   {
      output.write("--------Dumping GFX cubemap descriptions...----------");
      for(U32 i = 0; i < cubemaps.size(); i++)
      {
         cubemaps[i]->describeSelf(descriptionBuffer, sizeof(descriptionBuffer));
         dSprintf(writeBuffer, sizeof(writeBuffer), "Addr: %x %s", cubemaps[i], descriptionBuffer);
         output.write(writeBuffer);
      }
      output.write("--------------------Done---------------------");
      output.write("");
   }

   if(shaders.size())
   {
      output.write("--------Dumping GFX shader descriptions...----------");
      for(U32 i = 0; i < shaders.size(); i++)
      {
         shaders[i]->describeSelf(descriptionBuffer, sizeof(descriptionBuffer));
         dSprintf(writeBuffer, sizeof(writeBuffer), "Addr: %x %s", shaders[i], descriptionBuffer);
         output.write(writeBuffer);
      }
      output.write("--------------------Done---------------------");
      output.write("");
   }
}


void GFXDevice::flagCurrentResources()
{
   GFXResource* walk = mResourceListHead;
   while(walk)
   {
      walk->setFlag();
      walk = walk->getNextResource();
   }
}

void GFXDevice::clearResourceFlags()
{
   GFXResource* walk = mResourceListHead;
   while(walk)
   {
      walk->clearFlag();
      walk = walk->getNextResource();
   }
}

ConsoleFunction(listGFXResources, void, 1, 2, "(bool unflaggedOnly = false)")
{
   bool unflaggedOnly = false;
   if(argc == 2)
      unflaggedOnly = dAtob(argv[1]);
   GFX->listResources(unflaggedOnly);
}

ConsoleFunction(flagCurrentGFXResources, void, 1, 1, "")
{
   GFX->flagCurrentResources();
}

ConsoleFunction(clearGFXResourceFlags, void, 1, 1, "")
{
   GFX->clearResourceFlags();
}

ConsoleFunction(describeGFXResources, void, 3, 4, "(string resourceNames, string filePath, bool unflaggedOnly = false)\n"
                                                  " If resourceNames is "", this function describes all resources.\n"
                                                  " If filePath is "", this function writes the resource descriptions to the console")
{
   bool unflaggedOnly = false;
   if(argc == 4)
      unflaggedOnly = dAtob(argv[3]);
   GFX->describeResources(argv[1], argv[2], unflaggedOnly);
}

//-----------------------------------------------------------------------------
// Get pixel shader version - for script
//-----------------------------------------------------------------------------
ConsoleFunction( getPixelShaderVersion, F32, 1, 1, "Get pixel shader version.\n\n" )
{
   return GFX->getPixelShaderVersion();
}   

//-----------------------------------------------------------------------------
// Set pixel shader version - for script
//-----------------------------------------------------------------------------
ConsoleFunction( setPixelShaderVersion, void, 2, 2, "Set pixel shader version.\n\n" )
{
   GFX->setPixelShaderVersion( dAtof(argv[1]) );
}
//-----------------------------------------------------------------------------
// Set Fill Mode - for script
//-----------------------------------------------------------------------------
ConsoleFunction( setFillMode, void, 2,2,"Set render fill mode\n\n")
{
#ifdef STATEBLOCK
	GFX->setRenderState(GFXRSFillMode, (GFXFillMode)dAtoi(argv[1]));
#else
	GFX->setFillMode((GFXFillMode)dAtoi(argv[1]));
#endif
}
void GFXDevice::resetStateBlock()
{
	//mBaseRenderSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaBlendEnable, false);
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);
	GFX->setRenderState(GFXRSLighting, false);
	GFX->setRenderState(GFXRSZWriteEnable, true);
	GFX->setTextureStageState( 0, GFXTSSColorOp, GFXTOPDisable);
	GFX->endStateBlock(mBaseRenderSB);

	//mBaseDrawSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaBlendEnable, false);
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);
	GFX->setRenderState(GFXRSLighting, false);
	GFX->setRenderState(GFXRSZEnable, true);
	GFX->setRenderState(GFXRSZWriteEnable, false);
	GFX->setTextureStageState( 0, GFXTSSColorOp, GFXTOPDisable);
	GFX->endStateBlock(mBaseDrawSB);
}
void GFXDevice::releaseStateBlock()
{
	if (mBaseRenderSB)
	{
		mBaseRenderSB->release();
	}

	if (mBaseDrawSB)
	{
		mBaseDrawSB->release();
	}
}
void GFXDevice::initsb()
{
	if (mBaseRenderSB == NULL)
	{
		mBaseRenderSB = new GFXD3D9StateBlock;
		mBaseRenderSB->registerResourceWithDevice(GFX);
		mBaseRenderSB->mZombify = &releaseStateBlock;
		mBaseRenderSB->mResurrect = &resetStateBlock;

		mBaseDrawSB = new GFXD3D9StateBlock;

		resetStateBlock();
	}
}
void GFXDevice::shutdown()
{
	SAFE_DELETE(mBaseRenderSB);
	SAFE_DELETE(mBaseDrawSB);
}