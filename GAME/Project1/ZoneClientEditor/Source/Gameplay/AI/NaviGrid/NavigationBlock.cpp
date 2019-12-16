
#include <Windows.h>
#include <algorithm>

#include "Gameplay/ai/NaviGrid/NavigationBlock.h"
#include "terrain/terrData.h"
#include "util/triRayCheck.h"
#include "sceneGraph/sceneGraph.h"


NavigationManager *g_NavigationManager = NULL;
NavigationManager gNavigationManager;

F32 NavigationBlock::MAP_OFFSET_X	= SINGLE_MAP_OFFSET_X;
F32 NavigationBlock::MAP_OFFSET_Y	= SINGLE_MAP_OFFSET_X;
U32 NavigationBlock::ROW_SIZE		= SINGLE_ROW_SIZE;
U32 NavigationBlock::COL_SIZE		= SINGLE_COL_SIZE;
U32 NavigationBlock::GRID_NUM		= SINGLE_GRID_NUM;
U32 NavigationBlock::MAX_WIDTH		= 1;
U32 NavigationBlock::MAX_HEIGHT		= 1;

// test
//S64 tStart = 0;
//S64 tEnd = 0;
//S64 tDelta = 0;

//-------------------------------------------------------------------------
// NaviCell
//

NaviCell::NaviCell() :
	m_Idx(0),
	m_SessionID(0),
	m_ArrivalCell(0),
	m_ArrivalCost(0),
	m_Heuristic(0),
	m_Height(0),
	m_HeightTopLeft(0),
	m_HeightTopRight(0),
	m_HeightBottomRight(0),
	m_HeightBottomLeft(0),
	m_Open(false),
	m_Close(false)
{
}

NaviCell::~NaviCell()
{
}

void NaviCell::ComputeHeuristic(const Point3F& Goal)
{
	S32 xIdx = m_Idx%NavigationBlock::COL_SIZE;
	S32 yIdx = m_Idx/NavigationBlock::COL_SIZE;

	Point3F pos((xIdx+0.5f)*GRID_SIZE-NavigationBlock::MAP_OFFSET_X, (yIdx+0.5f)*GRID_SIZE-NavigationBlock::MAP_OFFSET_Y,m_Height);
	m_Heuristic = (pos - Goal).len();
}

Point3F NaviCell::GetCenter()
{
	S32 xIdx = m_Idx%NavigationBlock::COL_SIZE;
	S32 yIdx = m_Idx/NavigationBlock::COL_SIZE;

	return Point3F((xIdx+0.5f)*GRID_SIZE-NavigationBlock::MAP_OFFSET_X, (yIdx+0.5f)*GRID_SIZE-NavigationBlock::MAP_OFFSET_Y,m_Height);
}

U32 NaviCell::GetArrivalDirection()
{
	if(m_Idx < 0 || m_ArrivalCell < 0 || m_Idx == m_ArrivalCell)
		return DirectionUnknown;

	S32 x = m_Idx%NavigationBlock::COL_SIZE;
	S32 y = m_Idx/NavigationBlock::COL_SIZE;
	S32 _x = m_ArrivalCell%NavigationBlock::COL_SIZE;
	S32 _y = m_ArrivalCell/NavigationBlock::COL_SIZE;

	U32 ret = 0;
	if(x > _x)
		ret |= DirectionLeft;
	else if(x < _x)
		ret |= DirectionRight;
	if(y > _y)
		ret |= DirectionBottom;
	else if(y < _y)
		ret |= DirectionTop;

	return ret;
}


//-------------------------------------------------------------------------
// NavigationBlock
//

NavigationBlock::NavigationBlock()
{
	mCellIndex = NULL;
	Clear();
}

NavigationBlock::~NavigationBlock()
{
	SAFE_DELETE(mCellIndex)
}

void NavigationBlock::Clear()
{
	SAFE_DELETE(mCellIndex)
	mCellInfo.clear();
	MAX_WIDTH = 1;
	MAX_HEIGHT = 1;
	mSubInfo.clear();
}

void NavigationBlock::AddCell(U32 idx, U32 count, const F32 height, const F32 tl, const F32 tr, const F32 bl, const F32 br)
{
	AssertFatal(idx >= 0 && idx < GRID_NUM && mCellIndex, "we got a wrong idx!");
	if(count >= mCellInfo.size())
		mCellInfo.setSize(count);
	mCellInfo[count].m_Idx = idx;
	mCellInfo[count].m_Height = height;
	mCellInfo[count].m_HeightTopLeft = tl;
	mCellInfo[count].m_HeightTopRight = tr;
	mCellInfo[count].m_HeightBottomRight = br;
	mCellInfo[count].m_HeightBottomLeft = bl;
	mCellIndex[idx] = count;
}

