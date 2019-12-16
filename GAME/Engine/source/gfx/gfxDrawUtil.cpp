//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "gfx/gfxDrawUtil.h"
#include "core/unicode.h"
#include "gfx/gfxFontRenderBatcher.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"
#include "gfx/gfxFontRenderBatcher.h"
//===================================================================================
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
//#define STATEBLOCK
//draw
GFXStateBlock* GFXDrawUtil::mSetBitmapSB = NULL;
GFXStateBlock* GFXDrawUtil::mSetRectSB = NULL;//rect & 2dsquare & line
GFXStateBlock* GFXDrawUtil::mCubeSB = NULL;
//blend status
GFXStateBlock* GFXDrawUtil::mAlphaBlendSB = NULL;
GFXStateBlock* GFXDrawUtil::mSepAlphaBlendSB = NULL;
GFXStateBlock* GFXDrawUtil::mLerpAlphaBlendSB = NULL;
GFXStateBlock* GFXDrawUtil::mClearAlphaBlendSB = NULL;
GFXStateBlock* GFXDrawUtil::mClearSepAlphaBlendSB = NULL;
U8 GFXDrawUtil::blendType = GFXDrawUtil::LerpAlphaBlend;

ColorI gDefaultColor = ColorI(255,255,255);
const int gDefaultMaxStrLen = 1024;
char Buffer[gDefaultMaxStrLen];
char *gBuffer = Buffer;

GFXDrawUtil::GFXDrawUtil( GFXDevice * d)
{
   mDevice = d;
   mBitmapModulation.set(0xFF, 0xFF, 0xFF, 0xFF);
   mTextAnchorColor.set(0xFF, 0xFF, 0xFF, 0xFF);
   initCubeFlag = true;
   mCubePB = NULL;
   z = 0.0f;
   mLockedAlpha = 255;
   mIsAlphaLocked = false;
}

GFXDrawUtil::~GFXDrawUtil()
{
	mCubePB = NULL;
}

#ifdef STATEBLOCK
//采用stateblock时，不再需要device参数，因为程序中都是使用的GFX
void GFXDrawUtil::setBlendStatus(GFXDevice * device)
{
	switch(GFXDrawUtil::blendType)
	{
	case BlendSrcOne:
		{
			AssertFatal(mAlphaBlendSB, "GFXDrawUtil::setBlendStatus -- mAlphaBlendSB cannot be NULL.");
			mAlphaBlendSB->apply();

			mBitmapModulationBak = mBitmapModulation;
			ColorI col;
			mBitmapModulation.getColor(&col);
			col.alpha = 130;
			mBitmapModulation = col;
		}
	case LerpAlphaBlend:
		AssertFatal(mLerpAlphaBlendSB, "GFXDrawUtil::setBlendStatus -- mLerpAlphaBlendSB cannot be NULL.");
		mLerpAlphaBlendSB->apply();
		break;
	case SeparateAlphaBlend:
		AssertFatal(mSepAlphaBlendSB, "GFXDrawUtil::setBlendStatus -- mSepAlphaBlendSB cannot be NULL.");
		mSepAlphaBlendSB->apply();
		break;
	}
}
#else
void GFXDrawUtil::setBlendStatus(GFXDevice * device)
{
	switch(GFXDrawUtil::blendType)
	{
	case BlendSrcOne:
		{
			device->setSrcBlend( GFXBlendOne );
			device->setDestBlend( GFXBlendZero );

			//device->setAlphaRef(130);
			//device->setAlphaFunc(GFXCmpGreater);
			//device->setAlphaTestEnable(true);
			//device->setTextureStageAlphaOp( 0, GFXTOPAdd );

			mBitmapModulationBak = mBitmapModulation;
			ColorI col;
			mBitmapModulation.getColor(&col);
			col.alpha = 130;
			mBitmapModulation = col;
		}
	case LerpAlphaBlend:
		device->setSrcBlend( GFXBlendSrcAlpha );
		device->setDestBlend( GFXBlendInvSrcAlpha );
		break;
	case SeparateAlphaBlend:
		device->setSeparateAlphaBlendEnable( true );
		device->setSrcBlend( GFXBlendSrcAlpha );
		device->setDestBlend( GFXBlendInvSrcAlpha );
		device->setSrcBlendAlpha( GFXBlendSrcAlpha );
		device->setDestBlendAlpha( GFXBlendDestAlpha );
		break;
	}
}
#endif
//-----------------------------------------------------------------------------

void GFXDrawUtil::setBitmapModulation( const ColorI &modColor )
{
   mBitmapModulation = modColor;

   if( mIsAlphaLocked )
	   mBitmapModulation.set( modColor.red, modColor.green, modColor.blue, mLockedAlpha );
}

void GFXDrawUtil::clearBitmapModulation()
{
	if( mIsAlphaLocked )
		mBitmapModulation.set( 255, 255, 255, mLockedAlpha );
	else
		mBitmapModulation.set( 255, 255, 255, 255 );
}

void GFXDrawUtil::getBitmapModulation( ColorI *color )
{
   mBitmapModulation.getColor( color );
}

GFXVertexColor &GFXDrawUtil::getBitmapModulation()
{
	return mBitmapModulation;
}

void GFXDrawUtil::setTextAnchorColor( const ColorI &ancColor )
{
   mTextAnchorColor = ancColor;
}

//-----------------------------------------------------------------------------

U32 GFXDrawUtil::drawText( GFont *font, const Point2I &ptDraw, const UTF16 *in_string, 
                        const ColorI *colorTable, const U32 maxColorIndex, F32 rot )
{
   return drawTextN( font, ptDraw, in_string, dStrlen(in_string), colorTable, maxColorIndex, rot );
}

U32 GFXDrawUtil::drawTextBorder( GFont *font, const Point2I &ptDraw, const UTF16 *in_string, 
						  const ColorI *colorTable, const U32 maxColorIndex, F32 rot )
{
	return drawTextNBorder( font, ptDraw, in_string, dStrlen(in_string), colorTable, maxColorIndex, rot );
}

U32 GFXDrawUtil::drawText( GFont *font, const Point2I &ptDraw, const UTF8 *in_string, 
						  const ColorI *colorTable, const U32 maxColorIndex, F32 rot )
{
	return drawTextN( font, ptDraw, in_string, dStrlen(in_string), colorTable, maxColorIndex, rot );
}

U32 GFXDrawUtil::drawText( CommonFontEX *font, const Point2I &ptDraw, const UTF8 *in_string, 
						  const ColorI *colorTable, const U32 maxColorIndex, F32 rot )
{
	font->BeginRender();
	if(!colorTable)
		font->SetText(ptDraw, gDefaultColor, const_cast<char*>(in_string));
	else
		font->SetText(ptDraw, *colorTable, const_cast<char*>(in_string));
	font->Render();
	return font->getStrWidth(const_cast<char*>(in_string));
}

