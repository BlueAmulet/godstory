//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#include "Gameplay/ai/AIPath.h"
#include "Gameplay/ai/NaviGrid/NavigationBlock.h"
#include <map>

bool CAIPath::FindPath(Point3F& start, Point3F& dest, bool bNearby)
{
	m_Path.clear();
	m_Start = start;
	m_End = dest;

	return g_NavigationManager->FindPath(m_Path,m_Start,m_End);
}
