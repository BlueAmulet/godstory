//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _SFXMP3RESOURCE_H_
#define _SFXMP3RESOURCE_H_


#ifndef _SFXRESOURCE_H_
#include "sfx/sfxResource.h"
#endif


class SFXMP3Resource : public SFXResource
{
protected:


	SFXMP3Resource();

	virtual ~SFXMP3Resource();

	bool load( Stream& stream );

public:
	static ResourceInstance* create( Stream &stream, ResourceObject *resObject );
};


#endif  