U32 GFXDrawUtil::drawTextOutline( CommonFontEX *font, const Point2I &ptDraw, const UTF8 *in_string, 
			 const ColorI *colorTable, bool border, ColorI bordercolor)
{
	font->BeginRender();
	if(!colorTable)
		font->SetText(ptDraw, gDefaultColor, const_cast<char*>(in_string), border, bordercolor);
	else
		font->SetText(ptDraw, *colorTable, const_cast<char*>(in_string), border, bordercolor);
	font->Render();
	return font->getStrWidth(const_cast<char*>(in_string));
}

U32 GFXDrawUtil::drawTextBorder( GFont *font, const Point2I &ptDraw, const UTF8 *in_string, 
						  const ColorI *colorTable, const U32 maxColorIndex, F32 rot )
{
	return drawTextNBorder( font, ptDraw, in_string, dStrlen(in_string), colorTable, maxColorIndex, rot );
}

U32 GFXDrawUtil::drawTextBorder( CommonFontEX *font, const Point2I &ptDraw, const UTF8 *in_string, 
								const ColorI *colorTable, const U32 maxColorIndex, F32 rot )
{
	font->BeginRender();
	if(!colorTable)
		font->SetText(ptDraw, gDefaultColor, const_cast<char*>(in_string));
	else
		font->SetText(ptDraw, *colorTable, const_cast<char*>(in_string));
	font->Render();
	return font->getStrWidth(const_cast<char*>(in_string));
}

U32 GFXDrawUtil::drawTextN( GFont *font, const Point2I &ptDraw, const UTF8 *in_string, U32 n,
                         const ColorI *colorTable, const U32 maxColorIndex, F32 rot )
{
   // Convert to UTF16 temporarily.
   n++; // space for null terminator
   FrameTemp<UTF16> ubuf( n * sizeof(UTF16) );
   convertUTF8toUTF16(in_string, ubuf, n);

   return drawTextN( font, ptDraw, ubuf, n, colorTable, maxColorIndex, rot );
}

U32 GFXDrawUtil::drawTextNBorder( GFont *font, const Point2I &ptDraw, const UTF8 *in_string, U32 n,
						   const ColorI *colorTable, const U32 maxColorIndex, F32 rot )
{
	// Convert to UTF16 temporarily.
	n++; // space for null terminator
	FrameTemp<UTF16> ubuf( n * sizeof(UTF16) );
	convertUTF8toUTF16(in_string, ubuf, n);

	return drawTextNBorder( font, ptDraw, ubuf, n, colorTable, maxColorIndex, rot );
}

U32 GFXDrawUtil::drawTextN( CommonFontEX *font, const Point2I &ptDraw, const UTF16 *in_string, 
						   U32 n, const ColorI *colorTable, const U32 maxColorIndex, F32 rot )
{
      UTF16 temp[1024];
	  ZeroMemory(temp, 1024);
	  memcpy(temp, in_string, 2 * n);
	  convertUTF16toUTF8(temp, gBuffer, gDefaultMaxStrLen);
	  //WideCharToMultiByte( 936, WC_NO_BEST_FIT_CHARS, in_string, n, gBuffer, 1024, NULL, false);
	  font->BeginRender();
	  if(!colorTable)
		  font->SetText(ptDraw, gDefaultColor, const_cast<char*>(gBuffer));
	  else
		  font->SetText(ptDraw, *colorTable, const_cast<char*>(gBuffer));
	  font->Render();
	  return font->getStrWidth(gBuffer);
}

U32 GFXDrawUtil::drawTextN( CommonFontEX *font, const Point2I &ptDraw, const UTF8 *in_string, U32 n,
						   const ColorI *colorTable, const U32 maxColorIndex, F32 rot )
{
	int Len = (n<gDefaultMaxStrLen-1) ? n : (gDefaultMaxStrLen-1);
	dMemcpy(gBuffer,in_string,Len);
	gBuffer[Len] = 0;
	font->BeginRender();
	if(!colorTable)
		font->SetText(ptDraw, gDefaultColor, const_cast<char*>(gBuffer));
	else
		font->SetText(ptDraw, *colorTable, const_cast<char*>(gBuffer));
	font->Render();
	return font->getStrWidth(const_cast<char*>(gBuffer));
}

U32 GFXDrawUtil::drawTextN( GFont *font, const Point2I &ptDraw, const UTF16 *in_string, 
                         U32 n, const ColorI *colorTable, const U32 maxColorIndex, F32 rot )
{
   // return on zero length strings
   if( n < 1 )
      return ptDraw.x;

   // If it's over about 4000 verts we want to break it up
   if( n > 666 )
   {
      U32 left = drawTextN(font, ptDraw, in_string, 666, colorTable, maxColorIndex, rot);

      Point2I newDrawPt(left, ptDraw.y);
      const UTF16* str = (const UTF16*)in_string;

      return drawTextN(font, newDrawPt, &(str[666]), n - 666, colorTable, maxColorIndex, rot);
   }

   PROFILE_START(GFXDevice_drawTextN);

   Point2I pt(0,0);
   UTF16 c;   
   GFXVertexColor currentColor;

   currentColor = mBitmapModulation;

   // Queue everything for render.
   FontRenderBatcher frb;
   frb.init(font, n);

   U32 i;
   for(i = 0, c = in_string[i];
      in_string[i] && i < n;
      i++, c = in_string[i])
   {
      // We have to do a little dance here since \t = 0x9, \n = 0xa, and \r = 0xd
      if ((c >=  1 && c <=  7) ||
         (c >= 11 && c <= 12) ||
         (c == 14)) 
      {
         // Color code
         if (colorTable) 
         {
            static U8 remap[15] = 
            { 
               0x0,
                  0x0, 
                  0x1, 
                  0x2, 
                  0x3, 
                  0x4, 
                  0x5, 
                  0x6, 
                  0x0, 
                  0x0,
                  0x0, 
                  0x7, 
                  0x8,
                  0x0,
                  0x9 
            };

            U8 remapped = remap[c];

            // Ignore if the color is greater than the specified max index:
            if ( remapped <= maxColorIndex )
            {
               const ColorI &clr = colorTable[remapped];
               currentColor = mBitmapModulation = clr;
            }
         }

         // And skip rendering this character.
         continue;
      }
      // reset color?
      else if ( c == 15 )
      {
         currentColor = mBitmapModulation = mTextAnchorColor;

         // And skip rendering this character.
         continue;
      }
      // push color:
      else if ( c == 16 )
      {
         mTextAnchorColor = mBitmapModulation;

         // And skip rendering this character.
         continue;
      }
      // pop color:
      else if ( c == 17 )
      {
         currentColor = mBitmapModulation = mTextAnchorColor;

         // And skip rendering this character.
         continue;
      }
      // Tab character
      else if ( c == dT('\t') ) 
      {
         const PlatformFont::CharInfo &ci = font->getCharInfo( dT(' ') );
         pt.x += ci.xIncrement * GFont::TabWidthInSpaces;

         // And skip rendering this character.
         continue;
      }
      // Don't draw invalid characters.
      else if( !font->isValidChar( c ) ) 
         continue;

      // Queue char for rendering..
      frb.queueChar(c, pt.x, currentColor);
   }


   frb.render(rot, Point2F((F32)ptDraw.x, (F32)ptDraw.y));

   PROFILE_END(GFXDevice_drawTextN);

   return pt.x - ptDraw.x;
}

