//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "gui/missionEditor/terrainActions.h"
#include "gui/core/guiCanvas.h"

//------------------------------------------------------------------------------

void SelectAction::process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type)
{
   if(sel == mTerrainEditor->getCurrentSel())
      return;

   if(type == Process)
      return;

   if(selChanged)
   {
      if(event.modifier & SI_CTRL)
      {
         for(U32 i = 0; i < sel->size(); i++)
            mTerrainEditor->getCurrentSel()->remove((*sel)[i]);
      }
      else
      {
         for(U32 i = 0; i < sel->size(); i++)
         {
            GridInfo gInfo;
            if(mTerrainEditor->getCurrentSel()->getInfo((*sel)[i].mGridPoint.gridPos, gInfo))
            {
               if(!gInfo.mPrimarySelect)
                  gInfo.mPrimarySelect = (*sel)[i].mPrimarySelect;

               if(gInfo.mWeight < (*sel)[i].mWeight)
                  gInfo.mWeight = (*sel)[i].mWeight;

               mTerrainEditor->getCurrentSel()->setInfo(gInfo);
            }
            else
               mTerrainEditor->getCurrentSel()->add((*sel)[i]);
         }
      }
   }
}

void DeselectAction::process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type)
{
   if(sel == mTerrainEditor->getCurrentSel())
      return;

   if(type == Process)
      return;

   if(selChanged)
   {
      for(U32 i = 0; i < sel->size(); i++)
         mTerrainEditor->getCurrentSel()->remove((*sel)[i]);
   }
}

//------------------------------------------------------------------------------

void SoftSelectAction::process(Selection * sel, const Gui3DMouseEvent &, bool selChanged, Type type)
{
   TerrainBlock* terrBlock = mTerrainEditor->getActiveTerrain();

   if (!terrBlock)
      return;

   if (!mTerrainEditor)
      return;

   // allow process of current selection
   Selection tmpSel;
   if(sel == mTerrainEditor->getCurrentSel())
   {
      tmpSel = *sel;
      sel = &tmpSel;
   }

   if(type == Begin || type == Process)
      mFilter.set(1, &mTerrainEditor->mSoftSelectFilter);

   //
   if(selChanged)
   {
      F32 radius = mTerrainEditor->mSoftSelectRadius;
      if(radius == 0.f)
         return;

      S32 squareSize = terrBlock->getSquareSize();
      U32 offset = U32(radius / F32(squareSize)) + 1;

      for(U32 i = 0; i < sel->size(); i++)
      {
         GridInfo & info = (*sel)[i];

         info.mPrimarySelect = true;
         info.mWeight = mFilter.getValue(0);

         if(!mTerrainEditor->getCurrentSel()->add(info))
            mTerrainEditor->getCurrentSel()->setInfo(info);

         Point2F infoPos((F32)info.mGridPoint.gridPos.x, (F32)info.mGridPoint.gridPos.y);

         //
         for(S32 x = info.mGridPoint.gridPos.x - offset; x < info.mGridPoint.gridPos.x + (offset << 1); x++)
            for(S32 y = info.mGridPoint.gridPos.y - offset; y < info.mGridPoint.gridPos.y + (offset << 1); y++)
            {
               //
               Point2F pos((F32)x, (F32)y);

               F32 dist = Point2F(pos - infoPos).len() * F32(squareSize);

               if(dist > radius)
                  continue;

               F32 weight = mFilter.getValue(dist / radius);

               //
               GridInfo gInfo;
               GridPoint gridPoint = info.mGridPoint;
               gridPoint.gridPos.set(x, y);

               if(mTerrainEditor->getCurrentSel()->getInfo(Point2I(x, y), gInfo))
               {
                  if(gInfo.mPrimarySelect)
                     continue;

                  if(gInfo.mWeight < weight)
                  {
                     gInfo.mWeight = weight;
                     mTerrainEditor->getCurrentSel()->setInfo(gInfo);
                  }
               }
               else
               {
                  Vector<GridInfo> gInfos;
                  mTerrainEditor->getGridInfos(gridPoint, gInfos);

                  for (U32 z = 0; z < gInfos.size(); z++)
                  {
                     gInfos[z].mWeight = weight;
                     gInfos[z].mPrimarySelect = false;
                     mTerrainEditor->getCurrentSel()->add(gInfos[z]);
                  }
               }
            }
      }
   }
}

