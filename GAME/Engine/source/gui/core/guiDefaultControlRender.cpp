//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "guiDefaultControlRender.h"
#include "gui/core/guiDefaultControlRender.h"
#include "gui/core/guiTypes.h"
#include "core/color.h"
#include "math/mRect.h"
#include "gfx/gfxDevice.h"
#include "gfx/gfxDrawUtil.h"

static ColorI colorLightGray(192, 192, 192);
static ColorI colorGray(128, 128, 128);
static ColorI colorDarkGray(64, 64, 64);
static ColorI colorWhite(255,255,255);
static ColorI colorBlack(0,0,0);

void renderRaisedBox(RectI &bounds, GuiControlProfile *profile)
{
   S32 l = bounds.point.x, r = bounds.point.x + bounds.extent.x - 1;
   S32 t = bounds.point.y, b = bounds.point.y + bounds.extent.y - 1;

   GFX->getDrawUtil()->drawRectFill( bounds, profile->mFillColor);
   GFX->getDrawUtil()->drawLine(l, t, l, b - 1, colorWhite);
   GFX->getDrawUtil()->drawLine(l, t, r - 1, t, colorWhite);

   GFX->getDrawUtil()->drawLine(l, b, r, b, colorBlack);
   GFX->getDrawUtil()->drawLine(r, b - 1, r, t, colorBlack);

   GFX->getDrawUtil()->drawLine(l + 1, b - 1, r - 1, b - 1, profile->mBorderColor);
   GFX->getDrawUtil()->drawLine(r - 1, b - 2, r - 1, t + 1, profile->mBorderColor);
}

void renderSlightlyRaisedBox(RectI &bounds, GuiControlProfile *profile)
{
   S32 l = bounds.point.x, r = bounds.point.x + bounds.extent.x - 1;
   S32 t = bounds.point.y, b = bounds.point.y + bounds.extent.y - 1;

   GFXDrawUtil *drawer = GFX->getDrawUtil();
   drawer->drawRectFill( bounds, profile->mFillColor);
   drawer->drawLine(l, t, l, b, colorWhite);
   drawer->drawLine(l, t, r, t, colorWhite);
   drawer->drawLine(l + 1, b, r, b, profile->mBorderColor);
   drawer->drawLine(r, t + 1, r, b - 1, profile->mBorderColor);
}

void renderLoweredBox(RectI &bounds, GuiControlProfile *profile)
{
   S32 l = bounds.point.x, r = bounds.point.x + bounds.extent.x - 1;
   S32 t = bounds.point.y, b = bounds.point.y + bounds.extent.y - 1;

   GFX->getDrawUtil()->drawRectFill( bounds, profile->mFillColor);

   GFX->getDrawUtil()->drawLine(l, b, r, b, colorWhite);
   GFX->getDrawUtil()->drawLine(r, b - 1, r, t, colorWhite);

   GFX->getDrawUtil()->drawLine(l, t, r - 1, t, colorBlack);
   GFX->getDrawUtil()->drawLine(l, t + 1, l, b - 1, colorBlack);

   GFX->getDrawUtil()->drawLine(l + 1, t + 1, r - 2, t + 1, profile->mBorderColor);
   GFX->getDrawUtil()->drawLine(l + 1, t + 2, l + 1, b - 2, profile->mBorderColor);
}

void renderSlightlyLoweredBox(RectI &bounds, GuiControlProfile *profile)
{
   S32 l = bounds.point.x, r = bounds.point.x + bounds.extent.x - 1;
   S32 t = bounds.point.y, b = bounds.point.y + bounds.extent.y - 1;

   GFX->getDrawUtil()->drawRectFill( bounds, profile->mFillColor);
   GFX->getDrawUtil()->drawLine(l, b, r, b, colorWhite);
   GFX->getDrawUtil()->drawLine(r, t, r, b - 1, colorWhite);
   GFX->getDrawUtil()->drawLine(l, t, l, b - 1, profile->mBorderColor);
   GFX->getDrawUtil()->drawLine(l + 1, t, r - 1, t, profile->mBorderColor);
}

