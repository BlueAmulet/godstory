//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#pragma once

#include "platform/types.h"
#include "core/tVector.h"
#include <hash_map>


// ========================================================================================================================================
//  ShapesSet
// ========================================================================================================================================

struct ShapesSet
{
	enum
	{
		MaxMountedImages = 8,
	};

	struct subImage {
		StringTableEntry shapeName;
		StringTableEntry skinName;
		StringTableEntry mountPoint;
		StringTableEntry mountPointSelf;
		U32 effectId;

		subImage() : shapeName(NULL), skinName(NULL), mountPoint(NULL), mountPointSelf(NULL), effectId(0) {}
	};

	StringTableEntry	shapeName;
	StringTableEntry	skinName;
	U32					effectId;
	subImage			subImages[MaxMountedImages];

	ShapesSet() : shapeName(NULL), skinName(NULL), effectId(0) {}
};


// ========================================================================================================================================
//  ShapesSetRepository
// ========================================================================================================================================

class ShapesSetRepository
{
public:
	typedef stdext::hash_map<U32, ShapesSet*> ShapesSetMap;

	ShapesSetRepository									();
	~ShapesSetRepository								();

	void					Clear						();
	void					Read						();
	const ShapesSet*		GetSet						(U32 id);
private:
	ShapesSetMap			m_ShapesSetMap;
};

extern ShapesSetRepository g_ShapesSetRepository;