//------------------------------------------------------------------------------

void OutlineSelectAction::process(Selection * sel, const Gui3DMouseEvent & event, bool, Type type)
{
   sel;event;type;
   switch(type)
   {
      case Begin:
         if(event.modifier & SI_SHIFT)
            break;

         mTerrainEditor->getCurrentSel()->reset();
         break;

      case End:
      case Update:

      default:
         return;
   }

   mLastEvent = event;
}

//------------------------------------------------------------------------------

void PaintMaterialAction::process(Selection * sel, const Gui3DMouseEvent &, bool selChanged, Type)
{
   S32 mat = mTerrainEditor->getPaintMaterial();
   if(selChanged && mat != -1)
   {
      for(U32 i = 0; i < sel->size(); i++)
      {
         GridInfo &inf = (*sel)[i];

         mTerrainEditor->getUndoSel()->add(inf);
         inf.mMaterialChanged = true;

         U32 dAmt = (U32)(inf.mWeight * 255);
         if(inf.mMaterialAlpha[mat] < dAmt)
         {
            inf.mMaterialAlpha[mat] = dAmt;
            U32 total = 0;

            for(S32 i = 0; i < TerrainBlock::MaterialGroups; i++)
            {
               if(i != mat)
                  total += inf.mMaterialAlpha[i];
            }

            if(total != 0)
            {
               // gotta scale them down...

               F32 scaleFactor = (255 - dAmt) / F32(total);
               for(S32 i = 0; i < TerrainBlock::MaterialGroups; i++)
               {
                  if(i != mat)
                     inf.mMaterialAlpha[i] = (U8)(inf.mMaterialAlpha[i] * scaleFactor);
               }
            }
			}
         mTerrainEditor->setGridInfo(inf, true);
      }
      mTerrainEditor->materialUpdateComplete();
   }
}

//------------------------------------------------------------------------------

void ClearMaterialsAction::process(Selection * sel, const Gui3DMouseEvent &, bool selChanged, Type)
{
   if(selChanged)
   {
      for(U32 i = 0; i < sel->size(); i++)
      {
         GridInfo &inf = (*sel)[i];

         mTerrainEditor->getUndoSel()->add(inf);
         inf.mMaterialChanged = true;

         for(S32 i = 0; i < TerrainBlock::MaterialGroups; i++)
         {                  
            inf.mMaterialAlpha[i] = 0; 
         }
         mTerrainEditor->setGridInfo(inf);
      }
      mTerrainEditor->materialUpdateComplete();
   }
}

//------------------------------------------------------------------------------

void RaiseHeightAction::process(Selection * sel, const Gui3DMouseEvent &, bool selChanged, Type)
{
   // ok the raise height action is our "dirt pour" action
   // only works on brushes...

   Brush *brush = dynamic_cast<Brush *>(sel);
   if(!brush)
      return;
   Point2I brushPos = brush->getPosition();
   Point2I brushSize = brush->getSize();
   GridPoint brushGridPoint = brush->getGridPoint();

   Vector<GridInfo> cur; // the height at the brush position
   mTerrainEditor->getGridInfos(brushGridPoint, cur);

   if (cur.size() == 0)
      return;

   // we get 30 process actions per second (at least)
   F32 heightAdjust = mTerrainEditor->mAdjustHeightVal / 30;
   // nothing can get higher than the current brush pos adjusted height

   F32 maxHeight = cur[0].mHeight + heightAdjust;

   for(U32 i = 0; i < sel->size(); i++)
   {
      mTerrainEditor->getUndoSel()->add((*sel)[i]);
      if((*sel)[i].mHeight < maxHeight)
      {
         (*sel)[i].mHeight += heightAdjust * (*sel)[i].mWeight;
         if((*sel)[i].mHeight > maxHeight)
            (*sel)[i].mHeight = maxHeight;
      }
      mTerrainEditor->setGridInfo((*sel)[i]);
   }

   mTerrainEditor->gridUpdateComplete();
}

