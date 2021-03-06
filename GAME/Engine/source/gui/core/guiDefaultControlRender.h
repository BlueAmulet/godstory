//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _H_GUIDEFAULTCONTROLRENDER_
#define _H_GUIDEFAULTCONTROLRENDER_

#include "math/mRect.h"
class GuiControlProfile;
class ColorI;

void renderRaisedBox(RectI &bounds, GuiControlProfile *profile);
void renderSlightlyRaisedBox(RectI &bounds, GuiControlProfile *profile);
void renderLoweredBox(RectI &bounds, GuiControlProfile *profile);
void renderSlightlyLoweredBox(RectI &bounds, GuiControlProfile *profile);
void renderBorder(RectI &bounds, GuiControlProfile *profile);
void renderFilledBorder( RectI &bounds, GuiControlProfile *profile );
void renderFilledBorder( RectI &bounds, ColorI &borderColor, ColorI &fillColor );
void renderSizableBitmapBordersFilled(RectI &bounds, S32 baseMultiplier, GuiControlProfile *profile); //  Added
void renderSizableBitmapBordersFilledIndex(RectI &bounds, S32 startIndex, GuiControlProfile *profile);
void renderFixedBitmapBordersFilled(RectI &bounds, S32 baseMultiplier, GuiControlProfile *profile); //  Added
void renderFixedBitmapBordersFilled(RectI &bounds, S32 startIndex, GuiControlProfile *profile);

#endif
