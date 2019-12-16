//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _TERRAINEDITOR_H_
#define _TERRAINEDITOR_H_

#ifndef _EDITTSCTRL_H_
#include "gui/missionEditor/editTSCtrl.h"
#endif
#ifndef _TERRDATA_H_
#include "terrain/terrData.h"
#endif
#pragma message(ENGINE(新增导航网格))
#include <hash_map>

//------------------------------------------------------------------------------

// Each 2D grid position must be associated with a terrainBlock
struct GridPoint
{
   Point2I        gridPos;
   TerrainBlock*  terrainBlock;

   GridPoint() { gridPos.set(0, 0); terrainBlock = NULL; };
};

class GridInfo
{
   public:
      GridPoint                  mGridPoint;
      TerrainBlock::Material     mMaterial;
      U8                         mMaterialAlpha[TerrainBlock::MaterialGroups];
		U8									mMatAlpha[TerrainBlock::Max_Texture_Layer];
      F32                        mHeight;
      U8                         mMaterialGroup;
      F32                        mWeight;
      F32                        mStartHeight;

		ColorF									mShadow;//阴影刷(颜色刷)刷的颜色
		//bool								mShadowChanged;
	  // 为导航网格编辑所作扩展
	  bool                       bValid;			//是否碰撞
	  Point3F                    mVector[4];		//4个顶点的坐标
	  Point3F                    mNormal;

      bool                       mPrimarySelect;
      bool                       mMaterialChanged;

      // hash table
      S32                        mNext;
      S32                        mPrev;
};

//------------------------------------------------------------------------------

class Selection : public Vector<GridInfo>
{
   private:

      StringTableEntry     mName;
      BitSet32             mUndoFlags;

      // hash table
      S32 lookup(const Point2I & pos);
      void insert(GridInfo info);
      U32 getHashIndex(const Point2I & pos);
      bool validate();

      Vector<S32>          mHashLists;
      U32                  mHashListSize;

   public:

      Selection();
      virtual ~Selection();

      void reset();
      bool add(const GridInfo &info);
      bool getInfo(Point2I pos, GridInfo & info);
      bool setInfo(GridInfo & info);
      bool remove(const GridInfo &info);
      void setName(StringTableEntry name);
      StringTableEntry getName(){return(mName);}
      F32 getAvgHeight();
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

class TerrainEditor;
class Brush : public Selection
{
   protected:
      TerrainEditor *   mTerrainEditor;
      Point2I           mSize;
      GridPoint         mGridPoint;
	  Point3F           mWorldPos;			//鼠标指向的世界坐标系位置
	  Gui3DMouseEvent   mEvent;				//消息事件


   public:

      enum {
         MaxBrushDim    =  40
      };

      Brush(TerrainEditor * editor);
      virtual ~Brush(){};

      // Brush appears to intentionally bypass Selection's hash table, so we
      // override validate() here.
      bool validate() { return true; }
      void setPosition(const Point3F & pos);
      void setPosition(const Point2I & pos);
      const Point2I & getPosition();
      const GridPoint & getGridPoint();
      void setTerrain(TerrainBlock* terrain) { mGridPoint.terrainBlock = terrain; };
	  void SetEvent(const Gui3DMouseEvent& event) { mEvent = event; }

      void update();
      virtual void rebuild() = 0;

      Point2I getSize(){return(mSize);}
      virtual void setSize(const Point2I & size){mSize = size;}
};

class BoxBrush : public Brush
{
   public:
      BoxBrush(TerrainEditor * editor) : Brush(editor){}
      void rebuild();
};

class EllipseBrush : public Brush
{
   public:
      EllipseBrush(TerrainEditor * editor) : Brush(editor){}
      void rebuild();
};

class SelectionBrush : public Brush
{
   public:
      SelectionBrush(TerrainEditor * editor);
      void rebuild();
      void setSize(const Point2I &){}
};

class NaviGridBrush : public Brush
{
public:
	NaviGridBrush(TerrainEditor * editor) : Brush(editor){};
	void rebuild();
	VectorF m_v3DMousePoint;
protected:
	bool rayHeight(Point3F& wPos, int gridId, float baseZ);
	bool checkBlockGrid(GridInfo* info, bool checkStatic);

