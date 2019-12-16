//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _WORLDEDITOR_H_
#define _WORLDEDITOR_H_

#ifndef _EDITTSCTRL_H_
#include "gui/missionEditor/editTSCtrl.h"
#endif
#ifndef _CONSOLETYPES_H_
#include "console/consoleTypes.h"
#endif
#ifndef _GFXTEXTUREHANDLE_H_
#include "gfx/gfxTextureHandle.h"
#endif
#ifndef _UTIL_SIGNAL_H_
#include "util/tSignal.h"
#endif

class Path;
class SceneObject;
class GFXStateBlock;
class WorldEditor : public EditTSCtrl
{
   typedef EditTSCtrl Parent;

	public:

	void ignoreObjClass(U32 argc, const char** argv);
	void clearIgnoreList();

	void undo();
	void redo();
	
	void clearSelection();
	void selectObject(const char* obj);
	void unselectObject(const char* obj);
	
	S32 getSelectionSize();
	S32 getSelectObject(S32 index);	
   const Point3F& getSelectionCentroid();
	const char* getSelectionCentroidText();
   const Box3F& getSelectionBounds();
	
	void dropCurrentSelection();
	void deleteCurrentSelection();
	void copyCurrentSelection();	
	bool canPasteSelection();

   bool alignByBounds(S32 boundsAxis);
   bool alignByAxis(S32 axis);

   void resetSelectedRotation();
   void resetSelectedScale();

	const char* getMode();
   bool setMode(const char *mode);

	void addUndoState();
	void redirectConsole(S32 objID);

   public:

      struct CollisionInfo
      {
         SceneObject *     obj;
         Point3F           pos;
         VectorF           normal;
      };

      class Selection : public SimObject
      {
         typedef SimObject    Parent;

         private:

            Point3F        mCentroid;
            Point3F        mBoxCentroid;
            Box3F          mBoxBounds;
            bool           mCentroidValid;
            SimObjectList  mObjectList;
            bool           mAutoSelect;

            void           updateCentroid();

         public:

            Selection();
            ~Selection();

            //
            U32 size()  { return(mObjectList.size()); }
            SceneObject * operator[] (S32 index) { return((SceneObject*)mObjectList[index]); }

            bool objInSet(SceneObject *);

            bool addObject(SceneObject *);
            bool removeObject(SceneObject *);
            void clear();

            void onDeleteNotify(SimObject *);

            const Point3F & getCentroid();
            const Point3F & getBoxCentroid();
            const Box3F & getBoxBounds();

            void enableCollision();
            void disableCollision();

            //
            void autoSelect(bool b) { mAutoSelect = b; }
            void invalidateCentroid() { mCentroidValid = false; }

            //
            void offset(const Point3F &);
            void orient(const MatrixF &, const Point3F &);
            void rotate(const EulerF &, const Point3F &);
            void scale(const VectorF &);
            void setScale(const VectorF &);
      };

      //
      static SceneObject * getClientObj(SceneObject *);
      static void setClientObjInfo(SceneObject *, const MatrixF &, const VectorF &);
      static void updateClientTransforms(Selection &);

   // VERY basic undo functionality - only concerned with transform/scale/...
   private:

      struct SelectionState
      {
         struct Entry
         {
            MatrixF     mMatrix;
            VectorF     mScale;

            // validation
            U32         mObjId;
            U32         mObjNumber;
         };

         Vector<Entry>  mEntries;

         SelectionState() 
         {
            VECTOR_SET_ASSOCIATION(mEntries);
         }
      };

      SelectionState * createUndo(Selection &);
      void addUndo(Vector<SelectionState *> & list, SelectionState * sel);
      bool processUndo(Vector<SelectionState *> & src, Vector<SelectionState *> & dest);
      void clearUndo(Vector<SelectionState *> & list);

      Vector<SelectionState*>       mUndoList;
      Vector<SelectionState*>       mRedoList;

   public:
      // someday get around to creating a growing memory stream...
      struct StreamedObject {
         U8 data[2048];
      };
      Vector<StreamedObject>              mStreamBufs;