void NavigationBlock::SetCellSize(U32 size)
{
	mCellInfo.setSize(size);
}

void NavigationBlock::SetGridSize(U32 size)
{
	mCellIndex = new S32[size];
	dMemset(mCellIndex, -1, sizeof(S32)*size);
}

NaviCell* NavigationBlock::GetCell(U32 idx)
{
	AssertFatal(idx >= 0 && idx < GRID_NUM && mCellIndex, "we got a wrong idx!");
	if(mCellIndex[idx] >=0 && mCellIndex[idx] < mCellInfo.size())
		return &mCellInfo[mCellIndex[idx]];
	return NULL;
}

void NavigationBlock::Initialization(U32 singleCol, U32 singleRow, U32 width, U32 height, U32 CellSize)
{
	// 先取得所有地形的统一偏移
#ifndef NTJ_CLIENT
	Vector<TerrainBlock*>& list = gServerSceneGraph->getTerrainList();
#else
	Vector<TerrainBlock*>& list = gClientSceneGraph->getTerrainList();
#endif
	Point3F center(0,0,0);
	for(S32 i=0; i<list.size(); i++)
		center += list[i]->getPosition();
	center = center/getMin(1,list.size());

	Clear();
	COL_SIZE = singleCol * width;
	ROW_SIZE = singleRow * height;
	MAX_WIDTH = width;
	MAX_HEIGHT = height;
	MAP_OFFSET_X = COL_SIZE * GRID_SIZE / 2 - (center.x + SINGLE_MAP_OFFSET_X);
	MAP_OFFSET_Y = ROW_SIZE * GRID_SIZE / 2 - (center.y + SINGLE_MAP_OFFSET_Y);
	GRID_NUM = ROW_SIZE * COL_SIZE;
	mSubInfo.setSize(MAX_WIDTH * MAX_HEIGHT);
	for(int i=0; i<mSubInfo.size(); ++i)
		mSubInfo[i] = 0;
	SetGridSize(GRID_NUM);
	SetCellSize(CellSize);
}

void NavigationBlock::AddToSub(U32 width, U32 height)
{
	AssertFatal(width < MAX_WIDTH && height < MAX_HEIGHT, "error block info");
	++mSubInfo[height * MAX_WIDTH + width];
}

S32 NavigationBlock::GetIndex(const Point3F& pos)
{
	S32 xIdx = (pos.x + MAP_OFFSET_X)/GRID_SIZE;
	S32 yIdx = (pos.y + MAP_OFFSET_Y)/GRID_SIZE;

	if(!(xIdx>=0 && xIdx<COL_SIZE && yIdx>=0 && yIdx<ROW_SIZE))
		return -1;
	return yIdx * COL_SIZE + xIdx;
}

TerrainBlock* NavigationBlock::GetTerrain(const Point3F& pos)
{
	S32 xIdx = (pos.x + MAP_OFFSET_X)/(SINGLE_MAP_OFFSET_X*2);
	S32 yIdx = (pos.y + MAP_OFFSET_Y)/(SINGLE_MAP_OFFSET_X*2);

	return TerrainBlock::getTerMap(xIdx, yIdx);
}

bool NavigationBlock::CanReach(U32 idx)
{
	if(idx >= GRID_NUM || !mCellIndex)
		return false;

	return mCellIndex[idx] >= 0;
}

bool NavigationBlock::castRay(U32 idx, const Point3F& start, const Point3F& end, Point3F& pos)
{
	AssertFatal(idx < GRID_NUM && mCellIndex,"get a wrong pos here!");

	if(mCellIndex[idx] < 0)
		return false;

	NaviCell& cell = mCellInfo[mCellIndex[idx]];
	static Point3F pointA,pointB,pointC,pointD;
	static F32 t,u,v;

	S32 xIdx = idx%COL_SIZE;
	S32 yIdx = idx/COL_SIZE;
	pointA.set(xIdx*GRID_SIZE-MAP_OFFSET_X, yIdx*GRID_SIZE-MAP_OFFSET_Y, cell.m_HeightTopLeft);
	pointB.set((xIdx+1)*GRID_SIZE-MAP_OFFSET_X, yIdx*GRID_SIZE-MAP_OFFSET_Y, cell.m_HeightTopRight);
	pointC.set((xIdx+1)*GRID_SIZE-MAP_OFFSET_X, (yIdx+1)*GRID_SIZE-MAP_OFFSET_Y, cell.m_HeightBottomRight);
	pointD.set(xIdx*GRID_SIZE-MAP_OFFSET_X, (yIdx+1)*GRID_SIZE-MAP_OFFSET_Y, cell.m_HeightBottomLeft);
	// t:射线到交点的距离，u:交点在v1-v0的投影，v:交点在v2-v0的投影
	if(intersect_triangle(start, end-start, pointD, pointA, pointB, t, u, v))
	{
		pos = start + (end-start)*t;
		//pos = pointD + u*(pointA - pointD) + v*(pointB - pointD);
		return true;
	}
	if(intersect_triangle(start, end-start, pointB, pointC, pointD, t, u, v))
	{
		pos = start + (end-start)*t;
		//pos = pointB + u*(pointC - pointB) + v*(pointD - pointB);
		return true;
	}
	return false;
}