U32 GFXDrawUtil::drawTextNBorder( GFont *font, const Point2I &ptDraw, const UTF16 *in_string, 
						   U32 n, const ColorI *colorTable, const U32 maxColorIndex, F32 rot )
{
	// return on zero length strings
	if( n < 1 )
		return ptDraw.x;

	// If it's over about 4000 verts we want to break it up
	if( n > 666 )
	{
		U32 left = drawTextNBorder(font, ptDraw, in_string, 666, colorTable, maxColorIndex, rot);

		Point2I newDrawPt(left, ptDraw.y);
		const UTF16* str = (const UTF16*)in_string;

		return drawTextNBorder(font, newDrawPt, &(str[666]), n - 666, colorTable, maxColorIndex, rot);
	}

	PROFILE_START(GFXDevice_drawTextNBorder);

	Point2I pt(0,0);
	UTF16 c;   
	GFXVertexColor currentColor;

	currentColor = mBitmapModulation;

	// Queue everything for render.
	FontRenderBatcher frb;
	frb.init(font, n);

	U32 i;
	for(i = 0, c = in_string[i];
		in_string[i] && i < n;
		i++, c = in_string[i])
	{
		// We have to do a little dance here since \t = 0x9, \n = 0xa, and \r = 0xd
		if ((c >=  1 && c <=  7) ||
			(c >= 11 && c <= 12) ||
			(c == 14)) 
		{
			// Color code
			if (colorTable) 
			{
				static U8 remap[15] = 
				{ 
					0x0,
					0x0, 
					0x1, 
					0x2, 
					0x3, 
					0x4, 
					0x5, 
					0x6, 
					0x0, 
					0x0,
					0x0, 
					0x7, 
					0x8,
					0x0,
					0x9 
				};

				U8 remapped = remap[c];

				// Ignore if the color is greater than the specified max index:
				if ( remapped <= maxColorIndex )
				{
					const ColorI &clr = colorTable[remapped];
					currentColor = mBitmapModulation = clr;
				}
			}

			// And skip rendering this character.
			continue;
		}
		// reset color?
		else if ( c == 15 )
		{
			currentColor = mBitmapModulation = mTextAnchorColor;

			// And skip rendering this character.
			continue;
		}
		// push color:
		else if ( c == 16 )
		{
			mTextAnchorColor = mBitmapModulation;

			// And skip rendering this character.
			continue;
		}
		// pop color:
		else if ( c == 17 )
		{
			currentColor = mBitmapModulation = mTextAnchorColor;

			// And skip rendering this character.
			continue;
		}
		// Tab character
		else if ( c == dT('\t') ) 
		{
			const PlatformFont::CharInfo &ci = font->getCharInfo( dT(' ') );
			pt.x += ci.xIncrement * GFont::TabWidthInSpaces;

			// And skip rendering this character.
			continue;
		}
		// Don't draw invalid characters.
		else if( !font->isValidChar( c ) ) 
			continue;

		// Queue char for rendering..
		frb.queueChar(c, pt.x, currentColor);
	}


	frb.renderBorder(rot, Point2F((F32)ptDraw.x, (F32)ptDraw.y));

	PROFILE_END(GFXDevice_drawTextNBorder);

	return pt.x - ptDraw.x;
}

U32 GFXDrawUtil::drawTextToBatch( FontRenderBatcher * renderBatch, GFont *font, const Point2I &ptDraw, const UTF16 *in_string, 
                               const ColorI *colorTable, const U32 maxColorIndex, F32 rot )
{
   return drawTextToBatchN( renderBatch, font, ptDraw, in_string, dStrlen(in_string), colorTable, maxColorIndex, rot );
}

U32 GFXDrawUtil::drawTextToBatchBorder( FontRenderBatcher * renderBatch, GFont *font, const Point2I &ptDraw, const UTF16 *in_string, 
								 const ColorI *colorTable, const U32 maxColorIndex, F32 rot )
{
	return drawTextToBatchNBorder( renderBatch, font, ptDraw, in_string, dStrlen(in_string), colorTable, maxColorIndex, rot );
}

U32 GFXDrawUtil::drawTextToBatch( FontRenderBatcher * renderBatch, GFont *font, const Point2I &ptDraw, const UTF8 *in_string, 
                               const ColorI *colorTable, const U32 maxColorIndex, F32 rot )
{
   return drawTextToBatchN( renderBatch, font, ptDraw, in_string, dStrlen(in_string), colorTable, maxColorIndex, rot );
}

U32 GFXDrawUtil::drawTextToBatchBorder( FontRenderBatcher * renderBatch, GFont *font, const Point2I &ptDraw, const UTF8 *in_string, 
								 const ColorI *colorTable, const U32 maxColorIndex, F32 rot )
{
	return drawTextToBatchNBorder( renderBatch, font, ptDraw, in_string, dStrlen(in_string), colorTable, maxColorIndex, rot );
}

U32 GFXDrawUtil::drawTextToBatchN( FontRenderBatcher * renderBatch, GFont *font, const Point2I &ptDraw, const UTF8 *in_string, U32 n,
                                const ColorI *colorTable, const U32 maxColorIndex, F32 rot )
{
   // Convert to UTF16 temporarily.
   n++; // space for null terminator
   FrameTemp<UTF16> ubuf( n * sizeof(UTF16) );
   convertUTF8toUTF16(in_string, ubuf, n);

   return drawTextToBatchN( renderBatch, font, ptDraw, ubuf, n, colorTable, maxColorIndex, rot );
}

U32 GFXDrawUtil::drawTextToBatchNBorder( FontRenderBatcher * renderBatch, GFont *font, const Point2I &ptDraw, const UTF8 *in_string, U32 n,
								  const ColorI *colorTable, const U32 maxColorIndex, F32 rot )
{
	// Convert to UTF16 temporarily.
	n++; // space for null terminator
	FrameTemp<UTF16> ubuf( n * sizeof(UTF16) );
	convertUTF8toUTF16(in_string, ubuf, n);

	return drawTextToBatchNBorder( renderBatch, font, ptDraw, ubuf, n, colorTable, maxColorIndex, rot );
}