      bool deleteSelection(Selection & sel);
      bool copySelection(Selection & sel);
      bool pasteSelection(bool dropSel=true);
      void dropSelection(Selection & sel);
      void dropToGround(Selection & sel, bool hitOnly, const Point3F & centroid, bool useBottomBounds=false);

      // work off of mSelected
      void hideSelection(bool hide);
      void lockSelection(bool lock);

   public:
      bool objClassIgnored(const SceneObject * obj);
      void renderObjectBox(SceneObject * obj, const ColorI & col);
      
   private:
      SceneObject * getControlObject();
      bool collide(const Gui3DMouseEvent & event, CollisionInfo & info,S32 collision_mask = -1);

      // gfx methods
      //void renderObjectBox(SceneObject * obj, const ColorI & col);
      void renderObjectFace(SceneObject * obj, const VectorF & normal, const ColorI & col);
      void renderSelectionWorldBox(Selection & sel);

      void renderPlane(const Point3F & origin);
      void renderMousePopupInfo();
      void renderScreenObj(SceneObject * obj, Point2I sPos);

      void renderPaths(SimObject *obj);
      void renderSplinePath(Path *path);

      // axis gizmo methods...
      void calcAxisInfo();
      bool collideAxisGizmo(const Gui3DMouseEvent & event);
      void renderAxisGizmo();
      void renderAxisGizmoArrows();
      void renderAxisGizmoBoxes();
      void renderAxisGizmoCircles();
      void renderAxisGizmoText();

      // axis gizmo state...
      Point3F     mAxisGizmoCenter;
      VectorF     mAxisGizmoVector[3];
      F32         mAxisGizmoProjLen;
      S32         mAxisGizmoSelAxis;
      PlaneF      mAxisGizmoSelPlane;
      Point3F     mAxisGizmoSelPlanePoint;
      Point3F     mAxisGizmoSelStart;
      ColorI      mAxisGizmoColors[6][2];
      U32         mAxisGizmoPlanarVectors[3][2];
      U32         mAxisGizmoPlanarRotAxis[3];
      bool        mUsingAxisGizmo;

      //
      Point3F snapPoint(const Point3F & pnt);
      bool                       mIsDirty;

      //
      bool                       mMouseDown;
      Selection                  mSelected;
      bool                       mUseVertMove;

      Selection                  mDragSelected;
      bool                       mDragSelect;
      RectI                      mDragRect;
      Point2I                    mDragStart;

      // modes for when dragging a selection
      enum {
         Move = 0,
         Rotate,
         Scale
      };

      //
      U32                        mCurrentMode;
      U32                        mDefaultMode;

      S32                        mRedirectID;

      CollisionInfo              mHitInfo;
      Point3F                    mHitOffset;
      SimObjectPtr<SceneObject>  mHitObject;
      Point2I                    mHitMousePos;
      Point3F                    mHitCentroid;
      EulerF                     mHitRotation;
      bool                       mMouseDragged;
      Gui3DMouseEvent            mLastMouseEvent;
      F32                        mLastRotation;
      Point3F                    mLastMousePlanarPos;

      //
      class ClassInfo
      {
         public:
            ~ClassInfo();

            struct Entry
            {
               StringTableEntry  mName;
               bool              mIgnoreCollision;
               GFXTexHandle      mDefaultHandle;
               GFXTexHandle      mSelectHandle;
               GFXTexHandle      mLockedHandle;
            };

            Vector<Entry*>       mEntries;
      };


      ClassInfo            mClassInfo;
      ClassInfo::Entry     mDefaultClassEntry;

      //bool objClassIgnored(const SceneObject * obj);
      ClassInfo::Entry * getClassEntry(StringTableEntry name);
      ClassInfo::Entry * getClassEntry(const SceneObject * obj);
      bool addClassEntry(ClassInfo::Entry * entry);

   // persist field data
   public:

      enum {
         DropAtOrigin = 0,
         DropAtCamera,
         DropAtCameraWithRot,
         DropBelowCamera,
         DropAtScreenCenter,
         DropAtCentroid,
         DropToGround
      };

