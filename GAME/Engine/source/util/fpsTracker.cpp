//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "util/fpsTracker.h"
#include "console/console.h"

FPSTracker gFPS;

FPSTracker::FPSTracker()
{
   mUpdateInterval = 0.25f;
   reset();
}

void FPSTracker::reset()
{
   fpsNext         = (F32)Platform::getRealMilliseconds()/1000.0f + mUpdateInterval;

   fpsRealLast       = 0.0f;
   fpsReal           = 0.0f;
   fpsVirtualLast    = 0.0f;
   fpsVirtual        = 0.0f;
   fpsFrames         = 0;
}

void FPSTracker::update()
{
   const float alpha  = 0.07f;
   F32 realSeconds    = (F32)Platform::getRealMilliseconds()/1000.0f;
   F32 virtualSeconds = (F32)Platform::getVirtualMilliseconds()/1000.0f;

   fpsFrames++;
   F32 dtReal = realSeconds-fpsRealLast;
   F32 dtVirtual = virtualSeconds-fpsVirtualLast;
   if ( dtReal > 1.0f )
   {
       fpsReal = fpsFrames / dtReal;
       fpsVirtual = fpsFrames / dtVirtual;
       fpsRealLast = realSeconds;
       fpsVirtualLast = virtualSeconds;
       fpsFrames = 0;

       Con::setVariable("fps::real",    avar("%4.1f", fpsReal));
       Con::setVariable("fps::virtual", avar("%4.1f", fpsVirtual));
   }
   
   /*
   if (fpsFrames > 1)
   {
      fpsReal    = fpsReal*(1.0-alpha) + (realSeconds-fpsRealLast)*alpha;
      fpsVirtual = fpsVirtual*(1.0-alpha) + (virtualSeconds-fpsVirtualLast)*alpha;
   }

   fpsRealLast    = realSeconds;
   fpsVirtualLast = virtualSeconds;

   // update variables every few frames
   F32 update = fpsRealLast - fpsNext;
   if (update > 0.5f)
   {
      Con::setVariable("fps::real",    avar("%4.1f", avgFPS));
      Con::setVariable("fps::virtual", avar("%4.1f", avgFPS));

      if (update > mUpdateInterval)
         fpsNext  = fpsRealLast + mUpdateInterval;
      else
         fpsNext += mUpdateInterval;
   }*/
}