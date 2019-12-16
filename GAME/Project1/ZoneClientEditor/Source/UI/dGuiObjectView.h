//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#pragma once

#include "gui/3d/guiTSControl.h"
#include "ts/tsShapeInstance.h"
#include "T3D/tsStatic.h"
#include "T3D/fx/particleEmitterNode.h"
#include <hash_map>
#include "Effects/EffectPacket.h"
class LightInfo;
class GFXStateBlock;

class dGuiObjectView : public GuiTSCtrl
{
public:
	enum Content
	{
		MaxMounted			= 8,
	};

	struct Model
	{
		StringTableEntry	shapeId;
		S32					mountNodeTarget;		///< Mount node ID. 这是目标的链接点
		S32					mountNodeSelf;			///< image自身上的链接点

		Resource<TSShape>	shape;
		TSShapeInstance*	shapeInstance;
		U32					action;
		TSThread*			animThread;
		Model*				target;

		Model();
		~Model();
		void clear();
		bool load(StringTableEntry id);
		bool setSkin(StringTableEntry id,char skinTag);
		bool isEmpty();
		void setAction(U32 act);
		bool setMountTarget(Model& tar, StringTableEntry tarPoint, StringTableEntry selfPoint);
		void getMountMatrix(StringTableEntry linkPoint, MatrixF& mat);		// linkPoint是目标链接点，可以为空
	};

    struct Part
    {
        S32					mountNodeTarget;    ///< Mount node ID. 这是目标的链接点
        Model*			    target;

        ParticleEmitterNode*partNodeInst;       ///粒子节点实例( 粒子对外都是以 ParticleEmitterNode 包好后作为最小单位提供给外部调用的 )

        Part();
        ~Part();
        void clear();
        bool load(StringTableEntry _partNodeDataBlockName);
        bool loadEx(StringTableEntry _partNodeDataBlockName);  
        bool isEmpty();

        bool setMountTarget(Model* tar, StringTableEntry tarPoint);
    };

    struct AttachedModel
    {
        Model* pAttachedModel;
        MatrixF ModelMatrix;        
        AttachedModel(){pAttachedModel = NULL;}       
    };   

private:
   typedef GuiTSCtrl Parent;

protected:
   enum MouseState
   {
      None,
      Rotating,
      Zooming
   };

   MouseState	mMouseState;

   Model		mScene;							     // 场景
   std::list<AttachedModel*>    mlistLinkedtoScene;  // 附属于场景的模型 
   std::list<Part*> mlistLinkedtoScene_part;         // 附属于场景的模型 

   Model		mRide;							  // 骑乘
   Model		mPlayer;					  	  // 主角
   Model		mMounted[MaxMounted];			  // 武器等
   Model		mPet;							  // 宠物

   MatrixF		mCameraMatrix;					   // 摄像机位置
   MatrixF		mPlayerMatrix;
   MatrixF		mPetMatrix;

   F32			mPlayerRotZ;

   S32			lastRenderTime;
   S32			mAnimationSeq;

   Point2I		mLastMousePoint;

   LightInfo*	mFakeSun;

   bool         bAutoLeftRotation;          //主角是否自动左旋转
   bool         bAutoRightRotation;         //主角是否自动右旋转
   // 特效模型、粒子
   Vector<Model*> mEffcModel;
   Vector<Part*> mEffcPart;
	stdext::hash_map<U8, stdext::hash_map<U8, Model*>> mEM;//mEffectModel: <特效包id， <特效对象(模型)id，特效对象(模型)指针>>
	typedef stdext::hash_map<U8, Model*>::iterator Mit;
	stdext::hash_map<U8, stdext::hash_map<U8, Part*>> mEP;//mEffectPart：<特效包id， <特效对象(粒子)id，特效对象(粒子)指针>>
	typedef stdext::hash_map<U8, Part*> Pit;

public:
   inline Model& getScene(){ return mScene; }
   inline Model& getRide(){ return mRide; }
   inline Model& getPlayer(){ return mPlayer; }
   inline Model& getMounted(int slot){ return mMounted[slot]; }
   inline Model& getPet(){ return mPet; }

   inline Model* getEffcModel(int index){ return mEffcModel[index]; }
   inline Part* getEffcPart(int index){ return mEffcPart[index]; }

   bool onWake();
   void onSleep();

   void onMouseEnter(const GuiEvent &event);
   void onMouseLeave(const GuiEvent &event);
   void onMouseDown(const GuiEvent &event);
   void onMouseUp(const GuiEvent &event);
   void onMouseDragged(const GuiEvent &event);
   void onRightMouseDown(const GuiEvent &event);
   void onRightMouseUp(const GuiEvent &event);
   void onRightMouseDragged(const GuiEvent &event);

   void setScene(const char * modelName);
   void setPlayer(const char * modelName, char skinTag = 0);
   void setPlayerAnimation(S32 index);
   void setMounted(U32 slot, const char * modelName, StringTableEntry tarPoint, StringTableEntry selfPoint,char skinTag = 0);
   void getMountedTransform(U32 slot, MatrixF *mat);
   void getNodeName_withPrefix(const char * prefix,char** NodeNameSet , int &count);
   
   // 特效模型、粒子
   // 脚本函数导出的相关说明(可以以数组下标来与脚本交互):
   //     返回刚新 mount 的特效的下标给脚本: return ( mEffcModel.size() - 1 ) 或者 return ( mEffcPart.size() - 1 )
   //     由下标返回各特效对象给脚本: return mEffcModel[index] 或者 return mEffcPart[index]
   void setEffcModelMounted(const char* modelName, Model& tarModel, StringTableEntry tarPoint, StringTableEntry selfPoint, char skinTag = NULL);
   void getEffcModelMountedTransform(Model* effcModel, MatrixF *_mat);

   void setEffcModelAnimation(Model* effcModel, S32 index);

   void setEffcPartMounted(const char* _partNodeDataBlockName, Model* tarModel, StringTableEntry tarPoint);
   void getEffcPartMountedTransform(Part* effcPart, MatrixF *_mat);

   void clearAllEffc();

	//特效对象hash_map
	void addEffectMounted(EP_ITEM id);

   /// Sets the distance at which the camera orbits the object. Clamped to the
   /// acceptable range defined in the class by min and max orbit distances.
   ///
   /// \param distance The distance to set the orbit to (will be clamped).
   void setOrbitDistance(F32 distance);

   bool processCameraQuery(CameraQuery *query);
   void renderWorld(const RectI &updateRect);

   DECLARE_CONOBJECT(dGuiObjectView);

   dGuiObjectView();
   ~dGuiObjectView();

   static void initPersistFields();

   void clear();

private:

   static const S32 MAX_ANIMATIONS  =  6;    ///< Maximum number of animations for the primary model displayed in this control
   static const S32 NO_NODE         =  -1;   ///< Indicates there is no node with a mounted object
	static GFXStateBlock* mSetSB;
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

