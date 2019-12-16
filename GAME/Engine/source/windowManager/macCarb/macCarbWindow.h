//-----------------------------------------------------------------------------
// Torque Game Engine Advanced
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include <Carbon/Carbon.h>
#include "windowManager/macCarb/macCarbWindowMgr.h"
#include "gfx/gfxStructs.h"
#include "gfx/gfxTarget.h"

class MacCarbWindow : public PlatformWindow
{
   friend class MacCarbWindowManager;
   friend class GFXGLDevice;
   
   EventHandlerUPP _windowUPP;
   EventHandlerUPP _mouseUPP;
   EventHandlerUPP _keyUPP;

   static pascal OSStatus _keyHandler(EventHandlerCallRef,EventRef,void*);
   static pascal OSStatus _mouseHandler(EventHandlerCallRef,EventRef,void*);
   static pascal OSStatus _windowHandler(EventHandlerCallRef,EventRef,void*);


   GFXDevice *mDevice;
   GFXWindowTargetRef mTarget;
   GFXVideoMode mCurrentMode;
   
   MacCarbWindow *mExtantNext;
   S32 mExtantId;
   
   MacCarbWindowManager *mManager;
   
public:
   ::WindowRef _window;

   MacCarbWindow();
   ~MacCarbWindow();
   
   virtual DeviceId getDeviceId();
   virtual GFXDevice *getGFXDevice();
   virtual GFXWindowTarget *getGFXTarget();
   virtual void setMode(const GFXVideoMode &mode);
   virtual const GFXVideoMode &getCurrentMode();
   virtual bool clearFullscreen();
   virtual bool setCaption(const char *cap);
   virtual const char *getCaption();
   virtual bool setBounds(const RectI &newBounds);
   virtual bool setSize(const Point2I &newSize);
   virtual bool setPosition(const Point2I);
   virtual RectI getBounds();
   virtual bool isOpen();
   virtual bool isVisible();
   virtual bool isFocused();

   static void _realWindowHandler(EventRef event, MacCarbWindow *window);
};