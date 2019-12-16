//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "platform/platformInput.h"

#include "app/game.h"
#include "math/mMath.h"
#include "core/dnet.h"
#include "core/fileStream.h"
#include "core/frameAllocator.h"
#include "core/iTickable.h"
#include "core/findMatch.h"
#include "console/simBase.h"
#include "console/console.h"
#include "console/consoleTypes.h"
#include "gui/controls/guiMLTextCtrl.h"
#ifdef POWER_TGB_ONLY
#include "T2D/t2dGameConnection.h"
#else
#include "T3D/gameConnection.h"
#include "T3D/gameFunctions.h"
#include "T3D/gameProcess.h"
#endif
#include "platform/profiler.h"
#include "gfx/gfxCubemap.h"
#include "sfx/sfxSystem.h"
//#include "Gameplay/GameObjects/PlayerObject.h"

#ifdef POWER_PLAYER
// See matching #ifdef in editor/editor.cpp
bool gEditingMission = false;
#endif

//--------------------------------------------------------------------------

ConsoleFunctionGroupBegin( InputManagement, "Functions that let you deal with input from scripts" );

ConsoleFunction( deactivateDirectInput, void, 1, 1, "Deactivate input. (ie, ungrab the mouse so the user can do other things." )
{
   argc; argv;
   if ( Input::isActive() )
      Input::deactivate();
}

ConsoleFunction( activateDirectInput, void, 1, 1, "Activate input. (ie, grab the mouse again so the user can play our game." )
{
   argc; argv;
   if ( !Input::isActive() )
      Input::activate();
}
ConsoleFunctionGroupEnd( InputManagement );

//--------------------------------------------------------------------------
static const U32 MaxPlayerNameLength = 16;

#ifdef NTJ_GUI
ConsoleFunction( strToPlayerName, const char*, 2, 2, "strToPlayerName( string )" )
{
   argc;

   const char* ptr = argv[1];

	// Strip leading spaces and underscores:
   while ( *ptr == ' ' || *ptr == '_' )
      ptr++;

   U32 len = dStrlen( ptr );
   if ( len )
   {
      char* ret = Con::getReturnBuffer( MaxPlayerNameLength + 1 );
      char* rptr = ret;
      ret[MaxPlayerNameLength - 1] = '\0';
      ret[MaxPlayerNameLength] = '\0';
      bool space = false;

      U8 ch;
      while ( *ptr && dStrlen( ret ) < MaxPlayerNameLength )
      {
         ch = (U8) *ptr;

         // Strip all illegal characters:
         if ( ch < 32 || ch == ',' || ch == '.' || ch == '\'' || ch == '`' )
         {
            ptr++;
            continue;
         }

         // Don't allow double spaces or space-underline combinations:
         if ( ch == ' ' || ch == '_' )
         {
            if ( space )
            {
               ptr++;
               continue;
            }
            else
               space = true;
         }
         else
            space = false;

         *rptr++ = *ptr;
         ptr++;
      }
      *rptr = '\0';

		//finally, strip out the ML text control chars...
		return GuiMLTextCtrl::stripControlChars(ret);
   }

	return( "" );
}
#endif

//--------------------------------------------------------------------------
ConsoleFunction( flushTextureCache, void, 1, 1, "Flush the texture cache.")
{
   GFX->zombifyTextureManager();
   GFX->resurrectTextureManager();
}

ConsoleFunctionGroupEnd( GameFunctions );

ConsoleFunctionGroupBegin( Platform , "General platform functions.");

ConsoleFunction( lockMouse, void, 2, 2, "(bool isLocked)"
                "Lock the mouse (or not, depending on the argument's value) to the window.")
{
   Platform::setWindowLocked(dAtob(argv[1]));
}

ConsoleFunction( setNetPort, bool, 2, 2, "(int port)"
                "Set the network port for the game to use.")
{
   return Net::openPort(dAtoi(argv[1]));
}

ConsoleFunction( setNetPortEx, int, 2, 2, "(int port)"
				"Set the addition network port for the game to use." )
{
	return Net::openPortEx( dAtoi( argv[1]) );
}

ConsoleFunction( saveJournal, void, 2, 2, "(string filename)"
                "Save the journal to the specified file.")
{
   Journal::Record(argv[1]);
}

ConsoleFunction( playJournal, void, 2, 3, "(string filename, bool break=false)"
                "Begin playback of a journal from a specified field, optionally breaking at the start.")
{
   // CodeReview - BJG 4/24/2007 - The break flag needs to be wired back in.
   // bool jBreak = (argc > 2)? dAtob(argv[2]): false;
   Journal::Play(argv[1]);
}

ConsoleFunction( getSimTime, S32, 1, 1, "Return the current sim time in milliseconds.\n\n"
                "Sim time is time since the game started.")
{
   return Sim::getCurrentTime();
}

ConsoleFunction( getRealTime, S32, 1, 1, "Return the current real time in milliseconds.\n\n"
                "Real time is platform defined; typically time since the computer booted.")
{
   return Platform::getRealMilliseconds();
}

ConsoleFunctionGroupEnd(Platform);

//-----------------------------------------------------------------------------

bool clientProcess(U32 timeDelta)
{
   // ShowTSShape::advanceTime(timeDelta);
   ITickable::advanceTime(timeDelta);

   bool ret = true;

#ifndef POWER_TGB_ONLY
   ret = gClientProcessList.advanceTime(timeDelta);
#endif

   // Run the collision test and update the Audio system
   // by checking the controlObject
   MatrixF mat;
   Point3F velocity;

#ifndef POWER_TGB_ONLY
 /*if (GameGetCameraTransform(&mat, &velocity))
   {
      SFX->getListener().setTransform(mat);
      SFX->getListener().setVelocity(velocity);
   }*/
  
  
#endif

   // Determine if we're lagging
   GameConnection* connection = GameConnection::getConnectionToServer();
   if(connection)
   {
	connection->detectLag();
	GameBase* pPlayer = connection->getControlObject();
	if(pPlayer)
	{
		SFX->getListener().setTransform(pPlayer->getTransform());
		SFX->getListener().setVelocity(pPlayer->getVelocity());

	}
   }
      
   
   // Let SFX process.

   SFX->_update();

   return ret;
}

bool serverProcess(U32 timeDelta)
{
   bool ret = true;
#ifndef POWER_TGB_ONLY
   ret =  gServerProcessList.advanceTime(timeDelta);
#endif
   return ret;
}

