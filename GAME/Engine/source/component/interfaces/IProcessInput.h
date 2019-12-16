//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _I_PROCESSINPUT_H_
#define _I_PROCESSINPUT_H_

#include "platform/event.h"
#include "component/componentInterface.h"

class IProcessInput
{
public:
   virtual bool processInputEvent( InputEventInfo &inputEvent ) = 0;
};





#endif