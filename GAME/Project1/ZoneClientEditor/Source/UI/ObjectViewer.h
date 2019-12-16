//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#pragma once

#include "gui/3d/guiTSControl.h"
#include "ts/tsShapeInstance.h"
#include "T3D/tsStatic.h"
#include "T3D/fx/particleEmitterNode.h"
//#include "gfx/gFont.h"

class LightInfo;
class sgDecalProjector;
class ParticleEmitter;
class TerrainBlock;
class CommonFontEX;

class CObjectViewer : public GuiTSCtrl
{
public:
	struct Model
	{
		static S32 shapeIdseed;

		S32					shapeId;
		S32					mountNodeParent;		///< Mount node ID. 这是目标的链接点
		S32					mountNodeSelf;			///< image自身上的链接点
		NetStringHandle		skinNameHandle;

		S32					curNode;
		S32					curMesh;

		Vector<S32>            CollisionDetails;
		Vector<S32>            MeshDetails;

		Vector<ConvexFeature>	feature;
		Resource<TSShape>	shape;
		TSShapeInstance*	shapeInstance;
		U32					action;
		TSThread*			animThread;
		char				actionFileName[1024];
		Model*				parent;
		Vector<Model*>		child;

		Model();
		~Model();
		void clear();
		void selectNode(StringTableEntry node);
		void selectMesh(S32 i);
		Model *selectShape(S32 shapeId);
		bool mountShape(StringTableEntry modelName,StringTableEntry parentPoint,StringTableEntry childPoint);
		bool loadShape(StringTableEntry filename);
		bool isEmpty();

		StringTableEntry getShapeFileName();
		StringTableEntry getActionFileName();
		void loadAction(StringTableEntry filename);
        void transitionAction(StringTableEntry filename); // 平滑过渡切换动作
		void clearAction();
		void setTimeScale(F32 scale);

		bool setMountParent(Model* parentModel, StringTableEntry parentPoint, StringTableEntry selfPoint);
		bool addMountChild(Model* childModel);
		bool deleteMountChild(Model* childModel);
		void getMountMatrix(StringTableEntry linkPoint, MatrixF& mat);		// linkPoint是目标链接点，可以为空
		void getWorldMX(MatrixF &mat);					//获得从基础模型开始的矩阵
		void getNodeMX(S32 mountNode,MatrixF &mat);		//获得从基础模型开始的node矩阵
		int getNodeCount();
		int getCollisionCount();
		int getDetailCount();
		int getMeshCount(int i);
		int getMeshFaceCount(int i);
		int getTextureCount();
		StringTableEntry getTextureName(int i);
		StringTableEntry getNodeName(int i);
		StringTableEntry getMountNodeName();
		StringTableEntry getParentNodeName();

		void rebuildMaterials();
		void render(F32 dt);
		void renderNode();
		void renderCollection();

		void fillItemMsg(StringTableEntry);
		void fillMeshMsg();
		void fillNodeMsg();
	};

    struct Part
    {
        UTF8                name[256];
        ParticleEmitterNode* part;
        Model*				parent;
        UTF8                parentPointName[32];
        S32					mountNodeParent;
        bool                reset;

        Part();
        ~Part();        
        bool loadPart(StringTableEntry _name);    
        bool setMountParent(Model* parentModel = NULL , StringTableEntry parentPoint = NULL);
        void setTime(F32 time); 
        void setTimeScale(F32 scale);        
        void render(CObjectViewer *,SceneState *pSceneState);
    };

private:
   typedef GuiTSCtrl Parent;

protected:
   enum MouseState
   {
      None,
      MouseDown,
	  RightMouseDown,
      MiddleMouseDown,
   };

   MouseState	mMouseState;

   Model		*mBaseObject;					// 主物件
   static Model		*mCurrentObject;				// 当前操作对象

   MatrixF		mCameraMatrix;					//摄像机位置
   MatrixF		mWorldMatrix;					//世界调整位置

   F32			mLightRotX;						//对象旋转
   F32			mLightRotZ;					//对象旋转
   F32			mWorldPatch;					//X轴旋转
   F32			mWorldRow;						//Z轴旋转
   F32			mCameraZoom;					//相机推进拉远
   F32			mWorldMoveX;					//世界平移
   F32			mWorldMoveZ;					//世界平移

   VectorF      mLightDir;
   ColorF		mLightColor;
   ColorF		mAmbientColor;

