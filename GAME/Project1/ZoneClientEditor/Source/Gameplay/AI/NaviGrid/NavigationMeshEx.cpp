#include "NavigationMeshEx.h"
#include "core/frameAllocator.h"
#include "core/fileStream.h"
#include "core/bitStream.h"
#include "core/memstream.h"

#undef NTJ_EDITOR
#include "NavigationOptimize.h"

CNavigationMeshEx::CNavigationMeshEx(void):
	m_pGridFlag(0),
    m_pNavBuf(0),
    m_navBufferSize(0)
{
	
}

CNavigationMeshEx::~CNavigationMeshEx(void)
{
	__Clear();
}

bool CNavigationMeshEx::Open(const char* fileName,bool isBackgroundLoad)
{
	try {

	if (0 == fileName)
		return false;

	if (m_lastLoadedFile == fileName)
		return true;

    if (isBackgroundLoad)
    {
        Finish();

        if (!__InitBackgroundLoad(fileName))
            return false;

        BackgroundLoad(fileName);
        return true;
    }
    else
    {
        //异步载入
        MemStream stream(m_navBufferSize,m_pNavBuf);

        // 写入版本号
        int id, ver;
        stream.read(&id);
        stream.read(&ver);

        if (id != NaviMeshID || ver != NaviMeshVer)
        {
            Con::errorf("invalid navigation file [%s] readed,version:%d",fileName,ver);
            return false;
        }

        //度缓冲
        std::vector<float> heightMap;
        size_t size;
        float  height;

        stream.read(&size);

        for (size_t i = 0; i < size; ++i)
        {
            stream.read(&height);
            heightMap.push_back(height);
        }

        //读取标志位
        unsigned char* buf = new unsigned char[singleTableSize];

        if (0 == buf)
        {
            return false;
        }

        memset(buf,0,singleTableSize);

        bool* pMarkTable = new bool[singleTableSize];

        if (0 == pMarkTable)
        {
            delete []buf;

            return false;
        }

        U32 markSize;
        stream.read(&markSize);
        stream.read(markSize,buf);
        BitStream bitStream((void*)buf,markSize);

        for (int row = 0; row < singleTableWidth; ++row)
        {
            for (int col = 0; col < singleTableWidth; ++col)
            {
                bool flag = bitStream.readFlag();

                if (flag)
                {
                    __AddCellCal(row,col);
                }

                pMarkTable[row * (int)singleTableWidth + col] = flag;
            }
        }

        delete []buf;

        //读取高度
        int          lastHeight;
        unsigned int heightCount = 0;

        //读取CELL的高度
        for (int row = 0; row < singleTableWidth; ++row)
        {
            for (int col = 0; col < singleTableWidth; ++col)
            {
                if (pMarkTable[row * (int)singleTableWidth + col])
                {	
                    CELL_CAL_MAP::iterator iter = m_cellCals.find(NaviGrid::GridToID(col,row));

                    if (iter == m_cellCals.end())
                        continue;

                    CellCal& cellCal = iter->second;

                    for (int iCorn = 0; iCorn < 4; ++iCorn)
                    {
                        if (0 == heightCount)
                        {
                            stream.read(&heightCount);

                            if (heightMap.size() > 65535)
                            {
                                stream.read(&lastHeight);
                            }
                            else
                            {
                                unsigned short tmpHeight;
                                stream.read(&tmpHeight);
                                lastHeight = tmpHeight;
                            }
                        }

                        if (heightCount == 0 || lastHeight >= heightMap.size())
                        {
                            delete []pMarkTable;
                            return false;
                        }

                        cellCal.height[iCorn] = heightMap[lastHeight];
                        heightCount--;
                    }
                }
            }
        }

        delete []pMarkTable;

        //生成大网格
        CNavigationOptimize<CNavigationMeshEx,CellCal,CellFind> navOptimize;
        navOptimize.Build(this,this);

        //拷贝基础格子的高度
        for (CELL_CAL_MAP::iterator iter = m_cellCals.begin(); iter != m_cellCals.end(); ++iter)
        {
            m_cellBases.insert(std::make_pair(iter->first,CellBase(iter->second.height[0],iter->second.height[1],iter->second.height[2],iter->second.height[3])));
        }

        m_cellCals.clear();
        m_lastLoadedFile = fileName;

        if (!isBackgroundLoad)
        {
            SetFinished();
        }

        if (0 != m_pNavBuf)
        {
            delete []m_pNavBuf;
            m_pNavBuf = 0;
            m_navBufferSize = 0;
        }

        return true;
    }

	}
	catch(...)
	{
		return false;
	}
}

