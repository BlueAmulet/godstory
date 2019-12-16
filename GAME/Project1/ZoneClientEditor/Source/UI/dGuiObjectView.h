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
		S32					mountNodeTarget;		///< Mount node ID. ����Ŀ������ӵ�
		S32					mountNodeSelf;			///< image�����ϵ����ӵ�

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
		void getMountMatrix(StringTableEntry linkPoint, MatrixF& mat);		// linkPoint��Ŀ�����ӵ㣬����Ϊ��
	};

    struct Part
    {
        S32					mountNodeTarget;    ///< Mount node ID. ����Ŀ������ӵ�
        Model*			    target;

        ParticleEmitterNode*partNodeInst;       ///���ӽڵ�ʵ��( ���Ӷ��ⶼ���� ParticleEmitterNode ���ú���Ϊ��С��λ�ṩ���ⲿ���õ� )

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

   Model		mScene;							     // ����
   std::list<AttachedModel*>    mlistLinkedtoScene;  // �����ڳ�����ģ�� 
   std::list<Part*> mlistLinkedtoScene_part;         // �����ڳ�����ģ�� 

   Model		mRide;							  // ���
   Model		mPlayer;					  	  // ����
   Model		mMounted[MaxMounted];			  // ������
   Model		mPet;							  // ����

   MatrixF		mCameraMatrix;					   // �����λ��
   MatrixF		mPlayerMatrix;
   MatrixF		mPetMatrix;

   F32			mPlayerRotZ;

   S32			lastRenderTime;
   S32			mAnimationSeq;

   Point2I		mLastMousePoint;

   LightInfo*	mFakeSun;

   bool         bAutoLeftRotation;          //�����Ƿ��Զ�����ת
   bool         bAutoRightRotation;         //�����Ƿ��Զ�����ת
   // ��Чģ�͡�����
   Vector<Model*> mEffcModel;
   Vector<Part*> mEffcPart;
	stdext::hash_map<U8, stdext::hash_map<U8, Model*>> mEM;//mEffectModel: <��Ч��id�� <��Ч����(ģ��)id����Ч����(ģ��)ָ��>>
	typedef stdext::hash_map<U8, Model*>::iterator Mit;
	stdext::hash_map<U8, stdext::hash_map<U8, Part*>> mEP;//mEffectPart��<��Ч��id�� <��Ч����(����)id����Ч����(����)ָ��>>
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
   
   // ��Чģ�͡�����
   // �ű��������������˵��(�����������±�����ű�����):
   //     ���ظ��� mount ����Ч���±���ű�: return ( mEffcModel.size() - 1 ) ���� return ( mEffcPart.size() - 1 )
   //     ���±귵�ظ���Ч������ű�: return mEffcModel[index] ���� return mEffcPart[index]
   void setEffcModelMounted(const char* modelName, Model& tarModel, StringTableEntry tarPoint, StringTableEntry selfPoint, char skinTag = NULL);
   void getEffcModelMountedTransform(Model* effcModel, MatrixF *_mat);

   void setEffcModelAnimation(Model* effcModel, S32 index);

   void setEffcPartMounted(const char* _partNodeDataBlockName, Model* tarModel, StringTableEntry tarPoint);
   void getEffcPartMountedTransform(Part* effcPart, MatrixF *_mat);

   void clearAllEffc();

	//��Ч����hash_map
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
	//�豸��ʧʱ����
	//
	static void releaseStateBlock();

	//
	//�豸����ʱ����
	//
	static void resetStateBlock();
};