//------------------------------------------------------------------------------

void LowerHeightAction::process(Selection * sel, const Gui3DMouseEvent &, bool selChanged, Type)
{
   // ok the lower height action is our "dirt dig" action
   // only works on brushes...

   Brush *brush = dynamic_cast<Brush *>(sel);
   if(!brush)
      return;
   Point2I brushPos = brush->getPosition();
   Point2I brushSize = brush->getSize();
   GridPoint brushGridPoint = brush->getGridPoint();

   Vector<GridInfo> cur; // the height at the brush position
   mTerrainEditor->getGridInfos(brushGridPoint, cur);

   if (cur.size() == 0)
      return;

   // we get 30 process actions per second (at least)
   F32 heightAdjust = -mTerrainEditor->mAdjustHeightVal / 30;
   // nothing can get higher than the current brush pos adjusted height

   F32 maxHeight = cur[0].mHeight + heightAdjust;
   if(maxHeight < 0)
      maxHeight = 0;

   for(U32 i = 0; i < sel->size(); i++)
   {
      mTerrainEditor->getUndoSel()->add((*sel)[i]);
      if((*sel)[i].mHeight > maxHeight)
      {
         (*sel)[i].mHeight += heightAdjust * (*sel)[i].mWeight;
         if((*sel)[i].mHeight < maxHeight)
            (*sel)[i].mHeight = maxHeight;
      }
      mTerrainEditor->setGridInfo((*sel)[i]);
   }
   mTerrainEditor->gridUpdateComplete();
}

//------------------------------------------------------------------------------

void SetHeightAction::process(Selection * sel, const Gui3DMouseEvent &, bool selChanged, Type)
{
   if(selChanged)
   {
      for(U32 i = 0; i < sel->size(); i++)
      {
         mTerrainEditor->getUndoSel()->add((*sel)[i]);
         (*sel)[i].mHeight = mTerrainEditor->mSetHeightVal;
         mTerrainEditor->setGridInfo((*sel)[i]);
      }
      mTerrainEditor->gridUpdateComplete();
   }
}

//------------------------------------------------------------------------------

void SetEmptyAction::process(Selection * sel, const Gui3DMouseEvent &, bool selChanged, Type)
{
   if(selChanged)
   {
      mTerrainEditor->setMissionDirty();
      for(U32 i = 0; i < sel->size(); i++)
      {
         mTerrainEditor->getUndoSel()->add((*sel)[i]);
         (*sel)[i].mMaterial.flags |= TerrainBlock::Material::Empty;
         mTerrainEditor->setGridInfo((*sel)[i]);
      }
      mTerrainEditor->gridUpdateComplete();
   }
}

//------------------------------------------------------------------------------

void ClearEmptyAction::process(Selection * sel, const Gui3DMouseEvent &, bool selChanged, Type)
{
   if(selChanged)
   {
      mTerrainEditor->setMissionDirty();
      for(U32 i = 0; i < sel->size(); i++)
      {
         mTerrainEditor->getUndoSel()->add((*sel)[i]);
         (*sel)[i].mMaterial.flags &= ~TerrainBlock::Material::Empty;
         mTerrainEditor->setGridInfo((*sel)[i]);
      }
      mTerrainEditor->gridUpdateComplete();
   }
}

//------------------------------------------------------------------------------

