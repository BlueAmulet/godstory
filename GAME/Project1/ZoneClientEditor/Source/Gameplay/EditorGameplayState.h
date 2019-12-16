//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#pragma once
#include <time.h>
#include "platform/platform.h"

class GameBase;

// ========================================================================================================================================
//	EditorGameplayState
// ========================================================================================================================================
///	Editor gameplay state.
///
class EditorGameplayState
{
public:
	EditorGameplayState									();
	~EditorGameplayState									();

	void			 Initialize								();
	void			 preShutdown							();
	void			 Shutdown								();
	void			 prepRenderImage						(S32 StateKey);
	void			 TimeLoop								(S32 timeDelta);
	void			 onDisconnect							();

	void			 clearResource							();
	GameBase*		 GetControlObject						();
};

// ========================================================================================================================================
//	Global Variables
// ========================================================================================================================================
extern EditorGameplayState* g_EditorGameplayState;