#define Slerp(a,b,c,d,tx,ty) ((a*(1-ty)+b*ty)*(1-tx)+(d*(1-ty)+c*ty)*tx)
VectorF NavigationBlock::SlerpNormal(VectorF& pos)
{
	S32 idx = GetIndex(pos);
	if(mCellIndex[idx] < 0)
		return pos;

	NaviCell& cell = mCellInfo[mCellIndex[idx]];
	static Point3F point[4];

	S32 xIdx = idx%COL_SIZE;
	S32 yIdx = idx/COL_SIZE;
	point[0].set(xIdx*GRID_SIZE-MAP_OFFSET_X, yIdx*GRID_SIZE-MAP_OFFSET_Y, cell.m_HeightTopLeft);
	point[1].set(xIdx*GRID_SIZE-MAP_OFFSET_X, (yIdx+1)*GRID_SIZE-MAP_OFFSET_Y, cell.m_HeightBottomLeft);
	point[2].set((xIdx+1)*GRID_SIZE-MAP_OFFSET_X, (yIdx+1)*GRID_SIZE-MAP_OFFSET_Y, cell.m_HeightBottomRight);
	point[3].set((xIdx+1)*GRID_SIZE-MAP_OFFSET_X, yIdx*GRID_SIZE-MAP_OFFSET_Y, cell.m_HeightTopRight);

	static VectorF p = pos - point[0];
	F32 tx = p.x / GRID_SIZE;
	F32 ty = p.y / GRID_SIZE;

	static VectorF line0, line1;
	static VectorF normal[4];
	int v1, v2;
	for (int i = 0; i < 4; i++)
	{
		v1 = (i + 1) % 4;
		v2 = (i + 3) % 4;
		line0 = point[v1] - point[i];
		line1 = point[v2] - point[i];
		mCross(line1, line0, &normal[i]);
		normal[i].normalize();
	}

	return Slerp(normal[0], normal[1], normal[2], normal[3], tx, ty);
}

VectorF NavigationBlock::SlerpPosition(VectorF& pos)
{
	S32 idx = GetIndex(pos);
	if(mCellIndex[idx] < 0)
		return pos;

	NaviCell& cell = mCellInfo[mCellIndex[idx]];
	static Point3F point[4];

	S32 xIdx = idx%COL_SIZE;
	S32 yIdx = idx/COL_SIZE;
	point[0].set(xIdx*GRID_SIZE-MAP_OFFSET_X, yIdx*GRID_SIZE-MAP_OFFSET_Y, cell.m_HeightTopLeft);
	point[1].set(xIdx*GRID_SIZE-MAP_OFFSET_X, (yIdx+1)*GRID_SIZE-MAP_OFFSET_Y, cell.m_HeightBottomLeft);
	point[2].set((xIdx+1)*GRID_SIZE-MAP_OFFSET_X, (yIdx+1)*GRID_SIZE-MAP_OFFSET_Y, cell.m_HeightBottomRight);
	point[3].set((xIdx+1)*GRID_SIZE-MAP_OFFSET_X, yIdx*GRID_SIZE-MAP_OFFSET_Y, cell.m_HeightTopRight);

	VectorF p = pos - point[0];
	F32 tx = p.x / GRID_SIZE;
	F32 ty = p.y / GRID_SIZE;

	return Slerp(point[0], point[1], point[2], point[3], tx, ty);
}

