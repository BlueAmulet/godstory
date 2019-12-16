//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GFX_GFXDRAWER_H_
#define _GFX_GFXDRAWER_H_

#include "gfx/gfxDevice.h"

class CommonFontEX;

/// Helper class containing utility functions for useful drawing routines
/// (line, box, rect, billboard, text).
class GFXDrawUtil
{
public:
   GFXDrawUtil(GFXDevice *);
   ~GFXDrawUtil();

   enum
   {
	   LerpAlphaBlend,
	   SeparateAlphaBlend,
	   BlendSrcOne,
   };

   ///
   void drawRect( const Point2I &upperL, const Point2I &lowerR, const ColorI &color );
   void drawRect( const RectI &rect, const ColorI &color );

   void drawRectFill( const Point2I &upperL, const Point2I &lowerR, const ColorI &color );
   void drawRectFill( const RectI &rect, const ColorI &color );
   
   void drawLine( const Point2I &startPt, const Point2I &endPt, const ColorI &color );
   void drawLine( S32 x1, S32 y1, S32 x2, S32 y2, const ColorI &color );
   
   void drawWireCube( const Point3F &size, const Point3F &pos, const ColorI &color );
   void buildCube( const Point3F &size, const Point3F &pos,GFXVertexBufferHandle<GFXVertexP> *);
   void drawCube();

   void draw2DSquare( const Point2F &screenPoint, F32 width, F32 spinAngle );

   U32 drawTextToBatch( FontRenderBatcher * renderBatch, GFont *font, const Point2I &ptDraw, const UTF8 *in_string, 
      const ColorI *colorTable = NULL, const U32 maxColorIndex = 9, F32 rot = 0.f );

   U32 drawTextToBatchBorder( FontRenderBatcher * renderBatch, GFont *font, const Point2I &ptDraw, const UTF8 *in_string, 
	   const ColorI *colorTable = NULL, const U32 maxColorIndex = 9, F32 rot = 0.f );

   U32 drawTextToBatchN( FontRenderBatcher * renderBatch, GFont *font, const Point2I &ptDraw, const UTF8 *in_string, 
      U32 n, const ColorI *colorTable = NULL, const U32 maxColorIndex = 9, F32 rot = 0.f );

   U32 drawTextToBatchNBorder( FontRenderBatcher * renderBatch, GFont *font, const Point2I &ptDraw, const UTF8 *in_string, 
	   U32 n, const ColorI *colorTable = NULL, const U32 maxColorIndex = 9, F32 rot = 0.f );

   U32 drawTextToBatch( FontRenderBatcher * renderBatch, GFont *font, const Point2I &ptDraw, const UTF16 *in_string, 
      const ColorI *colorTable = NULL, const U32 maxColorIndex = 9, F32 rot = 0.f );

   U32 drawTextToBatchBorder( FontRenderBatcher * renderBatch, GFont *font, const Point2I &ptDraw, const UTF16 *in_string, 
	   const ColorI *colorTable = NULL, const U32 maxColorIndex = 9, F32 rot = 0.f );

   U32 drawTextToBatchN( FontRenderBatcher * renderBatch, GFont *font, const Point2I &ptDraw, const UTF16 *in_string, 
      U32 n, const ColorI *colorTable = NULL, const U32 maxColorIndex = 9, F32 rot = 0.f );

   U32 drawTextToBatchNBorder( FontRenderBatcher * renderBatch, GFont *font, const Point2I &ptDraw, const UTF16 *in_string, 
	   U32 n, const ColorI *colorTable = NULL, const U32 maxColorIndex = 9, F32 rot = 0.f );

   U32 drawText( CommonFontEX *font, const Point2I &ptDraw, const UTF8 *in_string, 
      const ColorI *colorTable = NULL, const U32 maxColorIndex = 9, F32 rot = 0.f );

   U32 drawTextOutline( CommonFontEX *font, const Point2I &ptDraw, const UTF8 *in_string, 
	   const ColorI *colorTable = NULL, bool border = false, ColorI bordercolor = ColorI(0, 0, 0, 0));

   U32 drawText( GFont *font, const Point2I &ptDraw, const UTF8 *in_string, 
	   const ColorI *colorTable = NULL, const U32 maxColorIndex = 9, F32 rot = 0.f );

   U32 drawTextBorder( GFont *font, const Point2I &ptDraw, const UTF8 *in_string, 
	   const ColorI *colorTable = NULL, const U32 maxColorIndex = 9, F32 rot = 0.f );

   U32 drawTextBorder( CommonFontEX *font, const Point2I &ptDraw, const UTF8 *in_string, 
	   const ColorI *colorTable = NULL, const U32 maxColorIndex = 9, F32 rot = 0.f );

