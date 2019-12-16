//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#pragma once

#include "Gameplay/ai/NaviGrid/NavigationCellMgr.h"
#include <list>
#include <string>


class CAIPath
{
public:

	bool			FindPath			(Point3F& start, Point3F& dest, bool bNearby = true);

	bool			GetStep				(Point3F& step);		//获取当前的step
	void			OnReachStep			();						//当前到达位置

	std::list<Point3F>&	GetPathList			() { return m_Path; }
	void			ClearPath			() { m_Path.clear(); m_End.zero(); m_Start.zero();}
	bool			IsEmpty				() { return m_Path.empty();}
	Point3F& 		GetEnd				() { return m_End; }
	Point3F&		GetStart			() { return m_Start;}

private:
	Point3F			m_End;
	Point3F			m_Start;
	std::list<Point3F>	m_Path;
};


inline bool CAIPath::GetStep(Point3F& step)
{
	if (m_Path.empty())
		return false;
	else
	{
		step = m_Path.front();
		return true;
	}
}

inline void CAIPath::OnReachStep()
{
	if(!m_Path.empty())
		m_Path.pop_front();
}