	stdext::hash_map<int, float> rayCache;
};

class NavigationCell;

class NaviGridAutoGenerate : public NaviGridBrush
{
public:
	enum { MAX_CELLS = 256 };

	NaviGridAutoGenerate(TerrainEditor * editor) : NaviGridBrush(editor){};
	void rebuild(void);
protected:
	bool terrHeight(Point3F& wPos, int gridId, float baseZ);

	void TravelCell(NavigationCell* cell);
	std::vector<NavigationCell*> m_Array;
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

struct BaseMaterialInfo {
   StringTableEntry     mMaterialNames[TerrainBlock::MaterialGroups];
   U8                   mBaseMaterials[TerrainBlock::BlockSize * TerrainBlock::BlockSize];
};

class TerrainAction;
class GFXStateBlock;
class TerrainEditor : public EditTSCtrl
{
	// XA: This methods where added to replace the friend consoleMethods.
	public:
		void attachTerrain(TerrainBlock *terrBlock);
      void detachTerrain(TerrainBlock *terrBlock);
		void freshTerrains();
		
		void setBrushType(const char* type);
		void setBrushSize(S32 w, S32 h);
		const char* getBrushPos();
		void setBrushPos(Point2I pos);
		
		void setAction(const char* action);
		const char* getActionName(U32 index);
		const char* getCurrentAction();
		S32 getNumActions();
		void processAction(const char* sAction);
		
		void undo();
		void redo();
		
		void resetSelWeights(bool clear);
		void clearSelection();
		
		void buildMaterialMap();
		S32 getNumTextures();
		const char* getTextureName(S32 index);
		
		void markEmptySquares();
		void clearModifiedFlags();
		
		void mirrorTerrain(S32 mirrorIndex);
		
		void pushBaseMaterialInfo();
		void popBaseMaterialInfo();		
		
		void setLoneBaseMaterial(const char* materialListBaseName);

      TerrainBlock* getActiveTerrain() { return mActiveTerrain; };
		void reloadTerrainTexture();

	private:	
      typedef EditTSCtrl Parent;

      TerrainBlock* mActiveTerrain;
		U8 mCurSubBlock;//当前子地块
		U8 mCurTexLayer;//当前子地块中的当前纹理层
		bool mUseLayer[TerrainBlock::Max_Texture_Layer];

      // A list of all of the TerrainBlocks this editor can edit
      VectorPtr<TerrainBlock*> mTerrainBlocks;
      
      Point2I  mGridUpdateMin;
      Point2I  mGridUpdateMax;
      U32 mMouseDownSeq;

      Point3F                    mMousePos;
      Brush *                    mMouseBrush;
      bool                       mRenderBrush;
      Point2I                    mBrushSize;
      Vector<TerrainAction *>    mActions;
      TerrainAction *            mCurrentAction;
      bool                       mInAction;
      Selection                  mDefaultSel;
      bool                       mSelectionLocked;
      GuiCursor *                mDefaultCursor;
      GuiCursor *                mCurrentCursor;
      bool                       mCursorVisible;
      StringTableEntry           mPaintMaterial;//当前笔刷使用的材质文件名
		//U8									mPaintIndex;//当前笔刷使用的材质索引

      Selection *                mCurrentSel;

      //
      bool                       mRebuildEmpty;
      bool                       mRebuildTextures;
      void rebuild();

      void addUndo(Vector<Selection *> & list, Selection * sel);
      bool processUndo(Vector<Selection *> & src, Vector<Selection *> & dest);
      void clearUndo(Vector<Selection *> & list);

      U32                        mUndoLimit;
      Selection *                mUndoSel;

      Vector<Selection*>         mUndoList;
      Vector<Selection*>         mRedoList;