void renderBorder(RectI &bounds, GuiControlProfile *profile)
{
   S32 l = bounds.point.x, r = bounds.point.x + bounds.extent.x - 1;
   S32 t = bounds.point.y, b = bounds.point.y + bounds.extent.y - 1;

   GFXDrawUtil *drawer = GFX->getDrawUtil();

   switch(profile->mBorder)
   {
   case 1:
      drawer->drawRect(bounds, profile->mBorderColor);
      break;
   case 2:
      drawer->drawLine(l + 1, t + 1, l + 1, b - 2, profile->mBevelColorHL);
      drawer->drawLine(l + 2, t + 1, r - 2, t + 1, profile->mBevelColorHL);
      drawer->drawLine(r, t, r, b, profile->mBevelColorHL);
      drawer->drawLine(l, b, r - 1, b, profile->mBevelColorHL);
      drawer->drawLine(l, t, r - 1, t, profile->mBorderColorNA);
      drawer->drawLine(l, t + 1, l, b - 1, profile->mBorderColorNA);
      drawer->drawLine(l + 1, b - 1, r - 1, b - 1, profile->mBorderColorNA);
      drawer->drawLine(r - 1, t + 1, r - 1, b - 2, profile->mBorderColorNA);
      break;
   case 3:
      drawer->drawLine(l, b, r, b, profile->mBevelColorHL);
      drawer->drawLine(r, t, r, b - 1, profile->mBevelColorHL);
      drawer->drawLine(l + 1, b - 1, r - 1, b - 1, profile->mFillColor);
      drawer->drawLine(r - 1, t + 1, r - 1, b - 2, profile->mFillColor);
      drawer->drawLine(l, t, l, b - 1, profile->mBorderColorNA);
      drawer->drawLine(l + 1, t, r - 1, t, profile->mBorderColorNA);
      drawer->drawLine(l + 1, t + 1, l + 1, b - 2, profile->mBevelColorLL);
      drawer->drawLine(l + 2, t + 1, r - 2, t + 1, profile->mBevelColorLL);
      break;
   case 4:
      drawer->drawLine(l, t, l, b - 1, profile->mBevelColorHL);
      drawer->drawLine(l + 1, t, r, t, profile->mBevelColorHL);
      drawer->drawLine(l, b, r, b, profile->mBevelColorLL);
      drawer->drawLine(r, t + 1, r, b - 1, profile->mBevelColorLL);
      drawer->drawLine(l + 1, b - 1, r - 1, b - 1, profile->mBorderColor);
      drawer->drawLine(r - 1, t + 1, r - 1, b - 2, profile->mBorderColor);
      break;
   case 5:
      renderFilledBorder( bounds, profile );
      break;
      // 
   case -1:
      // Draw a simple sizable border with corners
      // Taken from the 'Skinnable GUI Controls in TGE' resource by Justin DuJardin       
      if(profile->mBitmapArrayRects.size() >= 8)
      {
         drawer->clearBitmapModulation();

         RectI destRect;
         RectI stretchRect;
		 RectI fillRect;
         RectI* mBitmapBounds = profile->mBitmapArrayRects.address();

         //  Indices into the bitmap array
         enum
         {
            BorderTopLeft = 0,
            BorderTop,
            BorderTopRight,
            BorderLeft,
            //Fill,
            BorderRight,
            BorderBottomLeft,
            BorderBottom,
            BorderBottomRight,
            NumBitmaps
         };

         // Draw all corners first.

		 // Draw Fill Rect //<Edit>:[thinking]
		 fillRect.point.x = bounds.point.x + mBitmapBounds[BorderLeft].extent.x;
		 fillRect.point.y = bounds.point.y + mBitmapBounds[BorderLeft].point.y-2;
		 fillRect.extent.x = bounds.extent.x - mBitmapBounds[BorderLeft].extent.x - mBitmapBounds[BorderRight].extent.x-1;
		 fillRect.extent.y = bounds.extent.y - mBitmapBounds[BorderTop].extent.y - mBitmapBounds[BorderBottom].extent.y-1;
		 ColorI col;
		 col = profile->mFillColor;
		 if( GFX->getDrawUtil()->isAlphaLocked() )
			 col.alpha *= GFX->getDrawUtil()->getLockedAlpha() / 255.0f;

		 drawer->drawRectFill(fillRect, col);
		 drawer->clearBitmapModulation();

         //top left border
         drawer->drawBitmapSR(profile->mTextureObject,Point2I(bounds.point.x,bounds.point.y),mBitmapBounds[BorderTopLeft]);
         //top right border
         drawer->drawBitmapSR(profile->mTextureObject,Point2I(bounds.point.x + bounds.extent.x - mBitmapBounds[BorderTopRight].extent.x,bounds.point.y),mBitmapBounds[BorderTopRight]);

         //bottom left border
         drawer->drawBitmapSR(profile->mTextureObject,Point2I(bounds.point.x,bounds.point.y + bounds.extent.y - mBitmapBounds[BorderBottomLeft].extent.y),mBitmapBounds[BorderBottomLeft]);
         //bottom right border
         drawer->drawBitmapSR(profile->mTextureObject,Point2I(
            bounds.point.x + bounds.extent.x - mBitmapBounds[BorderBottomRight].extent.x,
            bounds.point.y + bounds.extent.y - mBitmapBounds[BorderBottomRight].extent.y),
            mBitmapBounds[BorderBottomRight]);

         // End drawing corners

         // Begin drawing sides and top stretched borders

         //start with top line stretch
         destRect.point.x = bounds.point.x + mBitmapBounds[BorderTopRight].extent.x;
         destRect.extent.x = bounds.extent.x - mBitmapBounds[BorderTopRight].extent.x - mBitmapBounds[BorderTopLeft].extent.x;
         destRect.extent.y = mBitmapBounds[BorderTop].extent.y;
         destRect.point.y = bounds.point.y;
         //stretch it
         stretchRect = mBitmapBounds[BorderTop];
         stretchRect.inset(1,0);
         //draw it
         drawer->drawBitmapStretchSR(profile->mTextureObject,destRect,stretchRect);
         //bottom line stretch
         destRect.point.x = bounds.point.x + mBitmapBounds[BorderBottomRight].extent.x;
         destRect.extent.x = bounds.extent.x - mBitmapBounds[BorderBottomRight].extent.x - mBitmapBounds[BorderBottomLeft].extent.x;
         destRect.extent.y = mBitmapBounds[BorderBottom].extent.y;
         destRect.point.y = bounds.point.y + bounds.extent.y - mBitmapBounds[BorderBottom].extent.y;
         //stretch it
         stretchRect = mBitmapBounds[BorderBottom];
         stretchRect.inset(1,0);
         //draw it
         drawer->drawBitmapStretchSR(profile->mTextureObject,destRect,stretchRect);
         //left line stretch
         destRect.point.x = bounds.point.x;
         destRect.extent.x = mBitmapBounds[BorderLeft].extent.x;
         destRect.extent.y = bounds.extent.y - mBitmapBounds[BorderTopLeft].extent.y - mBitmapBounds[BorderBottomLeft].extent.y;
         destRect.point.y = bounds.point.y + mBitmapBounds[BorderTopLeft].extent.y;
         //stretch it
         stretchRect = mBitmapBounds[BorderLeft];
         stretchRect.inset(0,1);
         //draw it
         drawer->drawBitmapStretchSR(profile->mTextureObject,destRect,stretchRect);
         //left line stretch
         destRect.point.x = bounds.point.x + bounds.extent.x - mBitmapBounds[BorderRight].extent.x;
         destRect.extent.x = mBitmapBounds[BorderRight].extent.x;
         destRect.extent.y = bounds.extent.y - mBitmapBounds[BorderTopRight].extent.y - mBitmapBounds[BorderBottomRight].extent.y;
         destRect.point.y = bounds.point.y + mBitmapBounds[BorderTopRight].extent.y;
         //stretch it
         stretchRect = mBitmapBounds[BorderRight];
         stretchRect.inset(0,1);
         //draw it
         drawer->drawBitmapStretchSR(profile->mTextureObject,destRect,stretchRect);

         // End drawing sides and top stretched borders
         break;
      }
   case -2:
      // Draw a simple sizable border with corners that is filled in
      renderSizableBitmapBordersFilled(bounds, 1, profile);
      break;
   case -3:
      // Draw a simple fixed height border with center fill horizontally.
      renderFixedBitmapBordersFilled( bounds, 1, profile );
      break;

   }
}