bool CNavigationMeshEx::__InitBackgroundLoad(const char* fileName)
{   
    Stream *stream = ResourceManager->openStream(fileName);

    if(!stream)
        return false;

    __Clear();

    //读取导航文件到内存
    unsigned int bufferSize = ResourceManager->getSize(fileName) + 1;

    if (bufferSize > m_navBufferSize)
    {
        if (0 != m_pNavBuf)
            delete []m_pNavBuf;

        m_pNavBuf = new char[bufferSize];

        if (0 == m_pNavBuf)
            return false;

        m_navBufferSize = bufferSize;
    } 

    stream->read(bufferSize,m_pNavBuf);
    ResourceManager->closeStream(stream);

    return true;
}

bool CNavigationMeshEx::FindPath(VectorF& start, VectorF& end,int maxNodeSearch,bool isSmoothPath)
{
	if (!Finish())
		return false;

	unsigned int startId,endId;
	CellFind* startCell = __FindCell(start,&startId);
	CellFind* endCell   = __FindCell(end,&endId);

	m_Path.clear();

	if (0 == startCell)
		return false;

	if (0 == endCell)
	{
		float length = (start - end).len();
		int step = length / NaviGridSize;

		endCell  = __GetEndNearbyCell(start,end,&endId,step);

		while (0 != endCell)
		{
			if (__DoFindPath(startCell,endCell,startId,endId,start,end,maxNodeSearch,isSmoothPath))
				return true;

            //尝试寻找从终点到起始点的一个不可走的网格，然后再从这个网格开始查找下1个靠近的临近可行走的点
            unsigned int tmpId = 0;
            VectorF tmpEnd = end;

            if (__GetUnReachableNearbyCell(start,tmpEnd,&tmpId))
            {
                end    = tmpEnd;
                endId  = tmpId;
            }

			endCell = __GetEndNearbyCell(start,end,&endId,step);
		}

		// 直接找到直线最近点
		return __DoFindStraightLine(start,end);
	}
	else
	{
		return __DoFindPath(startCell,endCell,startId,endId,start,end,maxNodeSearch,isSmoothPath);
	}

	return false;
}

bool CNavigationMeshEx::__DoFindPath(CellFind* startCell,CellFind* endCell,unsigned int startId,unsigned int endId,VectorF& start, VectorF& end,int maxNodeSearch,bool isSmoothPath)
{
	//判断start和end是否在同一直线上
	if (startCell == endCell || __IsInLine(start,end))
	{
		//m_Path.push_back(start);
		m_Path.push_back(end);

		return true;
	}
	else
	{
		m_pathInfos.clear();

		if (__BuildNavigationPath(m_Path, endCell, end,endId, startCell, start,startId,maxNodeSearch))
		{
            m_Path.push_back(end);

			if (isSmoothPath)
			{
				m_Path.push_front(start);
				__SmoothPath();
				m_Path.pop_front();
			}

			return true;
		}
	}

	return false;
}

bool CNavigationMeshEx::__DoFindStraightLine(VectorF& start, VectorF& end)
{
	VectorF dir = end - start;
	S32 step = mFloor(dir.len()/NaviGridSize);
	dir.normalize();

	U32 id;
	VectorF tStart = start;
	end = start;

	CNavigationMeshEx::CellFind* pCell = NULL;
	while(step-- > 0)
	{
		tStart += dir * NaviGridSize;
		pCell = __FindCell(tStart,&id);

		if (!pCell)
			break;
		end = tStart;
	}
	m_Path.clear();
	m_Path.push_back(end);

	return true;
}

#define Slerp(a,b,c,d,tx,ty) ((a*(1-ty)+b*ty)*(1-tx)+(d*(1-ty)+c*ty)*tx)