bool NavigationBlock::Save()
{
	return true;

	//char fName[256];

	//// 遍历小块block
	//for (U32 width = 0; width < MAX_WIDTH; ++width)
	//{
	//	for (U32 height = 0; height < MAX_HEIGHT; ++height)
	//	{
	//		// 取得相对应的地形
	//		TerrainBlock* tblock = TerrainBlock::getTerMap(width,height);
	//		if(!tblock)
	//			continue;
	//		// 确定文件名
	//		dSprintf(fName, sizeof(fName), tblock->mTerrFileName);
	//		char *ext = dStrrchr(fName, '.');
	//		*ext = 0;
	//		dStrcat(fName, sizeof(fName), ".nb");

	//		// 打开文件用于写入地形数据
	//		FileStream file;
	//		if (!file.open(fName, FileStream::Write))
	//			continue;

	//		// 写入版本号
	//		int ver = NavigationBlockVer;
	//		file.write(ver);

	//		// 该小块地形的网格行列数
	//		U32 singleCol = COL_SIZE/MAX_WIDTH;
	//		U32 singleRow = ROW_SIZE/MAX_HEIGHT;
	//		file.write(singleCol);
	//		file.write(singleRow);

	//		// 该小块地形的可行网格总数
	//		file.write(mSubInfo[height * MAX_WIDTH + width]);

	//		for (int i = singleCol * width; i < singleCol * (width + 1); ++i)
	//		{
	//			for (int j = singleRow * height; j < singleRow * (height + 1); ++j)
	//			{
	//				file.write(bool(mCellIndex[j * COL_SIZE + i] != -1));
	//				if(mCellIndex[j * COL_SIZE + i] != -1)
	//				{
	//					file.write(mCellInfo[mCellIndex[j * COL_SIZE + i]].m_Height);
	//					file.write(mCellInfo[mCellIndex[j * COL_SIZE + i]].m_HeightTopLeft);
	//					file.write(mCellInfo[mCellIndex[j * COL_SIZE + i]].m_HeightTopRight);
	//					file.write(mCellInfo[mCellIndex[j * COL_SIZE + i]].m_HeightBottomRight);
	//					file.write(mCellInfo[mCellIndex[j * COL_SIZE + i]].m_HeightBottomLeft);
	//				}
	//			}
	//		}
	//		file.close();
	//	}
	//}

	//return true;
}

bool NavigationBlock::Load()
{
	char fName[256];
	bool bInit = false;

	// 小块block数量
	MAX_WIDTH = TerrainBlock::getMaxTerrainWidth();
	MAX_HEIGHT = TerrainBlock::getMaxTerrainHeight();

	U32 sumCount = 0;

	// 遍历小块block
	for (U32 width = 0; width < MAX_WIDTH; ++width)
	{
		for (U32 height = 0; height < MAX_HEIGHT; ++height)
		{
			// 取得相对应的地形
			TerrainBlock* tblock = TerrainBlock::getTerMap(width,height);
			if(!tblock)
				continue;
			// 确定文件名
			dSprintf(fName, sizeof(fName), tblock->mTerrFileName);
			char *ext = dStrstr(fName, ".ter");
			*ext = 0;
			dStrcat(fName, sizeof(fName), ".nb");

			FileStream file;
			if (!file.open(fName, FileStream::Read))
				continue;

			// 读取版本号
			int ver;
			file.read(&ver);

			// 该小块地形的网格行列数和可行网格总数
			U32 singleCol, singleRow,subCount;
			file.read(&singleCol);
			file.read(&singleRow);
			file.read(&subCount);

			if(!bInit)
			{
				Initialization(singleCol, singleRow, MAX_WIDTH, MAX_HEIGHT, 0);
				bInit = true;
			}
			mSubInfo[height * MAX_HEIGHT + width] = subCount;

			bool temp;
			F32 tempHeight,tl,tr,br,bl;
			U32 count = sumCount;
			sumCount += subCount;
			SetCellSize(sumCount);
			for (int i = singleCol * width; i < singleCol * (width + 1); ++i)
			{
				for (int j = singleRow * height; j < singleRow * (height + 1) && count < sumCount; ++j)
				{
					file.read(&temp);
					if(temp)
					{
						file.read(&tempHeight);
						file.read(&tl);
						file.read(&tr);
						file.read(&br);
						file.read(&bl);
						AddCell(j * COL_SIZE + i, count++, tempHeight, tl, tr, br, bl);
					}
				}
			}
			file.close();
		}
	}

	return true;
}


//-------------------------------------------------------------------------
// NavigationManager
//

NavigationManager::NavigationManager()
{
	g_NavigationManager = this;
	mOpenList.clear();
	m_PathSession = 0;
	mGoal.set(0,0,0);
}

NavigationManager::~NavigationManager()
{
	mOpenList.clear();
}