void renderFilledBorder( RectI &bounds, GuiControlProfile *profile )
{
   renderFilledBorder( bounds, profile->mBorderColor, profile->mFillColor );
}

void renderFilledBorder( RectI &bounds, ColorI &borderColor, ColorI &fillColor )
{
   RectI fillBounds = bounds;
   fillBounds.inset( 1, 1 );

   GFX->getDrawUtil()->drawRect( bounds, borderColor ); 
   GFX->getDrawUtil()->drawRectFill( fillBounds, fillColor );
}

//  Render out the sizable bitmap borders based on a multiplier into the bitmap array
// Based on the 'Skinnable GUI Controls in TGE' resource by Justin DuJardin
void renderSizableBitmapBordersFilled(RectI &bounds, S32 baseMultiplier, GuiControlProfile *profile)
{
   //  Indices into the bitmap array
   S32 numBitmaps = 9;
   S32 borderTopLeft =     numBitmaps * baseMultiplier - numBitmaps;
   S32 borderTop =         1 + borderTopLeft;
   S32 borderTopRight =    2 + borderTopLeft;
   S32 borderLeft =        3 + borderTopLeft;
   S32 fill =              4 + borderTopLeft;
   S32 borderRight =       5 + borderTopLeft;
   S32 borderBottomLeft =  6 + borderTopLeft;
   S32 borderBottom =      7 + borderTopLeft;
   S32 borderBottomRight = 8 + borderTopLeft;

   GFXDrawUtil *drawer = GFX->getDrawUtil();

   drawer->clearBitmapModulation();

   if(profile->mBitmapArrayRects.size() >= (numBitmaps * baseMultiplier))
   {
      RectI destRect;
      RectI stretchRect;
      RectI* mBitmapBounds = profile->mBitmapArrayRects.address();

      // Draw all corners first.

      //top left border
      drawer->drawBitmapSR(profile->mTextureObject,Point2I(bounds.point.x,bounds.point.y),mBitmapBounds[borderTopLeft]);
      //top right border
      drawer->drawBitmapSR(profile->mTextureObject,Point2I(bounds.point.x + bounds.extent.x - mBitmapBounds[borderTopRight].extent.x,bounds.point.y),mBitmapBounds[borderTopRight]);

      //bottom left border
      drawer->drawBitmapSR(profile->mTextureObject,Point2I(bounds.point.x,bounds.point.y + bounds.extent.y - mBitmapBounds[borderBottomLeft].extent.y),mBitmapBounds[borderBottomLeft]);
      //bottom right border
      drawer->drawBitmapSR(profile->mTextureObject,Point2I(
         bounds.point.x + bounds.extent.x - mBitmapBounds[borderBottomRight].extent.x,
         bounds.point.y + bounds.extent.y - mBitmapBounds[borderBottomRight].extent.y),
         mBitmapBounds[borderBottomRight]);

      // End drawing corners

      // Begin drawing sides and top stretched borders
      #pragma message(ENGINE("stretchRect??????X,Y??????????1,mBitmapBounds????????????constructBitmapArray??????????????????"))
	  //??????stretchRect??X??Y????1????[27/02/2009 by thinking]
      //start with top line stretch
      destRect.point.x = bounds.point.x + mBitmapBounds[borderTopRight].extent.x;
      destRect.extent.x = bounds.extent.x - mBitmapBounds[borderTopRight].extent.x - mBitmapBounds[borderTopLeft].extent.x;
      destRect.extent.y = mBitmapBounds[borderTop].extent.y;
      destRect.point.y = bounds.point.y;
      //stretch it
      stretchRect = mBitmapBounds[borderTop];
      //stretchRect.inset(1,0);
      //draw it
      drawer->drawBitmapStretchSR(profile->mTextureObject,destRect,stretchRect);
      //bottom line stretch
      destRect.point.x = bounds.point.x + mBitmapBounds[borderBottomRight].extent.x;
      destRect.extent.x = bounds.extent.x - mBitmapBounds[borderBottomRight].extent.x - mBitmapBounds[borderBottomLeft].extent.x;
      destRect.extent.y = mBitmapBounds[borderBottom].extent.y;
      destRect.point.y = bounds.point.y + bounds.extent.y - mBitmapBounds[borderBottom].extent.y;
      //stretch it
      stretchRect = mBitmapBounds[borderBottom];
      //stretchRect.inset(1,0);
      //draw it
      drawer->drawBitmapStretchSR(profile->mTextureObject,destRect,stretchRect);
      //left line stretch
      destRect.point.x = bounds.point.x;
      destRect.extent.x = mBitmapBounds[borderLeft].extent.x;
      destRect.extent.y = bounds.extent.y - mBitmapBounds[borderTopLeft].extent.y - mBitmapBounds[borderBottomLeft].extent.y;
      destRect.point.y = bounds.point.y + mBitmapBounds[borderTopLeft].extent.y;
      //stretch it
      stretchRect = mBitmapBounds[borderLeft];
      //stretchRect.inset(0,1);
      //draw it
      drawer->drawBitmapStretchSR(profile->mTextureObject,destRect,stretchRect);
      //left line stretch
      destRect.point.x = bounds.point.x + bounds.extent.x - mBitmapBounds[borderRight].extent.x;
      destRect.extent.x = mBitmapBounds[borderRight].extent.x;
      destRect.extent.y = bounds.extent.y - mBitmapBounds[borderTopRight].extent.y - mBitmapBounds[borderBottomRight].extent.y;
      destRect.point.y = bounds.point.y + mBitmapBounds[borderTopRight].extent.y;
      //stretch it
      stretchRect = mBitmapBounds[borderRight];
      //stretchRect.inset(0,1);
      //draw it
      drawer->drawBitmapStretchSR(profile->mTextureObject,destRect,stretchRect);
      //fill stretch
      destRect.point.x = bounds.point.x + mBitmapBounds[borderLeft].extent.x;
      destRect.extent.x = (bounds.extent.x) - mBitmapBounds[borderLeft].extent.x - mBitmapBounds[borderRight].extent.x;
      destRect.extent.y = bounds.extent.y - mBitmapBounds[borderTop].extent.y - mBitmapBounds[borderBottom].extent.y;
      destRect.point.y = bounds.point.y + mBitmapBounds[borderTop].extent.y;
      //stretch it
      stretchRect = mBitmapBounds[fill];
      //stretchRect.inset(1,1);
      //draw it
      drawer->drawBitmapStretchSR(profile->mTextureObject,destRect,stretchRect);

      // End drawing sides and top stretched borders
   }
}


