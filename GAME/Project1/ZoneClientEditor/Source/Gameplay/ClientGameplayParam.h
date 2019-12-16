//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#pragma once
#include "platform/types.h"

class GameplayEvent;

class ClientGameplayParam
{
public:
	ClientGameplayParam();
	virtual ~ClientGameplayParam();

	void			setIntArgValues(S32 argc, ...);
	S32				getIntArg(S32 index);
	void			setInt32ArgValues(S32 argc, ...);
	S32				getInt32Arg(S32 index);

	bool			sendParam(GameplayEvent* ev);
protected:
	U32				mArgCount;
	U32				mArg32Count;
	S16*			mArg;
	S32*			mArg32;
};