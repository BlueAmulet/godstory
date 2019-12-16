//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "gui/core/guiCanvas.h"
#include "console/consoleTypes.h"
#include "gui/missionEditor/terrainActions.h"
#include "sim/netConnection.h"
#include "core/frameAllocator.h"
#include "gfx/primBuilder.h"
#include "console/simEvents.h"
#include "interior/interiorInstance.h"
#pragma message(ENGINE(新增导航网格))
#include "Gameplay/ai/NaviGrid/NavigationCellMgr.h"
#include "Gameplay/ai/NaviGrid/NavigationBlock.h"
#include "collision\boxConvex.h"
#include "collision\earlyOutPolyList.h"
#include "sceneGraph\lightInfo.h"
//#define DEBUG_LMNM
#ifdef DEBUG_LMNM
#include "ts/tsLogicMarkNode.h"
#endif
#include "gfx/D3D9/gfxD3D9StateBlock.h"
//===================================================================================
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
//#define STATEBLOCK
GFXStateBlock* TerrainEditor::mSetSelSB = NULL;
GFXStateBlock* TerrainEditor::mSetBordSB = NULL;
GFXStateBlock* TerrainEditor::mCullCCWSB = NULL;
GFXStateBlock* TerrainEditor::mCullStoreSB = NULL;
GFXStateBlock* TerrainEditor::mSetNavSB = NULL;
GFXStateBlock* TerrainEditor::mAlphaBlendFalseSB = NULL;
GFXStateBlock* TerrainEditor::mZTrueSB = NULL;
GFXStateBlock* TerrainEditor::mZFalseSB = NULL;

const int NAVIGRID_COLLISION_MASK = (StaticShapeObjectType | TerrainObjectType | InteriorObjectType /*| ForceFieldObjectType*/ |StaticTSObjectType/*|TransparencyObjectType*/);
const int NAVIGRID_NO_TERRAIN_MASK = (StaticShapeObjectType | InteriorObjectType /*| ForceFieldObjectType*/ |StaticTSObjectType/*|TransparencyObjectType*/);

IMPLEMENT_CONOBJECT(TerrainEditor);

Selection::Selection() :
   Vector<GridInfo>(__FILE__, __LINE__),
   mName(0),
   mUndoFlags(0),
   mHashListSize(1024)
{
   VECTOR_SET_ASSOCIATION(mHashLists);

   // clear the hash list
   mHashLists.setSize(mHashListSize);
   reset();
}

Selection::~Selection()
{
}

void Selection::reset()
{
   for(U32 i = 0; i < mHashListSize; i++)
      mHashLists[i] = -1;
   clear();
}

bool Selection::validate()
{
   // scan all the hashes and verify that the heads they point to point back to them
   U32 hashesProcessed = 0;
   for(U32 i = 0; i < mHashLists.size(); i++)
   {
      U32 entry = mHashLists[i];
      if(entry == -1)
         continue;
      
      GridInfo info = (*this)[entry];
      U32 hashIndex = getHashIndex(info.mGridPoint.gridPos);
      
      if( entry != mHashLists[hashIndex] )
      {
         AssertFatal(false, "Selection hash lists corrupted");
         return false;
      }
      hashesProcessed++;
   }

   // scan all the entries and verify that anything w/ a prev == -1 is correctly in the hash table
   U32 headsProcessed = 0;
   for(U32 i = 0; i < size(); i++)
   {
      GridInfo info = (*this)[i];
      if(info.mPrev != -1)
         continue;

      U32 hashIndex = getHashIndex(info.mGridPoint.gridPos);

      if(mHashLists[hashIndex] != i)
      {
         AssertFatal(false, "Selection list heads corrupted");       
         return false;
      }
      headsProcessed++;
   }
   AssertFatal(headsProcessed == hashesProcessed, "Selection's number of hashes and number of list heads differ.");
   return true;
}

U32 Selection::getHashIndex(const Point2I & pos)
{
   Point2F pnt = Point2F((F32)pos.x, (F32)pos.y) + Point2F(1.3f,3.5f);
   return( (U32)(mFloor(mHashLists.size() * mFmod(pnt.len() * 0.618f, 1.0f))) );
}

S32 Selection::lookup(const Point2I & pos)
{
   U32 index = getHashIndex(pos);

   S32 entry = mHashLists[index];

   while(entry != -1)
   {
      if((*this)[entry].mGridPoint.gridPos == pos)
         return(entry);

      entry = (*this)[entry].mNext;
   }

   return(-1);
}

void Selection::insert(GridInfo info)
{
   //validate();
   // get the index into the hash table
   U32 index = getHashIndex(info.mGridPoint.gridPos);

   // if there is an existing linked list, make it our next
   info.mNext = mHashLists[index];
   info.mPrev = -1;

   // if there is an existing linked list, make us it's prev
   U32 indexOfNewEntry = size();
   if(info.mNext != -1)
      (*this)[info.mNext].mPrev = indexOfNewEntry;

   // the hash table holds the heads of the linked lists. make us the head of this list.
   mHashLists[index] = indexOfNewEntry;

   // copy us into the vector
   push_back(info);
   //validate();
}

bool Selection::remove(const GridInfo &info)
{
   if(size() < 1)
      return false;

   //AssertFatal( validate(), "Selection hashLists corrupted before Selection.remove()");

   U32 hashIndex = getHashIndex(info.mGridPoint.gridPos);
   S32 listHead = mHashLists[hashIndex];
   //AssertFatal(listHead < size(), "A Selection's hash table is corrupt.");

   if(listHead == -1)
      return(false);

   const S32 victimEntry = lookup(info.mGridPoint.gridPos);
   if( victimEntry == -1 )
      return(false);

   const GridInfo victim = (*this)[victimEntry];
   const S32 vicPrev = victim.mPrev;
   const S32 vicNext = victim.mNext;
      
   // remove us from the linked list, if there is one.
   if(vicPrev != -1)
      (*this)[vicPrev].mNext = vicNext;
   if(vicNext != -1)
      (*this)[vicNext].mPrev = vicPrev;
   
   // if we were the head of the list, make our next the new head in the hash table.
   if(vicPrev == -1)
      mHashLists[hashIndex] = vicNext;

   // if we're not the last element in the vector, copy the last element to our position.
   if(victimEntry != size() - 1)
   {
      // copy last into victim, and re-cache next & prev
      const GridInfo lastEntry = last();
      const S32 lastPrev = lastEntry.mPrev;
      const S32 lastNext = lastEntry.mNext;
      (*this)[victimEntry] = lastEntry;
      
      // update the new element's next and prev, to reestablish it in it's linked list.
      if(lastPrev != -1)
         (*this)[lastPrev].mNext = victimEntry;
      if(lastNext != -1)
         (*this)[lastNext].mPrev = victimEntry;

      // if it was the head of it's list, update the hash table with its new position.
      if(lastPrev == -1)
      {
         const U32 lastHash = getHashIndex(lastEntry.mGridPoint.gridPos);
         AssertFatal(mHashLists[lastHash] == size() - 1, "Selection hashLists corrupted during Selection.remove() (oldmsg)");
         mHashLists[lastHash] = victimEntry;
      }
   }
   
   // decrement the vector, we're done here
   pop_back();
   //AssertFatal( validate(), "Selection hashLists corrupted after Selection.remove()");
   return true;
}

// add unique grid info into the selection - test uniqueness by grid position
bool Selection::add(const GridInfo &info)
{
   S32 index = lookup(info.mGridPoint.gridPos);
   if(index != -1)
      return(false);

   insert(info);
   return(true);
}

bool Selection::getInfo(Point2I pos, GridInfo & info)
{

   S32 index = lookup(pos);
   if(index == -1)
      return(false);

   info = (*this)[index];
   return(true);
}

bool Selection::setInfo(GridInfo & info)
{
   S32 index = lookup(info.mGridPoint.gridPos);
   if(index == -1)
      return(false);

   S32 next = (*this)[index].mNext;
   S32 prev = (*this)[index].mPrev;

   (*this)[index] = info;
   (*this)[index].mNext = next;
   (*this)[index].mPrev = prev;

   return(true);
}

F32 Selection::getAvgHeight()
{
   if(!size())
      return(0);

   F32 avg = 0.f;
   for(U32 i = 0; i < size(); i++)
      avg += (*this)[i].mHeight;

   return(avg / size());
}

//------------------------------------------------------------------------------

Brush::Brush(TerrainEditor * editor) :
   mTerrainEditor(editor)
{
   mSize = mTerrainEditor->getBrushSize();
}

const Point2I & Brush::getPosition()
{
   return(mGridPoint.gridPos);
}

const GridPoint & Brush::getGridPoint()
{
   return mGridPoint;
}

void Brush::setPosition(const Point3F & pos)
{
   mTerrainEditor->worldToGrid(pos, mGridPoint);
   mWorldPos = pos;
   update();
}

void Brush::setPosition(const Point2I & pos)
{
   mGridPoint.gridPos = pos;
   update();
}

//------------------------------------------------------------------------------

void Brush::update()
{
   rebuild();
   // soft selection?
//   if(mTerrainEditor->mEnableSoftBrushes)
//   {
//      Gui3DMouseEvent event;
//      TerrainAction * action = mTerrainEditor->lookupAction("softSelect");
//      AssertFatal(action, "Brush::update: no 'softSelect' action found!");

      //
//      mTerrainEditor->setCurrentSel(this);
//      action->process(this, event, true, TerrainAction::Process);
//      mTerrainEditor->resetCurrentSel();
//   }
}

//------------------------------------------------------------------------------

void BoxBrush::rebuild()
{
   reset();
   Filter filter;
   filter.set(1, &mTerrainEditor->mSoftSelectFilter);
   //
   // mSize should always be odd.

   S32 centerX = (mSize.x - 1) / 2;
   S32 centerY = (mSize.y - 1) / 2;

   F32 xFactorScale = F32(centerX) / (F32(centerX) + 0.5);
   F32 yFactorScale = F32(centerY) / (F32(centerY) + 0.5);

   for(S32 x = 0; x < mSize.x; x++)
   {
      for(S32 y = 0; y < mSize.y; y++)
      {
         GridInfo info;
         GridPoint gridPoint = mGridPoint;
         gridPoint.gridPos.set(mGridPoint.gridPos.x + x - centerX, mGridPoint.gridPos.y + y - centerY);

         mTerrainEditor->getGridInfo(gridPoint, info);
         if(mTerrainEditor->mEnableSoftBrushes && centerX != 0 && centerY != 0)
         {

            F32 xFactor = (mAbs(centerX - x) / F32(centerX)) * xFactorScale;
            F32 yFactor = (mAbs(centerY - y) / F32(centerY)) * yFactorScale;

            info.mWeight = filter.getValue(xFactor > yFactor ? xFactor : yFactor);
         }
         push_back(info);
      }
   }
}

//------------------------------------------------------------------------------

void EllipseBrush::rebuild()
{
   reset();
   Point3F center(F32(mSize.x - 1) / 2, F32(mSize.y - 1) / 2, 0);
   Filter filter;
   filter.set(1, &mTerrainEditor->mSoftSelectFilter);

   // a point is in a circle if:
   // x^2 + y^2 <= r^2
   // a point is in an ellipse if:
   // (ax)^2 + (by)^2 <= 1
   // where a = 1/halfEllipseWidth and b = 1/halfEllipseHeight

   // for a soft-selected ellipse,
   // the factor is simply the filtered: ((ax)^2 + (by)^2)

   F32 a = 1 / (F32(mSize.x) * 0.5);
   F32 b = 1 / (F32(mSize.y) * 0.5);

   for(S32 x = 0; x < mSize.x; x++)
   {
      for(S32 y = 0; y < mSize.y; y++)
      {
         F32 xp = center.x - x;
         F32 yp = center.y - y;

         F32 factor = (a * a * xp * xp) + (b * b * yp * yp);
         if(factor > 1)
            continue;

         Vector<GridInfo> infos;
         GridPoint gridPoint = mGridPoint;
         gridPoint.gridPos.set((S32)(mGridPoint.gridPos.x + x - (S32)center.x), (S32)(mGridPoint.gridPos.y + y - (S32)center.y));

         mTerrainEditor->getGridInfos(gridPoint, infos);

         for (U32 z = 0; z < infos.size(); z++)
         {
            if(mTerrainEditor->mEnableSoftBrushes)
               infos[z].mWeight = filter.getValue(factor);

            push_back(infos[z]);
         }
      }
   }
}

//------------------------------------------------------------------------------

SelectionBrush::SelectionBrush(TerrainEditor * editor) :
   Brush(editor)
{
   //... grab the current selection
}

void SelectionBrush::rebuild()
{
   reset();
   //... move the selection
}

//------------------------------------------------------------------------------