void SetModifiedAction::process(Selection * sel, const Gui3DMouseEvent &, bool selChanged, Type)
{
   if(selChanged)
   {
      for(U32 i = 0; i < sel->size(); i++)
      {
         mTerrainEditor->getUndoSel()->add((*sel)[i]);
         (*sel)[i].mMaterial.flags |= TerrainBlock::Material::Modified;
         mTerrainEditor->setGridInfo((*sel)[i]);
      }
      mTerrainEditor->gridUpdateComplete();
   }
}

//------------------------------------------------------------------------------

void ClearModifiedAction::process(Selection * sel, const Gui3DMouseEvent &, bool selChanged, Type)
{
   if(selChanged)
   {
      for(U32 i = 0; i < sel->size(); i++)
      {
         mTerrainEditor->getUndoSel()->add((*sel)[i]);
         (*sel)[i].mMaterial.flags &= ~TerrainBlock::Material::Modified;
         mTerrainEditor->setGridInfo((*sel)[i]);
      }
      mTerrainEditor->gridUpdateComplete();
   }
}

//------------------------------------------------------------------------------

void ScaleHeightAction::process(Selection * sel, const Gui3DMouseEvent &, bool selChanged, Type)
{
   if(selChanged)
   {
      for(U32 i = 0; i < sel->size(); i++)
      {
         mTerrainEditor->getUndoSel()->add((*sel)[i]);
         (*sel)[i].mHeight *= mTerrainEditor->mScaleVal;
         mTerrainEditor->setGridInfo((*sel)[i]);
      }
      mTerrainEditor->gridUpdateComplete();
   }
}

void BrushAdjustHeightAction::process(Selection * sel, const Gui3DMouseEvent & event, bool, Type type)
{
   if(type == Process)
      return;

   TerrainBlock* terrBlock = mTerrainEditor->getActiveTerrain();

   if (!terrBlock)
      return;

   if (!mTerrainEditor)
      return;

   if(type == Begin)
   {
      mTerrainEditor->lockSelection(true);
      mTerrainEditor->getRoot()->mouseLock(mTerrainEditor);

      // the way this works is:
      // construct a plane that goes through the collision point
      // with one axis up the terrain Z, and horizontally parallel to the
      // plane of projection

      // the cross of the camera ffdv and the terrain up vector produces
      // the cross plane vector.

      // all subsequent mouse actions are collided against the plane and the deltaZ
      // from the previous position is used to delta the selection up and down.
      Point3F cameraDir;

      EditTSCtrl::smCamMatrix.getColumn(1, &cameraDir);
      terrBlock->getTransform().getColumn(2, &mTerrainUpVector);

      // ok, get the cross vector for the plane:
      Point3F planeCross;
      mCross(cameraDir, mTerrainUpVector, &planeCross);

      planeCross.normalize();
      Point3F planeNormal;

      Point3F intersectPoint;
      mTerrainEditor->collide(event, intersectPoint);

      mCross(mTerrainUpVector, planeCross, &planeNormal);
      mIntersectionPlane.set(intersectPoint, planeNormal);

      // ok, we have the intersection point...
      // project the collision point onto the up vector of the terrain

      mPreviousZ = mDot(mTerrainUpVector, intersectPoint);

      // add to undo
      // and record the starting heights
      for(U32 i = 0; i < sel->size(); i++)
      {
         mTerrainEditor->getUndoSel()->add((*sel)[i]);
         (*sel)[i].mStartHeight = (*sel)[i].mHeight;
      }
   }
   else if(type == Update)
   {
      // ok, collide the ray from the event with the intersection plane:

      Point3F intersectPoint;
      Point3F start = event.pos;
      Point3F end = start + event.vec * 1000;

      F32 t = mIntersectionPlane.intersect(start, end);

      m_point3F_interpolate( start, end, t, intersectPoint);
      F32 currentZ = mDot(mTerrainUpVector, intersectPoint);

      F32 diff = currentZ - mPreviousZ;

      for(U32 i = 0; i < sel->size(); i++)
      {
         (*sel)[i].mHeight = (*sel)[i].mStartHeight + diff * (*sel)[i].mWeight;

         // clamp it
         if((*sel)[i].mHeight < 0.f)
            (*sel)[i].mHeight = 0.f;
         if((*sel)[i].mHeight > 2047.f)
            (*sel)[i].mHeight = 2047.f;

         mTerrainEditor->setGridInfoHeight((*sel)[i]);
      }
      mTerrainEditor->gridUpdateComplete();
   }
   else if(type == End)
   {
      mTerrainEditor->getRoot()->mouseUnlock(mTerrainEditor);
   }
}

