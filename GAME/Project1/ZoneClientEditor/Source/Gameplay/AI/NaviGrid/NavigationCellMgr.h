//
// NTianJi Game Engine V1.0
//
//
// Name:
//
//
//

#pragma once
#include <hash_map>

//#include "System/SysManager.h"
#include "math/mPoint.h"
#include "math/mPlane.h"
//#include "Graphics/Scene/RenderableObject.h"
#include "sceneGraph/sceneObject.h"
#include "NavigationCell.h"
#include "NaviGridDefine.h"
#include "NavigationHeap.h"
#include <map>
#include <vector>

typedef std::list<VectorF> WAYPOINT_LIST;

#ifdef NTJ_EDITOR
#include "NavigationOptimize.h"
#endif

class GFXStateBlock;
class NavigationCellMgr
{
public:
    struct Line
    {
        Point3F pos1;
        Point3F pos2;
    };
	//typedef	stdext::hash_multimap<int, NavigationCell*> CELL_ARRAY;
	typedef Table<NavigationCell*> CELL_ARRAY;

	NavigationCellMgr();
	~NavigationCellMgr();

	// ----- MUTATORS ---------------------
	void				Clear			();
	void				InitCellArray	(int initSize);
    NavigationCell*		AddCell			(const VectorF& a, const VectorF& b, const VectorF& c, const VectorF& d,bool isNavFixed = false,std::string line = "");
	void				DeleteCell		(const VectorF& a, const VectorF& b, const VectorF& c, const VectorF& d);
	void				DeleteCell		(const NavigationCell* cellDel);
	void				LinkNeighbor	(NavigationCell* cell);
	void				UnlinkNeighbor	(NavigationCell* cell);
	void				SplitCell		(NavigationCell* cell, NavigationCell::CELL_SIDE side);
	
	NavigationCell*		FindCell		(const VectorF& point);
	NavigationCell*		FindCell		(int id);
	VectorF				GetPoint		(int index)					{ return s_GridPointBase.GetPoint(index); }
	void				UpdatePoint		(int index, VectorF pos)	{ s_GridPointBase.UpdatePoint(index, pos);}
	int					GetArraySize	() { return m_CellArray.size(); }
	void				ClearPath		();
	WAYPOINT_LIST&		GetPath() { return m_Path; }

	bool FindPath(VectorF& start, VectorF& end);
	
	//--------------------------------------------------------
	void				Render			();
	bool				Save			(const char* fileName);
	bool				Open			(const char* fileName);

	void            DelCell(int id) {}
	// ----------static --------------------------------------
	static int			sLinePoint[6][2];
	static GridPointBase s_GridPointBase;
public:
	
	/************************************************************************/
	/* 显示优化后的网格
	/************************************************************************/
	void			ShowOptimizeMesh(void);
	void			EnableZBuffer(void);
private:
	CELL_ARRAY		m_CellArray; 
	WAYPOINT_LIST	m_Path;

#ifdef NTJ_EDITOR
	std::vector<NavigationCell*> m_ClosedCell;	//for test
#endif

	bool m_isCellChanged;
	bool m_isShowOptimized;
	bool m_isEnableZBuffer;

	static GFXStateBlock* mSetSB;
	static GFXStateBlock* mZTrueSB;
	static GFXStateBlock* mZFalseSB;
	static GFXStateBlock* mAlphaBlendFalseSB;
public:
	static void init();
	static void shutdown();
	//
	//设备丢失时调用
	//
	static void releaseStateBlock();

	//
	//设备重置时调用
	//
	static void resetStateBlock();
};


inline NavigationCellMgr::~NavigationCellMgr()
{
	Clear();
}

inline void NavigationCellMgr::Clear()
{
	CELL_ARRAY::iterator it, end;
	end = m_CellArray.end();

	for(int i = 0; i < m_CellArray.capacity(); i++)
	{
		it = m_CellArray.at(i);
		for(;it != end; it = it->next)
			delete it->value;
	}

	m_CellArray.clear();
	s_GridPointBase.Clear();
    m_Path.clear();
}

inline void NavigationCellMgr::ClearPath()
{
	m_Path.clear(); 

#ifdef NTJ_EDITOR
	m_ClosedCell.clear(); 
#endif
}

extern NavigationCellMgr g_NavigationMesh;