VectorF CNavigationMeshEx::SlerpNormal(VectorF& pos)
{
	if (!Finish())
		return pos;

	VectorF corn[4];

	if (!__GetCorn(pos,corn))
		return pos;
	
	VectorF p = pos - corn[0];;
	F32 tx = p.x / NaviGridSize;
	F32 ty = p.y / NaviGridSize;

	VectorF line0, line1;
	VectorF normal[4];
	int v1, v2;

	for (int i = 0; i < 4; i++)
	{
		v1 = (i + 1) % 4;
		v2 = (i + 3) % 4;
		line0 = corn[v1] - corn[i];
		line1 = corn[v2] - corn[i];
		mCross(line1, line0, &normal[i]);
		normal[i].normalize();
	}

	return Slerp(normal[0], normal[1], normal[2], normal[3], tx, ty);
}

VectorF CNavigationMeshEx::SlerpPosition(VectorF& pos)
{
	if (!Finish())
		return pos;

	VectorF corn[4];

	if (!__GetCorn(pos,corn))
		return pos;

	VectorF p0 = corn[0];
	VectorF p1 = corn[1];
	VectorF p2 = corn[2];
	VectorF p3 = corn[3];

	VectorF p = pos - corn[0];;
	F32 tx = p.x / NaviGridSize;
	F32 ty = p.y / NaviGridSize;

	return Slerp(p0, p1, p2, p3, tx, ty);
}

bool CNavigationMeshEx::CanReach(Point3F& pos)
{
	if (!Finish())
		return false;

	CellFind* startCell = __FindCell(pos);

	return 0 != startCell;
}

CNavigationMeshEx::CellCal* CNavigationMeshEx::FindCell(int id)
{
	CELL_CAL_MAP::iterator iter = m_cellCals.find(id);

	if (iter == m_cellCals.end())
		return 0;

	return &iter->second;
}

void CNavigationMeshEx::DelCell(int id)
{
	m_cellCals.erase(id);
}

void CNavigationMeshEx::InitNavGridFlag(void)
{
	if (0 != m_pGridFlag)
	{
		delete []m_pGridFlag;
	}

	m_pGridFlag = new unsigned int[singleTableSize];
	memset(m_pGridFlag,0,sizeof(unsigned int) * singleTableSize);
}

void CNavigationMeshEx::AddGenCell(unsigned int id,float height)
{
	m_cellFinds.insert(std::make_pair(id,CellFind(height)));
}

CNavigationMeshEx::CellFind* CNavigationMeshEx::GetGenCell(unsigned int id)
{
	CELL_MAP::iterator iter = m_cellFinds.find(id);

	if (iter == m_cellFinds.end())
		return 0;

	return &iter->second;
}

void CNavigationMeshEx::ClrGenCells(void)
{
	m_cellFinds.clear();
}

void CNavigationMeshEx::__Clear(void)
{
	m_cellCals.clear();
	m_cellBases.clear();
	m_cellFinds.clear();
	m_pathInfos.clear();

	if (0 != m_pGridFlag)
	{
		delete []m_pGridFlag;
		m_pGridFlag = 0;
	}
}

static int xoff[4] = {-1, +0, +1, -0};
static int yoff[4] = {-0, +1, +0, -1};

void CNavigationMeshEx::__AddCellCal(int row,int col)
{
	int id = NaviGrid::GridToID(col,row);

	if (id < 0)
		return;

	CELL_CAL_MAP::iterator iter = m_cellCals.find(id);

	if (iter != m_cellCals.end())
		return;

	CellCal& cell = m_cellCals.insert(std::make_pair(id,CellCal())).first->second;

	//链接CELL的关系
	cell.m_Link[0] = cell.m_Link[1] = cell.m_Link[2] = cell.m_Link[3] = 0;

	for (int i = 0; i < 4; i++)
	{
		int idNeighbour = NaviGrid::GridToID(col + xoff[i], row + yoff[i]);

		if (idNeighbour < 0)
			continue;

		iter = m_cellCals.find(idNeighbour);

		if (iter == m_cellCals.end())
			continue;

		int nearSide = (i + 2) % 4;
		iter->second.m_Link[nearSide] = &cell;
		cell.m_Link[i] = &iter->second;
	}
}

CNavigationMeshEx::CellFind* CNavigationMeshEx::__FindCell(const VectorF& point,unsigned int* pID)
{
	int row,col;
	NaviGrid::PosToGrid(point,col,row);

	if (row < 0 || col < 0 || row >= singleTableWidth || col >= singleTableWidth)
	{
		return 0;
	}

	if (0 == m_pGridFlag)
		return 0;

	int id = GetGridFlag(row,col);

	//查找对应的网格
	CELL_MAP::iterator iter = m_cellFinds.find(id);

	if (iter == m_cellFinds.end())
		return 0;

	if (0 != pID)
		*pID = id;

	return &iter->second;
}

