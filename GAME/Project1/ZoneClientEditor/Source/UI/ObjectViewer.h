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
		S32					mountNodeParent;		///< Mount node ID. ����Ŀ������ӵ�
		S32					mountNodeSelf;			///< image�����ϵ����ӵ�
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
        void transitionAction(StringTableEntry filename); // ƽ�������л�����
		void clearAction();
		void setTimeScale(F32 scale);

		bool setMountParent(Model* parentModel, StringTableEntry parentPoint, StringTableEntry selfPoint);
		bool addMountChild(Model* childModel);
		bool deleteMountChild(Model* childModel);
		void getMountMatrix(StringTableEntry linkPoint, MatrixF& mat);		// linkPoint��Ŀ�����ӵ㣬����Ϊ��
		void getWorldMX(MatrixF &mat);					//��ôӻ���ģ�Ϳ�ʼ�ľ���
		void getNodeMX(S32 mountNode,MatrixF &mat);		//��ôӻ���ģ�Ϳ�ʼ��node����
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

   Model		*mBaseObject;					// �����
   static Model		*mCurrentObject;				// ��ǰ��������

   MatrixF		mCameraMatrix;					//�����λ��
   MatrixF		mWorldMatrix;					//�������λ��

   F32			mLightRotX;						//������ת
   F32			mLightRotZ;					//������ת
   F32			mWorldPatch;					//X����ת
   F32			mWorldRow;						//Z����ת
   F32			mCameraZoom;					//����ƽ���Զ
   F32			mWorldMoveX;					//����ƽ��
   F32			mWorldMoveZ;					//����ƽ��

   VectorF      mLightDir;
   ColorF		mLightColor;
   ColorF		mAmbientColor;

   S32			lastRenderTime;

   Point2I		mLastMousePoint;				//����

   LightInfo*	mFakeSun;

   //Vector<ParticleEmitter *> mParticleEmitter;
   //Vector<sgDecalProjector *> mDecalList;
   //TerrainBlock *pTerrain;

public:
   static bool	mShowBounds;					//��ʾbounds
   static bool	mShowCollection;				//��ʾ��ײ��
   static S32	mFillMode;						//��ʾʵ�壬���񣬹���
   static bool	mShowMesh;						//��ʾѡ��Mesh
   static bool	mAltFlag;						//
   static bool	mPlayAnimation;					//
   static F32   mAnimationScale;
   static bool  mAnimationLoop;
   static bool	mShowLightDir;					//��ʾ���շ���
   static bool	mlightEnable;					//ʹ�ù���

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

public: // �������
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
	//ȫ�ֺ���ʹ�õ�״̬��
	static GFXStateBlock* mColorOpStoreSB;
	static GFXStateBlock* mColorOpSB;
	static void init();
	static void shutdown();
	//
	//�豸��ʧʱ����
	//
	static void releaseStateBlock();

	//
	//�豸����ʱ����
	//
	static void resetStateBlock();
};