//------------------------------------------------------------------------------

AdjustHeightAction::AdjustHeightAction(TerrainEditor * editor) :
   BrushAdjustHeightAction(editor)
{
   mCursor = 0;
}

void AdjustHeightAction::process(Selection *sel, const Gui3DMouseEvent & event, bool b, Type type)
{
   Selection * curSel = mTerrainEditor->getCurrentSel();
   BrushAdjustHeightAction::process(curSel, event, b, type);
}

//------------------------------------------------------------------------------
// flatten the primary selection then blend in the rest...

void FlattenHeightAction::process(Selection * sel, const Gui3DMouseEvent &, bool selChanged, Type)
{
   if(!sel->size())
      return;

   if(selChanged)
   {
      F32 average = 0.f;

      // get the average height
      U32 cPrimary = 0;
      for(U32 k = 0; k < sel->size(); k++)
         if((*sel)[k].mPrimarySelect)
         {
            cPrimary++;
            average += (*sel)[k].mHeight;
         }

      average /= cPrimary;

      // set it
      for(U32 i = 0; i < sel->size(); i++)
      {
         mTerrainEditor->getUndoSel()->add((*sel)[i]);

         //
         if((*sel)[i].mPrimarySelect)
            (*sel)[i].mHeight = average;
         else
         {
            F32 h = average - (*sel)[i].mHeight;
            (*sel)[i].mHeight += (h * (*sel)[i].mWeight);
         }

         mTerrainEditor->setGridInfo((*sel)[i]);
      }
      mTerrainEditor->gridUpdateComplete();
   }
}

//------------------------------------------------------------------------------

void SmoothHeightAction::process(Selection * sel, const Gui3DMouseEvent &, bool selChanged, Type)
{
   if(!sel->size())
      return;

   if(selChanged)
   {
      F32 avgHeight = 0.f;
      for(U32 k = 0; k < sel->size(); k++)
      {
         mTerrainEditor->getUndoSel()->add((*sel)[k]);
         avgHeight += (*sel)[k].mHeight;
      }

      avgHeight /= sel->size();

      // clamp the terrain smooth factor...
      if(mTerrainEditor->mSmoothFactor < 0.f)
         mTerrainEditor->mSmoothFactor = 0.f;
      if(mTerrainEditor->mSmoothFactor > 1.f)
         mTerrainEditor->mSmoothFactor = 1.f;

      // linear
      for(U32 i = 0; i < sel->size(); i++)
      {
         (*sel)[i].mHeight += (avgHeight - (*sel)[i].mHeight) * mTerrainEditor->mSmoothFactor * (*sel)[i].mWeight;
         mTerrainEditor->setGridInfo((*sel)[i]);
      }
      mTerrainEditor->gridUpdateComplete();
   }
}

void SetMaterialGroupAction::process(Selection * sel, const Gui3DMouseEvent &, bool selChanged, Type)
{
   if(selChanged)
   {
      for(U32 i = 0; i < sel->size(); i++)
      {
         mTerrainEditor->getUndoSel()->add((*sel)[i]);

         (*sel)[i].mMaterial.flags |= TerrainBlock::Material::Modified;
         (*sel)[i].mMaterialGroup = mTerrainEditor->mMaterialGroup;
         mTerrainEditor->setGridInfo((*sel)[i]);
      }
      mTerrainEditor->gridUpdateComplete();
   }
}
//----------------------------------------------------------------------------------------------------
//刷导航网格
#pragma message(ENGINE(新增导航网格))

