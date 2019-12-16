//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _ENGINE_CONFIG_H_
#define _ENGINE_CONFIG_H_

#ifdef NTJ_SERVER
#include "Server/powerEngineConfig.h"
#elif NTJ_CLIENT
#include "Client/powerEngineConfig.h"
#elif NTJ_EDITOR
#include "Editor/powerEngineConfig.h"
#endif

#endif