U32 GFXDrawUtil::drawTextToBatchN( FontRenderBatcher * renderBatch, GFont *font, const Point2I &ptDraw, const UTF16 *in_string, 
                                U32 n, const ColorI *colorTable, const U32 maxColorIndex, F32 rot )
{
   // return on zero length strings
   if( n < 1 )
      return ptDraw.x;

   // Removed this block as it recurses and when rendering to a batch
   // with this implementation, drawText only supports a single
   // line of text so it's not really useful to assume we have lines
   // rendering to screen that are longer than 600 characters 
   // CodeReview [6/3/2007 justind]

   // If it's over about 4000 verts we want to break it up
   //if( n > 666 )
   //{
   //   U32 left = drawTextToBatchN(renderBatch,font, ptDraw, in_string, 666, colorTable, maxColorIndex, rot);

   //   Point2I newDrawPt(left, ptDraw.y);
   //   const UTF16* str = (const UTF16*)in_string;

   //   return drawTextToBatchN(font, newDrawPt, &(str[666]), n - 666, colorTable, maxColorIndex, rot);
   //}

   PROFILE_START(GFXDevice_drawTextN_Batch);

   Point2I     pt = ptDraw;
   UTF16 c;   
   GFXVertexColor currentColor;

   currentColor = mBitmapModulation;

   // If no batch is specified, add it to the default batch
   //  and then render it immediately
   FontRenderBatcher frb;
   bool doRenderBatch = true;
   if( renderBatch == NULL )
   {
      renderBatch = &frb;
      renderBatch->init( font, n );
   }
   else
      doRenderBatch = false;

   U32 i;
   for(i = 0, c = in_string[i];
      in_string[i] && i < n;
      i++, c = in_string[i])
   {
      // We have to do a little dance here since \t = 0x9, \n = 0xa, and \r = 0xd
      if ((c >=  1 && c <=  7) ||
         (c >= 11 && c <= 12) ||
         (c == 14)) 
      {
         // Color code
         if (colorTable) 
         {
            static U8 remap[15] = 
            { 
               0x0,
                  0x0, 
                  0x1, 
                  0x2, 
                  0x3, 
                  0x4, 
                  0x5, 
                  0x6, 
                  0x0, 
                  0x0,
                  0x0, 
                  0x7, 
                  0x8,
                  0x0,
                  0x9 
            };

            U8 remapped = remap[c];

            // Ignore if the color is greater than the specified max index:
            if ( remapped <= maxColorIndex )
            {
               const ColorI &clr = colorTable[remapped];
               currentColor = mBitmapModulation = clr;
            }
         }

         // And skip rendering this character.
         continue;
      }
      // reset color?
      else if ( c == 15 )
      {
         currentColor = mBitmapModulation = mTextAnchorColor;

         // And skip rendering this character.
         continue;
      }
      // push color:
      else if ( c == 16 )
      {
         mTextAnchorColor = mBitmapModulation;

         // And skip rendering this character.
         continue;
      }
      // pop color:
      else if ( c == 17 )
      {
         currentColor = mBitmapModulation = mTextAnchorColor;

         // And skip rendering this character.
         continue;
      }
      // Tab character
      else if ( c == dT('\t') ) 
      {
         const PlatformFont::CharInfo &ci = font->getCharInfo( dT(' ') );
         pt.x += ci.xIncrement * GFont::TabWidthInSpaces;

         // And skip rendering this character.
         continue;
      }
      // Don't draw invalid characters.
      else if( !font->isValidChar( c ) ) 
         continue;

      // Queue char for rendering..
      renderBatch->queueChar(c, pt.x, currentColor);
   }


   if( doRenderBatch )
      renderBatch->render(rot, Point2F((F32)ptDraw.x, (F32)ptDraw.y) );

   PROFILE_END(GFXDevice_drawTextN_Batch);

   return pt.x - ptDraw.x;
}

U32 GFXDrawUtil::drawTextToBatchNBorder( FontRenderBatcher * renderBatch, GFont *font, const Point2I &ptDraw, const UTF16 *in_string, 
								  U32 n, const ColorI *colorTable, const U32 maxColorIndex, F32 rot )
{
	// return on zero length strings
	if( n < 1 )
		return ptDraw.x;

	// Removed this block as it recurses and when rendering to a batch
	// with this implementation, drawText only supports a single
	// line of text so it's not really useful to assume we have lines
	// rendering to screen that are longer than 600 characters 
	// CodeReview [6/3/2007 justind]

	// If it's over about 4000 verts we want to break it up
	//if( n > 666 )
	//{
	//   U32 left = drawTextToBatchN(renderBatch,font, ptDraw, in_string, 666, colorTable, maxColorIndex, rot);

	//   Point2I newDrawPt(left, ptDraw.y);
	//   const UTF16* str = (const UTF16*)in_string;

	//   return drawTextToBatchN(font, newDrawPt, &(str[666]), n - 666, colorTable, maxColorIndex, rot);
	//}

	PROFILE_START(GFXDevice_drawTextN_BatchBorder);

	Point2I     pt = ptDraw;
	UTF16 c;   
	GFXVertexColor currentColor;

	currentColor = mBitmapModulation;

	// If no batch is specified, add it to the default batch
	//  and then render it immediately
	FontRenderBatcher frb;
	bool doRenderBatch = true;
	if( renderBatch == NULL )
	{
		renderBatch = &frb;
		renderBatch->init( font, n );
	}
	else
		doRenderBatch = false;

	U32 i;
	for(i = 0, c = in_string[i];
		in_string[i] && i < n;
		i++, c = in_string[i])
	{
		// We have to do a little dance here since \t = 0x9, \n = 0xa, and \r = 0xd
		if ((c >=  1 && c <=  7) ||
			(c >= 11 && c <= 12) ||
			(c == 14)) 
		{
			// Color code
			if (colorTable) 
			{
				static U8 remap[15] = 
				{ 
					0x0,
					0x0, 
					0x1, 
					0x2, 
					0x3, 
					0x4, 
					0x5, 
					0x6, 
					0x0, 
					0x0,
					0x0, 
					0x7, 
					0x8,
					0x0,
					0x9 
				};

				U8 remapped = remap[c];

				// Ignore if the color is greater than the specified max index:
				if ( remapped <= maxColorIndex )
				{
					const ColorI &clr = colorTable[remapped];
					currentColor = mBitmapModulation = clr;
				}
			}

			// And skip rendering this character.
			continue;
		}
		// reset color?
		else if ( c == 15 )
		{
			currentColor = mBitmapModulation = mTextAnchorColor;

			// And skip rendering this character.
			continue;
		}
		// push color:
		else if ( c == 16 )
		{
			mTextAnchorColor = mBitmapModulation;

			// And skip rendering this character.
			continue;
		}
		// pop color:
		else if ( c == 17 )
		{
			currentColor = mBitmapModulation = mTextAnchorColor;

			// And skip rendering this character.
			continue;
		}
		// Tab character
		else if ( c == dT('\t') ) 
		{
			const PlatformFont::CharInfo &ci = font->getCharInfo( dT(' ') );
			pt.x += ci.xIncrement * GFont::TabWidthInSpaces;

			// And skip rendering this character.
			continue;
		}
		// Don't draw invalid characters.
		else if( !font->isValidChar( c ) ) 
			continue;

		// Queue char for rendering..
		renderBatch->queueChar(c, pt.x, currentColor);
	}


	if( doRenderBatch )
		renderBatch->renderBorder(rot, Point2F((F32)ptDraw.x, (F32)ptDraw.y) );

	PROFILE_END(GFXDevice_drawTextN_BatchBorder);

	return pt.x - ptDraw.x;
}

