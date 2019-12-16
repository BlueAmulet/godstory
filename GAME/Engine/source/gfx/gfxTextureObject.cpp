//-----------------------------------------------------------------------------
// PowerEngine
// 
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "gfx/gfxTextureObject.h"
#include "gfx/gfxDevice.h"
#include "platform/profiler.h"

#ifdef POWER_DEBUG
GFXTextureObject *GFXTextureObject::smHead = NULL;
U32 GFXTextureObject::smActiveTOCount = 0;

void GFXTextureObject::dumpActiveTOs()
{
   if(!smActiveTOCount)
   {
      Con::printf("GFXTextureObject::dumpActiveTOs - no active TOs to dump. You are A-OK!");
      return;
   }

   Con::printf("GFXTextureObject Usage Report - %d active TOs", smActiveTOCount);
   Con::printf("---------------------------------------------------------------");
   Con::printf(" Address    Dimensions    RefCount    GFXTextureProfile                     Profiler Path                                   Texture Name");

   for(GFXTextureObject *walk = smHead; walk; walk=walk->mDebugNext)
   {
      char profilePad[36];
      char profilerPathPad[47];

      S32 profilePadLen = 36 - dStrlen(walk->mProfile->getName());
      if (profilePadLen < 0)
         profilePadLen = 0;

#if defined(POWER_ENABLE_PROFILER)
      S32 profilerPathPadLen = 46 - dStrlen(walk->mDebugCreationPath);
      if (profilerPathPadLen < 0)
         profilerPathPadLen = 0;
#else
      S32 profilerPathPadLen = 46;
#endif

      for (U32 i = 0; i < profilePadLen; i++)
         profilePad[i] = ' ';
      profilePad[profilePadLen] = 0;

      for (U32 i = 0; i < profilerPathPadLen; i++)
         profilerPathPad[i] = ' ';
      profilerPathPad[profilerPathPadLen] = 0;

      Con::printf(" 0x%p  (%4d, %4d)  %4d  %s%s  %s%s  %s", walk, walk->getWidth(), 
#if defined(POWER_ENABLE_PROFILER)   
                  walk->getHeight(), walk->mRefCount,walk->mProfile->getName(), profilePad, walk->mDebugCreationPath, profilerPathPad, walk->mTextureFileName);
#else                  
                  walk->getHeight(), walk->mRefCount,walk->mProfile->getName(), profilePad, "", profilerPathPad, walk->mTextureFileName);
#endif
   }

   Con::printf("----- dump complete -------------------------------------------");
   AssertFatal(false, "There is a texture object leak, check the log for more details.");
}

#endif

//-----------------------------------------------------------------------------
// GFXTextureObject
//-----------------------------------------------------------------------------
GFXTextureObject::GFXTextureObject(GFXDevice *aDevice, GFXTextureProfile *aProfile) 
{
   mHashNext = mNext = mPrev = NULL;

   mDevice = aDevice;
   mProfile = aProfile;

   mTextureFileName = NULL;
   mProfile        = NULL;
   mBitmap         = NULL;
   mMipLevels      = 1;

   mTextureSize.set( 0, 0, 0 );

   mDead = false;
   mDirty = false;

   cacheId = 0;
   cacheTime = 0;

   hasTransparency = false;

   mBitmap = NULL;
   mDDS    = NULL;
   
   mFormat = GFXFormatR8G8B8;

#if defined(POWER_DEBUG)
   // Active object tracking.
   smActiveTOCount++;
#if defined(POWER_ENABLE_PROFILER)   
   mDebugCreationPath = gProfiler->getProfilePath();
#endif
   mDebugNext = smHead;
   mDebugPrev = NULL;

   if(smHead)
   {
      AssertFatal(smHead->mDebugPrev == NULL, "GFXTextureObject::GFXTextureObject - found unexpected previous in current head!");
      smHead->mDebugPrev = this;
   }

   smHead = this;
#endif
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
GFXTextureObject::~GFXTextureObject() 
{
   kill();

#ifdef POWER_DEBUG
   if(smHead == this)
      smHead = this->mDebugNext;

   if(mDebugNext)
      mDebugNext->mDebugPrev = mDebugPrev;

   if(mDebugPrev)
      mDebugPrev->mDebugNext = mDebugNext;

   mDebugPrev = mDebugNext = NULL;

   smActiveTOCount--;
#endif
}

//-----------------------------------------------------------------------------
// kill - this function clears out the data in texture object.  It's done like
// this because the texture object needs to release its pointers to textures
// before the GFXDevice is shut down.  The texture objects themselves get
// deleted by the refcount structure - which may be after the GFXDevice has
// been destroyed.
//-----------------------------------------------------------------------------
void GFXTextureObject::kill()
{
   if( mDead ) return;

#ifdef POWER_DEBUG
   // This makes sure that nobody is forgetting to call kill from the derived
   // destructor.  If they are, then we should get a pure virtual function
   // call here.
   pureVirtualCrash();
#endif

   // If we're a dummy, don't do anything...
   if( !mDevice || !mDevice->mTextureManager ) return;

   // Remove ourselves from the texture list and hash
   mDevice->mTextureManager->deleteTexture(this);

   // Delete bitmap(s)
   SAFE_DELETE(mBitmap)
   SAFE_DELETE(mDDS);

   // Clean up linked list
   if(mNext) mNext->mPrev = mPrev;
   if(mPrev) mPrev->mNext = mNext;

   mDead = true;
}

void GFXTextureObject::describeSelf( char* buffer, U32 sizeOfBuffer )
{
   dSprintf(buffer, sizeOfBuffer, " (width: %4d, height: %4d)  profile: %s   creation path: %s", getWidth(), 
#if defined(POWER_DEBUG) && defined(POWER_ENABLE_PROFILER)  
      getHeight(), mProfile->getName(), mDebugCreationPath);
#else                  
      getHeight(), mProfile->getName(), "");
#endif
}

void GFXTextureObject::savePng( const char* path )
{
	//U32 w = getWidth();
	//U32 h = getHeight();
	//GBitmap bm(w, h, 0, mFormat);

	//GFXLockedRect *rt = NULL;
	//U8 * bit = bm.getWritableBits();
	//rt = lock();
	//for (U32 i=0; i<h; i++)
	//{
	//	for (U32 j=0; j<rt->pitch; j++)
	//	{
	//		bit[i*rt->pitch + j] = rt->bits[i*rt->pitch + j];
	//	}
	//}
	//unlock();

	//FileStream fs;
	//if (ResourceManager->openFileForWrite(fs, path, FileStream::Write))
	//{
	//	bm.writePNG(fs);
	//	fs.close();
	//}
}