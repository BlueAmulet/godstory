//-----------------------------------------------------------------------------
// PowerEngine
// 
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "gfx/gfxTextureProfile.h"
#include "gfx/gfxTextureObject.h"
#include "gfx/gBitmap.h"

// Set up defaults...
GFX_ImplementTextureProfile(GFXDefaultRenderTargetProfile, 
                            GFXTextureProfile::DiffuseMap, 
                            GFXTextureProfile::PreserveSize | GFXTextureProfile::NoMipmap | GFXTextureProfile::RenderTarget, 
                            GFXTextureProfile::None);
GFX_ImplementTextureProfile(GFXDefaultRenderTargetZBufferProfile, 
                            GFXTextureProfile::DiffuseMap, 
                            GFXTextureProfile::PreserveSize | GFXTextureProfile::NoMipmap | GFXTextureProfile::RenderTarget | GFXTextureProfile::RenderTargetZBuffer, 
                            GFXTextureProfile::None);
GFX_ImplementTextureProfile(GFXDefaultStaticDiffuseProfile, 
                            GFXTextureProfile::DiffuseMap, 
                            GFXTextureProfile::Static, 
                            GFXTextureProfile::None);
GFX_ImplementTextureProfile(GFXDefaultPersistentProfile,
                            GFXTextureProfile::DiffuseMap, 
                            GFXTextureProfile::PreserveSize | GFXTextureProfile::Static | GFXTextureProfile::KeepBitmap, 
                            GFXTextureProfile::None);
GFX_ImplementTextureProfile(GFXSystemMemProfile, 
                            GFXTextureProfile::DiffuseMap, 
                            GFXTextureProfile::PreserveSize | GFXTextureProfile::NoMipmap | GFXTextureProfile::SystemMemory,
                            GFXTextureProfile::None);
GFX_ImplementTextureProfile(GFXDefaultZTargetProfile,
                            GFXTextureProfile::DiffuseMap, 
                            GFXTextureProfile::PreserveSize | GFXTextureProfile::NoMipmap | GFXTextureProfile::ZTarget, 
                            GFXTextureProfile::None);
GFX_ImplementTextureProfile(GFXDefaultZTargetNoMSAAProfile,
                            GFXTextureProfile::DiffuseMap, 
                            GFXTextureProfile::PreserveSize | GFXTextureProfile::NoMipmap | GFXTextureProfile::ZTarget | GFXTextureProfile::NoMSAA, 
                            GFXTextureProfile::None);

GFX_ImplementTextureProfile(GFXDynamicDiffuseProfile, 
							GFXTextureProfile::DiffuseMap, 
							GFXTextureProfile::Dynamic, 
							GFXTextureProfile::None);

GFX_ImplementTextureProfile(GFXDefaultVertexMapProfile,
							GFXTextureProfile::VertexMap, 
							GFXTextureProfile::PreserveSize | GFXTextureProfile::Dynamic, 
							GFXTextureProfile::None);

GFX_ImplementTextureProfile(GFXDefaultDMapProfile,
							GFXTextureProfile::DMap, 
							GFXTextureProfile::PreserveSize | GFXTextureProfile::RenderTarget /*| GFXTextureProfile::RenderTargetZBuffer*/, 
							GFXTextureProfile::None);

GFX_ImplementTextureProfile(GFXDefaultDMapDebugProfile,
							GFXTextureProfile::DMap, 
							GFXTextureProfile::PreserveSize | GFXTextureProfile::Dynamic, 
							GFXTextureProfile::None);
//-----------------------------------------------------------------------------

GFXTextureProfile *GFXTextureProfile::mHead = NULL;

GFXTextureProfile::GFXTextureProfile(const char *name, Types type, U32 flag, Compression compression)
{
   mName = name;
   
   // Take type, flag, and compression and produce a munged profile word.
   mProfile = (type & (BIT(TypeBits + 1) - 1)) |
             ((flag & (BIT(FlagBits + 1) - 1)) << TypeBits) | 
             ((compression & (BIT(CompressionBits + 1) - 1)) << (FlagBits + TypeBits));   

   // Stick us on the linked list.
   mNext = mHead;
   mHead = this;

   // Now do some sanity checking. (Ben is not proud of this code.)
   AssertFatal( (testFlag(Dynamic) && !testFlag(Static)) 
                  || (!testFlag(Dynamic) && !testFlag(Static)) 
                  || (!testFlag(Dynamic) &&  testFlag(Static)), 
                  "GFXTextureProfile::GFXTextureProfile - Cannot have a texture profile be both static and dynamic!");
   mDownscale = 0;
   mActiveCount = 0;
   mActiveTexels = 0;
   mActiveBytes = 0;
   mAllocatedTextures = 0;
   mAllocatedTexels = 0;
   mAllocatedBytes= 0;
}

void GFXTextureProfile::init()
{
   // Do something, anything?
}

GFXTextureProfile * GFXTextureProfile::find(StringTableEntry name)
{
   // Not really necessary at this time.
   return NULL;
}

void GFXTextureProfile::updateStatsForCreation(GFXTextureObject *t)
{
   if(t->mProfile)
   {
      t->mProfile->incActiveCopies();
      t->mProfile->mAllocatedTextures++;
      
      U32 texSize = t->getHeight() * t->getWidth();
      t->mProfile->mAllocatedTexels += texSize;
      t->mProfile->mAllocatedBytes  += texSize * (t->mBitmap ? t->mBitmap->bytesPerPixel : 4);  // BJGTODO - update to track mips

      t->mProfile->mActiveTexels += texSize;
      t->mProfile->mActiveBytes += texSize * (t->mBitmap ? t->mBitmap->bytesPerPixel : 4); 
   }
}

void GFXTextureProfile::updateStatsForDeletion(GFXTextureObject *t)
{
   if(t->mProfile)
   {
      t->mProfile->decActiveCopies();
      
      U32 texSize = t->getHeight() * t->getWidth();
      t->mProfile->mActiveTexels -= texSize;
      t->mProfile->mActiveBytes -= texSize * (t->mBitmap ? t->mBitmap->bytesPerPixel : 4); 
   }
}