NavigationBlock& NavigationManager::GetBlock()
{
	return mBlock;
}

bool NavigationManager::FindPath(std::list<Point3F>& Waypointlist, VectorF& start, VectorF& end)
{
	if (mIsNewVersion)
	{
		bool rslt = mNavMesh.FindPath(start,end);
		
		if (rslt)
			Waypointlist = mNavMesh.GetPath();

		return rslt;
	}
	else
	{
		U32 StartIdx = mBlock.GetIndex(start);
		U32 EndIdx = mBlock.GetIndex(end);

		if(StartIdx == -1 || !mBlock.CanReach(StartIdx))
		{
			F32 xOff = mFmod(start.x,GRID_SIZE);
			F32 yOff = mFmod(start.y,GRID_SIZE);
			if(xOff < 0)
				xOff += GRID_SIZE;
			if(yOff < 0)
				yOff += GRID_SIZE;

			F32 xPri = getMin(xOff, GRID_SIZE-xOff);
			F32 yPri = getMin(yOff, GRID_SIZE-yOff);

			Point3F nearBy1 = start;
			Point3F nearBy2 = start;
			if(xPri < yPri)
			{
				nearBy1.x += (xOff > (GRID_SIZE/2) ? GRID_SIZE : -GRID_SIZE);
				nearBy2.y += (yOff > (GRID_SIZE/2) ? GRID_SIZE : -GRID_SIZE);
			}
			else
			{
				nearBy1.y += (yOff > (GRID_SIZE/2) ? GRID_SIZE : -GRID_SIZE);
				nearBy2.x += (xOff > (GRID_SIZE/2) ? GRID_SIZE : -GRID_SIZE);
			}

			start = nearBy1;
			StartIdx = mBlock.GetIndex(start);
			if(StartIdx == -1 || !mBlock.CanReach(StartIdx))
			{
				start = nearBy2;
				StartIdx = mBlock.GetIndex(start);
				if(StartIdx == -1 || !mBlock.CanReach(StartIdx))
					return false;
			}
		}

		if(EndIdx == -1 || !mBlock.CanReach(EndIdx))
		{
			// 直线测试
			static Point3F tempPos;
			//AssertFatal(!LineOfSightTest(start, end, tempPos),"End pos is invalid, but Line test is successful !");
			if(LineOfSightTest(start, end, tempPos))
			{
				Waypointlist.push_back(end);
				return true;
			}

			// 看目标点是否比现在的点更接近终点
			if(mDot(start - end, start - tempPos) < 0.0f)
				return false;

			end.set(tempPos);
			Waypointlist.push_back(end);
			return true;
		}

		// 交换起点终点
		Setup(end,start);
		if(BuildNavigationPath(Waypointlist,EndIdx,StartIdx))
		{
			return true;
		}
		return false;
	}
}

bool NavigationManager::BuildNavigationPath(std::list<Point3F>& Waypointlist, U32 StartIdx, U32 EndIdx)
{
	//tDelta = 0;
	//QueryPerformanceCounter((LARGE_INTEGER *) &tStart);
	bool FoundPath = false;
	static Point3F tmpPos;

	QueryForPath(StartIdx,StartIdx,0);

	NaviCell* pCell = NULL;
	while(mOpenList.size() && !FoundPath)
	{
		// pop the top cell (the open cell with the lowest cost) off the Heap
		GetTop(pCell);

		// if this cell is our StartCell, we are done
		if(pCell->m_Idx == EndIdx)
		{
			FoundPath = true;
		}
		else
		{
			// Process the Cell, Adding it's neighbors to the Heap as needed
			ProcessCell(pCell);
		}
	}
	//QueryPerformanceCounter((LARGE_INTEGER *) &tEnd);
	//tDelta += tEnd - tStart;

	if(FoundPath)
	{
		// 搜索拐点 消除同一直线上多余的结点
		Waypointlist.push_back(mGoal);
		pCell = mBlock.GetCell(pCell->m_ArrivalCell);
		U32 lastDirection = pCell->GetArrivalDirection();
		U32 Direction = 0;
		for( ; pCell && pCell->m_Idx != StartIdx; pCell = mBlock.GetCell(pCell->m_ArrivalCell))
		{
			Direction = pCell->GetArrivalDirection();
			if(lastDirection != Direction)
			{
				Waypointlist.push_back(pCell->GetCenter());
				lastDirection = Direction;
			}
		}
		if(pCell && pCell->m_Idx == StartIdx)
			Waypointlist.push_back(mOrigin);
		else
			return false;

		// 消除多余拐点
		if (Waypointlist.size() > 2)
		{
			std::list<Point3F>::iterator itStart = Waypointlist.begin();		
			while (itStart != Waypointlist.end())
			{
				std::list<Point3F>::iterator itTest = itStart;
				itTest++;
				if(itTest != Waypointlist.end())
					itTest++;	// itTest == Waypointlist.end()时 ++itTest会等于Waypointlist.begin()
				while (itTest != Waypointlist.end() && itTest != Waypointlist.begin())
				{
					if (LineOfSightTest(*itStart, *itTest, tmpPos))
					{
						Waypointlist.erase(--itTest);
						itTest = itStart;
						itTest++;
						itTest++;
					}
					else
						break;
				}
				itStart++;
			}
		}

		Waypointlist.pop_front();
		return true;
	}

	return false;
}