#define GRID_EPSILON 0.05f //(1e-5)
void NaviGridBrush::rebuild()
{
	reset();

	if (!mTerrainEditor->mRenderNavigationGrid)
		return;

	//1.用camera和position确定第一点
	//2.定位到第一格子worldToGrid->GirdToWorld
	//3.提升坐标,4角作射线碰撞
	//4,其他格子

	Point3F wPos;
	float baseZ;//碰撞的基准点
	Point3F startPnt = EditTSCtrl::smCamPos;
	Point3F endPnt = mWorldPos;
	bool bForce = (mEvent.modifier & SI_CTRL);
    Point3F dir = endPnt - startPnt;
    endPnt = startPnt + 2 * dir;
    
	RayInfo ri;
	bool hit = gServerContainer.castRay(startPnt, endPnt, NAVIGRID_COLLISION_MASK, &ri);
       
	if(hit)
		wPos = ri.point;
	else
		wPos = mWorldPos;

	baseZ = wPos.z;
	m_v3DMousePoint = wPos;

	//if ((startPnt - m_v3DMousePoint).len() > 100.f)//太远看不清
	//	return;


	//mTerrainEditor->worldToGrid(wPos, mGridPoint.gridPos);
	NaviGrid::PosToGrid(wPos, mGridPoint.gridPos.x, mGridPoint.gridPos.y);

	Point2I offset[4] = { Point2I(0,0), Point2I(0,1), Point2I(1,1), Point2I(1,0) };

	Point3F point[2] = { Point3F(-GRID_EPSILON,0,0), Point3F(0,-GRID_EPSILON,0) };
	Point3F tempPoint;

	S32 centerX = (mSize.x) / 2;
	S32 centerY = (mSize.y) / 2;
	rayCache.clear();


	for(S32 x = 0; x < mSize.x; x++)
	{
		for(S32 y = 0; y < mSize.y; y++)
		{
			GridInfo info;
			bool invalid = false;

			//一个cell的4角,注意这里的gridId是不同的概念
			for (int i = 0; i < 4; i++)
			{
				Point2I node = Point2I(mGridPoint.gridPos.x + x - centerX, mGridPoint.gridPos.y + y - centerY) + offset[i];
				int gridId = NaviGrid::GridToID(node.x, node.y);

				info.mVector[i] = NaviGrid::GeneratePosition(node.x, node.y);
				tempPoint = info.mVector[i];
				if(node.x == NaviGrid::TableWidthX)
					tempPoint += point[0];
				if(node.y == NaviGrid::TableWidthY)
					tempPoint += point[1];
				invalid |= !rayHeight(tempPoint, gridId, baseZ);
				info.mVector[i].z = tempPoint.z;
			}

			if (bForce)
            {
				invalid = false;
                
                Point3F& a = info.mVector[NavigationCell::VERT_A];
                float minZ = a.z;
                float maxZ = a.z;
                
                for (int i = 0; i < 4; i++)
                {
                    if (info.mVector[i].z < minZ)
                    {
                        minZ = info.mVector[i].z;
                    }

                    if (info.mVector[i].z > maxZ)
                    {
                        maxZ = info.mVector[i].z;
                    }
                }
                    
                if (maxZ - minZ > 2.0f)
                {
                    for (int i = 0; i < 4; i++)
                    {
                        info.mVector[i].z = minZ;
                    }
                }
            }
			else if (!invalid)
				invalid |= !checkBlockGrid(&info, true);

			info.bValid = !invalid;
			push_back(info);
		}
	}
}

bool NaviGridBrush::rayHeight(Point3F& wPos, int gridId, float baseZ)
{
	stdext::hash_map<int, float>::iterator it = rayCache.find(gridId);
	if (it != rayCache.end())
	{
		wPos.z = it->second;
		return true;
	}

	Point3F startPnt = wPos;
	Point3F endPnt = wPos;

	//startPnt.z = baseZ + NaviGridSize * 2500;
	//endPnt.z = baseZ - NaviGridSize * 2500;

	startPnt.z = baseZ + mTerrainEditor->mNavGridCollisionHeight;
	endPnt.z   = baseZ - mTerrainEditor->mNavGridCollisionHeight;

	RayInfo ri;
	bool hit = gServerContainer.castRay(startPnt, endPnt, NAVIGRID_COLLISION_MASK, &ri);;
	if(hit)
	{
		wPos.z = ri.point.z;
		rayCache[gridId] = ri.point.z;
	}
	else
	{
		wPos.z = baseZ;
	}

	return hit;
}

bool NaviGridBrush::checkBlockGrid(GridInfo* info, bool checkStatic)
{
	//计算normal
	F32 maxheight = NaviGridSize * 0.839f;//tangent40
	for (int i = 0; i < 4; i++)
	{
		Point3F& a = info->mVector[ NavigationCellMgr::sLinePoint[i][0] ];
		Point3F& b = info->mVector[ NavigationCellMgr::sLinePoint[i][1] ];
		if (mFabs(a.z - b.z) > maxheight)
			return false;
	}

	Point3F& a = info->mVector[NavigationCell::VERT_A];
	Point3F& b = info->mVector[NavigationCell::VERT_B];
	Point3F& c = info->mVector[NavigationCell::VERT_C];
	Point3F& d = info->mVector[NavigationCell::VERT_D];
	float o = getMin( (a.z + c.z) / 2.f, (b.z + d.z) / 2.f );
	for (int i = 0; i < 4; i++)
	{
		if (mFabs(info->mVector[i].z - o) > maxheight * 0.7071f)//45degree
			return false;
	}

	if (!checkStatic)
		return true;
	//1.检查boundbox是否相交
	//2.创建碰撞盒碰撞测试是否相交
	float base = getMax(getMax(a.z, b.z), getMax(c.z, d.z));
	Point3F min = info->mVector[0];
	Point3F max = info->mVector[2];
	min.z = base + 0.6f;
	max.z = base + NaviGridSize * 2;

	Box3F box(min, max);

	SimpleQueryList sql;
	gServerContainer.findObjects(box, NAVIGRID_NO_TERRAIN_MASK, SimpleQueryList::insertionCallback, (&sql));
	if (!sql.mList.empty())
	{
		BoxConvex mConvex;
		for (U32 i = 0; i < sql.mList.size(); i++)
			sql.mList[i]->buildConvex(box, &mConvex);

		EarlyOutPolyList eaPolyList;
		eaPolyList.mNormal.set(0,0,0);
		eaPolyList.mPlaneList.setSize(6);
		eaPolyList.mPlaneList[0].set(box.min,VectorF(-1,0,0));
		eaPolyList.mPlaneList[1].set(box.max,VectorF(0,1,0));
		eaPolyList.mPlaneList[2].set(box.max,VectorF(1,0,0));
		eaPolyList.mPlaneList[3].set(box.min,VectorF(0,-1,0));
		eaPolyList.mPlaneList[4].set(box.min,VectorF(0,0,-1));
		eaPolyList.mPlaneList[5].set(box.max,VectorF(0,0,1));

		// Build list from convex states here...
		CollisionWorkingList& rList = mConvex.getWorkingList();
		CollisionWorkingList* pList = rList.wLink.mNext;
		while (pList != &rList)
		{
			Convex* pConvex = pList->mConvex;
			{
				Box3F convexBox = pConvex->getBoundingBox();
				pConvex->getPolyList(&eaPolyList);
			}
			pList = pList->wLink.mNext;
		}

		if (!eaPolyList.isEmpty())
			return false;
	}

	return true;
}

void NaviGridAutoGenerate::rebuild(void)
{
	reset();

	if (!mTerrainEditor->mRenderNavigationGrid)
		return;

	//从地形的中心点开始，中心点由所有地形块决定
	Vector<TerrainBlock*>& list = gServerSceneGraph->getTerrainList();

	Point3F center(0,0,0);

	for(S32 i=0; i<list.size(); i++)
	{
		center += list[i]->getPosition();
	}

	center = center/list.size();
	center.x += singleTerrainOffest;
	center.y += singleTerrainOffest;

	F32 baseZ;//碰撞的基准点
	//mTerrainEditor->gridToWorld(Point2I(TerrainBlock::BlockSize >> 1, TerrainBlock::BlockSize >> 1), mWorldPos, mTerrainEditor->getActiveTerrain());
	mWorldPos = center;
	baseZ = mWorldPos.z;
	m_v3DMousePoint = mWorldPos;

	NaviGrid::PosToGrid(mWorldPos, mGridPoint.gridPos.x, mGridPoint.gridPos.y);

	Point2I offset[4] = { Point2I(0,0), Point2I(0,1), Point2I(1,1), Point2I(1,0) };
	Point3F point[2] = { Point3F(-GRID_EPSILON,0,0), Point3F(0,-GRID_EPSILON,0) };
	Point3F tempPoint;

	mSize = Point2I(NaviGrid::TableWidthX, NaviGrid::TableWidthY);
	//this->reserve(mSize.x * mSize.y);

	S32 centerX = (mSize.x) / 2;
	S32 centerY = (mSize.y) / 2;
	rayCache.clear();

	NavigationCell* cell;
	std::vector<NavigationCell*> aloneChunk;

	for(S32 x = 0; x < mSize.x; x++)
	{
		for(S32 y = 0; y < mSize.y; y++)
		{
			GridInfo info;
			bool invalid = false;

			//一个cell的4角
			for (int i = 0; i < 4; i++)
			{
				Point2I node = Point2I(mGridPoint.gridPos.x + x - centerX, mGridPoint.gridPos.y + y - centerY) + offset[i];
				int gridId = NaviGrid::GridToID(node.x, node.y);

				info.mVector[i] = NaviGrid::GeneratePosition(node.x, node.y);
				tempPoint = info.mVector[i];
				if(node.x == NaviGrid::TableWidthX)
					tempPoint += point[0];
				if(node.y == NaviGrid::TableWidthY)
					tempPoint += point[1];
				invalid |= !rayHeight(tempPoint, gridId, baseZ);
				info.mVector[i].z = tempPoint.z;
			}

			if (!invalid)
				invalid |= !checkBlockGrid(&info, true);

			info.bValid = !invalid;
			
			if (info.bValid)
			{
				Point3F v1, v2, v3, v4;
				v1 = info.mVector[0];
				v2 = info.mVector[1];;
				v3 = info.mVector[2];
				v4 = info.mVector[3];

				cell = g_NavigationMesh.AddCell(v1, v2, v3, v4);

				if(!cell)
					continue;

				if (!(cell->GetLink(0) || cell->GetLink(1) || cell->GetLink(2) || cell->GetLink(3)))
					aloneChunk.push_back(cell);
			}
		}
	}

	//检查每个chunk的规模,删除小块的单元
	for (int i = 0; i < aloneChunk.size(); i++)
	{
		cell = aloneChunk[i];
		m_Array.clear();
		TravelCell(cell);

		if (m_Array.size() <= MAX_CELLS)
		{
			for (int i = 0; i < m_Array.size(); i++)
				g_NavigationMesh.DeleteCell(m_Array[i]);
		}
		m_Array.clear();
	}
}

bool NaviGridAutoGenerate::terrHeight(Point3F& wPos, int gridId, F32 baseZ)
{
	stdext::hash_map<int, F32>::iterator it = rayCache.find(gridId);
	if (it != rayCache.end())
	{
		wPos.z = it->second;
		return true;
	}

	const MatrixF & mat = mTerrainEditor->getActiveTerrain()->getTransform();
	Point3F origin;
	mat.getColumn(3, &origin);
	F32 x = wPos.x - origin.x;
	F32 y = wPos.y - origin.y;

	bool hit = mTerrainEditor->getActiveTerrain()->getHeight(Point2F(x, y), &wPos.z);
	if(hit)
	{
		rayCache[gridId] = wPos.z; //store
	}
	else
	{
		wPos.z = baseZ;
	}

	return hit;
}