      Vector<BaseMaterialInfo*>  mBaseMaterialInfos;
      bool mIsDirty; // dirty flag for writing terrain.
      bool mIsMissionDirty; // dirty flag for writing mission.
   public:

      TerrainEditor();
      ~TerrainEditor();

      // conversion functions
      // Returns true if the grid position is on the main tile
      bool isMainTile(const Point2I & gPos);

      // Takes a world point and find the "highest" terrain underneath it
      // Returns true if the returned GridPoint includes a valid terrain and grid position
      TerrainBlock* getTerrainUnderWorldPoint(const Point3F & wPos);

      // Converts a GridPoint to a world position
      bool gridToWorld(const GridPoint & gPoint, Point3F & wPos);
      bool gridToWorld(const Point2I & gPos, Point3F & wPos, TerrainBlock* terrain);

      // Converts a world position to a grid point
      // If the grid point does not have a TerrainBlock already it will find the nearest
      // terrian under the world position
      bool worldToGrid(const Point3F & wPos, GridPoint & gPoint);
      bool worldToGrid(const Point3F & wPos, Point2I & gPos, TerrainBlock* terrain = NULL);

      // Converts any point that is off of the main tile to its equivalent on the main tile
      // Returns true if the point was already on the main tile
      bool gridToCenter(const Point2I & gPos, Point2I & cPos);

      //bool getGridInfo(const Point3F & wPos, GridInfo & info);
      // Gets the grid info for a point on a TerrainBlock's grid
      bool getGridInfo(const GridPoint & gPoint, GridInfo & info);
      bool getGridInfo(const Point2I & gPos, GridInfo & info, TerrainBlock* terrain);

      // Returns a list of infos for all points on the terrain that are at that point in space
      void getGridInfos(const GridPoint & gPoint, Vector<GridInfo>& infos);

      void setGridInfo(const GridInfo & info, bool checkActive = false);
      void setGridInfoHeight(const GridInfo & info);
      void gridUpdateComplete();
      void materialUpdateComplete();
      void processActionTick(U32 sequence);

      TerrainBlock* collide(const Gui3DMouseEvent & event, Point3F & pos);
      void lockSelection(bool lock) { mSelectionLocked = lock; };

      Selection * getUndoSel(){return(mUndoSel);}
      Selection * getCurrentSel(){return(mCurrentSel);}
      void setCurrentSel(Selection * sel) { mCurrentSel = sel; }
      void resetCurrentSel() {mCurrentSel = &mDefaultSel; }

      S32 getPaintMaterial();

      Point2I getBrushSize() { return(mBrushSize); }
      TerrainBlock * getClientTerrain(TerrainBlock* serverTerrain);
      bool terrainBlockValid() { return(mActiveTerrain ? true : false); }
      void setCursor(GuiCursor * cursor);
      void getCursor(GuiCursor *&cursor, bool &showCursor, const GuiEvent &lastGuiEvent);
      void setDirty();
      void setMissionDirty()  { mIsMissionDirty = true; }

      TerrainAction * lookupAction(const char * name);

	  TerrainBlock* castRayI(const Point3F &start, const Point3F &end, RayInfo *info, bool collideEmpty = false);

	  void setCurLayer(U32 layer){ mCurTexLayer = layer;}
	  U32 getCurLayer(){ return mCurTexLayer;}
	  void setUseLayer(U32 layer, bool enable){AssertFatal(layer<TerrainBlock::Max_Texture_Layer, "TerrainEditor::setUseLayer"); mUseLayer[layer] = enable;}
	  bool getUseLayer(U32 layer){AssertFatal(layer<TerrainBlock::Max_Texture_Layer, "TerrainEditor::getUseLayer"); return mUseLayer[layer];}
	  U8 getCurSubBlock(){return mCurSubBlock; };
   private:


      // terrain interface functions
      // Returns the height at a grid point
      F32 getGridHeight(const GridPoint & gPoint);
      // Sets a height at a grid point
      void setGridHeight(const GridPoint & gPoint, const F32 height);