//  Render out the sizable bitmap borders based on a multiplier into the bitmap array
// Based on the 'Skinnable GUI Controls in TGE' resource by Justin DuJardin
void renderSizableBitmapBordersFilledIndex(RectI &bounds, S32 startIndex, GuiControlProfile *profile)
{
   //  Indices into the bitmap array
   S32 numBitmaps = 9;
   S32 borderTopLeft =     startIndex;
   S32 borderTop =         1 + borderTopLeft;
   S32 borderTopRight =    2 + borderTopLeft;
   S32 borderLeft =        3 + borderTopLeft;
   S32 fill =              4 + borderTopLeft;
   S32 borderRight =       5 + borderTopLeft;
   S32 borderBottomLeft =  6 + borderTopLeft;
   S32 borderBottom =      7 + borderTopLeft;
   S32 borderBottomRight = 8 + borderTopLeft;

   GFXDrawUtil *drawer = GFX->getDrawUtil();

   drawer->clearBitmapModulation();
   if(profile->mBitmapArrayRects.size() >= (startIndex + numBitmaps))
   {
      RectI destRect;
      RectI stretchRect;
      RectI* mBitmapBounds = profile->mBitmapArrayRects.address();

      // Draw all corners first.

      //top left border
      drawer->drawBitmapSR(profile->mTextureObject,Point2I(bounds.point.x,bounds.point.y),mBitmapBounds[borderTopLeft]);
      //top right border
      drawer->drawBitmapSR(profile->mTextureObject,Point2I(bounds.point.x + bounds.extent.x - mBitmapBounds[borderTopRight].extent.x,bounds.point.y),mBitmapBounds[borderTopRight]);

      //bottom left border
      drawer->drawBitmapSR(profile->mTextureObject,Point2I(bounds.point.x,bounds.point.y + bounds.extent.y - mBitmapBounds[borderBottomLeft].extent.y),mBitmapBounds[borderBottomLeft]);
      //bottom right border
      drawer->drawBitmapSR(profile->mTextureObject,Point2I(
         bounds.point.x + bounds.extent.x - mBitmapBounds[borderBottomRight].extent.x,
         bounds.point.y + bounds.extent.y - mBitmapBounds[borderBottomRight].extent.y),
         mBitmapBounds[borderBottomRight]);

      // End drawing corners

      // Begin drawing sides and top stretched borders

      //start with top line stretch
      destRect.point.x = bounds.point.x + mBitmapBounds[borderTopLeft].extent.x;
      destRect.extent.x = bounds.extent.x - mBitmapBounds[borderTopRight].extent.x - mBitmapBounds[borderTopLeft].extent.x;
      destRect.extent.y = mBitmapBounds[borderTop].extent.y;
      destRect.point.y = bounds.point.y;
      //stretch it
      stretchRect = mBitmapBounds[borderTop];
      stretchRect.inset(1,0);
      //draw it
      drawer->drawBitmapStretchSR(profile->mTextureObject,destRect,stretchRect);
      //bottom line stretch
      destRect.point.x = bounds.point.x + mBitmapBounds[borderBottomLeft].extent.x;
      destRect.extent.x = bounds.extent.x - mBitmapBounds[borderBottomRight].extent.x - mBitmapBounds[borderBottomLeft].extent.x;
      destRect.extent.y = mBitmapBounds[borderBottom].extent.y;
      destRect.point.y = bounds.point.y + bounds.extent.y - mBitmapBounds[borderBottom].extent.y;
      //stretch it
      stretchRect = mBitmapBounds[borderBottom];
      stretchRect.inset(1,0);
      //draw it
      drawer->drawBitmapStretchSR(profile->mTextureObject,destRect,stretchRect);
      //left line stretch
      destRect.point.x = bounds.point.x;
      destRect.extent.x = mBitmapBounds[borderLeft].extent.x;
      destRect.extent.y = bounds.extent.y - mBitmapBounds[borderTopLeft].extent.y - mBitmapBounds[borderBottomLeft].extent.y;
      destRect.point.y = bounds.point.y + mBitmapBounds[borderTopLeft].extent.y;
      //stretch it
      stretchRect = mBitmapBounds[borderLeft];
      stretchRect.inset(0,1);
      //draw it
      drawer->drawBitmapStretchSR(profile->mTextureObject,destRect,stretchRect);
      //left line stretch
      destRect.point.x = bounds.point.x + bounds.extent.x - mBitmapBounds[borderRight].extent.x;
      destRect.extent.x = mBitmapBounds[borderRight].extent.x;
      destRect.extent.y = bounds.extent.y - mBitmapBounds[borderTopRight].extent.y - mBitmapBounds[borderBottomRight].extent.y;
      destRect.point.y = bounds.point.y + mBitmapBounds[borderTopRight].extent.y;
      //stretch it
      stretchRect = mBitmapBounds[borderRight];
      stretchRect.inset(0,1);
      //draw it
      drawer->drawBitmapStretchSR(profile->mTextureObject,destRect,stretchRect);
      //fill stretch
      destRect.point.x = bounds.point.x + mBitmapBounds[borderLeft].extent.x;
      destRect.extent.x = (bounds.extent.x) - mBitmapBounds[borderLeft].extent.x - mBitmapBounds[borderRight].extent.x;
      destRect.extent.y = bounds.extent.y - mBitmapBounds[borderTop].extent.y - mBitmapBounds[borderBottom].extent.y;
      destRect.point.y = bounds.point.y + mBitmapBounds[borderTop].extent.y;
      //stretch it
      stretchRect = mBitmapBounds[fill];
      stretchRect.inset(1,1);
      //draw it
      drawer->drawBitmapStretchSR(profile->mTextureObject,destRect,stretchRect);

      // End drawing sides and top stretched borders
   }
}



