//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _TERRAINACTIONS_H_
#define _TERRAINACTIONS_H_

#ifndef _TERRAINEDITOR_H_
#include "gui/missionEditor/terrainEditor.h"
#endif
#ifndef _GUIFILTERCTRL_H_
#include "gui/editor/guiFilterCtrl.h"
#endif

#pragma message(ENGINE(新增导航网格))
#include "Gameplay/ai/NaviGrid/NavigationCellMgr.h"

class TerrainAction
{
   protected:
      TerrainEditor *         mTerrainEditor;

   public:

      virtual ~TerrainAction(){};
      TerrainAction(TerrainEditor * editor) : mTerrainEditor(editor){}

      virtual StringTableEntry getName() = 0;

      enum Type {
         Begin = 0,
         Update,
         End,
         Process
      };

      //
      virtual void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type) = 0;
      virtual bool useMouseBrush() { return(true); }
};

//------------------------------------------------------------------------------

class SelectAction : public TerrainAction
{
   public:
      SelectAction(TerrainEditor * editor) : TerrainAction(editor){};
      StringTableEntry getName(){return("select");}

      void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type);
};

class DeselectAction : public TerrainAction
{
   public:
      DeselectAction(TerrainEditor * editor) : TerrainAction(editor){};
      StringTableEntry getName(){return("deselect");}

      void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type);
};

class ClearAction : public TerrainAction
{
   public:
      ClearAction(TerrainEditor * editor) : TerrainAction(editor){};
      StringTableEntry getName(){return("clear");}

      void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type) {};
      bool useMouseBrush() { mTerrainEditor->getCurrentSel()->reset(); return true; }
};


class SoftSelectAction : public TerrainAction
{
   public:
      SoftSelectAction(TerrainEditor * editor) : TerrainAction(editor){};
      StringTableEntry getName(){return("softSelect");}

      void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type);

      Filter   mFilter;
};

//------------------------------------------------------------------------------

class OutlineSelectAction : public TerrainAction
{
   public:
      OutlineSelectAction(TerrainEditor * editor) : TerrainAction(editor){};
      StringTableEntry getName(){return("outlineSelect");}

      void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type);
      bool useMouseBrush() { return(false); }

   private:

      Gui3DMouseEvent   mLastEvent;
};

//------------------------------------------------------------------------------

class PaintMaterialAction : public TerrainAction
{
   public:
      PaintMaterialAction(TerrainEditor * editor) : TerrainAction(editor){}
      StringTableEntry getName(){return("paintMaterial");}

      void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type);
};

//------------------------------------------------------------------------------

class ClearMaterialsAction : public TerrainAction
{
public:
   ClearMaterialsAction(TerrainEditor * editor) : TerrainAction(editor){}
   StringTableEntry getName(){return("clearMaterials");}

   void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type);
};

//------------------------------------------------------------------------------

class RaiseHeightAction : public TerrainAction
{
   public:
      RaiseHeightAction(TerrainEditor * editor) : TerrainAction(editor){}
      StringTableEntry getName(){return("raiseHeight");}

      void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type);
};

//------------------------------------------------------------------------------

class LowerHeightAction : public TerrainAction
{
   public:
      LowerHeightAction(TerrainEditor * editor) : TerrainAction(editor){}
      StringTableEntry getName(){return("lowerHeight");}

      void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type);
};

//------------------------------------------------------------------------------

class SetHeightAction : public TerrainAction
{
   public:
      SetHeightAction(TerrainEditor * editor) : TerrainAction(editor){}
      StringTableEntry getName(){return("setHeight");}

      void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type);
};

//------------------------------------------------------------------------------

class SetEmptyAction : public TerrainAction
{
   public:
      SetEmptyAction(TerrainEditor * editor) : TerrainAction(editor){}
      StringTableEntry getName(){return("setEmpty");}

      void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type);
};

//------------------------------------------------------------------------------

class ClearEmptyAction : public TerrainAction
{
   public:
      ClearEmptyAction(TerrainEditor * editor) : TerrainAction(editor){}
      StringTableEntry getName(){return("clearEmpty");}

      void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type);
};

//------------------------------------------------------------------------------

class SetModifiedAction : public TerrainAction
{
   public:
      SetModifiedAction(TerrainEditor * editor) : TerrainAction(editor){}
      StringTableEntry getName(){return("setModified");}

      void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type);
};

//------------------------------------------------------------------------------

