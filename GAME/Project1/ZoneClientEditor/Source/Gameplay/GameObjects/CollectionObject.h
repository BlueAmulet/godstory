#pragma once

#include <map>
#include "math/mPoint.h"
#include "math/mMatrix.h"
#include "T3D/shapeBase.h"
#include "T3D/item.h"
#include "Gameplay/Data/EventTriggerData.h"

struct CollectionObjectData : public ShapeBaseData
{
private:
	typedef ShapeBaseData Parent;

public:
	void				FillTaskIds				(StringTableEntry sId);

public:
	StringTableEntry	mCollectionName;		//物件名称
	U32					mTriggeredShapesSetId;	//触发后模型组编号
	U32					mType;					//物件类别
	U32					mSubType;				//物件子类
	U32					mRefreshTime;			//刷新时间
	U32					mReserveTime;			//触发后保留时间
	U32					mEventTriggerId;		//物件类触发器ID
	U32					mRouteId;				//路径ID
	bool				mIsLoopTrigger;			//可循环触发
	StringTableEntry	mTriggerSound;			//触发后音效
	bool				mIsSensible;			//是否可感知
	F32					mSenseDistance;			//感知范围
	F32					mTriggerDistance;		//触发范围
	U32					mFadeTime;				//淡入淡出时间
	U32					mTriggeredTimes;		//可触发次数
	Vector<U32>			mArrTaskIds;			//任务ID列表
	Point3F				mObjScale;				//缩放
};

class CollectionRepository
{
public:
	typedef stdext::hash_map<U32, CollectionObjectData*> CollectionDataMap;

public:
	CollectionRepository();
	~CollectionRepository();
	void Read();
	CollectionObjectData *GetCollectionObjectData(U32 dataBlockId);
	void Clear();
	S32 GetDataCount();
	U32 GetCollectionIdByIndex(S32 index);

private:
	bool Insert(CollectionObjectData *pData = NULL);
	void AssertErrorMsg(bool bAssert, const char *msg, int rowNum);

private:
	CollectionDataMap mMap;
};

extern CollectionRepository gCollectionRepository;

class Player;
class DropItemList;
class CollectionObject : public Item
{
	typedef ShapeBase Parent;

public:

	const static U64 PositionMask		= Parent::NextFreeMask;
	const static U64 TriggerTimesMask	= Parent::NextFreeMask << 1;
	const static U64 RefreshMask		= Parent::NextFreeMask << 2;

	enum Main_Collection_Type
	{
		
	};

	enum Sub_Collection_Type
	{
		
	};

	struct ActionAnimation {
		U32 action;
		TSThread* thread;
		S32 delayTicks;               // before picking another.
		bool forward;
		bool atEnd;
	} mActionAnimation;

public:
	CollectionObject();
	~CollectionObject();

	DECLARE_CONOBJECT(CollectionObject);

	bool			prepRenderImage(SceneState* state, const U32 stateKey, const U32 startZone, const bool modifyBaseZoneState);
	virtual bool	onNewDataBlock(GameBaseData* dptr);
	void			processTick(const Move * move);
	bool			onAdd();
	void			onRemove();
	virtual bool	canAddToScope(NetConnection *con);
	U64				packUpdate		(NetConnection *conn, U64 mask, BitStream *stream);
	void			unpackUpdate	(NetConnection *conn, BitStream *stream);

	bool			castRay(const Point3F &start, const Point3F &end, RayInfo* info);
	bool			collideBox(const Point3F &start, const Point3F &end, RayInfo* info);
	void			buildConvex(const Box3F& box, Convex* convex);
	bool			checkTaskList(Player *pPlayer);

	void			updateAnimations();
	bool			updateShape();

	//逻辑部分
	enWarnMessage	potentialEnterTrigger(Player *pPlayer);
	enWarnMessage	enterTrigger(Player *pPlayer);
	void			leaveTrigger(Player *pPlayer);
	
#ifdef NTJ_SERVER
	void			serverSendStartMsg(Player *pPlayer, U32 nEventType);		//服务端通知客户端掉物列表生成完毕,可以开始拾取掉落的物品
	void			serverSendEndMsg(Player *pPlayer);			//服务端通知客户端掉物拾取结束
	void			ExecPredefinedEvent(Player *pPlayer, U8 nEventType, U32 nEventId);
#endif

#ifdef NTJ_CLIENT
	void			drawObjectInfo(Player *,void *,void *);
	void			clientSendStartMsg(Player *pPlayer);		//客户端通知服务端触发开始
	void			clientSendEndMsg(Player *pPlayer);			//客户端通知服务端触发结束
#endif

	SimTime			getVocalTime();
	bool			IsTriggerCountExceed();
	bool			IsLoopTrigger();

protected:
	U32				mCurrTriggerTimes;
	Convex*			mConvexList;

public:
	CollectionObjectData*	mDataBlock;

#ifndef NTJ_SERVER
	bool			m_bRemove;
#endif

#ifdef NTJ_SERVER
	DropItemList*	m_DropItemList;
	SimTime			mElapseTime;
#endif
};
