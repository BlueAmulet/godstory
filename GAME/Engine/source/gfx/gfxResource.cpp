//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "gfx/gfxResource.h"
#include "gfx/gfxDevice.h"

//#define ENABLE_MAINTHREAD_CHECK

#include "util/BackgroundLoadMgr.h"

GFXResource::GFXResource()
{
   mPrevResource = mNextResource = NULL;
   mOwningDevice = NULL;
   mFlagged = false;
}

GFXResource::~GFXResource()
{
	MAIN_THREAD_CHECK;

   // Make sure we aren't about to use a deleted device.
   AssertFatal(!mOwningDevice || mOwningDevice->getDeviceIndex() != U32(-1), "GFXDevice already deleted");

   if(mOwningDevice)
   {
	   CLockGuard gfxGuard(mOwningDevice->m_mutex);

	   // Make sure we're not the head of the list and referencd on the device.
	   if(mOwningDevice->mResourceListHead == this)
	   {
		   AssertFatal(mPrevResource == NULL, 
			   "GFXResource::~GFXResource - head of list but have a previous item!");
		   mOwningDevice->mResourceListHead = mNextResource;
	   }

	   // Unlink ourselves from the list.
	   if(mPrevResource)
		   mPrevResource->mNextResource = mNextResource;
	   if(mNextResource)
		   mNextResource->mPrevResource = mPrevResource;
   }
   else
   {
	   // Unlink ourselves from the list.
	   if(mPrevResource)
		   mPrevResource->mNextResource = mNextResource;
	   if(mNextResource)
		   mNextResource->mPrevResource = mPrevResource;
   }

   mPrevResource = mNextResource = NULL;
}

void GFXResource::registerResourceWithDevice( GFXDevice *device )
{
	MAIN_THREAD_CHECK;

	CLockGuard gfxGuard(device->m_mutex);

   mOwningDevice = device;
   mNextResource = device->mResourceListHead;
   device->mResourceListHead = this;

   if(mNextResource)
      mNextResource->mPrevResource = this;
}