#define INLINE_RANGE 30.0f

bool CNavigationMeshEx::__IsInLine(VectorF& start, VectorF& end)
{
	float length = (start - end).len();

	//判断是否在指定范围内
	if(length > INLINE_RANGE)
		return false;

	bool isInline = true;

	VectorF dir = end - start;
	dir.normalize();

	int step = length / NaviGridSize;

	VectorF tStart = start;

	while(step-- > 0)
	{
        VectorF oldStart = tStart;
		tStart += dir * NaviGridSize;

		if (!__FindCell(tStart))
		{
			isInline = false;
			break;
		}

        //判断斜脚处是否可通行
        VectorF c1(oldStart.x,tStart.y,oldStart.z);

        if (!__FindCell(c1))
        {
            isInline = false;
            break;
        }

        VectorF c2(tStart.x,oldStart.y,oldStart.z);

        if (!__FindCell(c2))
        {
            isInline = false;
            break;
        }
	}

	return isInline;
}

CNavigationMeshEx::CellFind* CNavigationMeshEx::__GetEndNearbyCell(VectorF& start, VectorF& end,unsigned int* id,int& step)
{
	VectorF dir = start - end;
	dir.normalize();

	VectorF tStart = end;

	CNavigationMeshEx::CellFind* pCell = 0;

	while(step-- > 0)
	{
		tStart += dir * NaviGridSize;
		pCell = __FindCell(tStart,id);

		if (0 != pCell)
		{
            end = tStart;
			break;
		}
	}

	return pCell;
}

bool CNavigationMeshEx::__GetUnReachableNearbyCell(VectorF& start, VectorF& end,unsigned int* id)
{
    float length = (start - end).len();
    int step = length / NaviGridSize;

    VectorF dir = start - end;
    dir.normalize();

    VectorF tStart = end;

    CNavigationMeshEx::CellFind* pCell = 0;

    while(step-- > 0)
    {
        tStart += dir * NaviGridSize;
        pCell = __FindCell(tStart,id);

        if (0 == pCell)
        {
            end = tStart;
            return true;
        }
    }

    return false;
}

bool CNavigationMeshEx::__BuildNavigationPath(WAYPOINT_LIST& NavPath, CellFind* StartCell, const VectorF& StartPos,unsigned int StartId,CellFind* EndCell, const VectorF& EndPos,unsigned int EndId,int maxNodeSearch)
{
	bool FoundPath = false;

	// Increment our path finding session ID
	// This Identifies each pathfinding session
	// so we do not need to clear out old data
	// in the cells from previous sessions.
	++m_PathSession;

	// load our data into the NavigationHeap object
	// to prepare it for use.
	m_NavHeap.Setup(m_PathSession, StartPos);

	//最大寻找次数
	int nodeSearchCount = maxNodeSearch;

	// We are doing a reverse search, from EndCell to StartCell.
	// Push our EndCell onto the Heap at the first cell to be processed
	EndCell->QueryForPath(this,&m_NavHeap,EndId,0,0,0);

	// process the heap until empty, or a path is found
	while(m_NavHeap.NotEmpty() && !FoundPath)
	{
		if (0 == nodeSearchCount)
		{
			break;
		}
		
		if (nodeSearchCount > 0)
			nodeSearchCount--;

		NavigationNode<CellFind> ThisNode;

		// pop the top cell (the open cell with the lowest cost) off the Heap
		m_NavHeap.GetTop(ThisNode);

		// if this cell is our StartCell, we are done
		if(ThisNode.cell == StartCell)
		{
			FoundPath = true;
		}
		else
		{
			// Process the Cell, Adding it's neighbors to the Heap as needed
			ThisNode.cell->ProcessCell(this,&m_NavHeap,ThisNode.id);
		}
	}

	// if we found a path, build a waypoint list
	// out of the cells on the path
	if (FoundPath)
	{
		unsigned int TestCell = StartId;
		VectorF NewWayPoint;

		// Setup the Path object, clearing out any old data
		//NavPath.push_back(StartPos);

		// Step through each cell linked by our A* algorythm 
		// from StartCell to EndCell
		while (TestCell != EndId)
		{
			NewWayPoint = __GetCenterPoint(TestCell);

			NavPath.push_back(NewWayPoint);

			// and on to the next cell
			PATHINFO_MAP::iterator iter = m_pathInfos.find(TestCell);

			if (iter != m_pathInfos.end())
			{
				TestCell = iter->second.m_ArrivalCell;
			}
			else
			{
				break;
			}
		}

        NavPath.push_back(__GetCenterPoint(EndId));

		// cap the end of the path.
		NavPath.push_back(EndPos);

		//将路径反过来
		NavPath.reverse();
		return(true);
	}

	return(false);
}