class ClearModifiedAction : public TerrainAction
{
   public:
      ClearModifiedAction(TerrainEditor * editor) : TerrainAction(editor){}
      StringTableEntry getName(){return("clearModified");}

      void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type);
};

//------------------------------------------------------------------------------

class ScaleHeightAction : public TerrainAction
{
   public:
      ScaleHeightAction(TerrainEditor * editor) : TerrainAction(editor){}
      StringTableEntry getName(){return("scaleHeight");}

      void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type);
};

//------------------------------------------------------------------------------

class BrushAdjustHeightAction : public TerrainAction
{
   public:
      BrushAdjustHeightAction(TerrainEditor * editor) : TerrainAction(editor){}
      StringTableEntry getName(){return("brushAdjustHeight");}

      void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type);

   private:
      PlaneF mIntersectionPlane;
      Point3F mTerrainUpVector;
      F32 mPreviousZ;
};

class AdjustHeightAction : public BrushAdjustHeightAction
{
   public:
      AdjustHeightAction(TerrainEditor * editor);
      StringTableEntry getName(){return("adjustHeight");}

      void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type);
      bool useMouseBrush() { return(false); }

   private:
      //
      Point3F                    mHitPos;
      Point3F                    mLastPos;
      SimObjectPtr<GuiCursor>    mCursor;
};

//------------------------------------------------------------------------------

class FlattenHeightAction : public TerrainAction
{
   public:
      FlattenHeightAction(TerrainEditor * editor) : TerrainAction(editor){}
      StringTableEntry getName(){return("flattenHeight");}

      void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type);
};

class SmoothHeightAction : public TerrainAction
{
   public:
      SmoothHeightAction(TerrainEditor * editor) : TerrainAction(editor){}
      StringTableEntry getName(){return("smoothHeight");}

      void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type);
};

class SetMaterialGroupAction : public TerrainAction
{
   public:
      SetMaterialGroupAction(TerrainEditor * editor) : TerrainAction(editor){}
      StringTableEntry getName(){return("setMaterialGroup");}

      void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type);
};

//----------------------------------------------------------------------------
// 新增操作

class PaintNavigationGridAction : public TerrainAction
{
public:
	PaintNavigationGridAction(TerrainEditor * editor) : TerrainAction(editor){}
	StringTableEntry getName(){return("PaintNavigationGrid");}

	void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type);
};

class RemoveNavigationGridAction : public TerrainAction
{
public:
	RemoveNavigationGridAction(TerrainEditor * editor) : TerrainAction(editor){}
	StringTableEntry getName(){return("RemoveNavigationGrid");}

	void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type);
};

class RemoveTreeNavigationGridAction : public TerrainAction
{
public:
	RemoveTreeNavigationGridAction(TerrainEditor * editor) : TerrainAction(editor){}
	StringTableEntry getName(){return("RemoveTreeNavigationGrid");}

	void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type);
private:
	void TravelCell(NavigationCell* cell);
	std::vector<NavigationCell*> m_Array;
};

class AdjustNavigationGridAction : public TerrainAction
{
public:
	AdjustNavigationGridAction(TerrainEditor * editor) : TerrainAction(editor){m_nVertexIndex = -1;}
	StringTableEntry getName(){return("AdjustNavigationGrid");}

	void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type);
	int		m_nVertexIndex;
private:
	PlaneF mIntersectionPlane;
	Point3F mTerrainUpVector;
	F32	mPreviousZ;
};

class SplitCellNavigationGridAction : public TerrainAction
{
public:
	SplitCellNavigationGridAction(TerrainEditor * editor) : TerrainAction(editor){}
	StringTableEntry getName(){return("SplitCellNavigationGrid");}

	void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type);
};

class TestPathAction : public TerrainAction
{
public:
	TestPathAction(TerrainEditor * editor) : TerrainAction(editor), m_bSet(false){}
	StringTableEntry getName(){return("TestPathAction");}

	void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type);
private:
	bool	 m_bSet;
	Point3F m_ptStart;
	Point3F m_ptEnd;
};

class ShadowAction : public TerrainAction
{
public:
	ShadowAction(TerrainEditor * editor) : TerrainAction(editor){}
	StringTableEntry getName(){return("shadowAction");}

	void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type);
};

class ClearShadowAction : public TerrainAction
{
public:
	ClearShadowAction(TerrainEditor * editor) : TerrainAction(editor){}
	StringTableEntry getName(){return("clearShadowAction");}

	void process(Selection * sel, const Gui3DMouseEvent & event, bool selChanged, Type type);
};

#endif