void PaintNavigationGridAction::process(Selection * sel, const Gui3DMouseEvent &, bool selChanged, Type)
{
	if (!mTerrainEditor->mRenderNavigationGrid)
		return;

	if(selChanged)
	{
		bool isNavFixed  = false;
		std::string line = "";

		for(U32 i = 0; i < sel->size(); i++)
		{
			GridInfo &inf = (*sel)[i];
			if (inf.bValid)
			{
				Point3F& v1 = inf.mVector[0];
				Point3F& v2 = inf.mVector[1];
				Point3F& v3 = inf.mVector[2];
				Point3F& v4 = inf.mVector[3];

				g_NavigationMesh.AddCell(v1, v2, v3, v4,isNavFixed,line);
			}
		}
	}
}

//删除
void RemoveNavigationGridAction::process(Selection * sel, const Gui3DMouseEvent &, bool selChanged, Type)
{
	if (!mTerrainEditor->mRenderNavigationGrid)
		return;

	if(selChanged)
	{
		for(U32 i = 0; i < sel->size(); i++)
		{
			GridInfo &inf = (*sel)[i];
			//if (inf.bValid)
			{
				Point3F v1, v2, v3, v4;
				v1 = inf.mVector[0];
				v2 = inf.mVector[1];;
				v3 = inf.mVector[2];
				v4 = inf.mVector[3];

				g_NavigationMesh.DeleteCell(v1, v2, v3, v4);
			}
		}
	}
}

//整片删除
void RemoveTreeNavigationGridAction::process(Selection * sel, const Gui3DMouseEvent &, bool selChanged, Type)
{
	if (!mTerrainEditor->mRenderNavigationGrid)
		return;

	if(selChanged)
	{
		g_NavigationMesh.ClearPath();
		for(U32 i = 0; i < sel->size(); i++)
		{
			GridInfo &inf = (*sel)[i];

			Point3F& v1 = inf.mVector[0];
			Point3F& v2 = inf.mVector[1];;
			Point3F& v3 = inf.mVector[2];
			Point3F& v4 = inf.mVector[3];
			Point3F cen = (v1 + v2 + v3 + v4) / 4.0f;

			m_Array.reserve(10001);
			NavigationCell* cell = g_NavigationMesh.FindCell(NaviGrid::GenerateID(cen));
			TravelCell(cell);
			for (int n = 0; n < m_Array.size(); n++)
				g_NavigationMesh.DeleteCell(m_Array[n]);

			m_Array.resize(0);
		}
	}
}

void RemoveTreeNavigationGridAction::TravelCell(NavigationCell* cell)
{
	if (!cell || cell->m_Open)
		return;

	if (m_Array.size() > 10000)
		return;

	cell->m_Open = true;
	m_Array.push_back(cell);

	for (int i = 0; i < 4; i++)
	{
		TravelCell(cell->m_Link[i]);
	}
}