bool CNavigationMeshEx::CellFind::QueryForPath(CNavigationMeshEx* parent,NavigationHeap<CellFind>* pHeap,unsigned int ownId,CellFind* Caller,unsigned int fromId,float arrivalcost)
{
	PATHINFO_MAP::iterator iter = parent->m_pathInfos.find(ownId);

	if (iter == parent->m_pathInfos.end())
	{
		iter = parent->m_pathInfos.insert(std::make_pair(ownId,PathInfo())).first;
	}

	PathInfo& cellInfo = iter->second;

	if (cellInfo.m_SessionID != pHeap->SessionID())
	{
		// this is a new session, reset our internal data
		cellInfo.m_SessionID = pHeap->SessionID();

		if (Caller)
		{
			cellInfo.m_Open        = true;
			cellInfo.m_Heuristic   = parent->__ComputeHeuristic(ownId,pHeap->Goal());
			cellInfo.m_ArrivalCost = arrivalcost;

			// remember the side this caller is entering from
			cellInfo.m_ArrivalCell = fromId;
		}
		else
		{
			// we are the cell that contains the starting location
			// of the A* search.
			cellInfo.m_Open        = false;
			cellInfo.m_ArrivalCost = 0;
			cellInfo.m_Heuristic   = 0;
			cellInfo.m_ArrivalCell = 0;
		}

		// add this cell to the Open heap
		pHeap->AddCell(this,ownId,cellInfo.m_ArrivalCost + cellInfo.m_Heuristic);
		return(true);
	}
	else if (cellInfo.m_Open)
	{
		// m_Open means we are already in the Open Heap.
		// If this new caller provides a better path, adjust our data
		// Then tell the Heap to resort our position in the list.
		if ((arrivalcost + cellInfo.m_Heuristic) < (cellInfo.m_ArrivalCost + cellInfo.m_Heuristic))
		{
			cellInfo.m_ArrivalCost = arrivalcost;

			// remember the side this caller is entering from
			cellInfo.m_ArrivalCell = fromId;

			// ask the heap to resort our position in the priority heap
			pHeap->AdjustCell(this,cellInfo.m_ArrivalCost + cellInfo.m_Heuristic);

			return(true);
		}
	}

	// this cell is closed
	return(false);
}

bool CNavigationMeshEx::CellFind::ProcessCell(CNavigationMeshEx* parent,NavigationHeap<CellFind>* pHeap,unsigned ownId)
{
	PATHINFO_MAP::iterator iter = parent->m_pathInfos.find(ownId);

	if (iter == parent->m_pathInfos.end())
	{
		return false;
	}

	PathInfo& cellInfo = iter->second;

	if (cellInfo.m_SessionID == pHeap->SessionID())
	{
		// once we have been processed, we are closed
		cellInfo.m_Open  = false;

		for (size_t i = 0; i < m_neighbours.size(); ++i)
		{
			CellFind* pCell = parent->GetGenCell(m_neighbours[i]);

			if (0 == pCell)
				continue;

			pCell->QueryForPath(parent,pHeap,m_neighbours[i],this,ownId,cellInfo.m_ArrivalCost + NaviGridSize);
		}

		return(true);
	}

	return(false);
}

void _GetIdInfo(unsigned int id,int& layer,int& row,int& col)
{
	layer = (id >> 24);
	row   = ((id & 0x00ffffff) >> 12);
	col   = id & 0x00000fff;
}

float CNavigationMeshEx::__ComputeHeuristic(unsigned int id,const VectorF& goal)
{
	VectorF centerPos = __GetCenterXY(id);

	//return fabs(goal.x - centerPos.x) + fabs(goal.y - centerPos.y);
	return Point2F(goal.x - centerPos.x,goal.y - centerPos.y).len();
}

