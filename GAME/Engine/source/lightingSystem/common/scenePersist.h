//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _SGSCENEPERSIST_H_
#define _SGSCENEPERSIST_H_

#ifndef _TVECTOR_H_
#include "core/tVector.h"
#endif

class Stream;

struct PersistInfo
{
	struct PersistChunk
	{
		enum {
			MissionChunkType = 0,
			InteriorChunkType,
			TerrainChunkType,
			StaticChunkType,
			AtlasLightMapChunkType
		};

		U32            mChunkType;
		U32            mChunkCRC;

		virtual ~PersistChunk() {}

		virtual bool read(Stream &);
		virtual bool write(Stream &);
	};

	struct MissionChunk : public PersistChunk
	{
		typedef PersistChunk Parent;
		MissionChunk();
	};

	~PersistInfo();

	Vector<PersistChunk*>      mChunks;
	static U32                 smFileVersion;

	bool read(Stream &);
	bool write(Stream &);
};


#endif//_SGSCENEPERSIST_H_