	  enum StickyObjectActions
	  {
		  ACTION_NULL = 0,
		  ACTION_MOVE,
		  ACTION_COPY
	  };
	  
      bool              mPlanarMovement;
      S32               mUndoLimit;
      S32               mDropType;
      F32               mProjectDistance;
      bool              mBoundingBoxCollision;
      bool              mRenderPlane;
      bool              mRenderPlaneHashes;
      ColorI            mGridColor;
      F32               mPlaneDim;
      Point3F           mGridSize;
      bool              mRenderPopupBackground;
      ColorI            mPopupBackgroundColor;
      ColorI            mPopupTextColor;
      StringTableEntry  mSelectHandle;
      StringTableEntry  mDefaultHandle;
      StringTableEntry  mLockedHandle;
      ColorI            mObjectTextColor;
      bool               mObjectsUseBoxCenter;
      S32               mAxisGizmoMaxScreenLen;
      bool              mAxisGizmoActive;
      F32               mMouseMoveScale;
      F32               mMouseRotateScale;
      Point3F           mCurrentScaleBuffer;
      Point3F           mMouseScaleBuffer;
      F32               mMouseScaleScale;
      F32               mMinScaleFactor;
      F32               mMaxScaleFactor;
      ColorI            mObjSelectColor;
      ColorI            mObjMouseOverSelectColor;
      ColorI            mObjMouseOverColor;
      bool              mShowMousePopupInfo;
      ColorI            mDragRectColor;
      bool              mRenderObjText;
      bool              mRenderObjHandle;
	  static bool		mRenderStatic;
	  static bool       mRenderNPC;
      StringTableEntry  mObjTextFormat;
      ColorI            mFaceSelectColor;
      bool              mRenderSelectionBox;
      ColorI            mSelectionBoxColor;
      bool              mSelectionLocked;
      bool              mPerformedDragCopy;
      bool              mSnapToGrid;
      bool              mAllowSnapRotations;
      F32               mRotationSnap;
      bool              mAllowSnapScale;
      F32               mScaleSnap;
      bool              mSameScaleAllAxis;
      bool              mToggleIgnoreList;
      bool              mRenderNav;
      bool              mNoMouseDrag;
      bool              mStickToGround;
      bool              mDropAtBounds;
      F32               mDropBelowCameraOffset;
	  S32		        mStickyObjectAction;
	  bool              mDropWithArbitraryRotation;

   private:
      S32 mCurrentCursor;
      void setCursor(U32 cursor);
      void get3DCursor(GuiCursor *&cursor, bool &visible, const Gui3DMouseEvent &event);
   public:

      WorldEditor();
      ~WorldEditor();

      // SimObject
      bool onAdd();
      void onEditorEnable();
      void setDirty();

      // EditTSCtrl
      void on3DMouseMove(const Gui3DMouseEvent & event);
      void on3DMouseDown(const Gui3DMouseEvent & event);
      void on3DMouseUp(const Gui3DMouseEvent & event);
      void on3DMouseDragged(const Gui3DMouseEvent & event);
      void on3DMouseEnter(const Gui3DMouseEvent & event);
      void on3DMouseLeave(const Gui3DMouseEvent & event);
      void on3DRightMouseDown(const Gui3DMouseEvent & event);
      void on3DRightMouseUp(const Gui3DMouseEvent & event);

      void updateGuiInfo();

      //
      void renderScene(const RectI & updateRect);

      static void initPersistFields();

      DECLARE_CONOBJECT(WorldEditor);

	  static Signal<WorldEditor*> smRenderSceneSignal;
private:
	static GFXStateBlock* mSetSelSB;
	static GFXStateBlock* mSetObjBoxSB;
	static GFXStateBlock* mSetObjFaceSB;
	static GFXStateBlock* mClearObjFaceSB;
	static GFXStateBlock* mSetPlaneSB;
	static GFXStateBlock* mSetPathSB;
	//static GFXStateBlock* mSetPath2SB;
	static GFXStateBlock* mSetAxisSB;
	static GFXStateBlock* mSetAxisTextSB;
	static GFXStateBlock* mSetSceneSB;
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

#endif