void NaviGridAutoGenerate::TravelCell(NavigationCell* cell)
{
	if (!cell || cell->m_Open)
		return;

	if (m_Array.size() > MAX_CELLS)
		return;

	cell->m_Open = true;
	m_Array.push_back(cell);

	for (int i = 0; i < 4; i++)
	{
		TravelCell(cell->m_Link[i]);
	}
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

TerrainEditor::TerrainEditor() :
   mActiveTerrain(0),
   mMousePos(0,0,0),
   mMouseBrush(0),
   mInAction(false),
   mUndoLimit(20),
   mUndoSel(0),
   mRebuildEmpty(false),
   mRebuildTextures(false),
   mGridUpdateMin(255, 255),
   mGridUpdateMax(0, 0),
   mMaxBrushSize(48,48),
	mColor(0.5f, 0.5f, 0.5f, 0.0f),
	mPaintIndex(-1),
	mCurTexLayer(0)
{
   VECTOR_SET_ASSOCIATION(mActions);
   VECTOR_SET_ASSOCIATION(mUndoList);
   VECTOR_SET_ASSOCIATION(mRedoList);
   VECTOR_SET_ASSOCIATION(mBaseMaterialInfos);

   //
   resetCurrentSel();

   //
   mBrushSize.set(1,1);
   mMouseBrush = new BoxBrush(this);
   mMouseDownSeq = 0;
   mIsDirty = false;
   mIsMissionDirty = false;
   mPaintMaterial = NULL;

   // add in all the actions here..
   mActions.push_back(new SelectAction(this));
   mActions.push_back(new DeselectAction(this));
   mActions.push_back(new ClearAction(this));
   mActions.push_back(new SoftSelectAction(this));
   mActions.push_back(new OutlineSelectAction(this));
   mActions.push_back(new PaintMaterialAction(this));
   mActions.push_back(new ClearMaterialsAction(this));
   mActions.push_back(new RaiseHeightAction(this));
   mActions.push_back(new LowerHeightAction(this));
   mActions.push_back(new SetHeightAction(this));
   mActions.push_back(new SetEmptyAction(this));
   mActions.push_back(new ClearEmptyAction(this));
   mActions.push_back(new ScaleHeightAction(this));
   mActions.push_back(new BrushAdjustHeightAction(this));
   mActions.push_back(new AdjustHeightAction(this));
   mActions.push_back(new FlattenHeightAction(this));
   mActions.push_back(new SmoothHeightAction(this));
   mActions.push_back(new SetMaterialGroupAction(this));
   mActions.push_back(new SetModifiedAction(this));
   mActions.push_back(new ClearModifiedAction(this));
   //导航网格的动作
   mActions.push_back(new PaintNavigationGridAction(this));
   mActions.push_back(new RemoveNavigationGridAction(this));
   mActions.push_back(new RemoveTreeNavigationGridAction(this));
   mActions.push_back(new AdjustNavigationGridAction(this));
   mActions.push_back(new SplitCellNavigationGridAction(this));
   mActions.push_back(new TestPathAction(this));
	//阴影刷操作
	mActions.push_back(new ShadowAction(this));
	mActions.push_back(new ClearShadowAction(this));

   // set the default action
   mCurrentAction = mActions[0];
   mRenderBrush = mCurrentAction->useMouseBrush();

   // persist data defaults
   mRenderBorder = true;
   mBorderHeight = 10;
   mBorderFillColor.set(0,255,0,20);
   mBorderFrameColor.set(0,255,0,128);
   mBorderLineMode = false;
   mSelectionHidden = false;
   mEnableSoftBrushes = false;
   mRenderVertexSelection = false;
   mProcessUsesBrush = false;
   mCurrentCursor = NULL;
   mCursorVisible = true;
   mRenderNavigationGrid = false;
   mNavGridCollisionHeight = 1000.0f;
   //
	mShadowMod = 1.0f;
   mAdjustHeightVal = 10;
   mSetHeightVal = 100;
   mScaleVal = 1;
   mSmoothFactor = 0.1f;
   mMaterialGroup = 0;
   mSoftSelectRadius = 50.f;
   mAdjustHeightMouseScale = 0.1f;

   mSoftSelectDefaultFilter = StringTable->insert("1.000000 0.833333 0.666667 0.500000 0.333333 0.166667 0.000000");
   mSoftSelectFilter = mSoftSelectDefaultFilter;;

	for (U32 i=0; i<TerrainBlock::Max_Texture_Layer; i++)
	{
		mUseLayer[i] = true;
	}
}

TerrainEditor::~TerrainEditor()
{
   // mouse
   delete mMouseBrush;

   // terrain actions
   U32 i;
   for(i = 0; i < mActions.size(); i++)
      delete mActions[i];

   // undo stuff
   clearUndo(mUndoList);
   clearUndo(mRedoList);
   delete mUndoSel;

   // base material infos
   for(i = 0; i < mBaseMaterialInfos.size(); i++)
      delete mBaseMaterialInfos[i];
}

//------------------------------------------------------------------------------

TerrainAction * TerrainEditor::lookupAction(const char * name)
{
   for(U32 i = 0; i < mActions.size(); i++)
      if(!dStricmp(mActions[i]->getName(), name))
         return(mActions[i]);
   return(0);
}

//------------------------------------------------------------------------------

bool TerrainEditor::onAdd()
{
   if(!Parent::onAdd())
      return(false);

   SimObject * obj = Sim::findObject("EditorArrowCursor");
   if(!obj)
   {
      Con::errorf(ConsoleLogEntry::General, "TerrainEditor::onAdd: failed to load cursor");
      return(false);
   }

   mDefaultCursor = dynamic_cast<GuiCursor*>(obj);

   return(true);
}

//------------------------------------------------------------------------------

void TerrainEditor::onDeleteNotify(SimObject * object)
{
   Parent::onDeleteNotify(object);

   for (U32 i = 0; i < mTerrainBlocks.size(); i++)
   {
      if(mTerrainBlocks[i] != dynamic_cast<TerrainBlock*>(object))
         continue;

      if (mTerrainBlocks[i] == mActiveTerrain)
         mActiveTerrain = NULL;
   }
}

void TerrainEditor::setCursor(GuiCursor * cursor)
{
   mCurrentCursor = cursor ? cursor : mDefaultCursor;
}


S32 TerrainEditor::getPaintMaterial()
{
   if(!mPaintMaterial)
      return -1;

   return getClientTerrain(mActiveTerrain)->getMaterialAlphaIndex(mPaintMaterial);
}

//------------------------------------------------------------------------------

TerrainBlock * TerrainEditor::getClientTerrain(TerrainBlock* serverTerrain)
{
   // do the client..

   NetConnection * toServer = NetConnection::getConnectionToServer();
   NetConnection * toClient = NetConnection::getLocalClientConnection();

   S32 index = toClient->getGhostIndex(serverTerrain);

   return(dynamic_cast<TerrainBlock*>(toServer->resolveGhost(index)));
}

//------------------------------------------------------------------------------

bool TerrainEditor::isMainTile(const Point2I & gPos)
{
   Point2I testGridPos = gPos;

   if (!dStrcmp(getCurrentAction(),"paintMaterial"))
   {
      if (testGridPos.x == (1 << TerrainBlock::BlockShift))
         testGridPos.x--;
      if (testGridPos.y == (1 << TerrainBlock::BlockShift))
         testGridPos.y--;
   }

   return (!(testGridPos.x >> TerrainBlock::BlockShift || testGridPos.y >> TerrainBlock::BlockShift));
}

TerrainBlock* TerrainEditor::getTerrainUnderWorldPoint(const Point3F & wPos)
{
   // Cast a ray straight down from the world position and see which
   // Terrain is the closest to our starting point
   Point3F startPnt = wPos;
   Point3F endPnt = wPos + Point3F(0.0f, 0.0f, -1000.0f);

   S32 blockIndex = -1;
   F32 nearT = 1.0f;

   for (U32 i = 0; i < mTerrainBlocks.size(); i++)
   {
      Point3F tStartPnt, tEndPnt;

      mTerrainBlocks[i]->getWorldTransform().mulP(startPnt, &tStartPnt);
      mTerrainBlocks[i]->getWorldTransform().mulP(endPnt, &tEndPnt);

      RayInfo ri;
      if (mTerrainBlocks[i]->castRayI(tStartPnt, tEndPnt, &ri, true))
      {
         if (ri.t < nearT)
         {
            blockIndex = i;
            nearT = ri.t;
         }
      }
   }

   if (blockIndex > -1)
      return mTerrainBlocks[blockIndex];

   return NULL;
}

TerrainBlock* TerrainEditor::castRayI(const Point3F &start, const Point3F &end, RayInfo *info, bool collideEmpty)
{
	S32 blockIndex = -1;
	F32 nearT = 1.0f;

	for (U32 i = 0; i < mTerrainBlocks.size(); i++)
	{
		Point3F tStartPnt, tEndPnt;

		mTerrainBlocks[i]->getWorldTransform().mulP(start, &tStartPnt);
		mTerrainBlocks[i]->getWorldTransform().mulP(end, &tEndPnt);

		RayInfo ri;
		if (mTerrainBlocks[i]->castRayI(tStartPnt, tEndPnt, info, collideEmpty))
		{
			if (info->t < nearT)
			{
				blockIndex = i;
				nearT = info->t;
			}
		}
	}
	if (blockIndex > -1)
	{
		info->point.interpolate(start, end, nearT);
		return mTerrainBlocks[blockIndex];
	}

	return NULL;
}

bool TerrainEditor::gridToWorld(const GridPoint & gPoint, Point3F & wPos)
{
   const MatrixF & mat = gPoint.terrainBlock->getTransform();
   Point3F origin;
   mat.getColumn(3, &origin);

   wPos.x = gPoint.gridPos.x * (float)gPoint.terrainBlock->getSquareSize() + origin.x;
   wPos.y = gPoint.gridPos.y * (float)gPoint.terrainBlock->getSquareSize() + origin.y;
   wPos.z = getGridHeight(gPoint) + origin.z;

   return isMainTile(gPoint.gridPos);
}

bool TerrainEditor::gridToWorld(const Point2I & gPos, Point3F & wPos, TerrainBlock* terrain)
{
   GridPoint gridPoint;
   gridPoint.gridPos = gPos;
   gridPoint.terrainBlock = terrain;

   return gridToWorld(gridPoint, wPos);
}

bool TerrainEditor::worldToGrid(const Point3F & wPos, GridPoint & gPoint)
{
   // If the grid point TerrainBlock is NULL then find the closest Terrain underneath that
   // point - pad a little upward in case our incoming point already lies exactly on the terrain
   if (!gPoint.terrainBlock)
      gPoint.terrainBlock = getTerrainUnderWorldPoint(wPos + Point3F(0.0f, 0.0f, 0.05f));

   if (gPoint.terrainBlock == NULL)
      return false;

   const MatrixF & worldMat = gPoint.terrainBlock->getWorldTransform();
   Point3F tPos = wPos;
   worldMat.mulP(tPos);

   F32 squareSize = (F32) gPoint.terrainBlock->getSquareSize();
   F32 halfSquareSize = squareSize / 2;

   float x = (tPos.x + halfSquareSize) / squareSize;
   float y = (tPos.y + halfSquareSize) / squareSize;

   gPoint.gridPos.x = (S32)mFloor(x);
   gPoint.gridPos.y = (S32)mFloor(y);

   return isMainTile(gPoint.gridPos);
}

bool TerrainEditor::worldToGrid(const Point3F & wPos, Point2I & gPos, TerrainBlock* terrain)
{
   GridPoint gridPoint;
   gridPoint.terrainBlock = terrain;

   bool ret = worldToGrid(wPos, gridPoint);

   gPos = gridPoint.gridPos;

   return ret;
}

bool TerrainEditor::gridToCenter(const Point2I & gPos, Point2I & cPos)
{
   cPos.x = gPos.x & TerrainBlock::BlockMask;
   cPos.y = gPos.y & TerrainBlock::BlockMask;

   //if (gPos.x == TerrainBlock::BlockSize)
   //   cPos.x = gPos.x;
   //if (gPos.y == TerrainBlock::BlockSize)
   //   cPos.y = gPos.y;

   return isMainTile(gPos);
}

//------------------------------------------------------------------------------

//bool TerrainEditor::getGridInfo(const Point3F & wPos, GridInfo & info)
//{
//   Point2I gPos;
//   worldToGrid(wPos, gPos);
//   return getGridInfo(gPos, info);
//}

bool TerrainEditor::getGridInfo(const GridPoint & gPoint, GridInfo & info)
{
   //
   info.mGridPoint = gPoint;
   info.mMaterial = getGridMaterial(gPoint);
   info.mHeight = getGridHeight(gPoint);
   info.mWeight = 1.f;
   info.mPrimarySelect = true;
   info.mMaterialChanged = false;
	info.mShadow = getGridShadow(gPoint);

   Point2I cPos;
   gridToCenter(gPoint.gridPos, cPos);

   info.mMaterialGroup = gPoint.terrainBlock->getBaseMaterial(cPos.x, cPos.y);
   gPoint.terrainBlock->getMaterialAlpha(gPoint.gridPos, info.mMaterialAlpha);

   return isMainTile(gPoint.gridPos);
}

bool TerrainEditor::getGridInfo(const Point2I & gPos, GridInfo & info, TerrainBlock* terrain)
{
   GridPoint gridPoint;
   gridPoint.gridPos = gPos;
   gridPoint.terrainBlock = terrain;

   return getGridInfo(gridPoint, info);
}

void TerrainEditor::getGridInfos(const GridPoint & gPoint, Vector<GridInfo>& infos)
{
   // First we test against the brush terrain so that we can
   // favor it (this should be the same as the active terrain)
   bool foundBrush = false;

   GridInfo info;
   if (getGridInfo(gPoint, info))
   {
      infos.push_back(info);

      foundBrush = true;
   }

   // We are going to need the world position to test against
   Point3F wPos;
   gridToWorld(gPoint, wPos);

   // Now loop through our terrain blocks and decide which ones hit the point
   // If we already found a hit against our brush terrain we only add points
   // that are relatively close to the found point
   for (U32 i = 0; i < mTerrainBlocks.size(); i++)
   {
      // Get our grid position
      Point2I gPos;
      worldToGrid(wPos, gPos, mTerrainBlocks[i]);

      GridInfo info;
      if (getGridInfo(gPos, info, mTerrainBlocks[i]))
      {
         // Skip adding this if we already found a GridInfo from the brush terrain
         // and the resultant world point isn't equivalent
         if (foundBrush)
         {
            // Convert back to world (since the height can be different)
            // Possibly use getHeight() here?
            Point3F testWorldPt;
            gridToWorld(gPos, testWorldPt, mTerrainBlocks[i]);

            if (mFabs( wPos.z - testWorldPt.z ) > 4.0f )
               continue;
         }

         infos.push_back(info);
      }
   }
}

void TerrainEditor::setGridInfo(const GridInfo & info, bool checkActive)
{
   setGridHeight(info.mGridPoint, info.mHeight);
   setGridMaterial(info.mGridPoint, info.mMaterial);
   setGridMaterialGroup(info.mGridPoint, info.mMaterialGroup);
	setGridShadow(info.mGridPoint, info.mShadow);
   if(info.mMaterialChanged && (!checkActive || info.mGridPoint.terrainBlock == mActiveTerrain))
	{
		info.mGridPoint.terrainBlock->setMaterialAlpha(info.mGridPoint.gridPos, info.mMaterialAlpha);
	}
}


//------------------------------------------------------------------------------

F32 TerrainEditor::getGridHeight(const GridPoint & gPoint)
{
   Point2I cPos;
   gridToCenter(gPoint.gridPos, cPos);
   return(fixedToFloat(gPoint.terrainBlock->getHeight(cPos.x, cPos.y)));
}

void TerrainEditor::gridUpdateComplete()
{
   // Probably should optimize this at some point to only update dirty terrains
   if(mGridUpdateMin.x <= mGridUpdateMax.x)
   {
      for (U32 i = 0; i < mTerrainBlocks.size(); i++)
         mTerrainBlocks[i]->updateGrid(mGridUpdateMin, mGridUpdateMax);
   }

   mGridUpdateMin.set(256,256);
   mGridUpdateMax.set(0,0);
}

void TerrainEditor::materialUpdateComplete()
{
   if(mGridUpdateMin.x <= mGridUpdateMax.x)
   {
      TerrainBlock * clientTerrain = getClientTerrain(mActiveTerrain);
      clientTerrain->updateGridMaterials(mGridUpdateMin, mGridUpdateMax);
      mActiveTerrain->updateGrid(mGridUpdateMin, mGridUpdateMax);
   }
   mGridUpdateMin.set(256,256);
   mGridUpdateMax.set(0,0);
}

void TerrainEditor::setGridHeight(const GridPoint & gPoint, const F32 height)
{
   Point2I cPos;
   gridToCenter(gPoint.gridPos, cPos);
   if(cPos.x < mGridUpdateMin.x)
      mGridUpdateMin.x = cPos.x;
   if(cPos.y < mGridUpdateMin.y)
      mGridUpdateMin.y = cPos.y;
   if(cPos.x > mGridUpdateMax.x)
      mGridUpdateMax.x = cPos.x;
   if(cPos.y > mGridUpdateMax.y)
      mGridUpdateMax.y = cPos.y;

   gPoint.terrainBlock->setHeight(cPos, height);
}

TerrainBlock::Material TerrainEditor::getGridMaterial(const GridPoint & gPoint)
{
   Point2I cPos;
   gridToCenter(gPoint.gridPos, cPos);
   return(*gPoint.terrainBlock->getMaterial(cPos.x, cPos.y));
}

void TerrainEditor::setGridMaterial(const GridPoint & gPoint, const TerrainBlock::Material & material)
{
   Point2I cPos;
   gridToCenter(gPoint.gridPos, cPos);

   // check if empty has been altered...
   TerrainBlock::Material * mat = gPoint.terrainBlock->getMaterial(cPos.x, cPos.y);

   if((mat->flags & TerrainBlock::Material::Empty) ^ (material.flags & TerrainBlock::Material::Empty))
      mRebuildEmpty = true;

   *mat = material;
}

U8 TerrainEditor::getGridMaterialGroup(const GridPoint & gPoint)
{
   Point2I cPos;
   gridToCenter(gPoint.gridPos, cPos);

   return(gPoint.terrainBlock->getBaseMaterial(cPos.x, cPos.y));
}

// basematerials are shared through a resource... so work on client object
// so wont need to load textures....

void TerrainEditor::setGridMaterialGroup(const GridPoint & gPoint, const U8 group)
{
   Point2I cPos;
   gridToCenter(gPoint.gridPos, cPos);

   TerrainBlock * clientTerrain = getClientTerrain(gPoint.terrainBlock);

   clientTerrain->setBaseMaterial(cPos.x, cPos.y, group);
}

void TerrainEditor::setGridShadow(const GridPoint&gPoint, ColorF shadow)
{
	Point2I cPos;
	gridToCenter(gPoint.gridPos, cPos);
	if(cPos.x < mGridUpdateMin.x)
		mGridUpdateMin.x = cPos.x;
	if(cPos.y < mGridUpdateMin.y)
		mGridUpdateMin.y = cPos.y;
	if(cPos.x > mGridUpdateMax.x)
		mGridUpdateMax.x = cPos.x;
	if(cPos.y > mGridUpdateMax.y)
		mGridUpdateMax.y = cPos.y;

	gPoint.terrainBlock->setShadow(cPos, shadow);
}

ColorF		TerrainEditor::getGridShadow(const GridPoint& gPoint)
{
	Point2I cPos;
	gridToCenter(gPoint.gridPos, cPos);
	return gPoint.terrainBlock->getShadow(cPos);
}

//------------------------------------------------------------------------------

TerrainBlock* TerrainEditor::collide(const Gui3DMouseEvent & event, Point3F & pos)
{
   if (mTerrainBlocks.size() == 0)
      return NULL;

   // call the terrain block's ray collision routine directly
   Point3F startPnt = event.pos;
   Point3F endPnt = event.pos + event.vec * 1000.0f;

   S32 blockIndex = -1;
   F32 nearT = 1.0f;

   for (U32 i = 0; i < mTerrainBlocks.size(); i++)
   {
      Point3F tStartPnt, tEndPnt;

      mTerrainBlocks[i]->getWorldTransform().mulP(startPnt, &tStartPnt);
      mTerrainBlocks[i]->getWorldTransform().mulP(endPnt, &tEndPnt);

      RayInfo ri;
      if (mTerrainBlocks[i]->castRayI(tStartPnt, tEndPnt, &ri, true))
      {
         if (ri.t < nearT)
         {
            blockIndex = i;
            nearT = ri.t;
         }
      }
   }

   if (blockIndex > -1)
   {
      pos.interpolate(startPnt, endPnt, nearT);

      return mTerrainBlocks[blockIndex];
   }

   return NULL;
}

//------------------------------------------------------------------------------

void TerrainEditor::updateGuiInfo()
{
   char buf[128];

   // mouse num grids
   // mouse avg height
   // selection num grids
   // selection avg height
   dSprintf(buf, sizeof(buf), "%d %g %d %g",
      mMouseBrush->size(), mMouseBrush->getAvgHeight(),
      mDefaultSel.size(), mDefaultSel.getAvgHeight());
   Con::executef(this, "onGuiUpdate", buf);
}

//------------------------------------------------------------------------------

void TerrainEditor::renderScene(const RectI &)
{
   if(mTerrainBlocks.size() == 0)
      return;

   if(!mSelectionHidden)
      renderSelection(mDefaultSel, ColorF(1,0,0), ColorF(0,1,0), ColorF(0,0,1), ColorF(0,0,1), true, false);

   if (mRenderNavigationGrid)
	   renderNavigridSelection(*mMouseBrush, ColorF(1, 1, 1, 0.4f), ColorF(1, 1, 1, 0.1f), ColorF(0, 1, 1), ColorF(0, 0, 1), true, false);		
   else if(mRenderBrush && mMouseBrush->size())
      renderSelection(*mMouseBrush, ColorF(1,0,0), ColorF(0,1,0), ColorF(0,0,1), ColorF(0,0,1), false, true);

   if(mRenderBorder)
      renderBorder();

   if (mRenderNavigationGrid)
   {
	   g_NavigationMesh.Render();
   }

}

//------------------------------------------------------------------------------

void TerrainEditor::renderSelection( const Selection & sel, const ColorF & inColorFull, const ColorF & inColorNone, const ColorF & outColorFull, const ColorF & outColorNone, bool renderFill, bool renderFrame )
{
   // Draw nothing if nothing selected.
   if(sel.size() == 0)
      return;

   Vector<GFXVertexPC> vertexBuffer;
   ColorF color;
   ColorI iColor;

   vertexBuffer.setSize(sel.size() * 5);

   if(mRenderVertexSelection)
   {

      for(U32 i = 0; i < sel.size(); i++)
      {
         Point3F wPos;
         bool center = gridToWorld(sel[i].mGridPoint, wPos);

         if(center)
         {
            if(sel[i].mWeight < 0.f || sel[i].mWeight > 1.f)
               color = inColorFull;
            else
               color.interpolate(inColorNone, inColorFull, sel[i].mWeight);
         }
         else
         {
            if(sel[i].mWeight < 0.f || sel[i].mWeight > 1.f)
               color = outColorFull;
            else
               color.interpolate(outColorFull, outColorNone, sel[i].mWeight);
         }
         //
         iColor = color;

         GFXVertexPC *verts = &(vertexBuffer[i * 5]);

         verts[0].point = wPos + Point3F(-1, -1, 0);
         verts[0].color = iColor;
         verts[1].point = wPos + Point3F( 1, -1, 0);
         verts[1].color = iColor;
         verts[2].point = wPos + Point3F( 1,  1, 0);
         verts[2].color = iColor;
         verts[3].point = wPos + Point3F(-1,  1, 0);
         verts[3].color = iColor;
         verts[4].point = verts[0].point;
         verts[4].color = iColor;
      }
   }
   else
   {
      // walk the points in the selection
      for(U32 i = 0; i < sel.size(); i++)
      {
         Point2I gPos = sel[i].mGridPoint.gridPos;

         GFXVertexPC *verts = &(vertexBuffer[i * 5]);

         bool center = gridToWorld(sel[i].mGridPoint, verts[0].point);
         gridToWorld(Point2I(gPos.x + 1, gPos.y), verts[1].point, sel[i].mGridPoint.terrainBlock);
         gridToWorld(Point2I(gPos.x + 1, gPos.y + 1), verts[2].point, sel[i].mGridPoint.terrainBlock);
         gridToWorld(Point2I(gPos.x, gPos.y + 1), verts[3].point, sel[i].mGridPoint.terrainBlock);
         verts[4].point = verts[0].point;

         if(center)
         {
            if(sel[i].mWeight < 0.f || sel[i].mWeight > 1.f)
               color = inColorFull;
            else
               color.interpolate(inColorNone, inColorFull, sel[i].mWeight);
         }
         else
         {
            if(sel[i].mWeight < 0.f || sel[i].mWeight > 1.f)
               color = outColorFull;
            else
               color.interpolate(outColorFull, outColorNone, sel[i].mWeight);
         }

         iColor = color;

         verts[0].color = iColor;
         verts[1].color = iColor;
         verts[2].color = iColor;
         verts[3].color = iColor;
         verts[4].color = iColor;
      }
   }

   // Render this bad boy, by stuffing everything into a volatile buffer
   // and rendering...
   GFXVertexBufferHandle<GFXVertexPC> selectionVB(GFX, vertexBuffer.size(), GFXBufferTypeStatic);

   selectionVB.lock(0, vertexBuffer.size());

   // Copy stuff
   dMemcpy((void*)&selectionVB[0], (void*)&vertexBuffer[0], sizeof(GFXVertexPC) * vertexBuffer.size());

   selectionVB.unlock();

   GFX->setBaseRenderState();
#ifdef STATEBLOCK
	AssertFatal(mSetSelSB, "TerrainEditor::renderSelection -- mSetSelSB cannot be NULL.");
	mSetSelSB->apply();
	GFX->setVertexBuffer(selectionVB);
	GFX->disableShaders();
#else
	GFX->setAlphaBlendEnable(true);
	GFX->setSrcBlend(GFXBlendSrcAlpha);
	GFX->setDestBlend(GFXBlendDestAlpha);

	GFX->setVertexBuffer(selectionVB);
	GFX->disableShaders();
	GFX->setCullMode(GFXCullNone);
#endif


   if(renderFill)
      for(U32 i=0; i < sel.size(); i++)
         GFX->drawPrimitive( GFXTriangleFan, i*5, 4);
#ifdef STATEBLOCK
	AssertFatal(mAlphaBlendFalseSB, "TerrainEditor::renderSelection -- mAlphaBlendFalseSB cannot be NULL.");
	mAlphaBlendFalseSB->apply();
#else
   GFX->setAlphaBlendEnable(false);
#endif


   if(renderFrame)
      for(U32 i=0; i < sel.size(); i++)
         GFX->drawPrimitive( GFXLineStrip , i*5, 4);

}

//------------------------------------------------------------------------------

void TerrainEditor::renderNavigridSelection( const Selection & sel, const ColorF & inColorFull, const ColorF & inColorNone, const ColorF & outColorFull, const ColorF & outColorNone, bool renderFill, bool renderFrame )
{
	if(!sel.size())
		return;

	Vector<GFXVertexPC> vertexBuffer;
	ColorF color;
	ColorI iColor;

	//分裂相邻单元
	SplitCellNavigationGridAction* split= dynamic_cast<SplitCellNavigationGridAction*>(mCurrentAction);
	if (split)
	{
		NaviGridBrush* brush = (NaviGridBrush*)&sel;
		NavigationCell* cell = g_NavigationMesh.FindCell(NaviGrid::GenerateID(brush->m_v3DMousePoint));

		if (cell)
		{
			Point3F cen = (cell->GetVertex(0) + cell->GetVertex(1) + cell->GetVertex(2) + cell->GetVertex(3)) / 4.0f;
			float xlen = brush->m_v3DMousePoint.x - cen.x;
			float ylen = brush->m_v3DMousePoint.y - cen.y;

			NavigationCell::CELL_SIDE side;
			if (mFabs(xlen) > mFabs(ylen))
				side = xlen > 0 ? NavigationCell::SIDE_RIGHT : NavigationCell::SIDE_LEFT;
			else
				side = ylen > 0 ? NavigationCell::SIDE_TOP : NavigationCell::SIDE_BOTTOM;

			Point3F p0 = cell->GetVertex(NavigationCellMgr::sLinePoint[side][0]);
			Point3F p1 = cell->GetVertex(NavigationCellMgr::sLinePoint[side][1]);

			vertexBuffer.setSize(10);
			iColor.set(0, 255, 0, 255);
			vertexBuffer[0].point = p0 + Point3F(-0.5, -0.5, 0);
			vertexBuffer[0].color = iColor;
			vertexBuffer[1].point = p0 + Point3F( 0.5, -0.5, 0);
			vertexBuffer[1].color = iColor;
			vertexBuffer[2].point = p0 + Point3F( 0.5, 0.5, 0);
			vertexBuffer[2].color = iColor;
			vertexBuffer[3].point = p0 + Point3F(-0.5, 0.5, 0);
			vertexBuffer[3].color = iColor;
			vertexBuffer[4].point = vertexBuffer[0].point;
			vertexBuffer[4].color = iColor;
			vertexBuffer[5].point = p1 + Point3F(-0.5, -0.5, 0);
			vertexBuffer[5].color = iColor;
			vertexBuffer[6].point = p1 + Point3F( 0.5, -0.5, 0);
			vertexBuffer[6].color = iColor;
			vertexBuffer[7].point = p1 + Point3F( 0.5, 0.5, 0);
			vertexBuffer[7].color = iColor;
			vertexBuffer[8].point = p1 + Point3F(-0.5, 0.5, 0);
			vertexBuffer[8].color = iColor;
			vertexBuffer[9].point = vertexBuffer[5].point;
			vertexBuffer[9].color = iColor;
		}
	}

	//调整高度
	AdjustNavigationGridAction* adjustAction= dynamic_cast<AdjustNavigationGridAction*>(mCurrentAction);
	if (adjustAction)
	{
		NaviGridBrush* brush = (NaviGridBrush*)&sel;

		//获得导航网格单元和相应点
		NavigationCell* cell = g_NavigationMesh.FindCell(NaviGrid::GenerateID(brush->m_v3DMousePoint));

		if (cell || adjustAction->m_nVertexIndex >= 0)
		{
			int index = 0;
			if(adjustAction->m_nVertexIndex >= 0)
			{
				index = adjustAction->m_nVertexIndex;
			}
			else
			{
				F32 fmin = 10000;
				for (int i = 0; i < 4; i++)
				{
					Point3F intersect = cell->GetVertex(i);
					intersect.z = brush->m_v3DMousePoint.z;
					if ((intersect - brush->m_v3DMousePoint).lenSquared() < fmin)
					{
						fmin = (cell->GetVertex(i) - brush->m_v3DMousePoint).lenSquared();
						index = cell->GetVertexIndex(i);
					}
				}
			}
			VectorF pos = g_NavigationMesh.GetPoint(index);

			vertexBuffer.setSize(5);
			iColor.set(0, 255, 0, 255);

			vertexBuffer[0].point = pos + Point3F(-0.2, -0.2, 0);
			vertexBuffer[0].color = iColor;
			vertexBuffer[1].point = pos + Point3F( 0.2, -0.2, 0);
			vertexBuffer[1].color = iColor;
			vertexBuffer[2].point = pos + Point3F( 0.2, 0.2, 0);
			vertexBuffer[2].color = iColor;
			vertexBuffer[3].point = pos + Point3F(-0.2, 0.2, 0);
			vertexBuffer[3].color = iColor;
			vertexBuffer[4].point = vertexBuffer[0].point;
			vertexBuffer[4].color = iColor;

		}
	}
	if(split || adjustAction)
	{
		if(!vertexBuffer.size())
			return;

		GFXVertexBufferHandle<GFXVertexPC> VB_New(GFX, vertexBuffer.size(), GFXBufferTypeStatic);
		VB_New.lock(0, vertexBuffer.size());
		dMemcpy((void*)&VB_New[0], (void*)&vertexBuffer[0], sizeof(GFXVertexPC) * vertexBuffer.size());
		VB_New.unlock();

		GFX->setBaseRenderState();

		GFX->setVertexBuffer(VB_New);
		GFX->disableShaders();
#ifdef STATEBLOCK
		AssertFatal(mCullCCWSB, "TerrainEditor::renderNavigridSelection -- mCullCCWSB cannot be NULL.");
		mCullCCWSB->apply();
#else
		GFX->setCullMode(GFXCullCCW);
#endif


		int Max = vertexBuffer.size() / 5;
		for(U32 i=0; i < Max; i++)
			GFX->drawPrimitive( GFXLineStrip , i*5, 4);
		return;
	}

	RemoveNavigationGridAction* removeAction= dynamic_cast<RemoveNavigationGridAction*>(mCurrentAction);
	RemoveTreeNavigationGridAction* removeTree= dynamic_cast<RemoveTreeNavigationGridAction*>(mCurrentAction);
	bool bRemove = (removeAction ||removeTree) ? true : false;

	//寻径测试
	TestPathAction* test= dynamic_cast<TestPathAction*>(mCurrentAction);
	if(test)
	{
		NaviGridBrush* brush = (NaviGridBrush*)&sel;
		//获得导航网格单元和相应点，只渲染该点所在的网格
		NavigationCell* cell = g_NavigationMesh.FindCell(brush->m_v3DMousePoint);
		if(cell)
		{
			vertexBuffer.setSize(6);
			vertexBuffer[0].point = cell->GetVertex(0);
			vertexBuffer[1].point = cell->GetVertex(1);
			vertexBuffer[2].point = cell->GetVertex(2);
			vertexBuffer[3].point = cell->GetVertex(2);
			vertexBuffer[4].point = cell->GetVertex(3);
			vertexBuffer[5].point = cell->GetVertex(0);
			vertexBuffer[0].color = inColorFull;
			vertexBuffer[1].color = inColorFull;
			vertexBuffer[2].color = inColorFull;
			vertexBuffer[3].color = inColorFull;
			vertexBuffer[4].color = inColorFull;
			vertexBuffer[5].color = inColorFull;
		}
		else
			return;
	}
	else
	{
		//绘制
		vertexBuffer.setSize(sel.size() * 6);

		// walk the points in the selection
		for(U32 i = 0; i < sel.size(); i++)
		{
			Point2I gPos = sel[i].mGridPoint.gridPos;

			GFXVertexPC *verts = &(vertexBuffer[i * 6]);

			verts[0].point = sel[i].mVector[0];
			verts[1].point = sel[i].mVector[1];
			verts[2].point = sel[i].mVector[2];
			verts[3].point = sel[i].mVector[2];
			verts[4].point = sel[i].mVector[3];
			verts[5].point = sel[i].mVector[0];

			if (sel[i].bValid)
				iColor = inColorFull;
			else
				iColor = ColorF(0, 0, 0, 1);

			if (bRemove)
				iColor = ColorF(1, 0, 0, 1);

			verts[0].color = iColor;
			verts[1].color = iColor;
			verts[2].color = iColor;
			verts[3].color = iColor;
			verts[4].color = iColor;
			verts[5].color = iColor;
		}
	}

    for (size_t i = 0; i < vertexBuffer.size(); ++i)
    {
        vertexBuffer[i].point.z += 0.1f;
    }

	GFXVertexBufferHandle<GFXVertexPC> selectionVB(GFX, vertexBuffer.size(), GFXBufferTypeStatic);
	selectionVB.lock(0, vertexBuffer.size());
	// Copy stuff
	dMemcpy((void*)&selectionVB[0], (void*)&vertexBuffer[0], sizeof(GFXVertexPC) * vertexBuffer.size());
	selectionVB.unlock();

	GFX->setBaseRenderState();
#ifdef STATEBLOCK
	AssertFatal(mSetNavSB, "TerrainEditor::renderNavigridSelection -- mSetNavSB cannot be NULL.");
	mSetNavSB->apply();
	GFX->setVertexBuffer(selectionVB);
	GFX->disableShaders();
#else
	GFX->setAlphaBlendEnable(true);
	GFX->setSrcBlend(GFXBlendSrcAlpha);
	GFX->setDestBlend(GFXBlendDestAlpha);
	GFX->setZEnable(true);

	GFX->setVertexBuffer(selectionVB);
	GFX->disableShaders();
	GFX->setCullMode(GFXCullCCW);
#endif


	if(renderFill)
		GFX->drawPrimitive( GFXTriangleList, 0, vertexBuffer.size()/3);
#ifdef STATEBLOCK
	AssertFatal(mAlphaBlendFalseSB, "TerrainEditor::renderNavigridSelection -- mAlphaBlendFalseSB cannot be NULL.");
	mAlphaBlendFalseSB->apply();
#else
	GFX->setAlphaBlendEnable(false);
#endif
	// 寻径测试不需要渲染线框
	if(test)
		return;

	if(renderFrame)
		for(U32 i=0; i < sel.size(); i++)
			GFX->drawPrimitive( GFXLineStrip , i*6, 2);
}

//------------------------------------------------------------------------------

void TerrainEditor::renderBorder()
{
#ifdef STATEBLOCK
	AssertFatal(mCullStoreSB, "TerrainEditor::renderBorder -- mCullStoreSB cannot be NULL.");
	mCullStoreSB->capture();
	AssertFatal(mSetBordSB, "TerrainEditor::renderBorder -- mSetBordSB cannot be NULL.");
	mSetBordSB->apply();
#else
   GFXCullMode oldCull = GFX->getCullMode();
	GFX->setCullMode( GFXCullNone );
	GFX->setAlphaBlendEnable( true );
	GFX->setSrcBlend( GFXBlendSrcAlpha );
	GFX->setDestBlend( GFXBlendInvSrcAlpha );
#endif
   Point2I pos(0,0);
   Point2I dir[4] = {
      Point2I(1,0),
      Point2I(0,1),
      Point2I(-1,0),
      Point2I(0,-1)
   };

   //
   if(mBorderLineMode)
   {
      PrimBuild::color(mBorderFrameColor);
      
      PrimBuild::begin( GFXLineStrip, TerrainBlock::BlockSize * 4 + 1 );
      for(U32 i = 0; i < 4; i++)
      {
         for(U32 j = 0; j < TerrainBlock::BlockSize; j++)
         {
            Point3F wPos;
            gridToWorld(pos, wPos, mActiveTerrain);
            PrimBuild::vertex3fv( wPos );
            pos += dir[i];
         }
      }

      Point3F wPos;
      gridToWorld(Point2I(0,0), wPos, mActiveTerrain);
      PrimBuild::vertex3fv( wPos );
      PrimBuild::end();
   }
   else
   {
#ifdef STATEBLOCK
		AssertFatal(mZTrueSB, "TerrainEditor::renderBorder -- mZTrueSB cannot be NULL.");
		mZTrueSB->apply();
#else
      GFX->setZEnable( true );
#endif

      GridSquare * gs = mActiveTerrain->findSquare(TerrainBlock::BlockShift, Point2I(0,0));
      F32 height = F32(gs->maxHeight) * 0.03125f + mBorderHeight;

      const MatrixF & mat = mActiveTerrain->getTransform();
      Point3F pos;
      mat.getColumn(3, &pos);
#ifdef STATEBLOCK
		Point2F minPt;
		minPt.set(pos.x, pos.y);
		Point2F maxPt;
		maxPt.set(pos.x + TerrainBlock::BlockSize * mActiveTerrain->getSquareSize(),
			pos.y + TerrainBlock::BlockSize * mActiveTerrain->getSquareSize());
#else
		Point2F minPt(pos.x, pos.y);
		Point2F maxPt(pos.x + TerrainBlock::BlockSize * mActiveTerrain->getSquareSize(),
			pos.y + TerrainBlock::BlockSize * mActiveTerrain->getSquareSize());
#endif


      ColorI & a = mBorderFillColor;
      ColorI & b = mBorderFrameColor;

      for(U32 i = 0; i < 2; i++)
      {
         //
         if(i){ PrimBuild::color(a); PrimBuild::begin( GFXTriangleFan, 4 ); } else { PrimBuild::color(b); PrimBuild::begin( GFXLineStrip, 5 ); }

         PrimBuild::vertex3f(minPt.x, minPt.y, 0);
         PrimBuild::vertex3f(maxPt.x, minPt.y, 0);
         PrimBuild::vertex3f(maxPt.x, minPt.y, height);
         PrimBuild::vertex3f(minPt.x, minPt.y, height);
         if(!i) PrimBuild::vertex3f( minPt.x, minPt.y, 0.f );
         PrimBuild::end();

         //
         if(i){ PrimBuild::color(a); PrimBuild::begin( GFXTriangleFan, 4 ); } else { PrimBuild::color(b); PrimBuild::begin( GFXLineStrip, 5 ); }
         PrimBuild::vertex3f(minPt.x, maxPt.y, 0);
         PrimBuild::vertex3f(maxPt.x, maxPt.y, 0);
         PrimBuild::vertex3f(maxPt.x, maxPt.y, height);
         PrimBuild::vertex3f(minPt.x, maxPt.y, height);
         if(!i) PrimBuild::vertex3f( minPt.x, minPt.y, 0.f );
         PrimBuild::end();

         //
         if(i){ PrimBuild::color(a); PrimBuild::begin( GFXTriangleFan, 4 ); } else { PrimBuild::color(b); PrimBuild::begin( GFXLineStrip, 5 ); }
         PrimBuild::vertex3f(minPt.x, minPt.y, 0);
         PrimBuild::vertex3f(minPt.x, maxPt.y, 0);
         PrimBuild::vertex3f(minPt.x, maxPt.y, height);
         PrimBuild::vertex3f(minPt.x, minPt.y, height);
         if(!i) PrimBuild::vertex3f( minPt.x, minPt.y, 0.f );
         PrimBuild::end();

         //
         if(i){ PrimBuild::color(a); PrimBuild::begin( GFXTriangleFan, 4 ); } else { PrimBuild::color(b); PrimBuild::begin( GFXLineStrip, 5 ); }
         PrimBuild::vertex3f(maxPt.x, minPt.y, 0);
         PrimBuild::vertex3f(maxPt.x, maxPt.y, 0);
         PrimBuild::vertex3f(maxPt.x, maxPt.y, height);
         PrimBuild::vertex3f(maxPt.x, minPt.y, height);
         if(!i) PrimBuild::vertex3f( minPt.x, minPt.y, 0.f );
         PrimBuild::end();
      }

#ifdef STATEBLOCK
		AssertFatal(mZFalseSB, "TerrainEditor::renderBorder -- mZFalseSB cannot be NULL.");
		mZFalseSB->apply();
#else
      GFX->setZEnable( false );
#endif
   }
#ifdef STATEBLOCK
	AssertFatal(mAlphaBlendFalseSB, "TerrainEditor::renderBorder -- mAlphaBlendFalseSB cannot be NULL.");
	mAlphaBlendFalseSB->apply();
	AssertFatal(mCullStoreSB, "TerrainEditor::renderBorder -- mCullStoreSB cannot be NULL.");
	mCullStoreSB->apply();
#else
	GFX->setAlphaBlendEnable( false );
	GFX->setCullMode( oldCull );
#endif

}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void TerrainEditor::addUndo(Vector<Selection *> & list, Selection * sel)
{
   AssertFatal(sel!=NULL, "TerrainEditor::addUndo - invalid selection");
   list.push_front(sel);
   if(list.size() == mUndoLimit)
   {
      Selection * undo = list[list.size()-1];
      delete undo;
      list.pop_back();
   }
   setDirty();
}

void TerrainEditor::clearUndo(Vector<Selection *> & list)
{
   for(U32 i = 0; i < list.size(); i++)
      delete list[i];
   list.clear();
}

bool TerrainEditor::processUndo(Vector<Selection *> & src, Vector<Selection *> & dest)
{
   if(!src.size())
      return(false);

   Selection * task = src.front();
   src.pop_front();

   Selection * save = new Selection;
   for(U32 i = 0; i < task->size(); i++)
   {
      GridInfo info;
      getGridInfo((*task)[i].mGridPoint, info);
      save->add(info);
      setGridInfo((*task)[i]);
   }

   gridUpdateComplete();

   delete task;
   addUndo(dest, save);

   rebuild();

   return(true);
}

//------------------------------------------------------------------------------
//
void TerrainEditor::rebuild()
{
   // empty
   if(mRebuildEmpty)
   {
      for (U32 i = 0; i < mTerrainBlocks.size(); i++)
      {
         mTerrainBlocks[i]->rebuildEmptyFlags();
         mTerrainBlocks[i]->packEmptySquares();
      }

      mRebuildEmpty = false;
   }
}

//------------------------------------------------------------------------------
class TerrainProcessActionEvent : public SimEvent
{
   U32 mSequence;
public:
   TerrainProcessActionEvent(U32 seq)
   {
      mSequence = seq;
   }
   void process(SimObject *object)
   {
      ((TerrainEditor *) object)->processActionTick(mSequence);
   }
};

void TerrainEditor::processActionTick(U32 sequence)
{
   if(mMouseDownSeq == sequence)
   {
      Sim::postEvent(this, new TerrainProcessActionEvent(mMouseDownSeq), Sim::getCurrentTime() + 30);
      mCurrentAction->process(mMouseBrush, mLastEvent, false, TerrainAction::Update);
   }
}

void TerrainEditor::on3DMouseDown(const Gui3DMouseEvent & event)
{
   if(mTerrainBlocks.size() == 0)
      return;

   Point3F pos;
   TerrainBlock* hitTerrain = collide(event, pos);

   if(!hitTerrain)
	   return;

   // Set the active terrain
   bool changed = mActiveTerrain != hitTerrain;
   mActiveTerrain = hitTerrain;

   if (!dStrcmp(getCurrentAction(),"paintMaterial"))
   {
      if (changed)
      {
         Con::executef(this, "onActiveTerrainChange", Con::getIntArg(hitTerrain->getId()));
         mMouseBrush->setTerrain(mActiveTerrain);
         if(mRenderBrush)
            mCursorVisible = false;
         mMousePos = pos;

         mMouseBrush->setPosition(mMousePos);
         return;
      }
		else//查看当前子块是否改变
		{
			U8 hitSubBlock = mActiveTerrain->getSubBlockIndex(pos.x, pos.y);
			if (mCurSubBlock != hitSubBlock)//当前子块改变
			{
				mCurSubBlock = hitSubBlock;
				return;
			}
			else
			{
				//更新当前地形当前子块当前层的纹理索引
				U8& index = mActiveTerrain->getFileRef()->mTexIndex[mCurSubBlock][mCurTexLayer];
				mActiveTerrain->getFileRef()->mTexIndex[mCurSubBlock][mCurTexLayer] = mPaintIndex;
			}
		}
   }


   mSelectionLocked = false;

   mouseLock();
   mMouseDownSeq++;
   mUndoSel = new Selection;
   mCurrentAction->process(mMouseBrush, event, true, TerrainAction::Begin);
   // process on ticks - every 30th of a second.
   Sim::postEvent(this, new TerrainProcessActionEvent(mMouseDownSeq), Sim::getCurrentTime() + 30);

#ifdef DEBUG_LMNM
	Point3F posi = gClientLogicMarkNodeManager->getClosestMarkNode(pos, LMNT_STALL);
	Con::printf("pos(%f, %f, %f)-->posi(%f, %f, %f", pos.x, pos.y, pos.z, posi.x, posi.y, posi.z);
#endif
}

void TerrainEditor::on3DMouseMove(const Gui3DMouseEvent & event)
{
   if(mTerrainBlocks.size() == 0)
      return;

   Point3F pos;
   TerrainBlock* hitTerrain = collide(event, pos);

   if(!hitTerrain)
   {
      mMouseBrush->reset();
      mCursorVisible = true;
   }
   else
   {
      if (dStrcmp(getCurrentAction(),"paintMaterial"))
      {
         // Set the active terrain
         bool changed = mActiveTerrain != hitTerrain;
         mActiveTerrain = hitTerrain;

         if (changed)
            Con::executef(this, "onActiveTerrainChange", Con::getIntArg(hitTerrain->getId()));
      }

      //
      if(mRenderBrush)
         mCursorVisible = false;
      mMousePos = pos;

      mMouseBrush->setTerrain(mActiveTerrain);
      mMouseBrush->setPosition(mMousePos);
	  mMouseBrush->SetEvent(event);
   }
}

void TerrainEditor::on3DMouseDragged(const Gui3DMouseEvent & event)
{
   if(mTerrainBlocks.size() == 0)
      return;

   if(!isMouseLocked())
      return;

   Point3F pos;
   if(!mSelectionLocked)
   {
      if(!collide(event, pos))
      {
         mMouseBrush->reset();
         return;
      }
   }

   // check if the mouse has actually moved in grid space
   bool selChanged = false;
   if(!mSelectionLocked)
   {
      Point2I gMouse;
      Point2I gLastMouse;
      worldToGrid(pos, gMouse);
      worldToGrid(mMousePos, gLastMouse);

      //
      mMousePos = pos;
      mMouseBrush->setPosition(mMousePos);

      selChanged = gMouse != gLastMouse;
   }
   if(selChanged)
      mCurrentAction->process(mMouseBrush, event, true, TerrainAction::Update);
}

void TerrainEditor::on3DMouseUp(const Gui3DMouseEvent & event)
{
   if(mTerrainBlocks.size() == 0)
      return;

   if(isMouseLocked())
   {
      mouseUnlock();
      mMouseDownSeq++;
      mCurrentAction->process(mMouseBrush, event, false, TerrainAction::End);
      setCursor(0);

      if(mUndoSel->size())
      {
         addUndo(mUndoList, mUndoSel);
         clearUndo(mRedoList);
      }
      else
         delete mUndoSel;

      mUndoSel = 0;
      mInAction = false;

      rebuild();
   }
}

void TerrainEditor::on3DMouseWheelDown( const Gui3DMouseEvent & event )
{
   if( !( event.modifier & SI_CTRL ) )
      return;

   S32 brushIncrement = ( event.modifier & SI_SHIFT ) ? 5 : 1;
   Point2I newBrush = getBrushSize() - Point2I(brushIncrement,brushIncrement);
   
   if( newBrush.x < 1 || newBrush.y < 1 )
      return;

   setBrushSize( newBrush.x, newBrush.y );
   if( mMouseBrush )
      mMouseBrush->rebuild();

}

void TerrainEditor::on3DMouseWheelUp( const Gui3DMouseEvent & event )
{
   if( !( event.modifier & SI_CTRL )  )
      return;

   S32 brushIncrement = ( event.modifier & SI_SHIFT ) ? 5 : 1;
   Point2I newBrush = getBrushSize() + Point2I(brushIncrement,brushIncrement);

   if( !(newBrush.x <= mMaxBrushSize.x && newBrush.y <= mMaxBrushSize.y) )
      return;

   setBrushSize( newBrush.x, newBrush.y );
   if( mMouseBrush )
      mMouseBrush->rebuild();
}

void TerrainEditor::getCursor(GuiCursor *&cursor, bool &visible, const GuiEvent &event)
{
   event;
   cursor = mCurrentCursor;
   visible = mCursorVisible;
}

//------------------------------------------------------------------------------
// any console function which depends on a terrainBlock attached to the editor
// should call this
bool checkTerrainBlock(TerrainEditor * object, const char * funcName)
{
   if(!object->terrainBlockValid())
   {
      Con::errorf(ConsoleLogEntry::Script, "TerrainEditor::%s: not attached to a terrain block!", funcName);
      return(false);
   }
   return(true);
}

//------------------------------------------------------------------------------
static void findObjectsCallback(SceneObject* obj, void *val)
{
   Vector<SceneObject*> * list = (Vector<SceneObject*>*)val;
   list->push_back(obj);
}

// XA: Methods added for interfacing with the consoleMethods.

void TerrainEditor::attachTerrain(TerrainBlock *terrBlock)
{
   mActiveTerrain = terrBlock;

   for (U32 i = 0; i < mTerrainBlocks.size(); i++)
   {
      if (mTerrainBlocks[i] == terrBlock)
         return;
   }

   mTerrainBlocks.push_back(terrBlock);
}

void TerrainEditor::detachTerrain(TerrainBlock *terrBlock)
{
   if (mActiveTerrain == terrBlock)
      mActiveTerrain = NULL; //do we want to set this to an existing terrain?

   for (U32 i = 0; i < mTerrainBlocks.size(); i++)
   {
      if (mTerrainBlocks[i] == terrBlock)
      {
         mTerrainBlocks.erase_fast(i);
         break;
      }
   }
}

void TerrainEditor::freshTerrains()
{
	TerrainBlock *terr = NULL;
	TerrainBlock *serverTerr = NULL;
	LightManager *lm = gClientSceneGraph->getLightManager();
	LightInfo* sun = NULL;
	lm->registerGlobalLights(false);
	sun = lm->getSpecialLight(LightManager::slSunLightType);

	for (U32 i = 0; i < mTerrainBlocks.size(); i++)
	{
		serverTerr = static_cast<TerrainBlock*>(mTerrainBlocks[i]);
		terr = getClientTerrain(serverTerr);
		if (terr)
		{
			terr->freshShadowTex();
			terr->freshLightmapByShader(sun->mAmbient, sun->mColor, sun->mBackColor, true);
			terr->refreshLightmap();
		}
	}
}

void TerrainEditor::setBrushType(const char* type)
{
   if(!dStricmp(type, "box"))
   {
      delete mMouseBrush;
      mMouseBrush = new BoxBrush(this);
   }
   else if(!dStricmp(type, "ellipse"))
   {
      delete mMouseBrush;
      mMouseBrush = new EllipseBrush(this);
   }
   else if(!dStricmp(type, "selection"))
   {
      delete mMouseBrush;
      mMouseBrush = new SelectionBrush(this);
   }
   else if(!dStricmp(type, "NaviGrid"))
   {
	   delete mMouseBrush;
	   mMouseBrush = new NaviGridBrush(this);
   }
   else {}
}

void TerrainEditor::setBrushSize(S32 w, S32 h)
{
	mBrushSize.set(w, h);
	mMouseBrush->setSize(mBrushSize);
}

const char* TerrainEditor::getBrushPos()
{
   AssertFatal(mMouseBrush!=NULL, "TerrainEditor::getBrushPos: no mouse brush!");

   Point2I pos = mMouseBrush->getPosition();
   char * ret = Con::getReturnBuffer(32);
   dSprintf(ret, sizeof(ret), "%d %d", pos.x, pos.y);
   return(ret);
}

void TerrainEditor::setBrushPos(Point2I pos)
{
   AssertFatal(mMouseBrush!=NULL, "TerrainEditor::setBrushPos: no mouse brush!");
   mMouseBrush->setPosition(pos);
}

void TerrainEditor::setAction(const char* action)
{
   for(U32 i = 0; i < mActions.size(); i++)
   {
      if(!dStricmp(mActions[i]->getName(), action))
      {
         mCurrentAction = mActions[i];

         //
         mRenderBrush = mCurrentAction->useMouseBrush();
         return;
      }
   }
}

const char* TerrainEditor::getActionName(U32 index)
{
   if(index >= mActions.size())
      return("");
   return(mActions[index]->getName());
}

const char* TerrainEditor::getCurrentAction()
{
   return(mCurrentAction->getName());
}

S32 TerrainEditor::getNumActions()
{
   return(mActions.size());
}

void TerrainEditor::resetSelWeights(bool clear)
{
   //
   if(!clear)
   {
      for(U32 i = 0; i < mDefaultSel.size(); i++)
      {
         mDefaultSel[i].mPrimarySelect = false;
         mDefaultSel[i].mWeight = 1.f;
      }
      return;
   }

   Selection sel;

   U32 i;
   for(i = 0; i < mDefaultSel.size(); i++)
   {
      if(mDefaultSel[i].mPrimarySelect)
      {
         mDefaultSel[i].mWeight = 1.f;
         sel.add(mDefaultSel[i]);
      }
   }

   mDefaultSel.reset();

   for(i = 0; i < sel.size(); i++)
      mDefaultSel.add(sel[i]);
}

void TerrainEditor::undo()
{
   if(!checkTerrainBlock(this, "undoAction"))
      return;

   processUndo(mUndoList, mRedoList);
}

void TerrainEditor::redo()
{
   if(!checkTerrainBlock(this, "redoAction"))
      return;

   processUndo(mRedoList, mUndoList);
}

void TerrainEditor::clearSelection()
{
	mDefaultSel.reset();
}

void TerrainEditor::processAction(const char* sAction)
{
   if(!checkTerrainBlock(this, "processAction"))
      return;

   TerrainAction * action = mCurrentAction;
   if(sAction)
   {
      action = lookupAction(sAction);

      if(!action)
      {
         Con::errorf(ConsoleLogEntry::General, "TerrainEditor::cProcessAction: invalid action name '%s'.", sAction);
         return;
      }
   }

   if(!getCurrentSel()->size() && !mProcessUsesBrush)
      return;

   mUndoSel = new Selection;

   Gui3DMouseEvent event;
   if(mProcessUsesBrush)
      action->process(mMouseBrush, event, true, TerrainAction::Process);
   else
      action->process(getCurrentSel(), event, true, TerrainAction::Process);

   rebuild();

   // check if should delete the undo
   if(mUndoSel->size())
   {
      addUndo(mUndoList, mUndoSel);
      clearUndo(mRedoList);
   }
   else
      delete mUndoSel;

   mUndoSel = 0;
}

void TerrainEditor::buildMaterialMap()
{
   if(!checkTerrainBlock(this, "buildMaterialMap"))
      return;

   for (U32 i = 0; i < mTerrainBlocks.size(); i++)
      mTerrainBlocks[i]->buildMaterialMap();
}

S32 TerrainEditor::getNumTextures()
{
   if(!checkTerrainBlock(this, "getNumTextures"))
      return(0);

   // walk all the possible material lists and count them..
   U32 count = 0;

   for (U32 t = 0; t < mTerrainBlocks.size(); t++)
   {
      for(U32 i = 0; i < TerrainBlock::MaterialGroups; i++)
         if(mTerrainBlocks[t]->mMaterialFileName[i] &&
            *mTerrainBlocks[t]->mMaterialFileName[i])
            count++;
   }

   return count;
}

const char* TerrainEditor::getTextureName(S32 group)
{
   if(!checkTerrainBlock(this, "getTextureName"))
      return("");

   // textures only exist on the client..
   NetConnection * toServer = NetConnection::getConnectionToServer();
   NetConnection * toClient = NetConnection::getLocalClientConnection();

   S32 index = toClient->getGhostIndex(mActiveTerrain);

   TerrainBlock * terrBlock = dynamic_cast<TerrainBlock*>(toServer->resolveGhost(index));
   if(!terrBlock)
      return("");

   // possibly in range?
   if(group < 0 || group >= TerrainBlock::MaterialGroups)
      return("");

   // now find the i-th group
   U32 count = 0;
   bool found = false;
   for(U32 i = 0; !found && (i < TerrainBlock::MaterialGroups); i++)
   {
      // count it
      if(terrBlock->mMaterialFileName[i] &&
         *terrBlock->mMaterialFileName[i])
         count++;

      if((group + 1) == count)
      {
         group = i;
         found = true;
      }
   }

   if(!found)
      return("");
   return terrBlock->mMaterialFileName[group];
}

void TerrainEditor::markEmptySquares()
{
   if(!checkTerrainBlock(this, "markEmptySquares"))
      return;

   // build a list of all the marked interiors
   Vector<InteriorInstance*> interiors;
   U32 mask = InteriorObjectType;
   gServerContainer.findObjects(mask, findObjectsCallback, &interiors);

   // walk the terrains and empty any grid which clips to an interior
   for (U32 i = 0; i < mTerrainBlocks.size(); i++)
   {
      for(U32 x = 0; x < TerrainBlock::BlockSize; x++)
      {
         for(U32 y = 0; y < TerrainBlock::BlockSize; y++)
         {
            TerrainBlock::Material * material = mTerrainBlocks[i]->getMaterial(x,y);
            material->flags |= ~(TerrainBlock::Material::Empty);

            Point3F a, b;
            gridToWorld(Point2I(x,y), a, mTerrainBlocks[i]);
            gridToWorld(Point2I(x+1,y+1), b, mTerrainBlocks[i]);

            Box3F box;
            box.min = a;
            box.max = b;

            box.min.setMin(b);
            box.max.setMax(a);

            const MatrixF & terrOMat = mTerrainBlocks[i]->getTransform();
            const MatrixF & terrWMat = mTerrainBlocks[i]->getWorldTransform();

            terrWMat.mulP(box.min);
            terrWMat.mulP(box.max);

            for(U32 i = 0; i < interiors.size(); i++)
            {
               MatrixF mat = interiors[i]->getWorldTransform();
               mat.scale(interiors[i]->getScale());
               mat.mul(terrOMat);

               U32 waterMark = FrameAllocator::getWaterMark();
               U16* zoneVector = (U16*)FrameAllocator::alloc(interiors[i]->getDetailLevel(0)->getNumZones());
               U32 numZones = 0;
               interiors[i]->getDetailLevel(0)->scanZones(box, mat,
                                                          zoneVector, &numZones);
               if (numZones != 0)
               {
                  Con::printf("%d %d", x, y);
                  material->flags |= TerrainBlock::Material::Empty;
                  FrameAllocator::setWaterMark(waterMark);
                  break;
               }
               FrameAllocator::setWaterMark(waterMark);
            }
         }
      }
   }

   // rebuild stuff..
   for (U32 i = 0; i < mTerrainBlocks.size(); i++)
   {
      mTerrainBlocks[i]->buildGridMap();
      mTerrainBlocks[i]->rebuildEmptyFlags();
      mTerrainBlocks[i]->packEmptySquares();
   }
}

void TerrainEditor::clearModifiedFlags()
{
   if(!checkTerrainBlock(this, "clearModifiedFlags"))
      return;

   //
   for (U32 t = 0; t < mTerrainBlocks.size(); t++)
   {
      for(U32 i = 0; i < (TerrainBlock::BlockSize * TerrainBlock::BlockSize); i++)
         mTerrainBlocks[t]->materialMap[i].flags &= ~TerrainBlock::Material::Modified;
   }
}

void TerrainEditor::mirrorTerrain(S32 mirrorIndex)
{
   if(!checkTerrainBlock(this, "mirrorTerrain"))
      return;

   TerrainBlock * terrain = mActiveTerrain;
   setDirty();

   //
   enum {
      top = BIT(0),
      bottom = BIT(1),
      left = BIT(2),
      right = BIT(3)
   };

   U32 sides[8] =
   {
      bottom,
      bottom | left,
      left,
      left | top,
      top,
      top | right,
      right,
      bottom | right
   };

   U32 n = TerrainBlock::BlockSize;
   U32 side = sides[mirrorIndex % 8];
   bool diag = mirrorIndex & 0x01;

   Point2I src((side & right) ? (n - 1) : 0, (side & bottom) ? (n - 1) : 0);
   Point2I dest((side & left) ? (n - 1) : 0, (side & top) ? (n - 1) : 0);
   Point2I origSrc(src);
   Point2I origDest(dest);

   // determine the run length
   U32 minStride = ((side & top) || (side & bottom)) ? n : n / 2;
   U32 majStride = ((side & left) || (side & right)) ? n : n / 2;

   Point2I srcStep((side & right) ? -1 : 1, (side & bottom) ? -1 : 1);
   Point2I destStep((side & left) ? -1 : 1, (side & top) ? -1 : 1);

   //
   U16 * heights = terrain->getHeightAddress(0,0);
   U8 * baseMaterials = terrain->getBaseMaterialAddress(0,0);
   TerrainBlock::Material * materials = terrain->getMaterial(0,0);

   // create an undo selection
   Selection * undo = new Selection;

   // walk through all the positions
   for(U32 i = 0; i < majStride; i++)
   {
      for(U32 j = 0; j < minStride; j++)
      {
         // skip the same position
         if(src != dest)
         {
            U32 si = src.x + (src.y << TerrainBlock::BlockShift);
            U32 di = dest.x + (dest.y << TerrainBlock::BlockShift);

            // add to undo selection
            GridInfo info;
            getGridInfo(dest, info, terrain);
            undo->add(info);

            //... copy info... (height, basematerial, material)
            heights[di] = heights[si];
            baseMaterials[di] = baseMaterials[si];
            materials[di] = materials[si];
         }

         // get to the new position
         src.x += srcStep.x;
         diag ? (dest.y += destStep.y) : (dest.x += destStep.x);
      }

      // get the next position for a run
      src.y += srcStep.y;
      diag ? (dest.x += destStep.x) : (dest.y += destStep.y);

      // reset the minor run
      src.x = origSrc.x;
      diag ? (dest.y = origDest.y) : (dest.x = origDest.x);

      // shorten the run length for diag runs
      if(diag)
         minStride--;
   }

   // rebuild stuff..
   terrain->buildGridMap();
   terrain->rebuildEmptyFlags();
   terrain->packEmptySquares();

   // add undo selection to undo list and clear redo
   addUndo(mUndoList, undo);
   clearUndo(mRedoList);
}

void TerrainEditor::popBaseMaterialInfo()
{
   if(!checkTerrainBlock(this, "popMaterialInfo"))
      return;

   if(!mBaseMaterialInfos.size())
      return;

   BaseMaterialInfo * info = mBaseMaterialInfos.front();

   // names
   for (U32 t = 0; t < mTerrainBlocks.size(); t++)
   {
      for(U32 i = 0; i < TerrainBlock::MaterialGroups; i++)
         mTerrainBlocks[t]->mMaterialFileName[i] = info->mMaterialNames[i];
   }

   // base materials
   for (U32 t = 0; t < mTerrainBlocks.size(); t++)
   {
      dMemcpy(mTerrainBlocks[t]->mBaseMaterialMap, info->mBaseMaterials,
         TerrainBlock::BlockSize * TerrainBlock::BlockSize);
   }

   // kill it..
   delete info;
   mBaseMaterialInfos.pop_front();

   // rebuild
   for (U32 t = 0; t < mTerrainBlocks.size(); t++)
   {
      mTerrainBlocks[t]->refreshMaterialLists();
      mTerrainBlocks[t]->buildGridMap();
   }
}

void TerrainEditor::pushBaseMaterialInfo()
{
   if(!checkTerrainBlock(this, "pushMaterialInfo"))
      return;

   TerrainBlock * terrain = mActiveTerrain;

   BaseMaterialInfo * info = new BaseMaterialInfo;

   // copy the material list names
   for(U32 i = 0; i < TerrainBlock::MaterialGroups; i++)
      info->mMaterialNames[i] = terrain->mMaterialFileName[i];

   // copy the base materials
   dMemcpy(info->mBaseMaterials, terrain->mBaseMaterialMap,
      TerrainBlock::BlockSize * TerrainBlock::BlockSize);

   mBaseMaterialInfos.push_front(info);
}

void TerrainEditor::setLoneBaseMaterial(const char* materialListBaseName)
{
   if(!checkTerrainBlock(this, "setLoneBaseMaterial"))
      return;

   TerrainBlock * terrain = mActiveTerrain;

   // force the material group
   terrain->mMaterialFileName[0] = StringTable->insert(materialListBaseName);
   dMemset(terrain->getBaseMaterialAddress(0,0),
      TerrainBlock::BlockSize * TerrainBlock::BlockSize, 0);

   terrain->refreshMaterialLists();
   terrain->buildGridMap();
}
void TerrainEditor::setDirty()
{
	 mIsDirty = true;
#ifdef  NTJ_EDITOR
	 Con::executef("ShowEditorSaveBnt");
#endif
}


void TerrainEditor::resetStateBlock()
{
	//mSetSelSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcAlpha);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendDestAlpha);
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);
	GFX->endStateBlock(mSetSelSB);

	//mSetBordSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcAlpha);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendInvSrcAlpha);
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);
	GFX->endStateBlock(mSetBordSB);

	//mCullCCWSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSCullMode, GFXCullCCW);
	GFX->endStateBlock(mCullCCWSB);


	//mCullStoreSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);
	GFX->endStateBlock(mCullStoreSB);

	//mSetNavSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcAlpha);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendDestAlpha);
	GFX->setRenderState(GFXRSZEnable, true);
	GFX->setRenderState(GFXRSCullMode, GFXCullCCW);
	GFX->endStateBlock(mSetNavSB);

	//mAlphaBlendFalseSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaBlendEnable, false);
	GFX->endStateBlock(mAlphaBlendFalseSB);

	//mZTrueSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSZEnable, true);
	GFX->endStateBlock(mZTrueSB);

	//mZFalseSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSZEnable, false);
	GFX->endStateBlock(mZFalseSB);
}


