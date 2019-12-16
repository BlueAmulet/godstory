#ifndef _NAVIGATIONMESHEX_H_
#define _NAVIGATIONMESHEX_H_

#include <hash_map>

#include "math/mPoint.h"
#include "math/mPlane.h"
#include "NaviGridDefine.h"
#include "NavigationHeap.h"
#include "BackgroundLoad.h"
#include <map>
#include <vector>
#include <list>
#include <string>

typedef std::list<VectorF> WAYPOINT_LIST;

/************************************************************************/
/* 简化版的寻路
/************************************************************************/
class CNavigationMeshEx:
	public CBackgroundLoad
{
	//用来计算大格子用基础格子
	struct CellCal
	{
		enum CELL_VERT
		{
			VERT_A = 0,
			VERT_B,
			VERT_C,
			VERT_D,
			VERT_CENTER,
		};

		CellCal* m_Link[4];
		float	 height[4];
		
		float GetCenterHeight(void)
		{
			return (height[0] + height[1] + height[2] + height[3]) * 0.25f;
		}

		CellCal* GetLink(int Side)const { return m_Link[Side]; }
	};

	//地图的基础格子，用来计算高度和法线用
	struct CellBase
	{
	public:
		CellBase(float h0,float h1,float h2,float h3)
		{
			height[0] = h0;
			height[1] = h1;
			height[2] = h2;
			height[3] = h3;
		}

		float GetCenterHeight(void) {return (height[0] + height[1] + height[2] + height[3]) * 0.25f;}

		float height[4];
	};

	//寻路用的大格子
	struct CellFind
	{
	public:
		CellFind(void) : height(-0xffffff) {}
		CellFind(float height) : height(height) {}

		void	AddNeighbours(unsigned int id)
		{
			m_neighbours.push_back(id);
		}

		bool	ProcessCell(CNavigationMeshEx*parent,NavigationHeap<CellFind>* pHeap,unsigned ownId);

		bool	QueryForPath(CNavigationMeshEx* parent,NavigationHeap<CellFind>* pHeap,unsigned int id,CellFind* Caller,unsigned int fromId,float arrivalcost);

		float height;
		std::vector<unsigned int> m_neighbours;

		bool IsFlat(void) {return height != -0xffffff;}
	};
	
	//寻路用的基本信息
	struct PathInfo
	{
		PathInfo(void) : m_SessionID(-1),m_ArrivalCost(0),m_Heuristic(0),m_ArrivalCell(0) {}

		int		        m_SessionID;		// an identifier for the current pathfinding session.
		float			m_ArrivalCost;		// total cost to use this cell as part of a path
		float			m_Heuristic;		// our estimated cost to the goal from here
		unsigned int    m_ArrivalCell;
		bool			m_Open;				// are we currently listed as an Open cell to revisit and test?
	};

	//高度信息
	struct HeightInfo
	{
		float height[4];
	};
public:
	CNavigationMeshEx(void);

	~CNavigationMeshEx(void);
	
	bool			Open(const char* fileName,bool isBackgroundLoad = false);
	
	bool			FindPath(VectorF& start, VectorF& end,int maxNodeSearch = 3600,bool isSmoothPath = true);
	WAYPOINT_LIST&  GetPath(void) {return m_Path;}

	VectorF			SlerpNormal(VectorF& pos);
	VectorF			SlerpPosition(VectorF& pos);

	bool			CanReach(Point3F& pos);
	
	CellCal*		FindCell(int id);
	void            DelCell(int id);
	
	/************************************************************************/
	/* 设置标志位
	/************************************************************************/
	void            InitNavGridFlag(void);
	unsigned int	GetGridFlag(int row,int col)					{return m_pGridFlag[row * (int)singleTableWidth + col];}
	void			SetGridFlag(int row,int col,unsigned int flag)	{m_pGridFlag[row * (int)singleTableWidth + col] = flag;}

	/************************************************************************/
	/* 寻路用的大格子
	/************************************************************************/
	typedef stdext::hash_map<unsigned int,CellFind> CELL_MAP;

	void			AddGenCell(unsigned int id,float height);
	CellFind*		GetGenCell(unsigned int id);
	CELL_MAP&       GetGenCells(void)		 {return m_cellFinds;}
	const CELL_MAP& GetGenCells(void) const  {return m_cellFinds;}
	void			ClrGenCells(void);

	//method from CBackgroundLoad
	virtual bool OnLoad(const char* name)
	{
		return Open(name,false);
	}
private:
	void		__Clear(void);
    bool        __InitBackgroundLoad(const char* fileName);

	void		__AddCellCal(int row,int col);
	bool		__DoFindPath(CellFind* startCell,CellFind* endCell,unsigned int startId,unsigned int endId,VectorF& start, VectorF& end,int maxNodeSearch,bool isSmoothPath);
	bool		__DoFindStraightLine(VectorF& start, VectorF& end);

	CellFind*	__FindCell(const VectorF& point,unsigned int* pID = 0);
	bool		__IsInLine(VectorF& start, VectorF& end);
	CellFind*	__GetEndNearbyCell(VectorF& start, VectorF& end,unsigned int* id,int& step);
    bool        __GetUnReachableNearbyCell(VectorF& start, VectorF& end,unsigned int* id);

	bool		__BuildNavigationPath(WAYPOINT_LIST& NavPath, CellFind* StartCell, const VectorF& StartPos,unsigned int StartId,CellFind* EndCell, const VectorF& EndPos,unsigned int EndId,int maxNodeSearch);

	float		__ComputeHeuristic(unsigned int id,const VectorF& goal);

	VectorF		__GetCenterPoint(unsigned int id);
	VectorF		__GetCenterXY(unsigned int id,int* row = 0,int* col = 0);

	bool		__GetCorn(const VectorF& pos,VectorF* pCorn);

	void		__SmoothPath(void);

	typedef stdext::hash_map<unsigned int,CellCal> CELL_CAL_MAP;
	CELL_CAL_MAP m_cellCals;

	typedef stdext::hash_map<unsigned int,CellBase> CELL_BASE_MAP;
	CELL_BASE_MAP m_cellBases;

	typedef stdext::hash_map<unsigned int,PathInfo> PATHINFO_MAP;
	PATHINFO_MAP  m_pathInfos;

	CELL_MAP		m_cellFinds;
	
	unsigned int*	m_pGridFlag;
	
	int							m_PathSession;
	NavigationHeap<CellFind>	m_NavHeap;
	WAYPOINT_LIST				m_Path;

	std::string m_lastLoadedFile;

    char*         m_pNavBuf;
    unsigned int  m_navBufferSize;
};


#endif /*_NAVIGATIONMESHEX_H_*/