//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "gfx/gfxDevice.h"

//-----------------------------------------------------------------------------
// GFXShaderFeatureData
//-----------------------------------------------------------------------------
GFXShaderFeatureData::GFXShaderFeatureData()
{
   dMemset( this, 0, sizeof( GFXShaderFeatureData ) );
}

//-----------------------------------------------------------------------------
U32 GFXShaderFeatureData::codify()
{
   U32 result = 0;
   U32 multiplier = 1;

   for( U32 i=0; i<NumFeatures; i++ )
   {
      result += features[i] * multiplier;
      multiplier <<= 1;
   }
   
   return result;
}

//------------------------------------------------------------------------

void GFXDebugMarker::enter()
{
   AssertWarn(!mActive, "Entered already-active debug marker!");
   GFX->enterDebugEvent(mColor, mName);
   mActive = true;
}

void GFXDebugMarker::leave()
{
   AssertWarn(mActive, "Left inactive debug marker!");
   GFX->leaveDebugEvent();
   mActive = false;
}

//-----------------------------------------------------------------------------

GFXVideoMode::GFXVideoMode()
{
   bitDepth = 32;
   fullScreen = false;
   fullWindow = false;
   refreshRate = 60;
   wideScreen = false;
   resolution.set(800,600);
   antialiasLevel = 0;
}

void GFXVideoMode::parseFromString( const char *str )
{
   if(!str)
      return;

   S32 length = dStrlen( str ) + 1;
   // Copy the string, as dStrtok is destructive
   char *tempBuf = new char[length];
   dStrcpy( tempBuf, length, str );

#define PARSE_ELEM(type, var, func, tokParam, sep, context) \
   if(const char *ptr = dStrtok( tokParam, sep, &context)) \
   { type tmp = func(ptr); if(tmp > 0) var = tmp; }

   char * context = NULL;

   PARSE_ELEM(S32, resolution.x, dAtoi, tempBuf, " x\0", context)
   PARSE_ELEM(S32, resolution.y, dAtoi, NULL,    " x\0", context)
   PARSE_ELEM(S32, fullScreen,   dAtob, NULL,    " \0", context)
   PARSE_ELEM(S32, fullWindow,   dAtob, NULL,    " \0", context)
   PARSE_ELEM(S32, bitDepth,     dAtoi, NULL,    " \0", context)
   PARSE_ELEM(S32, refreshRate,  dAtoi, NULL,    " \0", context)
   PARSE_ELEM(S32, antialiasLevel, dAtoi, NULL,    " \0", context)

#undef PARSE_ELEM

   delete [] tempBuf;
}

const char * GFXVideoMode::toString()
{
   return avar("%d %d %s %s %d %d %d", resolution.x, resolution.y, (fullScreen ? "true" : "false"), (fullWindow ? "true" : "false"),bitDepth,  refreshRate, antialiasLevel);
}