   S32			lastRenderTime;

   Point2I		mLastMousePoint;				//鼠标点

   LightInfo*	mFakeSun;

   //Vector<ParticleEmitter *> mParticleEmitter;
   //Vector<sgDecalProjector *> mDecalList;
   //TerrainBlock *pTerrain;

public:
   static bool	mShowBounds;					//显示bounds
   static bool	mShowCollection;				//显示碰撞体
   static S32	mFillMode;						//显示实体，网格，骨骼
   static bool	mShowMesh;						//显示选择Mesh
   static bool	mAltFlag;						//
   static bool	mPlayAnimation;					//
   static F32   mAnimationScale;
   static bool  mAnimationLoop;
   static bool	mShowLightDir;					//显示光照方向
   static bool	mlightEnable;					//使用光照

public:
   bool onWake();

   bool onMouseWheelUp(const GuiEvent& event);
   bool onMouseWheelDown(const GuiEvent& event);
   void onMouseDown(const GuiEvent &event);
   void onMouseUp(const GuiEvent &event);
   void onMouseDragged(const GuiEvent &event);
   void onRightMouseDown(const GuiEvent &event);
   void onRightMouseUp(const GuiEvent &event);
   void onRightMouseDragged(const GuiEvent &event);
   void onMiddleMouseDown(const GuiEvent &event);
   void onMiddleMouseUp(const GuiEvent &event);
   void onMiddleMouseDragged(const GuiEvent &event);
   bool onKeyDown(const GuiEvent &event);
   bool onKeyUp(const GuiEvent &event);

   StringTableEntry getCurrentShapeFileName();
   void clearCurrentAction();
   void loadCurrentAction(StringTableEntry fileName);
   StringTableEntry getCurrentActionFileName();
   void setTimeScale(F32 );
   F32 getTimeScale();

   void selectNode(StringTableEntry node);
   void selectMesh(int i);
   void deleteCurrentShape();
   void selectCurrentShape(int shapeId);
   void loadBaseShape(StringTableEntry modelName);
   void mountShapeToCurrentObject(StringTableEntry modelName,StringTableEntry parentPoint,StringTableEntry childPoint);

	void beginItemSearch();
	void beginMeshSearch();
	void baseNodeSearch();
	void currentNodeSearch();
	StringTableEntry getNext();

	int getCurrentShapeTextureCount();
	StringTableEntry getCurrentShapeTextureName(int i);
	int getCurrentShapeMeshFaceCount(int i);
	int getCurrentShapeDetailCount();
	int getCurrentShapeMeshCount(int i);
	int getCurrentShapeCollisionCount();
	int getCurrentShapeNodeCount();
   StringTableEntry getCurrentShapeNodeName(int i);
   StringTableEntry getCurrentShapeMountNodeName();
   StringTableEntry getCurrentShapeParentNodeName();

   void rebuildMaterials();

   void setLightColor(F32 r,F32 g,F32 b);
   void setAmbientColor(F32 r,F32 g,F32 b);
   const char *getLightColor();
   const char *getAmbientColor();
   const char *getLightDir();


   void UpdateData();
   void resetCamera();
   void LinkCamera();
   bool processCameraQuery(CameraQuery *query);

   void setFillMode(int mode);
   void renderWorld(const RectI &updateRect);

public: // 粒子相关
    void loadBasePart(StringTableEntry filename);
    void mountPartToCurrentObject(StringTableEntry filename);
	 void mountPartToCurrentObjectByLink(StringTableEntry filename, StringTableEntry linkPoint);
    void clearAllParts();
	 void clearParts();
    Part* mBasePart;
    Vector<Part*> mLinkParts;

    static CObjectViewer* smObjectViewer;

    void animBgColor(S32 dt);

public:
   DECLARE_CONOBJECT(CObjectViewer);

   CObjectViewer();
   ~CObjectViewer();

   static void initPersistFields();

   SceneState * createSceneState();

   void clearModel();
	void clearOther();
private:
	//render collection
	static GFXStateBlock* mSetCollSB;
	static GFXStateBlock* mFillSolidSB;
	static GFXStateBlock* mFillWireSB;
	//render world
	static GFXStateBlock* mSetWorldSB;
public:
	//全局函数使用的状态块
	static GFXStateBlock* mColorOpStoreSB;
	static GFXStateBlock* mColorOpSB;
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