//  Render out the fixed bitmap borders based on a multiplier into the bitmap array
// It renders left and right caps, with a sizable fill area in the middle to reach
// the x extent.  It does not stretch in the y direction.
void renderFixedBitmapBordersFilled(RectI &bounds, S32 baseMultiplier, GuiControlProfile *profile)
{
   //  Indices into the bitmap array
   S32 numBitmaps = 3;
   S32 borderLeft =     numBitmaps * baseMultiplier - numBitmaps;
   S32 fill =              1 + borderLeft;
   S32 borderRight =       2 + borderLeft;

   GFXDrawUtil *drawer = GFX->getDrawUtil();

   drawer->clearBitmapModulation();
   if(profile->mBitmapArrayRects.size() >= (numBitmaps * baseMultiplier))
   {
      RectI destRect;
      RectI stretchRect;
      RectI* mBitmapBounds = profile->mBitmapArrayRects.address();

      // Draw all corners first.

      //left border
      drawer->drawBitmapSR(profile->mTextureObject,Point2I(bounds.point.x,bounds.point.y),mBitmapBounds[borderLeft]);
      //right border
      drawer->drawBitmapSR(profile->mTextureObject,Point2I(bounds.point.x + bounds.extent.x - mBitmapBounds[borderRight].extent.x,bounds.point.y),mBitmapBounds[borderRight]);

      // End drawing corners

      // Begin drawing fill

      //fill stretch
      destRect.point.x = bounds.point.x + mBitmapBounds[borderLeft].extent.x;
      destRect.extent.x = (bounds.extent.x) - mBitmapBounds[borderLeft].extent.x - mBitmapBounds[borderRight].extent.x;
      destRect.extent.y = mBitmapBounds[fill].extent.y;
      destRect.point.y = bounds.point.y;
      //stretch it
      stretchRect = mBitmapBounds[fill];
      stretchRect.inset(1,0);
      //draw it
      drawer->drawBitmapStretchSR(profile->mTextureObject,destRect,stretchRect);

      // End drawing fill
   }
}

