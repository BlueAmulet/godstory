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
		Limit_AtkMonster		= BIT(0),	// 可否打怪
		Limit__					= BIT(1),	// 可否被怪攻击（无效）
		Limit_PK				= BIT(2),	// 可否PK
		Limit_DropItem			= BIT(3),	// 死亡是否遗失物品
		Limit_HidePlayer		= BIT(4),	// 是否隐藏本区域玩家
		Limit_UseItem			= BIT(5),	// 是否可以使用道具
		Limit_Stall				= BIT(6),	// 可否摆摊
		Limit_UseSkill			= BIT(7),	// 是否可以使用技能
		Limit_CanJump			= BIT(8),	// 是否可以使用轻功
	};

   S32  tickPeriodMS;						// 多长时间检测一次
   
   U32	mTriggerID;							// 触发器ID
   U32 mFatherTriggerID;					// 父触发器ID
   StringTableEntry mPictureName;			// 一级触发器图片名
   StringTableEntry mAreaName;				// 区域名称
   U8	mTranslateType;						// 传送类型
   U16  mTargetMapID;						// 目标地图ID
   StringTableEntry mTranslatePos;			// 传送目标地点坐标
   StringTableEntry mEnterScript;			// 进入时执行脚本
   StringTableEntry mExitScript;			// 退出时执行脚本
   StringTableEntry mReluvePos;				// 复活坐标
   StringTableEntry mBackMusic;				// 背景音乐
   StringTableEntry mSunAbient;				// 环境光颜色
   StringTableEntry mLightColor;			// 灯光颜色
   StringTableEntry mShaderColor;			// shader颜色
   StringTableEntry mFogColor;				// 雾的颜色
   U16 mZoneID;								// 复活地图ID
   U32	mBaseLimit;							// 基本限制			
   U16	mStallPrice;						// 摆摊价格
   U16	mTradeTax;							// 交易税

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

   Polyhedron        mTriggerPolyhedron;	//触发器的多面体
   EarlyOutPolyList  mClippedList;
   Vector<GameBase*> mObjects;					

   TriggerData*      mDataBlock;
   TriggerData*      mFatherDataBlock;
  // StringTableEntry    mName;				//触发器name
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
	typedef stdext::hash_map<S32, TriggerData*>  dataMap;//存放触发器的相关信息的容器
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