void NavigationManager::Setup(const Point3F& origin, const Point3F& goal)
{
	mOrigin = origin;
	mGoal = goal;
	++m_PathSession;
	mOpenList.clear();
}

bool NavigationManager::QueryForPath(U32 Idx, U32 callerIdx, F32 ArrivalCost)
{
	NaviCell* pCell = mBlock.GetCell(Idx);
	if(!pCell)
		return false;

	if (m_PathSession != pCell->m_SessionID)
	{
		// this is a new session, reset our internal data
		pCell->m_SessionID = m_PathSession;

		pCell->m_Open  = true;
		pCell->ComputeHeuristic(mGoal);
		pCell->m_ArrivalCost = ArrivalCost;

		// remember the side this caller is entering from
		pCell->m_ArrivalCell = callerIdx;

		AddCell(pCell);
		return(true);
	}
	else if (pCell->m_Open)
	{
		// m_Open means we are already in the Open Heap.
		// If this new caller provides a better path, adjust our data
		// Then tell the Heap to resort our position in the list.
		if ((ArrivalCost + pCell->m_Heuristic + 0.01f) < (pCell->m_ArrivalCost + pCell->m_Heuristic))
		{
			pCell->m_ArrivalCost = ArrivalCost;

			// remember the side this caller is entering from
			pCell->m_ArrivalCell = callerIdx;

			// ask the heap to resort our position in the priority heap
			AdjustCell(pCell);
			return(true);
		}
	}
	// this cell is closed
	return(false);
}

void NavigationManager::ProcessCell(NaviCell* pCell)
{
	if (m_PathSession == pCell->m_SessionID)
	{
		// once we have been processed, we are closed
		pCell->m_Open  = false;

		S32 xIdx = pCell->m_Idx%NavigationBlock::COL_SIZE;
		S32 yIdx = pCell->m_Idx/NavigationBlock::COL_SIZE;

		// querry all our neigbors to see if they need to be added to the Open heap
		for (S32 i = xIdx-1; i <= xIdx+1; i+=2)		// 左右
			if(i>=0 && i<NavigationBlock::COL_SIZE)
				QueryForPath(yIdx*NavigationBlock::COL_SIZE + i,pCell->m_Idx, pCell->m_ArrivalCost + GRID_SIZE);
		for (S32 i = yIdx-1; i <= yIdx+1; i+=2)		// 上下
			if(i>=0 && i<NavigationBlock::ROW_SIZE)
				QueryForPath(i*NavigationBlock::COL_SIZE + xIdx,pCell->m_Idx, pCell->m_ArrivalCost + GRID_SIZE);

		//FOR TEST 测试对角的单元
		for (S32 i = xIdx-1; i <= xIdx+1; i+=2)
			for (S32 j = yIdx-1; j <= yIdx+1; j+=2)
				if(i>=0 && i<NavigationBlock::COL_SIZE && j>=0 && j<NavigationBlock::ROW_SIZE
				&& mBlock.CanReach(j*NavigationBlock::COL_SIZE + xIdx) && mBlock.CanReach(yIdx*NavigationBlock::COL_SIZE + i))
					QueryForPath(j*NavigationBlock::COL_SIZE + i,pCell->m_Idx, pCell->m_ArrivalCost + GRID_SIZE*M_SQRT2_F);
	}
}

