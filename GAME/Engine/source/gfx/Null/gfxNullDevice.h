//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GFXNullDevice_H_
#define _GFXNullDevice_H_

#include "platform/platform.h"

//-----------------------------------------------------------------------------

#include "gfx/gfxDevice.h"
#include "gfx/gfxInit.h"
#include "gfx/gfxFence.h"

class GFXNullWindowTarget : public GFXWindowTarget
{
   PlatformWindow *mWindow;

public:

   GFXNullWindowTarget(PlatformWindow *win)
   {
      mWindow = win;
   }

   virtual PlatformWindow *getWindow()
   {
      return mWindow;
   }

   virtual bool present()
   {
      return true;
   }

   virtual const Point2I getSize()
   {
      // Return something stupid.
      return Point2I(1,1);
   }

   virtual void resetMode()
   {

   }

   virtual void zombify() {};
   virtual void resurrect() {};

};

class GFXNullDevice : public GFXDevice
{
   typedef GFXDevice Parent;

private:
   RectI viewport;
   RectI clip;
public:
   GFXNullDevice();
   virtual ~GFXNullDevice();

   static GFXDevice *createInstance( U32 adapterIndex );

   static void enumerateAdapters( Vector<GFXAdapter*> &adapterList );

   void init( const GFXVideoMode &mode, PlatformWindow *window = NULL )  {};

   virtual void activate() { };
   virtual void deactivate() { };
   virtual GFXAdapterType getAdapterType() { return NullDevice; };

   /// @name Debug Methods
   /// @{
   virtual void enterDebugEvent(ColorI color, const char *name) { };
   virtual void leaveDebugEvent() { };
   virtual void setDebugMarker(ColorI color, const char *name) { };
   /// @}

   /// Enumerates the supported video modes of the device
   virtual void enumerateVideoModes() { };

   /// Sets the video mode for the device
   virtual void setVideoMode( const GFXVideoMode &mode ) { };
protected:
   /// Sets states which have to do with general rendering
   virtual void setRenderState( U32 state, U32 value) { };

   /// Sets states which have to do with how textures are displayed
   virtual void setTextureStageState( U32 stage, U32 state, U32 value ) { };

   /// Sets states which have to do with texture sampling and addressing
   virtual void setSamplerState( U32 stage, U32 type, U32 value ) { };
   /// @}

   virtual void setTextureInternal(U32 textureUnit, const GFXTextureObject*texture) { };

   virtual void setLightInternal(U32 lightStage, const GFXLightInfo light, bool lightEnable);
   virtual void setLightMaterialInternal(const GFXLightMaterial mat) { };
   virtual void setGlobalAmbientInternal(ColorF color) { };

   /// @name State Initalization.
   /// @{

   /// State initalization. This MUST BE CALLED in setVideoMode after the device
   /// is created.
   virtual void initStates() { };

   virtual void setMatrix( GFXMatrixType mtype, const MatrixF &mat ) { };

   virtual GFXVertexBuffer *allocVertexBuffer( U32 numVerts, U32 vertFlags, U32 vertSize, GFXBufferType bufferType,U8 streamNum );
   virtual GFXPrimitiveBuffer *allocPrimitiveBuffer( U32 numIndices, U32 numPrimitives, GFXBufferType bufferType );
public:
   virtual void copyBBToSfxBuff(int Idx) { };

   virtual void zombifyTextureManager() { };
   virtual void resurrectTextureManager() { };

   virtual GFXCubemap * createCubemap();

   virtual F32 getFillConventionOffset() const { return 0.0f; };

   ///@}

   virtual GFXTextureTarget *allocRenderToTextureTarget(){return NULL;};
   virtual GFXTextureTarget *allocRenderToTextureTarget(Point2I size, GFXFormat format){return NULL;};
   virtual GFXWindowTarget *allocWindowTarget(PlatformWindow *window)
   {
      return new GFXNullWindowTarget(window);
   };

   virtual void pushActiveRenderTarget(){};
   virtual void popActiveRenderTarget(){};
   virtual void setActiveRenderTarget( GFXTarget *target ){};
   virtual GFXTarget *getActiveRenderTarget(){return NULL;};

   virtual F32 getPixelShaderVersion() const { return 0.0f; };
   virtual void setPixelShaderVersion( F32 version ) { };
   virtual U32 getNumSamplers() const { return 0; };

   virtual GFXShader * createShader( const char *vertFile, const char *pixFile, F32 pixVersion ) { return NULL; };
   virtual GFXShader * createShader( GFXShaderFeatureData &featureData, GFXVertexFlags vertFlags ) { return NULL; };

   // This is called by MatInstance::reinitInstances to cause the shaders to be regenerated.
   virtual void flushProceduralShaders() { };


   virtual void clear( U32 flags, ColorI color, F32 z, U32 stencil ) { };
   virtual void beginSceneInternal() { };
   virtual void endSceneInternal() { };

   virtual void drawPrimitive( GFXPrimitiveType primType, U32 vertexStart, U32 primitiveCount ) { };
   virtual void drawIndexedPrimitive( GFXPrimitiveType primType, U32 minIndex, U32 numVerts, U32 startIndex, U32 primitiveCount ) { };

   virtual void setViewport( const RectI &rect ) { };
   virtual const RectI &getViewport() const { return viewport; };

   virtual void setClipRect( const RectI &rect ) { };
   virtual void setClipRectOrtho( const RectI &rect, const RectI &orthoRect ) { };
   virtual const RectI &getClipRect() const { return clip; };

   virtual void preDestroy() { };

   virtual U32 getMaxDynamicVerts() { return 16384; };
   virtual U32 getMaxDynamicIndices() { return 16384; };

   virtual GFXFormat selectSupportedFormat( GFXTextureProfile *profile, const Vector<GFXFormat> &formats, bool texture, bool mustblend ) { return GFXFormatR8G8B8A8; };
   GFXFence *createFence() { return new GFXGeneralFence( this ); }
};

#endif
