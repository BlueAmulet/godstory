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
	StringTableEntry	mCollectionName;		//�������
	U32					mTriggeredShapesSetId;	//������ģ������
	U32					mType;					//������
	U32					mSubType;				//�������
	U32					mRefreshTime;			//ˢ��ʱ��
	U32					mReserveTime;			//��������ʱ��
	U32					mEventTriggerId;		//����ഥ����ID
	U32					mRouteId;				//·��ID
	bool				mIsLoopTrigger;			//��ѭ������
	StringTableEntry	mTriggerSound;			//��������Ч
	bool				mIsSensible;			//�Ƿ�ɸ�֪
	F32					mSenseDistance;			//��֪��Χ
	F32					mTriggerDistance;		//������Χ
	U32					mFadeTime;				//���뵭��ʱ��
	U32					mTriggeredTimes;		//�ɴ�������
	Vector<U32>			mArrTaskIds;			//����ID�б�
	Point3F				mObjScale;				//����
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

	//�߼�����
	enWarnMessage	potentialEnterTrigger(Player *pPlayer);
	enWarnMessage	enterTrigger(Player *pPlayer);
	void			leaveTrigger(Player *pPlayer);
	
#ifdef NTJ_SERVER
	void			serverSendStartMsg(Player *pPlayer, U32 nEventType);		//�����֪ͨ�ͻ��˵����б��������,���Կ�ʼʰȡ�������Ʒ
	void			serverSendEndMsg(Player *pPlayer);			//�����֪ͨ�ͻ��˵���ʰȡ����
	void			ExecPredefinedEvent(Player *pPlayer, U8 nEventType, U32 nEventId);
#endif

#ifdef NTJ_CLIENT
	void			drawObjectInfo(Player *,void *,void *);
	void			clientSendStartMsg(Player *pPlayer);		//�ͻ���֪ͨ����˴�����ʼ
	void			clientSendEndMsg(Player *pPlayer);			//�ͻ���֪ͨ����˴�������
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