//------------------------------------------------------------------------------

void GFXDrawUtil::drawBitmap( GFXTextureObject* texture, const Point2I &in_rAt, const GFXBitmapFlip in_flip )
{
   AssertFatal( texture != 0, "No texture specified for drawBitmap()" );

   RectI subRegion( 0, 0, texture->mBitmapSize.x, texture->mBitmapSize.y );
   RectI stretch( in_rAt.x, in_rAt.y, texture->mBitmapSize.x, texture->mBitmapSize.y );
   drawBitmapStretchSR( texture, stretch, subRegion, in_flip );
}

void GFXDrawUtil::drawBitmapStretch( GFXTextureObject* texture, const RectI &dstRect, const GFXBitmapFlip in_flip )
{
   AssertFatal( texture != 0, "No texture specified for drawBitmapStretch()" );

   RectI subRegion( 0, 0, texture->mBitmapSize.x, texture->mBitmapSize.y );
   drawBitmapStretchSR( texture, dstRect, subRegion, in_flip );
}

void GFXDrawUtil::drawBitmapStretch( GFXTextureObject* texture, const RectF &dstRect, const GFXBitmapFlip in_flip )
{
	AssertFatal( texture != 0, "No texture specified for drawBitmapStretch()" );

	RectF subRegion( 0, 0, texture->mBitmapSize.x, texture->mBitmapSize.y );
	drawBitmapStretchSR( texture, dstRect, subRegion, in_flip );
}

void GFXDrawUtil::drawBitmapSR( GFXTextureObject* texture, const Point2I &in_rAt, const RectI &srcRect, const GFXBitmapFlip in_flip )
{
   AssertFatal( texture != 0, "No texture specified for drawBitmapSR()" );

   RectI stretch( in_rAt.x, in_rAt.y, srcRect.len_x(), srcRect.len_y() );
   drawBitmapStretchSR( texture, stretch, srcRect, in_flip );
}

void GFXDrawUtil::drawBitmapStretchSR( GFXTextureObject *texture, const RectI &dstRect, const RectI &srcRect, const GFXBitmapFlip in_flip ) 
{
   // Sanity if no texture is specified.
   if(!texture)
      return;

   mDevice->setBaseDrawState();

   GFXVertexBufferHandle<GFXVertexPCT> verts(mDevice, 4, GFXBufferTypeVolatile );
   verts.lock();

   F32 texLeft   = F32(srcRect.point.x)                    / F32(texture->mTextureSize.x);
   F32 texRight  = F32(srcRect.point.x + srcRect.extent.x) / F32(texture->mTextureSize.x);
   F32 texTop    = F32(srcRect.point.y)                    / F32(texture->mTextureSize.y);
   F32 texBottom = F32(srcRect.point.y + srcRect.extent.y) / F32(texture->mTextureSize.y);

   F32 screenLeft   = (F32)dstRect.point.x;
   F32 screenRight  = (F32)(dstRect.point.x + dstRect.extent.x);
   F32 screenTop    = (F32)dstRect.point.y;
   F32 screenBottom = (F32)(dstRect.point.y + dstRect.extent.y);

   if( in_flip & GFXBitmapFlip_X ) 
   {
      F32 temp = texLeft;
      texLeft = texRight;
      texRight = temp;
   }
   if( in_flip & GFXBitmapFlip_Y ) 
   {
      F32 temp = texTop;
      texTop = texBottom;
      texBottom = temp;
   }

   const F32 fillConv = mDevice->getFillConventionOffset();
   verts[0].point.set( screenLeft  - fillConv, screenTop    - fillConv, z );
   verts[1].point.set( screenRight - fillConv, screenTop    - fillConv, z );
   verts[2].point.set( screenLeft  - fillConv, screenBottom - fillConv, z );
   verts[3].point.set( screenRight - fillConv, screenBottom - fillConv, z );

   verts[0].color = verts[1].color = verts[2].color = verts[3].color = mBitmapModulation;

   verts[0].texCoord.set( texLeft,  texTop );
   verts[1].texCoord.set( texRight, texTop );
   verts[2].texCoord.set( texLeft,  texBottom );
   verts[3].texCoord.set( texRight, texBottom );

   verts.unlock();

   mDevice->setVertexBuffer( verts );

#ifdef STATEBLOCK
	AssertFatal(mSetBitmapSB, "GFXDrawUtil::drawBitmapStretchSR -- mSetBitmapSB cannot be NULL.");
	mSetBitmapSB->apply();
	setBlendStatus(mDevice);
#else
   mDevice->setCullMode( GFXCullNone );
   mDevice->setLightingEnable( false );
   mDevice->setAlphaBlendEnable( true );
   setBlendStatus(mDevice);
   mDevice->setTextureStageColorOp( 0, GFXTOPModulate );
   mDevice->setTextureStageColorOp( 1, GFXTOPDisable );
#endif
   mDevice->setTexture( 0, texture );
   mDevice->setupGenericShaders( GFXDevice::GSModColorTexture );

   mDevice->drawPrimitive( GFXTriangleStrip, 0, 2 );

   clearBlendStatus(mDevice);
}