void TerrainEditor::releaseStateBlock()
{
	if (mSetSelSB)
	{
		mSetSelSB->release();
	}

	if (mSetBordSB)
	{
		mSetBordSB->release();
	}

	if (mCullCCWSB)
	{
		mCullCCWSB->release();
	}

	if (mCullStoreSB)
	{
		mCullStoreSB->release();
	}

	if (mSetNavSB)
	{
		mSetNavSB->release();
	}

	if (mAlphaBlendFalseSB)
	{
		mSetSelSB->release();
	}

	if (mZTrueSB)
	{
		mZTrueSB->release();
	}

	if (mZFalseSB)
	{
		mZFalseSB->release();
	}
}

void TerrainEditor::init()
{
	if (mSetSelSB == NULL)
	{
		mSetSelSB = new GFXD3D9StateBlock;
		mSetSelSB->registerResourceWithDevice(GFX);
		mSetSelSB->mZombify = &releaseStateBlock;
		mSetSelSB->mResurrect = &resetStateBlock;
		
		mSetBordSB = new GFXD3D9StateBlock;
		mCullCCWSB = new GFXD3D9StateBlock;
		mCullStoreSB = new GFXD3D9StateBlock;
		mSetNavSB = new GFXD3D9StateBlock;
		mAlphaBlendFalseSB = new GFXD3D9StateBlock;
		mZTrueSB = new GFXD3D9StateBlock;
		mZFalseSB = new GFXD3D9StateBlock;
		resetStateBlock();
	}
}