// 该函数需要更详细的测试!
bool NavigationManager::LineOfSightTest(Point3F& start, Point3F& end, Point3F& pos)
{
	if (mIsNewVersion)
		return false;

	S32 sx = mClamp((start.x + NavigationBlock::MAP_OFFSET_X)/GRID_SIZE, 0, NavigationBlock::ROW_SIZE-1);
	S32 sy = mClamp((start.y + NavigationBlock::MAP_OFFSET_Y)/GRID_SIZE, 0, NavigationBlock::ROW_SIZE-1);
	S32 ex = mClamp((end.x + NavigationBlock::MAP_OFFSET_X)/GRID_SIZE, 0, NavigationBlock::ROW_SIZE-1);
	S32 ey = mClamp((end.y + NavigationBlock::MAP_OFFSET_Y)/GRID_SIZE, 0, NavigationBlock::ROW_SIZE-1);

	S32 xLast = -1;
	S32 yLast = -1;
	S32 xOffset = 0;
	S32 yOffset = 0;
	if(sx > ex)
		xOffset = -1;
	else if(sx < ex)
		xOffset = 1;
	if(sy > ey)
		yOffset = -1;
	else if(sy < ey)
		yOffset = 1;

	// 两点下x坐标相近，这里可能有细微的误差
	if(sx == ex || mFabs(start.x - end.x) < 0.00001f)
	{
		for (S32 j=sy; (yOffset>0 ? j<=ey : j>=ey); j+=yOffset)
		{
			if(!mBlock.CanReach(j * NavigationBlock::COL_SIZE + sx))
			{
				if(yLast != -1)
				{
					NaviCell* pCell = mBlock.GetCell(yLast * NavigationBlock::COL_SIZE + sx);
					if(pCell)
						pos.set(pCell->GetCenter());
				}
				else
					pos = start;
				return false;
			}
			else
				yLast = j;
		}
		return true;
	}
	// 以下xOffset不可能为零
	AssertFatal(xOffset, "NavigationManager::castRay!");

	S32 yStart = sy;
	S32 yEnd = sy;

	F32 k = (start.y - end.y)/(start.x - end.x);	// 斜率
	F32 constant = (start.y + NavigationBlock::MAP_OFFSET_Y) - k * (start.x + NavigationBlock::MAP_OFFSET_X);			// 常数

	for (S32 i=sx; (xOffset>0 ? i<=ex : i>=ex); i+=xOffset)
	{
		if(i >= NavigationBlock::COL_SIZE || i < 0)
			continue;
		yEnd = (k * (i + (xOffset>0 ? 1 : 0)) * GRID_SIZE + constant) / GRID_SIZE;
		for (S32 j = yStart; (yOffset>0 ? j<=yEnd : j>=yEnd); j+=yOffset)
		{
			if(j >= NavigationBlock::ROW_SIZE || j < 0)
			{
				if(0 == yOffset)// yOffset为零直接跳出
					break;
				continue;
			}
			if(!mBlock.CanReach(j * NavigationBlock::COL_SIZE + i))
			{
				if(xLast != -1 && yLast != -1)
				{
					NaviCell* pCell = mBlock.GetCell(yLast * NavigationBlock::COL_SIZE + xLast);
					if(pCell)
						pos.set(pCell->GetCenter());
				}
				else
					pos = start;
				return false;
			}
			else
			{
				xLast = i;
				yLast = j;
			}
			if(0 == yOffset)	// yOffset为零直接跳出
				break;
		}
		yStart = yEnd;
	}

	return true;
}

bool NavigationManager::LineOfSightTestWithWidth(Point3F& start, Point3F& end, float width)
{
	if (mIsNewVersion)
		return false;

	Point3F offset(0,0,0);
	static Point3F temPos;
	offset.x = start.y - end.y;
	offset.y = start.x - end.x;
	offset.normalize(width);

	return LineOfSightTest(start,end,temPos) && LineOfSightTest(start + offset,end + offset,temPos) && LineOfSightTest(start - offset,end - offset,temPos);
}