void GFXDrawUtil::drawBitmapStretchSR( GFXTextureObject *texture, const RectF &dstRect, const RectF &srcRect, const GFXBitmapFlip in_flip ) 
{
	// Sanity if no texture is specified.
	if(!texture)
		return;

	mDevice->setBaseDrawState();

	GFXVertexBufferHandle<GFXVertexPCT> verts(mDevice, 4, GFXBufferTypeVolatile );
	verts.lock();

	F32 texLeft   = F32(srcRect.point.x)                    / F32(texture->mTextureSize.x);
	F32 texRight  = F32(srcRect.point.x + srcRect.extent.x) / F32(texture->mTextureSize.x);
	F32 texTop    = F32(srcRect.point.y)                    / F32(texture->mTextureSize.y);
	F32 texBottom = F32(srcRect.point.y + srcRect.extent.y) / F32(texture->mTextureSize.y);

	F32 screenLeft   = (F32)dstRect.point.x;
	F32 screenRight  = (F32)(dstRect.point.x + dstRect.extent.x);
	F32 screenTop    = (F32)dstRect.point.y;
	F32 screenBottom = (F32)(dstRect.point.y + dstRect.extent.y);

	if( in_flip & GFXBitmapFlip_X ) 
	{
		F32 temp = texLeft;
		texLeft = texRight;
		texRight = temp;
	}
	if( in_flip & GFXBitmapFlip_Y ) 
	{
		F32 temp = texTop;
		texTop = texBottom;
		texBottom = temp;
	}

	const F32 fillConv = mDevice->getFillConventionOffset();
	verts[0].point.set( screenLeft  - fillConv, screenTop    - fillConv, z );
	verts[1].point.set( screenRight - fillConv, screenTop    - fillConv, z );
	verts[2].point.set( screenLeft  - fillConv, screenBottom - fillConv, z );
	verts[3].point.set( screenRight - fillConv, screenBottom - fillConv, z );

	verts[0].color = verts[1].color = verts[2].color = verts[3].color = mBitmapModulation;

	verts[0].texCoord.set( texLeft,  texTop );
	verts[1].texCoord.set( texRight, texTop );
	verts[2].texCoord.set( texLeft,  texBottom );
	verts[3].texCoord.set( texRight, texBottom );

	verts.unlock();

	mDevice->setVertexBuffer( verts );
#ifdef STATEBLOCK
	AssertFatal(mSetBitmapSB, "GFXDrawUtil::drawBitmapStretchSR -- mSetBitmapSB cannot be NULL.");
	mSetBitmapSB->apply();
	setBlendStatus(mDevice);
#else
	mDevice->setCullMode( GFXCullNone );
	mDevice->setLightingEnable( false );
	mDevice->setAlphaBlendEnable( true );
	setBlendStatus(mDevice);
	mDevice->setTextureStageColorOp( 0, GFXTOPModulate );
	mDevice->setTextureStageColorOp( 1, GFXTOPDisable );  
#endif
	mDevice->setTexture( 0, texture );
	mDevice->setupGenericShaders( GFXDevice::GSModColorTexture );

	mDevice->drawPrimitive( GFXTriangleStrip, 0, 2 );
#ifdef STATEBLOCK
	GFX->setTextureStageState(0, GFXTSSColorOp, GFXTOPModulate);
#endif
	clearBlendStatus(mDevice);
}

void GFXDrawUtil::drawRectFill( const Point2I &a, const Point2I &b, const ColorI &color ) 
{
   mDevice->setBaseDrawState();

   //
   // Convert Box   a----------x
   //               |          |
   //               x----------b
   // Into Quad
   //               v0---------v1
   //               | a       x |
   //					  |           |
   //               | x       b |
   //               v2---------v3
   //

   // NorthWest and NorthEast facing offset vectors
   Point2F nw(-0.5,-0.5); /*  \  */
   Point2F ne(0.5,-0.5); /*  /  */

   GFXVertexBufferHandle<GFXVertexPC> verts(mDevice, 4, GFXBufferTypeVolatile);
   verts.lock();

   verts[0].point.set( a.x+nw.x, a.y+nw.y, z );
   verts[1].point.set( b.x+ne.x, a.y+ne.y, z );
   verts[2].point.set( a.x-ne.x, b.y-ne.y, z );
   verts[3].point.set( b.x-nw.x, b.y-nw.y, z );

   for (int i=0; i<4; i++)
      verts[i].color = color;

   verts.unlock();
#ifdef STATEBLOCK
	AssertFatal(mSetRectSB, "GFXDrawUtil::drawRectFill -- mSetRectSB cannot be NULL.");
	mSetRectSB->apply();
	setBlendStatus(mDevice);
#else
   mDevice->setCullMode( GFXCullNone );
   mDevice->setAlphaBlendEnable( true );
   mDevice->setLightingEnable( false );
   setBlendStatus(mDevice);
   mDevice->setTextureStageColorOp( 0, GFXTOPDisable );
#endif

   mDevice->setVertexBuffer( verts );
   mDevice->setupGenericShaders();

   mDevice->drawPrimitive( GFXTriangleStrip, 0, 2 );
   clearBlendStatus(mDevice);
}

void GFXDrawUtil::drawRectFill( const RectI &rect, const ColorI &color )
{
   drawRectFill( rect.point, 
      Point2I(rect.extent.x + rect.point.x, rect.extent.y + rect.point.y),
      color );
}
void GFXDrawUtil::drawRect( const Point2I &a, const Point2I &b, const ColorI &color ) 
{
   mDevice->setBaseDrawState();

   //
   // Convert Box   a----------x
   //               |          |
   //               x----------b
   //
   // Into tri-Strip Outline
   //               v0-----------v2
   //               | a         x |
   //					  |  v1-----v3  |
   //               |   |     |   |
   //               |  v7-----v5  |
   //               | x         b |
   //               v6-----------v4
   //

   // NorthWest and NorthEast facing offset vectors
   // These adjust the thickness of the line, it'd be neat if one day
   // they were passed in as arguments.
   Point2F nw(-0.5,-0.5); /*  \  */
   Point2F ne(0.5,-0.5); /*  /  */

   GFXVertexBufferHandle<GFXVertexPC> verts (mDevice, 10, GFXBufferTypeVolatile );
   verts.lock();

   verts[0].point.set( a.x+nw.x, a.y+nw.y, z );
   verts[1].point.set( a.x-nw.x, a.y-nw.y, z );
   verts[2].point.set( b.x+ne.x, a.y+ne.y, z );
   verts[3].point.set( b.x-ne.x, a.y-ne.y, z );
   verts[4].point.set( b.x-nw.x, b.y-nw.y, z );
   verts[5].point.set( b.x+nw.x, b.y+nw.y, z );
   verts[6].point.set( a.x-ne.x, b.y-ne.y, z );
   verts[7].point.set( a.x+ne.x, b.y+ne.y, z );
   verts[8].point.set( a.x+nw.x, a.y+nw.y, z ); // same as 0
   verts[9].point.set( a.x-nw.x, a.y-nw.y, z ); // same as 1

   for (int i=0; i<10; i++)
      verts[i].color = color;

   verts.unlock();
   mDevice->setVertexBuffer( verts );

#ifdef STATEBLOCK
	AssertFatal(mSetRectSB, "GFXDrawUtil::drawRect -- mSetRectSB cannot be NULL.");
	mSetRectSB->apply();
	setBlendStatus(mDevice);
#else
   mDevice->setCullMode( GFXCullNone );
   mDevice->setAlphaBlendEnable( true );
   mDevice->setLightingEnable( false );
   setBlendStatus(mDevice);
   mDevice->setTextureStageColorOp( 0, GFXTOPDisable );
#endif
   mDevice->setTexture(0, NULL);
   mDevice->setupGenericShaders();

   mDevice->drawPrimitive( GFXTriangleStrip, 0, 8 );
   clearBlendStatus(mDevice);
}