void TerrainEditor::shutdown()
{
	SAFE_DELETE(mSetSelSB);
	SAFE_DELETE(mSetBordSB);
	SAFE_DELETE(mCullCCWSB);
	SAFE_DELETE(mCullStoreSB);
	SAFE_DELETE(mSetNavSB);
	SAFE_DELETE(mAlphaBlendFalseSB);
	SAFE_DELETE(mZTrueSB);
	SAFE_DELETE(mZFalseSB);
}

void TerrainEditor::reloadTerrainTexture()
{
	TerrainBlock *client = getClientTerrain(mActiveTerrain);
	client->setReloadTexture(true);
}
//------------------------------------------------------------------------------

ConsoleMethod( TerrainEditor, attachTerrain, void, 2, 3, "(TerrainBlock terrain)")
{
   SimSet * missionGroup = dynamic_cast<SimSet*>(Sim::findObject("MissionGroup"));
   if (!missionGroup)
   {
      Con::errorf(ConsoleLogEntry::Script, "TerrainEditor::attach: no mission group found");
      return;
   }

   VectorPtr<TerrainBlock*> terrains;

   // attach to first found terrainBlock
   if (argc == 2)
   {
      for(SimSetIterator itr(missionGroup); *itr; ++itr)
      {
         TerrainBlock* terrBlock = dynamic_cast<TerrainBlock*>(*itr);

         if (terrBlock)
            terrains.push_back(terrBlock);
      }

      if (terrains.size() == 0)
         Con::errorf(ConsoleLogEntry::Script, "TerrainEditor::attach: no TerrainBlock objects found!");
   }
   else  // attach to named object
   {
      TerrainBlock* terrBlock = dynamic_cast<TerrainBlock*>(Sim::findObject(argv[2]));

      if (terrBlock)
         terrains.push_back(terrBlock);

      if(terrains.size() == 0)
         Con::errorf(ConsoleLogEntry::Script, "TerrainEditor::attach: failed to attach to object '%s'", argv[2]);
   }

   if (terrains.size() > 0)
   {
      for (U32 i = 0; i < terrains.size(); i++)
      {
         if (!terrains[i]->isServerObject())
         {
            terrains[i] = NULL;

            Con::errorf(ConsoleLogEntry::Script, "TerrainEditor::attach: cannot attach to client TerrainBlock");
         }
      }
   }

   for (U32 i = 0; i < terrains.size(); i++)
   {
      if (terrains[i])
	      object->attachTerrain(terrains[i]);
   }
}

