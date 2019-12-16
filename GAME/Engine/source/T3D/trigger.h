//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _H_TRIGGER
#define _H_TRIGGER

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif
#ifndef _GAMEBASE_H_
#include "T3D/gameBase.h"
#endif
#ifndef _MBOX_H_
#include "math/mBox.h"
#endif
#ifndef _EARLYOUTPOLYLIST_H_
#include "collision/earlyOutPolyList.h"
#endif

#include <hash_map>

class IColumnData;
class Convex;

typedef const char * TriggerPolyhedronType;
DefineConsoleType( TypeTriggerPolyhedron, TriggerPolyhedronType * )

struct TriggerData: public GameBaseData 
{
	typedef GameBaseData Parent;
public:
	enum BaseLimit
	{
		Limit_AtkMonster		= BIT(0),	// �ɷ���
		Limit__					= BIT(1),	// �ɷ񱻹ֹ�������Ч��
		Limit_PK				= BIT(2),	// �ɷ�PK
		Limit_DropItem			= BIT(3),	// �����Ƿ���ʧ��Ʒ
		Limit_HidePlayer		= BIT(4),	// �Ƿ����ر��������
		Limit_UseItem			= BIT(5),	// �Ƿ����ʹ�õ���
		Limit_Stall				= BIT(6),	// �ɷ��̯
		Limit_UseSkill			= BIT(7),	// �Ƿ����ʹ�ü���
		Limit_CanJump			= BIT(8),	// �Ƿ����ʹ���Ṧ
	};

   S32  tickPeriodMS;						// �೤ʱ����һ��
   
   U32	mTriggerID;							// ������ID
   U32 mFatherTriggerID;					// ��������ID
   StringTableEntry mPictureName;			// һ��������ͼƬ��
   StringTableEntry mAreaName;				// ��������
   U8	mTranslateType;						// ��������
   U16  mTargetMapID;						// Ŀ���ͼID
   StringTableEntry mTranslatePos;			// ����Ŀ��ص�����
   StringTableEntry mEnterScript;			// ����ʱִ�нű�
   StringTableEntry mExitScript;			// �˳�ʱִ�нű�
   StringTableEntry mReluvePos;				// ��������
   StringTableEntry mBackMusic;				// ��������
   StringTableEntry mSunAbient;				// ��������ɫ
   StringTableEntry mLightColor;			// �ƹ���ɫ
   StringTableEntry mShaderColor;			// shader��ɫ
   StringTableEntry mFogColor;				// �����ɫ
   U16 mZoneID;								// �����ͼID
   U32	mBaseLimit;							// ��������			
   U16	mStallPrice;						// ��̯�۸�
   U16	mTradeTax;							// ����˰

   TriggerData();
   DECLARE_CONOBJECT(TriggerData);
   bool onAdd();
   static void initPersistFields();

   virtual void packData  (BitStream* stream);
   virtual void unpackData(BitStream* stream);
   StringTableEntry getBackSoundID();
   inline bool getBaseLimit(U32 l) { return (mBaseLimit & l);}
};

class Trigger : public GameBase
{
   typedef GameBase Parent;

   Polyhedron        mTriggerPolyhedron;	//�������Ķ�����
   EarlyOutPolyList  mClippedList;
   Vector<GameBase*> mObjects;					

   TriggerData*      mDataBlock;
   TriggerData*      mFatherDataBlock;
  // StringTableEntry    mName;				//������name
   U32               mLastThink;
   U32               mCurrTick;

  protected:
   bool onAdd();
   void onRemove();
   void onDeleteNotify(SimObject*);
   bool onNewDataBlock(GameBaseData* dptr);
   void onEditorEnable();
   void onEditorDisable();

   bool testObject(GameBase* enter);
   void processTick(const Move *move);

   Convex* mConvexList;
   void buildConvex(const Box3F& box, Convex* convex);

   // Rendering
  protected:
   bool prepRenderImage  ( SceneState *state, const U32 stateKey, const U32 startZone, const bool modifyBaseZoneState=false);
   void renderObject     ( SceneState *state);

  public:
   void setTransform(const MatrixF &mat);

  public:
   Trigger();
   ~Trigger();

   void setTriggerPolyhedron(const Polyhedron&);

   void      potentialEnterObject(GameBase*);
   U32       getNumTriggeringObjects() const;
   GameBase* getObject(const U32);

   DECLARE_CONOBJECT(Trigger);
   static void initPersistFields();

   void OnSerialize( CTinyStream& stream );
   void OnUnserialize( CTinyStream& stream );

   U64  packUpdate  (NetConnection *conn, U64 mask, BitStream* stream);
   void unpackUpdate(NetConnection *conn,           BitStream* stream);

   bool castRay(const Point3F &start, const Point3F &end, RayInfo* info);
   void setEnterSettings(TriggerData* data, GameBase* player);
   void setExistSettings(TriggerData* data, GameBase* player);
   TriggerData* getTriggerData() { return mDataBlock; }
};

inline U32 Trigger::getNumTriggeringObjects() const
{
   return mObjects.size();
}

inline GameBase* Trigger::getObject(const U32 index)
{
   AssertFatal(index < getNumTriggeringObjects(), "Error, out of range object index");

   return mObjects[index];
}
// -Edit> [3/11/2009 icerain]  TriggerData Manager
class triggerDataMannager
{
public:
	typedef stdext::hash_map<S32, TriggerData*>  dataMap;//��Ŵ������������Ϣ������
public:
	triggerDataMannager();
	~triggerDataMannager();

	void ReadTriggerData();
	TriggerData* getTriggerData(S32 id);
	void clearTriggerData();
	static void creatTriggerDataManager();
	static void destroyTriggerDataManager();
	void removeTriggerData(S32 id);
	StringTableEntry getBackMusicID(U32 ID);
	IColumnData* mColumnData;	
private:
	dataMap mDataMap;

};

extern triggerDataMannager* g_TriggerDataManager;
#endif