bool NavigationManager::castRay(Point3F& start, Point3F& end, Point3F& pos)
{
	if (mIsNewVersion)
		return false;

	S32 sx = mClamp((start.x + NavigationBlock::MAP_OFFSET_X)/GRID_SIZE, 0, NavigationBlock::ROW_SIZE-1);
	S32 sy = mClamp((start.y + NavigationBlock::MAP_OFFSET_Y)/GRID_SIZE, 0, NavigationBlock::ROW_SIZE-1);
	S32 ex = mClamp((end.x + NavigationBlock::MAP_OFFSET_X)/GRID_SIZE, 0, NavigationBlock::ROW_SIZE-1);
	S32 ey = mClamp((end.y + NavigationBlock::MAP_OFFSET_Y)/GRID_SIZE, 0, NavigationBlock::ROW_SIZE-1);

	S32 xOffset = 0;
	S32 yOffset = 0;
	if(sx > ex)
		xOffset = -1;
	else if(sx < ex)
		xOffset = 1;
	if(sy > ey)
		yOffset = -1;
	else if(sy < ey)
		yOffset = 1;

	// 两点下x坐标相近，这里可能有细微的误差
	if(sx == ex || mFabs(start.x - end.x) < 0.00001f)
	{
		for (S32 j=sy; (yOffset>0 ? j<=ey : j>=ey); j+=yOffset)
		{
			if(mBlock.castRay(j * NavigationBlock::COL_SIZE + sx, start, end, pos))
				return true;
		}
		return false;
	}
	// 以下xOffset不可能为零
	AssertFatal(xOffset, "NavigationManager::castRay!");

	S32 yStart = sy;
	S32 yEnd = sy;

	F32 k = (start.y - end.y)/(start.x - end.x);	// 斜率
	F32 constant = (start.y + NavigationBlock::MAP_OFFSET_Y) - k * (start.x + NavigationBlock::MAP_OFFSET_X);			// 常数

	for (S32 i=sx; (xOffset>0 ? i<=ex : i>=ex); i+=xOffset)
	{
		if(i >= NavigationBlock::COL_SIZE || i < 0)
			continue;
		yEnd = (k * (i + (xOffset>0 ? 1 : 0)) * GRID_SIZE + constant) / GRID_SIZE;
		for (S32 j = yStart; (yOffset>0 ? j<=yEnd : j>=yEnd); j+=yOffset)
		{
			if(j >= NavigationBlock::ROW_SIZE || j < 0)
			{
				if(0 == yOffset)// yOffset为零直接跳出
					break;
				continue;
			}
			if(mBlock.castRay(j * NavigationBlock::COL_SIZE + i, start, end, pos))
				return true;
			if(0 == yOffset)	// yOffset为零直接跳出
				break;
		}
		yStart = yEnd;
	}

	return false;
}

void NavigationManager::AddCell(NaviCell* Cell)
{
	//mOpenList.insert(OpenListType(Cell->m_ArrivalCost + Cell->m_Heuristic, Cell));
	mOpenList.push_back(Cell);
	std::push_heap( mOpenList.begin(), mOpenList.end(), CellCmp );
}

void NavigationManager::AdjustCell(NaviCell* Cell)
{
	Container::iterator iter = mOpenList.begin();
	for( ; iter != mOpenList.end(); ++iter )
	{
		if( *iter == Cell )
			break;
	}
	if ( iter == mOpenList.end() )
		return;

	// reorder the heap
	std::push_heap( mOpenList.begin(), iter+1, CellCmp );
}

void NavigationManager::GetTop(NaviCell*& Cell)
{
	Cell = mOpenList.front();
	std::pop_heap( mOpenList.begin(), mOpenList.end(), CellCmp );
	mOpenList.pop_back();
}

bool NavigationManager::Save()
{
	return mBlock.Save();
}

bool NavigationManager::Load()
{
	mIsNewVersion = false;

	// 确定文件名
	char fName[256] = {0};
	const char *pMissionFile = Con::getVariable("$Server::MissionFile");
	if(!pMissionFile || !pMissionFile[0])
		return false;
	dSprintf(fName, sizeof(fName), pMissionFile);
	char *ext = dStrstr(fName, ".mis");
	*ext = 0;
	dStrcat(fName, sizeof(fName), ".ng");

	Con::printf("loading navigation mesh %s",fName);

	if (mNavMesh.Open(fName,true))
	{
		mIsNewVersion = true;
		Con::printf("navigation mesh %s loaded",fName);
		return true;
	}
	else if (mBlock.Load())
	{
		Con::printf("navigation mesh %s loaded",fName);
		return true;
	}

	return false;
}

bool NavigationManager::CanReach(Point3F& pos)
{
	if (mIsNewVersion)
	{
		return mNavMesh.CanReach(pos);
	}
	else
	{
		S32 index = GetBlock().GetIndex(pos);
		return (index != -1 && GetBlock().CanReach(index));
	}
}

VectorF	NavigationManager::SlerpNormal(VectorF& pos)
{
	if (mIsNewVersion)
	{
		return mNavMesh.SlerpNormal(pos);
	}
	else
	{
		return GetBlock().SlerpNormal(pos);
	}
}

VectorF NavigationManager::SlerpPosition(VectorF& pos)
{
	if (mIsNewVersion)
	{
		return mNavMesh.SlerpPosition(pos);
	}
	else
	{
		return GetBlock().SlerpPosition(pos);
	}
}