ConsoleMethod( TerrainEditor, setBrushType, void, 3, 3, "(string type)"
              "One of box, ellipse, selection.")
{
	object->setBrushType(argv[2]);
}

ConsoleMethod( TerrainEditor, setBrushSize, void, 4, 4, "(int w, int h)")
{
   S32 w = dAtoi(argv[2]);
   S32 h = dAtoi(argv[3]);

   //
   if(w < 1 || w > Brush::MaxBrushDim || h < 1 || h > Brush::MaxBrushDim)
   {
      Con::errorf(ConsoleLogEntry::General, "TerrainEditor::cSetBrushSize: invalid brush dimension. [1-%d].", Brush::MaxBrushDim);
      return;
   }

	object->setBrushSize(w, h);
}

ConsoleMethod( TerrainEditor, getBrushPos, const char*, 2, 2, "Returns a Point2I.")
{
	return object->getBrushPos();
}

ConsoleMethod( TerrainEditor, setBrushPos, void, 3, 4, "(int x, int y)")
{
   //
   Point2I pos;
   if(argc == 3)
      dSscanf(argv[2], "%d %d", &pos.x, &pos.y);
   else
   {
      pos.x = dAtoi(argv[2]);
      pos.y = dAtoi(argv[3]);
   }

   object->setBrushPos(pos);
}