      // Gets the material of a TerrainBlock at a specified point on the grid
      TerrainBlock::Material getGridMaterial(const GridPoint & gPoint);
      // Sets the material of a TerrainBlock at a specified point on the grid
      void setGridMaterial(const GridPoint & gPoint, const TerrainBlock::Material & material);

      // Gets the material group of a specific spot on a TerrainBlock's grid
      U8 getGridMaterialGroup(const GridPoint & gPoint);
      // Sets a material group for a spot on a TerrainBlock's grid
      void setGridMaterialGroup(const GridPoint & gPoint, U8 group);

		void setGridShadow(const GridPoint&gPoint, ColorF shadow);
		ColorF		getGridShadow(const GridPoint& gPoint);
      //
      void updateBrush(Brush & brush, const Point2I & gPos);

      //
      Point3F getMousePos(){return(mMousePos);};

      //
      void renderSelection(const Selection & sel, const ColorF & inColorFull, const ColorF & inColorNone, const ColorF & outColorFull, const ColorF & outColorNone, bool renderFill, bool renderFrame);
      void renderNavigridSelection(const Selection & sel, const ColorF & inColorFull, const ColorF & inColorNone, const ColorF & outColorFull, const ColorF & outColorNone, bool renderFill, bool renderFrame);
      void renderBorder();

   public:

      // persist field data - these are dynamic
      bool                 mRenderBorder;
      F32                  mBorderHeight;
      ColorI               mBorderFillColor;
      ColorI               mBorderFrameColor;
      bool                 mBorderLineMode;
      bool                 mSelectionHidden;
      bool                 mEnableSoftBrushes;
      bool                 mRenderVertexSelection;
      bool                 mProcessUsesBrush;

      //
		F32						mShadowMod;
		ColorF						mColor;//颜色刷的颜色
      F32                  mAdjustHeightVal;
      F32                  mSetHeightVal;
      F32                  mScaleVal;
      F32                  mSmoothFactor;
      S32                  mMaterialGroup;
      F32                  mSoftSelectRadius;
      StringTableEntry     mSoftSelectFilter;
      StringTableEntry     mSoftSelectDefaultFilter;
      F32                  mAdjustHeightMouseScale;
      Point2I              mMaxBrushSize;

	  //导航网格
	  bool				   mRenderNavigationGrid;	//是否渲染导航网格
	  bool                 mNavloadFixed;
      F32                  mNavGridCollisionHeight;

		U8									mPaintIndex;//当前笔刷使用的材质索引
   public:

      // SimObject
      bool onAdd();
      void onDeleteNotify(SimObject * object);

      static void initPersistFields();

      // EditTSCtrl
      virtual void on3DMouseUp(const Gui3DMouseEvent & event);
      virtual void on3DMouseDown(const Gui3DMouseEvent & event);
      virtual void on3DMouseMove(const Gui3DMouseEvent & event);
      virtual void on3DMouseDragged(const Gui3DMouseEvent & event);
      virtual void on3DMouseWheelUp(const Gui3DMouseEvent & event);
      virtual void on3DMouseWheelDown(const Gui3DMouseEvent & event);
      void updateGuiInfo();
      void renderScene(const RectI & updateRect);

      DECLARE_CONOBJECT(TerrainEditor);

private:
	static GFXStateBlock* mSetSelSB;
	static GFXStateBlock* mSetBordSB;
	static GFXStateBlock* mCullCCWSB;
	static GFXStateBlock* mCullStoreSB;
	static GFXStateBlock* mSetNavSB;
	static GFXStateBlock* mAlphaBlendFalseSB;
	static GFXStateBlock* mZTrueSB;
	static GFXStateBlock* mZFalseSB;
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

inline void TerrainEditor::setGridInfoHeight(const GridInfo & info)
{
   setGridHeight(info.mGridPoint, info.mHeight);
}

#endif
