#pragma once

#include "math/mPoint.h"
#include "math/mPlane.h"
#include "sceneGraph/sceneObject.h"
#include <hash_map>
#include <list>
#include "NavigationMeshEx.h"

#define NavigationBlockVer		0x1

#define GRID_SIZE				2.0f			// 每块网格边长
#define SINGLE_MAP_OFFSET_X		256.0f			// 单块地图中心点位移 边长是其两倍
#define SINGLE_MAP_OFFSET_Y		256.0f			// 单块地图中心点位移
#define SINGLE_ROW_SIZE			256				// 单块地图每行的网格个数
#define SINGLE_COL_SIZE			256				// 单块地图每列的网格个数
#define SINGLE_GRID_NUM			65536			// 单块地图网格个数

#define PATH_WIDTH				0.4f			// 寻径时要求路径的宽度(半径)

//-------------------------------------------------------------------------
// 导航网格的单元
//
class TerrainBlock;

class NaviCell
{
public:
	enum
	{
		DirectionUnknown		= 0,
		DirectionLeft			= 0x01,
		DirectionTop			= 0x02,
		DirectionRight			= 0x04,
		DirectionBottom			= 0x08,
	};

	U32					m_Idx;
	U32					m_SessionID;
	U32					m_ArrivalCell;
	F32					m_ArrivalCost;		// total cost to use this cell as part of a path
	F32					m_Heuristic;		// our estimated cost to the goal from here
	F32					m_Height;
	F32					m_HeightTopLeft;
	F32					m_HeightTopRight;
	F32					m_HeightBottomRight;
	F32					m_HeightBottomLeft;
	bool				m_Open;				// are we currently listed as an Open cell to revisit and test?
	bool				m_Close;

public:
	NaviCell();
	~NaviCell();

	void				ComputeHeuristic(const Point3F& Goal);
	Point3F				GetCenter();
	U32					GetArrivalDirection();
};

//---------------------------------------------------------------------------------
// 将导航网格每块单元存一个索引值，值为-1表示该网格不可行
//

class NavigationBlock
{
public:
	static U32			ROW_SIZE;
	static U32			COL_SIZE;
	static U32			MAX_WIDTH;			// 大块block由 mWidth * mHeight 个独立的小块block拼接而成
	static U32			MAX_HEIGHT;
	static F32			MAP_OFFSET_X;
	static F32			MAP_OFFSET_Y;
	static U32			GRID_NUM;

private:
	S32*				mCellIndex;
	Vector<NaviCell>	mCellInfo;
	Vector<U32>			mSubInfo;			// 记录每小块block有几个网格可行

public:

	NavigationBlock();
	~NavigationBlock();

	void				Initialization	(U32 singleCol, U32 singleRow, U32 width, U32 height, U32 CellSize);
	void				Clear			();
	void				SetCellSize		(U32 size);
	void				SetGridSize		(U32 size);
	void				AddCell			(U32 idx, U32 count, const F32 height, const F32 tl, const F32 tr, const F32 bl, const F32 br);
	NaviCell*			GetCell			(U32 idx);
	void				DeleteCell		(const Point3F& pos);
	void				DeleteCell		(U32 idx);
	void				AddToSub		(U32 width, U32 height);

	S32					GetIndex		(const Point3F& pos);
	bool				CanReach		(U32 idx);
	bool				castRay			(U32 idx, const Point3F& start, const Point3F& end, Point3F& pos);
	VectorF				SlerpNormal		(VectorF& pos);
	VectorF				SlerpPosition	(VectorF& pos);

	bool				Save			();
	bool				Load			();
	static TerrainBlock*GetTerrain		(const Point3F& pos);
};


//---------------------------------------------------------------------------------
// 导航网格管理
//

class Cell_Greater:
	public std::binary_function<const NaviCell*,const NaviCell*,bool>
{
public:
	bool operator()(const NaviCell* left,const NaviCell* right)
	{
		return (left->m_ArrivalCost + left->m_Heuristic) > (right->m_ArrivalCost + right->m_Heuristic);
	}
};

class NavigationManager
{
public:
	typedef std::vector<NaviCell*> Container;
	Cell_Greater CellCmp;

private:
	NavigationBlock   mBlock;
	CNavigationMeshEx mNavMesh;
	Container mOpenList;

	U32 m_PathSession;
	Point3F mOrigin;
	Point3F mGoal;
	bool    mIsNewVersion;

	//typedef stdext::hash_map<F32,NaviCell*>::value_type OpenListType;
	//typedef stdext::hash_map<F32,NaviCell*>::iterator OpenListIterator;

public:
	NavigationManager();
	~NavigationManager();

	NavigationBlock&	GetBlock					();
	void				Setup						(const Point3F& origin, const Point3F& goal);
	bool				FindPath					(std::list<Point3F>& Waypointlist, VectorF& start, VectorF& end);
	bool				BuildNavigationPath			(std::list<Point3F>& Waypointlist, U32 StartIdx, U32 EndIdx);
	bool				QueryForPath				(U32 Idx, U32 callerIdx, F32 ArrivalCost);
	void				ProcessCell					(NaviCell* pCell);

	void				ClipPath					();									//裁值路径
	bool				LineOfSightTest				(Point3F& start, Point3F& end, Point3F& pos);		//测试两个单元的是否相通
	bool				LineOfSightTestWithWidth	(Point3F& start, Point3F& end, float width);		//带宽度的测试
	bool				castRay						(Point3F& start, Point3F& end, Point3F& pos);		//射线碰撞

	void				AddCell						(NaviCell* Cell);
	void				AdjustCell					(NaviCell* Cell);
	void				GetTop						(NaviCell*& Cell);

	bool				CanReach					(Point3F& pos);	
	VectorF				SlerpNormal(VectorF& pos);
	VectorF				SlerpPosition(VectorF& pos);

	bool				Save						();
	bool				Load						();
};

extern NavigationManager* g_NavigationManager;