ConsoleMethod( TerrainEditor, setAction, void, 3, 3, "(string action_name)")
{
	object->setAction(argv[2]);
}

ConsoleMethod( TerrainEditor, getActionName, const char*, 3, 3, "(int num)")
{
	return (object->getActionName(dAtoi(argv[2])));
}

ConsoleMethod( TerrainEditor, getNumActions, S32, 2, 2, "")
{
	return(object->getNumActions());
}

ConsoleMethod( TerrainEditor, getCurrentAction, const char*, 2, 2, "")
{
	return object->getCurrentAction();
}

ConsoleMethod( TerrainEditor, resetSelWeights, void, 3, 3, "(bool clear)")
{
	object->resetSelWeights(dAtob(argv[2]));
}

ConsoleMethod( TerrainEditor, undo, void, 2, 2, "")
{
	object->undo();
}

ConsoleMethod( TerrainEditor, redo, void, 2, 2, "")
{
	object->redo();
}

ConsoleMethod( TerrainEditor, clearSelection, void, 2, 2, "")
{
   object->clearSelection();
}

ConsoleMethod( TerrainEditor, processAction, void, 2, 3, "(string action=NULL)")
{
	if(argc == 3)
		object->processAction(argv[2]);
	else object->processAction("");
}

ConsoleMethod( TerrainEditor, buildMaterialMap, void, 2, 2, "")
{
	object->buildMaterialMap();
}