//调整高度
void AdjustNavigationGridAction::process(Selection * sel, const Gui3DMouseEvent &event, bool selChanged, Type type)
{
	if(type == Process)
		return;

	TerrainBlock* terrBlock = mTerrainEditor->getActiveTerrain();

	if (!terrBlock)
		return;

	if (!mTerrainEditor)
		return;

	if(type == Begin)
	{
		mTerrainEditor->lockSelection(true);
		mTerrainEditor->getRoot()->mouseLock(mTerrainEditor);

		//下面的代码计算面向camera,垂直地表的面,用于计算鼠标向上移动的量
		Point3F cameraDir;
		EditTSCtrl::smCamMatrix.getColumn(1, &cameraDir);
		terrBlock->getTransform().getColumn(2, &mTerrainUpVector);

		// ok, get the cross vector for the plane:
		Point3F planeCross;
		mCross(cameraDir, mTerrainUpVector, &planeCross);

		planeCross.normalize();
		Point3F planeNormal;
		Point3F intersectPoint;

		m_nVertexIndex = -1;
		NaviGridBrush* brush = (NaviGridBrush*)sel;
		NavigationCell* cell = g_NavigationMesh.FindCell(NaviGrid::GenerateID(brush->m_v3DMousePoint));

		if (cell)
		{
			int index = 0;
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
			intersectPoint = g_NavigationMesh.GetPoint(index);
			m_nVertexIndex = index;
		}


		mCross(mTerrainUpVector, planeCross, &planeNormal);
		mIntersectionPlane.set(brush->m_v3DMousePoint, planeNormal);

		// ok, we have the intersection point...
		// project the collision point onto the up vector of the terrain

		mPreviousZ = mDot(mTerrainUpVector, intersectPoint);	

	}
	else if(type == Update)
	{
		// ok, collide the ray from the event with the intersection plane:

		if (m_nVertexIndex != -1)
		{
			Point3F intersectPoint;
			Point3F start = event.pos;
			Point3F end = start + event.vec * 1000;

			F32 t = mIntersectionPlane.intersect(start, end);

			intersectPoint.interpolate( start, end, t);
			F32 currentZ = mDot(mTerrainUpVector, intersectPoint);

			F32 diff = currentZ - mPreviousZ;
			if (mFabs(diff) < NaviGridSize * 3)
			{
				Point3F pos = g_NavigationMesh.GetPoint(m_nVertexIndex);
				pos.z = currentZ;
				g_NavigationMesh.UpdatePoint(m_nVertexIndex, pos);
			}

		}
	} 
	else if(type == End)
	{  
		mTerrainEditor->getRoot()->mouseUnlock(mTerrainEditor);
		m_nVertexIndex = -1;
	}
}

//分裂
void SplitCellNavigationGridAction::process(Selection * sel, const Gui3DMouseEvent &, bool selChanged, Type)
{
	if (!mTerrainEditor->mRenderNavigationGrid)
		return;

	if(selChanged)
	{
		NaviGridBrush* brush = (NaviGridBrush*)sel;
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

			g_NavigationMesh.SplitCell(cell, side);
		}

	}
}

void TestPathAction::process(Selection * sel, const Gui3DMouseEvent &, bool selChanged, Type)
{
	if (!mTerrainEditor->mRenderNavigationGrid)
		return;

	if(!selChanged)
		return;

	NaviGridBrush* brush = (NaviGridBrush*)sel;
	if (!m_bSet)
	{
		m_bSet = true;
		m_ptStart = brush->m_v3DMousePoint;
	}
	else
	{
		m_bSet = false;
		m_ptEnd = brush->m_v3DMousePoint;;

		g_NavigationMesh.FindPath(m_ptStart, m_ptEnd);
	}
}

void ShadowAction::process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type)
{
	if(selChanged)
	{
		const F32 mod = 0.1;
		bool bFresh = false;
		for(U32 i = 0; i < sel->size(); i++)
		{
			GridInfo &inf = (*sel)[i];
			mTerrainEditor->getUndoSel()->add(inf);
			inf.mShadow = mTerrainEditor->mColor; 
			inf.mShadow.alpha = BRUSH_COLOR_ALPHA;
			mTerrainEditor->setGridInfo(inf, true);
			bFresh = true;
		}

		if (bFresh)
		{
			mTerrainEditor->freshTerrains();
		}
	}
}

void ClearShadowAction::process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type)
{
	if(selChanged)
	{
		bool bFresh = false;
		const F32 mod = 0.1;
		for(U32 i = 0; i < sel->size(); i++)
		{
			GridInfo &inf = (*sel)[i];
			if (inf.mShadow.alpha > MIN_BRUSH_COLOR_ALPHA)
			{
				mTerrainEditor->getUndoSel()->add(inf);
				inf.mShadow.set(0.0f, 0.0f, 0.0f, SHADOW_ALPHA);
				mTerrainEditor->setGridInfo(inf, true);
				bFresh = true;
			}
		}
		if (bFresh)
		{
			mTerrainEditor->freshTerrains();
		}
	}
}