VectorF CNavigationMeshEx::__GetCenterPoint(unsigned int id)
{
	int   row,col,layer; 
	_GetIdInfo(id,layer,row,col);
	
	int r,c;
	VectorF centerPos = __GetCenterXY(id,&r,&c);

	//查找导航网格
	CELL_MAP::iterator iterFind = m_cellFinds.find(id);

	if (iterFind != m_cellFinds.end())
	{
		if (iterFind->second.IsFlat())
		{
			centerPos.z = iterFind->second.height;
			return centerPos;
		}
	}

	CELL_BASE_MAP::iterator iterBase = m_cellBases.find(NaviGrid::GridToID(c,r));

	if (iterBase != m_cellBases.end())
	{
		centerPos.z = iterBase->second.GetCenterHeight();
		return centerPos;
	}
	
	centerPos.z = 0;
	return centerPos;
}

VectorF CNavigationMeshEx::__GetCenterXY(unsigned int id,int* rr,int* rc)
{
	int   row,col,layer; 
	_GetIdInfo(id,layer,row,col);

	int perWidth = powl(2,NAV_LAYER_COUNT - layer);

	int r	= row * perWidth + (perWidth * 0.5f);
	int c	= col * perWidth + (perWidth * 0.5f);

	if (0 != rr)
		*rr = r;

	if (0 != rc)
		*rc = c;

	VectorF centerPos = NaviGrid::GeneratePosition(c,r);

	centerPos.x += NaviGridSize * 0.5f * ((perWidth % 2) ? 1 : 0);
	centerPos.y += NaviGridSize * 0.5f * ((perWidth % 2) ? 1 : 0);
	centerPos.z = 0;

	return centerPos;
}

bool CNavigationMeshEx::__GetCorn(const VectorF& pos,VectorF* pCorn)
{
	if (0 == pCorn)
		return false;
	
	unsigned int id;
	CellFind* pCell = __FindCell(pos,&id);

	if (0 == pCell)
		return false;
	
	int row,col;
	NaviGrid::PosToGrid(pos,col,row);

	pCorn[0] = pCorn[1] = pCorn[2] = pCorn[3] = NaviGrid::GeneratePosition(col,row);

	static Point2I offset[4] = { Point2I(0,0), Point2I(0,1), Point2I(1,1), Point2I(1,0) };
	
	for (int iCorn = 0; iCorn < 4; ++iCorn)
	{
		pCorn[iCorn].x += offset[iCorn].x * NaviGridSize;
		pCorn[iCorn].y += offset[iCorn].y * NaviGridSize;
	}

	if (pCell->IsFlat())
	{
		pCorn[0].z = pCorn[1].z = pCorn[2].z = pCorn[3].z = pCell->height;
	}
	else
	{
		CELL_BASE_MAP::iterator iterBase = m_cellBases.find(NaviGrid::GridToID(col,row));

		if (iterBase != m_cellBases.end())
		{
			pCorn[0].z = iterBase->second.height[0];
			pCorn[1].z = iterBase->second.height[1];
			pCorn[2].z = iterBase->second.height[2];
			pCorn[3].z = iterBase->second.height[3];
		}
		else
		{
			pCorn[0].z = pCorn[1].z = pCorn[2].z = pCorn[3].z = 0;
		}
	}

	return true;	
}

void CNavigationMeshEx::__SmoothPath(void)
{
	if (m_Path.size() <= 2)
		return;

	bool isSmoothed = false;

	WAYPOINT_LIST::iterator iterStart = m_Path.begin();
	WAYPOINT_LIST::iterator iterEnd   = iterStart;

	for (;iterStart != m_Path.end() && iterEnd != m_Path.end(); )
	{
		iterStart = iterEnd;

		if (iterEnd != m_Path.end())
			++iterEnd;
		
		if (iterEnd != m_Path.end())
			++iterEnd;
			
		if (iterEnd == m_Path.end())
			break;

		if (__IsInLine(*iterStart,*iterEnd))
		{
			isSmoothed = true;
			WAYPOINT_LIST::iterator iterDel = iterStart;
			++iterDel;
			m_Path.erase(iterDel);
		}
	}

	if (isSmoothed)
		__SmoothPath();
}