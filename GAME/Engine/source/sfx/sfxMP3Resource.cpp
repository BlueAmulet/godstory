//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------


#include "platform/platform.h"
#include "sfx/sfxMP3Resource.h"


ResourceInstance* SFXMP3Resource::create( Stream &stream, ResourceObject * )
{
	SFXMP3Resource* res = new SFXMP3Resource;
	if(res->load(stream))
		return res;

	delete res;
	return NULL;
}

SFXMP3Resource::SFXMP3Resource()
: SFXResource()
{

}

SFXMP3Resource::~SFXMP3Resource()
{

}


bool SFXMP3Resource::load( Stream& stream )
{
	U32 size = stream.getStreamSize();
	mFiletype = 2; // Mp3 file type
	mData = new U8[size];
	if(mData)
	{
		stream.read(size,mData);		
	}
	mSize = size;
	mLength = size;
	return true;
}