ConsoleMethod( TerrainEditor, getActiveTerrain, S32, 2, 2, "")
{
   S32 ret = 0;

   TerrainBlock* terrain = object->getActiveTerrain();

   if (terrain)
      ret = terrain->getId();

	return ret;
}

ConsoleMethod( TerrainEditor, getNumTextures, S32, 2, 2, "")
{
	return object->getNumTextures();
}

ConsoleMethod( TerrainEditor, getTextureName, const char*, 3, 3, "(int index)")
{
	return object->getTextureName(dAtoi(argv[2]));
}

ConsoleMethod( TerrainEditor, markEmptySquares, void, 2, 2, "")
{
	object->markEmptySquares();
}

ConsoleMethod( TerrainEditor, clearModifiedFlags, void, 2, 2, "")
{
	object->clearModifiedFlags();
}

ConsoleMethod( TerrainEditor, mirrorTerrain, void, 3, 3, "")
{
	object->mirrorTerrain(dAtoi(argv[2]));
}

ConsoleMethod(TerrainEditor, pushBaseMaterialInfo, void, 2, 2, "")
{
	object->pushBaseMaterialInfo();
}

ConsoleMethod( TerrainEditor, popBaseMaterialInfo, void, 2, 2, "")
{
	object->popBaseMaterialInfo();
}

ConsoleMethod( TerrainEditor, setLoneBaseMaterial, void, 3, 3, "(string materialListBaseName)")
{
	object->setLoneBaseMaterial(argv[2]);
}

ConsoleMethod(TerrainEditor, setTerraformOverlay, void, 3, 3, "(bool overlayEnable) - sets the terraformer current heightmap to draw as an overlay over the current terrain.")
{
   // XA: This one needs to be implemented :)
}

ConsoleMethod(TerrainEditor, setTerrainMaterials, void, 3, 3, "(string matList) sets the list of current terrain materials.")
{
   TerrainEditor *tEditor = (TerrainEditor *) object;
   TerrainBlock *terr = NULL;
   Vector<TerrainBlock*>& TerrainList = gServerSceneGraph->getTerrainList();
   // 设置所有地表的材质
   for(S32 i=0; i<TerrainList.size(); i++)
   {
	   terr = TerrainList[i];
	   if(!terr)
		   continue;
	   Resource<TerrainFile> file = terr->getFile();
	   const char *fileList = argv[2];
	   for(U32 i = 0; i < TerrainBlock::MaterialGroups; i++)
	   {
		   U32 len;
		   const char *spos = dStrchr(fileList, '\n');
		   if(!spos)
			   len = dStrlen(fileList);
		   else
			   len = spos - fileList;

		   if(len)
			   file->mMaterialFileName[i] = StringTable->insertn(fileList, len, true);
		   else
			   file->mMaterialFileName[i] = 0;
		   fileList += len;
		   if(*fileList)
			   fileList++;
	   }
	   tEditor->getClientTerrain(terr)->buildMaterialMap();
	   // 自动刷新
	   tEditor->getClientTerrain(terr)->updateGridMaterials(Point2I(0,0),Point2I(TerrainBlock::BlockSize,TerrainBlock::BlockSize));
   }
   tEditor->setDirty();
}

ConsoleMethod(TerrainEditor, destoryMaterialMap, void, 3, 3, "(int index)")
{
	TerrainEditor *tEditor = (TerrainEditor *) object;
	TerrainBlock *terr = NULL;
	Vector<TerrainBlock*>& TerrainList = gServerSceneGraph->getTerrainList();
	// 设置所有地表的材质
	for(S32 i=0; i<TerrainList.size(); i++)
	{
		terr = TerrainList[i];
		if(!terr)
			continue;

		tEditor->getClientTerrain(terr)->buildMaterialMap();
		// 自动刷新
		//tEditor->getClientTerrain(terr)->updateGridMaterials(Point2I(0,0),Point2I(TerrainBlock::BlockSize,TerrainBlock::BlockSize));

		tEditor->getClientTerrain(terr)->destoryMaterialMap(dAtoi(argv[2]));
	}
	tEditor->setDirty();
}

ConsoleMethod(TerrainEditor, getTerrainMaterials, const char *, 2, 2, "() gets the list of current terrain materials.")
{
   char *ret = Con::getReturnBuffer(4096);
   TerrainEditor *tEditor = (TerrainEditor *) object;
   TerrainBlock *terr = tEditor->getActiveTerrain();
   if(!terr)
      return "";
   ret[0] = 0;
   Resource<TerrainFile> file = terr->getFile();
   for(U32 i = 0; i < TerrainBlock::MaterialGroups; i++)
   {
      if(file->mMaterialFileName[i])
         dStrcat(ret, 4096, file->mMaterialFileName[i]);
      dStrcat(ret, 4096, "\n");
   }
   return ret;
}

ConsoleMethod(TerrainEditor, getTerrainUnderWorldPoint, S32, 3, 5, "(x/y/z) Gets the terrain block that is located under the given world point.\n"
                                                                           "@param x/y/z The world coordinates (floating point values) you wish to query at. " 
                                                                           "These can be formatted as either a string (\"x y z\") or separately as (x, y, z)\n"
                                                                           "@return Returns the ID of the requested terrain block (0 if not found).\n\n")
{   
   TerrainEditor *tEditor = (TerrainEditor *) object;
   if(tEditor == NULL)
      return 0;
   Point3F pos;
   if(argc == 3)
      dSscanf(argv[2], "%f %f %f", &pos.x, &pos.y, &pos.z);
   else if(argc == 5)
   {
      pos.x = dAtof(argv[2]);
      pos.y = dAtof(argv[3]);
      pos.z = dAtof(argv[4]);
   }

   else
   {
      Con::errorf("TerrainEditor.getTerrainUnderWorldPoint(): Invalid argument count! Valid arguments are either \"x y z\" or x,y,z\n");
      return 0;
   }

   TerrainBlock* terrain = tEditor->getTerrainUnderWorldPoint(pos);
   if(terrain != NULL)
   {
      return terrain->getId();
   }

   return 0;
}

// 导航网格
ConsoleMethod( TerrainEditor, GeneralNavigationMesh, void, 2, 2, "")
{
	TerrainEditor * terrainEditor = static_cast<TerrainEditor*>(object);

	terrainEditor->getRoot()->mouseLock(terrainEditor);

	Gui3DMouseEvent event;
	NaviGridAutoGenerate* brush = new NaviGridAutoGenerate(terrainEditor);
	
	g_NavigationMesh.Clear();
	brush->rebuild();
	
	terrainEditor->getRoot()->mouseUnlock(terrainEditor);

	delete brush;
}

ConsoleMethod( TerrainEditor, DeleteNavigationMesh, void, 2, 2, "")
{
	g_NavigationMesh.ClearPath();
	g_NavigationMesh.Clear();
}

ConsoleMethod( TerrainEditor, OptimizeNavigationMesh, void, 2, 2, "")
{
	g_NavigationMesh.ClearPath();
}

ConsoleMethod( TerrainEditor, SaveNavigationMesh, bool, 3, 3, "")
{
	char filename[256];
	dStrcpy(filename, sizeof(filename), argv[2]);

	//保存导航网格
	char *ext = dStrrchr(filename, '.');
	*ext = '\0';
	dStrcat(filename, sizeof(filename), ".ng");
	g_NavigationMesh.Save(filename);
	g_NavigationManager->Save();

	return true;
}

ConsoleMethod( TerrainEditor, OpenNavigationMesh, bool, 3, 3, "")
{
	char filename[256] = "";
	dStrcpy(filename, sizeof(filename), argv[2]);
	char *ext = dStrrchr(filename, '.');
	//打开导航网络
	*ext = '\0';
	dStrcat(filename, sizeof(filename), ".ng");
	g_NavigationMesh.Open(filename);

	return true;
}

ConsoleMethod( TerrainEditor, ShowOptimizeNavigationMesh, bool, 2, 2, "")
{
	g_NavigationMesh.ShowOptimizeMesh();
	return true;
}

ConsoleMethod( TerrainEditor, EnableNavigationZBuffer, bool, 2, 2, "")
{
	g_NavigationMesh.EnableZBuffer();
	return true;
}

ConsoleMethod( TerrainEditor, SetNavigationBrushCollisionHeight, bool, 3, 3, "")
{
	TerrainEditor * terrainEditor = static_cast<TerrainEditor*>(object);

	terrainEditor->mNavGridCollisionHeight = dAtof(argv[2]);	
	return true;
}

ConsoleMethod( TerrainEditor, setUseLayer, bool, 4, 4, "")
{
	TerrainEditor * terrainEditor = static_cast<TerrainEditor*>(object);
	terrainEditor->setUseLayer(dAtoi(argv[2]), dAtoi(argv[3]));
	return true;
}

ConsoleMethod( TerrainEditor, getUseLayer, S32, 3, 3, "")
{
	TerrainEditor * terrainEditor = static_cast<TerrainEditor*>(object);
	return object->getUseLayer(dAtoi(argv[2]));	
}

ConsoleMethod( TerrainEditor, reloadTerrainTexture, void, 2, 2, "")
{
	object->reloadTerrainTexture();
}

ConsoleMethod( TerrainEditor, renderSubTerrEdge, void, 3, 3, "")
{
	TerrainBlock *client = object->getClientTerrain(object->getActiveTerrain());
	bool bRender = dAtob(argv[2]);
	client->setRenderSubTerrEdge(bRender);
}
//------------------------------------------------------------------------------

void TerrainEditor::initPersistFields()
{
   Parent::initPersistFields();
   addGroup("Misc");	
   addField("isDirty", TypeBool, Offset(mIsDirty, TerrainEditor));
   addField("isMissionDirty", TypeBool, Offset(mIsMissionDirty, TerrainEditor));
   addField("renderBorder", TypeBool, Offset(mRenderBorder, TerrainEditor));
   addField("borderHeight", TypeF32, Offset(mBorderHeight, TerrainEditor));
   addField("borderFillColor", TypeColorI, Offset(mBorderFillColor, TerrainEditor));
   addField("borderFrameColor", TypeColorI, Offset(mBorderFrameColor, TerrainEditor));
   addField("borderLineMode", TypeBool, Offset(mBorderLineMode, TerrainEditor));
   addField("selectionHidden", TypeBool, Offset(mSelectionHidden, TerrainEditor));
   addField("enableSoftBrushes", TypeBool, Offset(mEnableSoftBrushes, TerrainEditor));
   addField("renderVertexSelection", TypeBool, Offset(mRenderVertexSelection, TerrainEditor));
   addField("processUsesBrush", TypeBool, Offset(mProcessUsesBrush, TerrainEditor));
   addField("maxBrushSize", TypePoint2I, Offset(mMaxBrushSize, TerrainEditor));

   // action values...
	addField("shadowMod", TypeF32, Offset(mShadowMod, TerrainEditor));
	addField("brushColor", TypeColorF, Offset(mColor, TerrainEditor));
   addField("adjustHeightVal", TypeF32, Offset(mAdjustHeightVal, TerrainEditor));
   addField("setHeightVal", TypeF32, Offset(mSetHeightVal, TerrainEditor));
   addField("scaleVal", TypeF32, Offset(mScaleVal, TerrainEditor));
   addField("smoothFactor", TypeF32, Offset(mSmoothFactor, TerrainEditor));
   addField("materialGroup", TypeS32, Offset(mMaterialGroup, TerrainEditor));
   addField("softSelectRadius", TypeF32, Offset(mSoftSelectRadius, TerrainEditor));
   addField("softSelectFilter", TypeString, Offset(mSoftSelectFilter, TerrainEditor));
   addField("softSelectDefaultFilter", TypeString, Offset(mSoftSelectDefaultFilter, TerrainEditor));
   addField("adjustHeightMouseScale", TypeF32, Offset(mAdjustHeightMouseScale, TerrainEditor));
   addField("paintMaterial", TypeCaseString, Offset(mPaintMaterial, TerrainEditor));
   addField("paintIndex", TypeS8, Offset(mPaintIndex, TerrainEditor));
	addField("curTexLayer", TypeS8, Offset(mCurTexLayer, TerrainEditor));
	addField("useLayer", TypeBool, Offset(mUseLayer, TerrainEditor));

   // 导航网格
   addField("showNavigationGrid", TypeBool, Offset(mRenderNavigationGrid, TerrainEditor));
   endGroup("Misc");	
}
