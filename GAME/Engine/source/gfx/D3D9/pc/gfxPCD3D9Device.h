//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GFX_PC_D3D9DEVICE_H_
#define _GFX_PC_D3D9DEVICE_H_

#include "gfx/D3D9/gfxD3D9Device.h"

class GFXPCD3D9Device : public GFXD3D9Device
{
public:
   GFXPCD3D9Device( LPDIRECT3D9 d3d, U32 index ) : GFXD3D9Device( d3d, index ) {};
   ~GFXPCD3D9Device();

   static GFXDevice *createInstance( U32 adapterIndex );

   virtual GFXFormat selectSupportedFormat(GFXTextureProfile *profile,
	   const Vector<GFXFormat> &formats, bool texture, bool mustblend);
   
   virtual void enumerateVideoModes();


   virtual void beginSceneInternal();
   virtual void endSceneInternal();

   virtual void setActiveRenderTarget( GFXTarget *target );
   virtual GFXWindowTarget *allocWindowTarget(PlatformWindow *window);
   virtual GFXTextureTarget *allocRenderToTextureTarget();

   virtual void init( const GFXVideoMode &mode, PlatformWindow *window = NULL );

   virtual void enterDebugEvent(ColorI color, const char *name);
   virtual void leaveDebugEvent();
   virtual void setDebugMarker(ColorI color, const char *name);

   virtual GFXTexHandle &getSfxBackBuffer(int Idx=0);
   virtual void copyBBToSfxBuff(int Idx=0);
   virtual void copyBBToBuff(GFXTexHandle &);

   virtual void setMatrix( GFXMatrixType mtype, const MatrixF &mat );

   virtual void initStates();
   virtual void reset( D3DPRESENT_PARAMETERS &d3dpp );

   D3DPRESENT_PARAMETERS* GetD3DPP(void) {return &m_d3dpp;}
protected:
   virtual D3DPRESENT_PARAMETERS setupPresentParams( const GFXVideoMode &mode, const HWND &hwnd );
   virtual void setTextureStageState( U32 stage, U32 state, U32 value );
   
   void validateMultisampleParams(D3DFORMAT format, BOOL windowed);

   D3DPRESENT_PARAMETERS m_d3dpp;
};

#endif