void GFXDrawUtil::drawRect( const RectI &rect, const ColorI &color )
{
   drawRect( rect.point, 
      Point2I(rect.extent.x + rect.point.x, rect.extent.y + rect.point.y), 
      color );
}
void GFXDrawUtil::draw2DSquare( const Point2F &screenPoint, F32 width, F32 spinAngle )
{
   mDevice->setBaseDrawState();

   width *= 0.5;

   Point3F offset( screenPoint.x, screenPoint.y, 0.0 );

   GFXVertexBufferHandle<GFXVertexPC> verts( mDevice, 4, GFXBufferTypeVolatile );
   verts.lock();

   verts[0].point.set( -width, -width, z );
   verts[1].point.set( -width, width,  z );
   verts[2].point.set( width,  -width, z );
   verts[3].point.set( width,  width,  z );

   verts[0].color = verts[1].color = verts[2].color = verts[3].color = mBitmapModulation;

   if(spinAngle != 0.f)
   {
      MatrixF rotMatrix( EulerF( 0.0, 0.0, spinAngle ) );

      for( S32 i = 0; i < 4; i++ )
      {
         rotMatrix.mulP( verts[i].point );
         verts[i].point += offset;
      }
   }

   verts.unlock();
   mDevice->setVertexBuffer( verts );
#ifdef STATEBLOCK
	AssertFatal(mSetRectSB, "GFXDrawUtil::draw2DSquare -- mSetRectSB cannot be NULL.");
	mSetRectSB->apply();
	setBlendStatus(mDevice);
#else
   mDevice->setCullMode( GFXCullNone );
   mDevice->setAlphaBlendEnable( true );
   mDevice->setLightingEnable( false );
   setBlendStatus(mDevice);
   mDevice->setTextureStageColorOp( 0, GFXTOPDisable );
#endif
   mDevice->setupGenericShaders();

   mDevice->drawPrimitive( GFXTriangleStrip, 0, 2 );
   clearBlendStatus(mDevice);
}


void GFXDrawUtil::drawLine( S32 x1, S32 y1, S32 x2, S32 y2, const ColorI &color ) 
{
   mDevice->setBaseDrawState();

   //
   // Convert Line   a----------b
   //
   // Into Quad      v0---------v1
   //                 a         b
   //                v2---------v3
   //

   Point2F start((F32)x1, (F32)y1);
   Point2F end((F32)x2, (F32)y2);
   Point2F perp, lineVec;

   // handle degenerate case where point a = b
   if(x1 == x2 && y1 == y2)
   {
      perp.set(0.0f, 0.5f);
      lineVec.set(0.1f, 0.0f);
   }
   else
   {
      perp.set(start.y - end.y, end.x - start.x);
      lineVec.set(end.x - start.x, end.y - start.y);
      perp.normalize(0.5f);
      lineVec.normalize(0.1f);
   }
   start -= lineVec;
   end   += lineVec;

   GFXVertexBufferHandle<GFXVertexPC> verts(mDevice, 4, GFXBufferTypeVolatile);
   verts.lock();

   verts[0].point.set( start.x+perp.x, start.y+perp.y, z );
   verts[1].point.set( end.x+perp.x, end.y+perp.y, z );
   verts[2].point.set( start.x-perp.x, start.y-perp.y, z );
   verts[3].point.set( end.x-perp.x, end.y-perp.y, z );

   verts[0].color = color;
   verts[1].color = color;
   verts[2].color = color;
   verts[3].color = color;

   verts.unlock();
   mDevice->setVertexBuffer( verts );
#ifdef STATEBLOCK
	AssertFatal(mSetRectSB, "GFXDrawUtil::drawLine -- mSetRectSB cannot be NULL.");
	mSetRectSB->apply();
	setBlendStatus(mDevice);
#else
   mDevice->setCullMode( GFXCullNone );
   mDevice->setAlphaBlendEnable( true );
   mDevice->setLightingEnable( false );
   setBlendStatus(mDevice);
   mDevice->setTextureStageColorOp( 0, GFXTOPDisable );
#endif
   mDevice->setupGenericShaders();

   mDevice->drawPrimitive( GFXTriangleStrip, 0, 2 );
   clearBlendStatus(mDevice);
}

void GFXDrawUtil::drawLine( const Point2I &startPt, const Point2I &endPt, const ColorI &color )
{
   drawLine( startPt.x, startPt.y, endPt.x, endPt.y, color );
}

//-----------------------------------------------------------------------------
static const Point3F cubePoints[8] = {
   Point3F(-1, -1, -1), Point3F(-1, -1,  1), Point3F(-1,  1, -1), Point3F(-1,  1,  1),
      Point3F( 1, -1, -1), Point3F( 1, -1,  1), Point3F( 1,  1, -1), Point3F( 1,  1,  1)
};

static const U32 cubeFaces[6][4] = {
   { 0, 2, 6, 4 }, { 0, 2, 3, 1 }, { 0, 1, 5, 4 },
   { 3, 2, 6, 7 }, { 7, 6, 4, 5 }, { 3, 7, 5, 1 }
};

//Ray: BOX的三角形列表顶点索引
static const U16 cubeIndices[] = {0,2,6, 0,6,4, 0,4,5, 0,5,1, 0,1,3, 0,3,2, 7,3,1, 7,1,5, 7,5,4, 7,4,6, 7,3,2, 7,2,6};

void GFXDrawUtil::drawWireCube( const Point3F &size, const Point3F &pos, const ColorI &color )
{
   mDevice->setBaseDrawState();

   GFXVertexBufferHandle<GFXVertexPC> verts(mDevice, 30, GFXBufferTypeVolatile);
   verts.lock();

   // setup 6 line loops
   U32 vertexIndex = 0;
   for(int i = 0; i < 6; i++)
   {
      for(int j = 0; j < 5; j++)
      {
         int idx = cubeFaces[i][j%4];

         verts[vertexIndex].point = cubePoints[idx] * size + pos;
         verts[vertexIndex].color = color;
         vertexIndex++;
      }
   }

   verts.unlock();
#ifdef STATEBLOCK
	AssertFatal(mCubeSB, "GFXDrawUtil::drawWireCube -- mCubeSB cannot be NULL.");
	mCubeSB->apply();
#else
   mDevice->setCullMode( GFXCullNone );
   mDevice->setAlphaBlendEnable( false );
   mDevice->setTextureStageColorOp( 0, GFXTOPDisable );
#endif

   mDevice->setVertexBuffer( verts );
   mDevice->setupGenericShaders();

   for( U32 i=0; i<6; i++ )
   {
      mDevice->drawPrimitive( GFXLineStrip, i*5, 4 );
   }

}

void GFXDrawUtil::buildCube( const Point3F &size, const Point3F &pos,GFXVertexBufferHandle<GFXVertexP> *pVB)
{
	if(initCubeFlag)
	{
		int IndicesSize = sizeof(cubeIndices)>>1;
		mCubePB.set(GFX,IndicesSize,1,GFXBufferTypeStatic);
		U16 *ibIndices = NULL;
		GFXPrimitive *piInput = NULL;
		mCubePB.lock(&ibIndices, &piInput);

		GFXPrimitive pInfo;
		pInfo.type = GFXTriangleList;
		pInfo.numPrimitives = IndicesSize / 3;
		pInfo.startIndex = 0;
		pInfo.minIndex = 0;
		pInfo.numVertices = 8;

		dMemcpy( ibIndices, cubeIndices, sizeof(cubeIndices) );
		dMemcpy( piInput, &pInfo, sizeof(GFXPrimitive) );

		mCubePB.unlock();
		initCubeFlag = false;
	}

	GFXVertexBufferHandle<GFXVertexP> &vb = *pVB;
	if( vb == NULL )
	{
		vb.set(GFX, 8, GFXBufferTypeStatic);
	}

	GFXVertexP *vbVerts = vb.lock();

	for(int i = 0; i < 8; i++)
	{
		vbVerts[i].point = cubePoints[i] * size + pos;
	}

	vb.unlock();
}

