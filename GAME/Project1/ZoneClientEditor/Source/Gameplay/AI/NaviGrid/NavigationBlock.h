#pragma once

#include "math/mPoint.h"
#include "math/mPlane.h"
#include "sceneGraph/sceneObject.h"
#include <hash_map>
#include <list>
#include "NavigationMeshEx.h"

#define NavigationBlockVer		0x1

#define GRID_SIZE				2.0f			// ÿ������߳�
#define SINGLE_MAP_OFFSET_X		256.0f			// �����ͼ���ĵ�λ�� �߳���������
#define SINGLE_MAP_OFFSET_Y		256.0f			// �����ͼ���ĵ�λ��
#define SINGLE_ROW_SIZE			256				// �����ͼÿ�е��������
#define SINGLE_COL_SIZE			256				// �����ͼÿ�е��������
#define SINGLE_GRID_NUM			65536			// �����ͼ�������

#define PATH_WIDTH				0.4f			// Ѱ��ʱҪ��·���Ŀ��(�뾶)

//-------------------------------------------------------------------------
// ��������ĵ�Ԫ
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
// ����������ÿ�鵥Ԫ��һ������ֵ��ֵΪ-1��ʾ�����񲻿���
//

class NavigationBlock
{
public:
	static U32			ROW_SIZE;
	static U32			COL_SIZE;
	static U32			MAX_WIDTH;			// ���block�� mWidth * mHeight ��������С��blockƴ�Ӷ���
	static U32			MAX_HEIGHT;
	static F32			MAP_OFFSET_X;
	static F32			MAP_OFFSET_Y;
	static U32			GRID_NUM;

private:
	S32*				mCellIndex;
	Vector<NaviCell>	mCellInfo;
	Vector<U32>			mSubInfo;			// ��¼ÿС��block�м����������

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
// �����������
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

	void				ClipPath					();									//��ֵ·��
	bool				LineOfSightTest				(Point3F& start, Point3F& end, Point3F& pos);		//����������Ԫ���Ƿ���ͨ
	bool				LineOfSightTestWithWidth	(Point3F& start, Point3F& end, float width);		//����ȵĲ���
	bool				castRay						(Point3F& start, Point3F& end, Point3F& pos);		//������ײ

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