   U32 drawTextN( GFont *font, const Point2I &ptDraw, const UTF8 *in_string, 
      U32 n, const ColorI *colorTable = NULL, const U32 maxColorIndex = 9, F32 rot = 0.f );

   U32 drawTextN( CommonFontEX *font, const Point2I &ptDraw, const UTF8 *in_string, 
	   U32 n, const ColorI *colorTable = NULL, const U32 maxColorIndex = 9, F32 rot = 0.f );

   U32 drawTextNBorder( GFont *font, const Point2I &ptDraw, const UTF8 *in_string, 
	   U32 n, const ColorI *colorTable = NULL, const U32 maxColorIndex = 9, F32 rot = 0.f );

   U32 drawText( GFont *font, const Point2I &ptDraw, const UTF16 *in_string, 
      const ColorI *colorTable = NULL, const U32 maxColorIndex = 9, F32 rot = 0.f );

   U32 drawTextBorder( GFont *font, const Point2I &ptDraw, const UTF16 *in_string, 
	   const ColorI *colorTable = NULL, const U32 maxColorIndex = 9, F32 rot = 0.f );

   U32 drawTextN( GFont *font, const Point2I &ptDraw, const UTF16 *in_string, 
      U32 n, const ColorI *colorTable = NULL, const U32 maxColorIndex = 9, F32 rot = 0.f );

   U32 drawTextN( CommonFontEX *font, const Point2I &ptDraw, const UTF16 *in_string, 
	   U32 n, const ColorI *colorTable = NULL, const U32 maxColorIndex = 9, F32 rot = 0.f );

   U32 drawTextNBorder( GFont *font, const Point2I &ptDraw, const UTF16 *in_string, 
	   U32 n, const ColorI *colorTable = NULL, const U32 maxColorIndex = 9, F32 rot = 0.f );

   void setBitmapModulation( const ColorI &modColor );
   void setTextAnchorColor( const ColorI &ancColor );
   void clearBitmapModulation();
   void getBitmapModulation( ColorI *color );
   GFXVertexColor &getBitmapModulation();

   void lockAlpha( int alpha );
   void unlockAlpha();
   bool isAlphaLocked();
   int getLockedAlpha();


   void drawBitmap( GFXTextureObject*texture, const Point2I &in_rAt, const GFXBitmapFlip in_flip = GFXBitmapFlip_None );
   void drawBitmapSR( GFXTextureObject*texture, const Point2I &in_rAt, const RectI &srcRect, const GFXBitmapFlip in_flip = GFXBitmapFlip_None );
   void drawBitmapStretch( GFXTextureObject*texture, const RectI &dstRect, const GFXBitmapFlip in_flip = GFXBitmapFlip_None );
   void drawBitmapStretch( GFXTextureObject*texture, const RectF &dstRect, const GFXBitmapFlip in_flip = GFXBitmapFlip_None );
   void drawBitmapStretchSR( GFXTextureObject*texture, const RectI &dstRect, const RectI &srcRect, const GFXBitmapFlip in_flip = GFXBitmapFlip_None );
   void drawBitmapStretchSR( GFXTextureObject*texture, const RectF &dstRect, const RectF &srcRect, const GFXBitmapFlip in_flip = GFXBitmapFlip_None );
public:
	void setBlendTyps(U8 type)
	{
		blendType = type;
	}
	
	void setBlendStatus(GFXDevice * device);

    void clearBlendStatus(GFXDevice * device);

	void setZ(F32 setZ){z = setZ;}
private:

   /// The device we're rendering to.
   GFXDevice *mDevice;

   F32 z;

   /// Bitmap modulation color; bitmaps are multiplied by this color when
   /// drawn.
   GFXVertexColor mBitmapModulation;
   GFXVertexColor mBitmapModulationBak;

   /// Base text color; what color text is drawn at when no other color is
   /// specified.
   GFXVertexColor mTextAnchorColor;

   GFXPrimitiveBufferHandle mCubePB;
   bool initCubeFlag;
   static U8 blendType;
   int mLockedAlpha;
   bool mIsAlphaLocked;
	//draw
	static GFXStateBlock* mSetBitmapSB;
	static GFXStateBlock* mSetRectSB;//rect & 2dsquare & line
	static GFXStateBlock* mCubeSB;
	//blend status
	static GFXStateBlock* mAlphaBlendSB;
	static GFXStateBlock* mSepAlphaBlendSB;
	static GFXStateBlock* mLerpAlphaBlendSB;
	static GFXStateBlock* mClearAlphaBlendSB;
	static GFXStateBlock* mClearSepAlphaBlendSB;
public:
	static void init();
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

#endif