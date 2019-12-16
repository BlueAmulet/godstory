//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "console/console.h"
#include "console/consoleTypes.h"
#include "platform/threads/mutex.h"
#include "app/mainLoop.h"
#include "platform/event.h"

// The tools prefer to allow the CPU time to process
#ifndef POWER_TOOLS
S32 sgBackgroundProcessSleepTime = 25;
#else
S32 sgBackgroundProcessSleepTime = 200;
#endif
S32 sgTimeManagerProcessInterval = 1;

Vector<Platform::KeyboardInputExclusion> gKeyboardExclusionList;
bool gInitKeyboardExclusionList = false;

void Platform::initConsole()
{
   Con::addVariable("Pref::backgroundSleepTime", TypeS32, &sgBackgroundProcessSleepTime);
   Con::addVariable("Pref::timeManagerProcessInterval", TypeS32, &sgTimeManagerProcessInterval);
}

S32 Platform::getBackgroundSleepTime()
{
   return sgBackgroundProcessSleepTime;
}

ConsoleToolFunction(restartInstance, void, 1, 1, "restartInstance()")
{
   StandardMainLoop::setRestart(true);
   Platform::postQuitMessage( 0 );
}

void Platform::clearKeyboardInputExclusion()
{
   gKeyboardExclusionList.clear();
   gInitKeyboardExclusionList = true;
}

void Platform::addKeyboardInputExclusion(const KeyboardInputExclusion &kie)
{
   gKeyboardExclusionList.push_back(kie);
}

const bool Platform::checkKeyboardInputExclusion(const InputEventInfo *info)
{
   // Do one-time initialization of platform defaults.
   if(!gInitKeyboardExclusionList)
   {
      gInitKeyboardExclusionList = true;

      // CodeReview Looks like we don't even need to do #ifdefs here since
      // things like cmd-tab don't appear on windows, and alt-tab is an unlikely
      // desired bind on other platforms - might be best to simply have a 
      // global exclusion list and keep it standard on all platforms.
      // This might not be so, but it's the current assumption. [bjg 5/4/07]

      // Alt-tab
      {
         KeyboardInputExclusion kie;
         kie.key = KEY_TAB;
         kie.orModifierMask = SI_ALT;
         addKeyboardInputExclusion(kie);
      }

      // ... others go here...
   }

   // Walk the list and look for matches.
   for(S32 i=0; i<gKeyboardExclusionList.size(); i++)
      if(gKeyboardExclusionList[i].checkAgainstInput(info))
         return true;

   return false;
}

const bool Platform::KeyboardInputExclusion::checkAgainstInput( const InputEventInfo *info ) const
{
   if(info->objType != SI_KEY)
      return false;

   if(info->objInst != key)
      return false;

   if((info->modifier & andModifierMask) != andModifierMask)
      return false;

   if(!(info->modifier & orModifierMask))
      return false;

   return true;
}