//  Render out the fixed bitmap borders based on a multiplier into the bitmap array
// It renders left and right caps, with a sizable fill area in the middle to reach
// the x extent.  It does not stretch in the y direction.
void renderFixedBitmapBordersFilledIndex(RectI &bounds, S32 startIndex, GuiControlProfile *profile)
{
   //  Indices into the bitmap array
   S32 numBitmaps = 3;
   S32 borderLeft =     startIndex;
   S32 fill =              1 + startIndex;
   S32 borderRight =       2 + startIndex;

   GFXDrawUtil *drawer = GFX->getDrawUtil();
   drawer->clearBitmapModulation();
   if(profile->mBitmapArrayRects.size() >= (startIndex + numBitmaps))
   {
      RectI destRect;
      RectI stretchRect;
      RectI* mBitmapBounds = profile->mBitmapArrayRects.address();

      // Draw all corners first.

      //left border
      drawer->drawBitmapSR(profile->mTextureObject,Point2I(bounds.point.x,bounds.point.y),mBitmapBounds[borderLeft]);
      //right border
      drawer->drawBitmapSR(profile->mTextureObject,Point2I(bounds.point.x + bounds.extent.x - mBitmapBounds[borderRight].extent.x,bounds.point.y),mBitmapBounds[borderRight]);

      // End drawing corners

      // Begin drawing fill

      //fill stretch
      destRect.point.x = bounds.point.x + mBitmapBounds[borderLeft].extent.x;
      destRect.extent.x = (bounds.extent.x) - mBitmapBounds[borderLeft].extent.x - mBitmapBounds[borderRight].extent.x;
      destRect.extent.y = mBitmapBounds[fill].extent.y;
      destRect.point.y = bounds.point.y;
      //stretch it
      stretchRect = mBitmapBounds[fill];
      stretchRect.inset(1,0);
      //draw it
      drawer->drawBitmapStretchSR(profile->mTextureObject,destRect,stretchRect);

      // End drawing fill
   }
}
