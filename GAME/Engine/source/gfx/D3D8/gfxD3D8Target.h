//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GFX_D3D_GFXD3D8TARGET_H_
#define _GFX_D3D_GFXD3D8TARGET_H_

#include "gfx/gfxTarget.h"
#include "windowManager/win32/win32Window.h"

struct IDirect3DSurface8;
struct IDirect3DSwapChain8;

class GFXD3D8TextureTarget : public GFXTextureTarget
{
   friend class GFXD3D8Device;

   IDirect3DSurface8 * mTargets[MaxRenderSlotId];

   S32 mChangeToken;
   S32 mLastAppliedChange;

   /// Called whenever a change is made to this target.
   inline void invalidateState()
   {
      mChangeToken++;
   }

   /// Called to check if we have pending state to apply to the device.
   inline const bool isPendingState() const
   {
      return (mChangeToken != mLastAppliedChange);
   }

   /// Called when the device has applied the current state.
   inline void stateApplied()
   {
      mLastAppliedChange = mChangeToken;
   }

public:

   GFXD3D8TextureTarget();
   ~GFXD3D8TextureTarget();

   // Public interface.
   virtual const Point2I getSize();
   virtual void attachTexture(RenderSlot slot, GFXTextureObject *tex, U32 mipLevel=0, U32 zOffset = 0);
   virtual void attachTexture(RenderSlot slot, GFXCubemap *tex, U32 face, U32 mipLevel=0);
   virtual void clearAttachments();

   // GFXResource interface
   virtual void zombify();
   virtual void resurrect();
};

class GFXD3D8WindowTarget : public GFXWindowTarget
{
   friend class GFXD3D8Device;
   friend class Win32Window;

   /// Our depth stencil buffer, if any.
   IDirect3DSurface8 *mDepthStencil;

   /// Maximum size we can render to.
   Point2I mSize;

   /// Our swap chain, potentially the implicit device swap chain.
   IDirect3DSwapChain8 *mSwapChain;

   /// Reference to our window.
   Win32Window *mWindow;

   /// D3D presentation info.
   D3DPRESENT_PARAMETERS mPresentationParams;

   /// Owning d3d device.
   GFXD3D8Device  *mDevice;

   /// Is this the implicit swap chain?
   bool mImplicit;

   /// Internal interface that notifies us we need to reset our video mode.
   void resetMode();

public:

   GFXD3D8WindowTarget();
   ~GFXD3D8WindowTarget();

   virtual PlatformWindow *getWindow();
   virtual const Point2I getSize();
   virtual bool present();

   void initPresentationParams();

   // GFXResourceInterface
   virtual void zombify();
   virtual void resurrect();
};

#endif