void GFXDrawUtil::drawCube()
{
	if(!initCubeFlag)
	{
		GFX->setPrimitiveBuffer(mCubePB);
		GFX->drawPrimitives();
	}
}

#ifdef STATEBLOCK
void GFXDrawUtil::clearBlendStatus(GFXDevice * device)
{
	switch(GFXDrawUtil::blendType)
	{
	case BlendSrcOne:
		AssertFatal(mClearAlphaBlendSB, "GFXDrawUtil::clearBlendStatus -- mClearAlphaBlendSB cannot be NULL.");
		mClearAlphaBlendSB->apply();
		mBitmapModulation = mBitmapModulationBak;
		break;
	case SeparateAlphaBlend:
		AssertFatal(mClearSepAlphaBlendSB, "GFXDrawUtil::clearBlendStatus -- mClearSepAlphaBlendSB cannot be NULL.");
		mClearSepAlphaBlendSB->apply();
		break;
	}
}
#else
void GFXDrawUtil::clearBlendStatus(GFXDevice * device)
{
	switch(GFXDrawUtil::blendType)
	{
	case BlendSrcOne:
		device->setTextureStageAlphaOp( 0, GFXTOPModulate );
		device->setAlphaRef(0);
		device->setAlphaTestEnable(false);
		mBitmapModulation = mBitmapModulationBak;
		break;
	case SeparateAlphaBlend:
		device->setSeparateAlphaBlendEnable( false );
		break;
	}
}
#endif

void GFXDrawUtil::resetStateBlock()
{
	//mSetBitmapSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);
	GFX->setRenderState(GFXRSLighting, false);
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setTextureStageState(0, GFXTSSColorOp, GFXTOPModulate);
	GFX->setTextureStageState(1, GFXTSSColorOp, GFXTOPDisable);
	GFX->endStateBlock(mSetBitmapSB);

	//mSetRectSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);
	GFX->setRenderState(GFXRSLighting, false);
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setTextureStageState(0, GFXTSSColorOp, GFXTOPDisable);
	GFX->endStateBlock(mSetRectSB);

	//mCubeSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);
	GFX->setRenderState(GFXRSAlphaBlendEnable, false);
	GFX->setTextureStageState(0, GFXTSSColorOp, GFXTOPDisable);
	GFX->endStateBlock(mCubeSB);

	//mAlphaBlendSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendOne);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendZero);
	GFX->setRenderState(GFXRSAlphaRef, 130);
	GFX->setRenderState(GFXRSAlphaFunc, GFXCmpGreater);
	GFX->setRenderState(GFXRSAlphaTestEnable, true);
	GFX->setTextureStageState(0, GFXTSSAlphaOp, GFXTOPAdd);
	GFX->endStateBlock(mAlphaBlendSB);

	//mSepAlphaBlendSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSSeparateAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcAlpha);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendInvSrcAlpha);
	GFX->setRenderState(GFXRSSrcBlendAlpha, GFXBlendSrcAlpha);
	GFX->setRenderState(GFXRSDestBlendAlpha, GFXBlendDestAlpha);
	GFX->endStateBlock(mSepAlphaBlendSB);

	//mLerpAlphaBlendSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcAlpha);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendInvSrcAlpha);
	GFX->endStateBlock(mLerpAlphaBlendSB);

	//mClearAlphaBlendSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaRef, 0);
	GFX->setTextureStageState(0, GFXTSSAlphaOp, GFXTOPModulate);
	GFX->setRenderState(GFXRSAlphaTestEnable, false);
	GFX->endStateBlock(mClearAlphaBlendSB);

	//mClearSepAlphaBlendSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSSeparateAlphaBlendEnable, false);
	GFX->endStateBlock(mClearSepAlphaBlendSB);
}
void GFXDrawUtil::releaseStateBlock()
{
	if (mSetBitmapSB)
	{
		mSetBitmapSB->release();
	}
	if (mSetRectSB)
	{
		mSetRectSB->release();
	}
	if (mCubeSB)
	{
		mCubeSB->release();
	}

	if (mAlphaBlendSB)
	{
		mAlphaBlendSB->release();
	}

	if (mSepAlphaBlendSB)
	{
		mSepAlphaBlendSB->release();
	}
	if (mLerpAlphaBlendSB)
	{
		mLerpAlphaBlendSB->release();
	}

	if (mClearAlphaBlendSB)
	{
		mClearAlphaBlendSB->release();
	}

	if (mClearSepAlphaBlendSB)
	{
		mClearSepAlphaBlendSB->release();
	}
}
void GFXDrawUtil::init()
{
	if (mSetBitmapSB == NULL)
	{
		mSetBitmapSB = new GFXD3D9StateBlock;
		mSetBitmapSB->registerResourceWithDevice(GFX);
		mSetBitmapSB->mZombify = &releaseStateBlock;
		mSetBitmapSB->mResurrect = &resetStateBlock;

		mSetRectSB = new GFXD3D9StateBlock;
		mCubeSB = new GFXD3D9StateBlock;
		mAlphaBlendSB = new GFXD3D9StateBlock;
		mSepAlphaBlendSB = new GFXD3D9StateBlock;
		mLerpAlphaBlendSB = new GFXD3D9StateBlock;
		mClearAlphaBlendSB = new GFXD3D9StateBlock;
		mClearSepAlphaBlendSB = new GFXD3D9StateBlock;
		resetStateBlock();
	}
}
void GFXDrawUtil::shutdown()
{
	SAFE_DELETE(mSetBitmapSB);
	SAFE_DELETE(mSetRectSB);
	SAFE_DELETE(mCubeSB);
	SAFE_DELETE(mAlphaBlendSB);
	SAFE_DELETE(mSepAlphaBlendSB);
	SAFE_DELETE(mLerpAlphaBlendSB);
	SAFE_DELETE(mClearAlphaBlendSB);
	SAFE_DELETE(mClearSepAlphaBlendSB);
}

void GFXDrawUtil::lockAlpha( int alpha )
{
	mIsAlphaLocked = true;
	mLockedAlpha = alpha;
}

void GFXDrawUtil::unlockAlpha()
{
	mIsAlphaLocked = false;
	mLockedAlpha = 255;
}

bool GFXDrawUtil::isAlphaLocked()
{
	return mIsAlphaLocked;
}

int GFXDrawUtil::getLockedAlpha()
{
	return mLockedAlpha;
}