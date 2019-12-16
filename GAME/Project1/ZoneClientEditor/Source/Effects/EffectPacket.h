//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef EFFECT_PACKET_H
#define EFFECT_PACKET_H

#include "core/tVector.h"
#include <hash_map>
#include "Common/PacketType.h"
#include "math/mMatrix.h"
#include "math/mPoint.h"
//#include "sceneGraph/sceneObject.h"

class NetConnection;
class BitStream;
class GameBase;
class GameObject;
class SceneObject;

#define EP_ITEM  U32
//#define MAX_STATUS 4

// �̶�����Ч��ID
enum constEP
{
	EP_Wound_Player		= 620001,
	EP_Wound_Other		= 620004,
	EP_Target			= 660002,
	EP_TargetCombative	= 660001,
	EP_MouseMobile		= 660003,
	EP_MouseImmobile	= 660006,
	EP_MissionBegin		= 660004,
	EP_MissionEnd		= 660005,
	EP_LevelUp			= 660007,
	EP_PreCast			= 660008,
};

enum
{
	TM_UNKNOW	= -1,
	TM_IGNORE	= -2,
	TM_INFINITE	= -3,
};

//֧�ֵ���Ч����
enum EPO_Type
{
	EPO_Particle,		//����
	EPO_Shape,			//ģ��
	EPO_Sound,			//����
	EPO_Light,			//��Դ
	EPO_Decal,			//����
	EPO_Screen,			//��Ļ��Ч
	EPO_Camera,			//��ͷ��Ч
	EPO_Program,		//������Ч
	EPO_Spline,			//������Ч
	EPO_Electricity,	//������Ч
	EPO_FluidLight,		//������Ч
	EPO_Unknow,
};

struct _objectInfo
{
	S8					parentId;				//������ID,��������ΪShape����,������link��������������Ч��������
	S8					mountSlotType;			//shapeBase������	//-1Ϊ�������� ����Ϊװ�������
	StringTableEntry	parentPoint;			//���ڵ�
	StringTableEntry	linkPoint;				//�������ӵ�
};

//��Ч����洢����
struct EffectObjectData
{
	enum StateMask
	{
		StateMask_EnableA	= BIT(0),			//״̬1����
		StateMask_EnableB	= BIT(1),			//״̬2����
		StateMask_EnableC	= BIT(2),			//״̬3����
		StateMask_EnableD	= BIT(3),			//״̬4����
		StateMask_VisibleA	= BIT(4),			//״̬1��Ⱦ
		StateMask_VisibleB	= BIT(5),			//״̬2��Ⱦ
		StateMask_VisibleC	= BIT(6),			//״̬3��Ⱦ
		StateMask_VisibleD	= BIT(7),			//״̬4��Ⱦ
		StateMask_TraceA	= BIT(8),			//״̬1׷��
		StateMask_TraceB	= BIT(9),			//״̬2׷��
		StateMask_TraceC	= BIT(10),			//״̬3׷��
		StateMask_TraceD	= BIT(11),			//״̬4׷��
		StateMask_FadeInA	= BIT(12),			//״̬1����
		StateMask_FadeInB	= BIT(13),			//״̬2����
		StateMask_FadeInC	= BIT(14),			//״̬3����
		StateMask_FadeInD	= BIT(15),			//״̬4����
		StateMask_FadeOutA	= BIT(16),			//״̬1����
		StateMask_FadeOutB	= BIT(17),			//״̬2����
		StateMask_FadeOutC	= BIT(18),			//״̬3����
		StateMask_FadeOutD	= BIT(19),			//״̬4����
	};

	enum Flags
	{
		Flags_Bind			= BIT(0),			//�Ƿ���ڶ�����
		Flags_RandomOffset	= BIT(1),			//��������ƫ��һ�������
		Flags_RandomDelay	= BIT(2),			//���������ʱ
		Flags_Floor			= BIT(3),			//�ر���ײ
		Flags_FollowDataFade= BIT(4),			//ǿ����ѭdata�ĵ��뵭������
		Flags_InvisibleCycle= BIT(5),			//��ѭ��������
	};

	EP_ITEM				PacketId;				//��ID
	EPO_Type			type;					//��Ч��������

	S8					objectId;				//����Id
	U32					stateMask;				//״̬��־
	U32					flags;					//��־
	Point3F				scale;					//����ֵ
	Point3F				offset;					//����ƫ����
	F32					velocity;				//׷���ƶ����ٶ�
	S32					traceTime;				//���׷��ʱ�䣬����ʱ�����ʧ
	_objectInfo			link;					//���Ӷ�����Ϣ
	StringTableEntry	DataBlockName;			//datablock����
	U32					DataBlockId;			//datablockId
	S32					duration;				//��Ч����ʱ��, 0Ϊ�������п���
	S32					delay;					//��Ч��ʱʱ��, 0����ʱ

	inline bool			isFlags					(U32 _flag) const { return (bool)(flags & _flag);}
	inline bool			isStateMask				(U32 _mask, U32 _status) const { return (bool)(stateMask & (_mask << _status));}

	EffectObjectData() { dMemset(this, 0, sizeof(EffectObjectData));}
};

//��Ч���洢����
struct EffectPacketItemData
{
	EP_ITEM				PacketId;				//��ID
	S32					duration;				//������ʱ��, 0Ϊ�������п���
	Vector<EffectObjectData *> objects;			//���տ�ʼʱ�������˵���Ч�����б�

	EffectPacketItemData();
	~EffectPacketItemData();
};

//��Ч�ֿ�
class EffectDataRepository
{
	typedef stdext::hash_map<EP_ITEM,EffectPacketItemData *> EffectPacketItemData_Hash;
	EffectPacketItemData_Hash mData;

public:
	EffectDataRepository();
	~EffectDataRepository();

	bool Write();
	bool Read();
	void Clear();
	const EffectPacketItemData *getEffectPacketItemData(EP_ITEM);
};

//==========================================================================================================================================
//==========================================================================================================================================
//==========================================================================================================================================

class EPItem : public SceneObject
{
	typedef SceneObject Parent;

	U32					itemId;					//��Ч���������Id
	EP_ITEM				PacketId;				//��Ч�����
	U32					sourceObjectId;			//ԭ����getId()�����ڰ���Ч
	U32					targetObjectId;			//Ŀ�����getId()��������Ч׷��
	U8					curStatus;				//�������ĵ�ǰ״̬�׶�

public:
	enum {
		InitialUpdateMask =     Parent::NextFreeMask,
		NextFreeMask =          Parent::NextFreeMask << 1
	};

	EPItem() { itemId = 0; PacketId = 0; sourceObjectId = 0; targetObjectId = 0; curStatus = 0; mNetFlags.set(Ghostable);}
	EPItem(U32 dataId, U32 src, U32 tgt, U32 status):itemId(0), PacketId(dataId), sourceObjectId(src), targetObjectId(tgt), curStatus(status) {mNetFlags.set(Ghostable);}
	~EPItem();
	DECLARE_CONOBJECT(EPItem);

	void inline			setItemId				(U32 id) { itemId = id;}
	U64					packUpdate				(NetConnection *conn, U64 mask, BitStream *stream);
	void				unpackUpdate			(NetConnection *conn,           BitStream *stream);
	bool				onAdd();
	void				onRemove();
};

class EffectPacketItem;
//�������Ч�������Լ����������ֵ�ID�����ͣ���ʼ����ʱ��Ⱦ��������Ϣ
class EffectObject
{
	friend class EffectPacketItem;
	friend class EffectPacketContainer;
	U32					Id;						//������Ч�����Լ������ID
	U8					curStatus;				//����ǰ״̬�׶�
	F32					lastTime;				//��Ч����ʱ��
	F32					statusStartTime;		//��״̬��ʼʱ��
	F32					delayTime;				//��ʱ��ʼʱ��
	SimTime				lastRenderTime;			//�ϴ���Ⱦʱ��

	Point3F				currentPos;				//��ǰ����
	Point3F				targetPos;				//׷������(�ɱ仯)
	F32					currentVelocity;		//��ǰ�ٶ�
	F32					currentTraceTime;		//��ǰ׷��ʱ��

	bool				inited;					//�Ƿ��ʼ����
	SceneObject *		m_pMainObj;				//������(��������)

public:
	const EffectObjectData *m_pData;			//��Чdata
	const EffectPacketItem *m_pItem;			//Item����

public:
	Vector<EffectObject *> childs;				//�����Ӷ���
	EffectObject *		parent;					//�����Ӷ���
protected:
	bool				registerEffectObject	();
	bool				unregisterEffectObject	();

	bool				createParticle			();
	bool				deleteParticle			();

	bool				createShape				();
	bool				deleteShape				();

	bool				createSound				();
	bool				deleteSound				();

	bool				createLight				();
	bool				deleteLight				();

	bool				createDecal				();
	bool				deleteDecal				();

	bool				createScreen			();
	bool				deleteScreen			();

	bool				createCamera			();
	bool				deleteCamera			();

	bool				createSpline			();
	bool				deleteSpline			();

	bool				createElectricity		();
	bool				deleteElectricity		();

	bool				createFluidLight		();
	bool				deleteFluidLight		();
public:
	EffectObject();
	~EffectObject();
	bool				Init					(const EffectPacketItem *,const EffectObjectData *);	//��data��ʼ������
	void				InitTransform			(const MatrixF &SourceMat, const MatrixF &Objmat);
	void				prepRenderImage			(const U32 stateKey);
	bool				advanceTime				(S32 dt);		//ʱ���ƽ�
	bool				updatePos				(S32 dt);		//��������
	void				setStatus				(U8 status) ;
	const SceneObject*	getObj					() { return m_pMainObj;}

	MatrixF				getLinkMaterix			();				//����������ӵ����
	const _objectInfo*	getLinkInfo				(){return &m_pData->link;}		//���������Ϣ
	Point3F&			getObjPosition			(GameBase *pObj);

	F32					setFade					(bool val, bool fading = false, F32 fadeTime = 500.0f, F32 fadeDelay = 0.0f);
	void				setMatrix				(MatrixF &);	//���ö������
	void				setAnimationPos			(F32 pos);		//���ö���֡
	void				updateVisibility		();				//����͸����
};

//��Ч�����󣬰������Լ���ID����ʼʱ��Ȱ��������Ϣ
//��������һ��������������������ҵ�λ��ȷ��pack,unpack
class EffectPacketItem : public RefBase
{
	friend class EffectPacketContainer;
	friend class EffectObject;
	friend class GameBase;

public:
	// ��Ч����״̬
	enum EffectPacketItemStatus
	{
		Status_A,
		Status_B,
		Status_C,
		Status_D,
		MaxStatus,
	};

protected:
	U32					itemId;					//��Ч���������Id
	EP_ITEM				PacketId;				//��Ч�����
	bool				notify;					//�Ƿ�ɾ��gameObject�Ĺ���
	bool				deleteMask;				//ɾ����־
	bool				isBind;					//��Դ
	S32					startTime;				//����ʼʱ��
	S32					duration;				//������ʱ��, 0Ϊ�������п���
	S32					lastTime;				//���ƽ�ʱ��
	MatrixF				sourceMat;				//��������ϵ
	U8					curStatus;				//�������ĵ�ǰ״̬�׶�
	SimObjectPtr<GameBase> source;				//ԭ����
	SimObjectPtr<GameBase> target;				//Ŀ�����
	SimObjectPtr<EPItem>   netEP;				//�������
	F32					visibility;				//��͸��
	Point3F				targetPos;				//Ŀ������
	Vector<EffectObject *> objects;				//��Ч����
	bool				addToContainer;			//�Ѿ����뵽��Ч������

public:
	EffectPacketItem();
	~EffectPacketItem();

	inline U32			getId					() { return itemId;}
	inline U32			getPacketId				() { return PacketId;}
protected:
	bool				addEffectObject			(EffectObject *);
	void				prepRenderImage			(const U32 stateKey);
	bool				advanceTime				(S32 dt);		//ʱ���ƽ�
	void				setNotify				(bool flag) {notify = flag;}
	void				setStatus				(U8 status) ;
	void				setDelete				(bool val = true) {deleteMask = val;}
	void				resetTime				(S32 start, S32 dur) { startTime = lastTime = start; duration = dur;}
	F32					setFade					(bool val, bool fading = false, F32 fadeTime = 500.0f, F32 fadeDelay = 0.0f);
};


//��Ч�����࣬���𴴽������������Ч������
class EffectPacketContainer
{
	static U32			iDseed;

	typedef stdext::hash_map<U32,EffectPacketItem *> EffectPacketItem_Hash;
	EffectPacketItem_Hash items;			//��Ч���б�

public:

	static U32			getId					();
	U32					addEffectPacket			(EP_ITEM PacketId,GameBase *,const MatrixF &,GameBase *,Point3F &,F32 duration = TM_UNKNOW);
	U32					addEffectPacket			(EffectPacketItem *,EP_ITEM PacketId,GameBase *,const MatrixF &,GameBase *,Point3F &,F32 duration = TM_UNKNOW);
	bool				removeEffectPackets		(EP_ITEM PacketId);					//ɾ������ͬ����Ч��
	bool				removeEffectPacket		(U32 itemId,bool notify=true, U32 time = 0);		//��IDɾ����Ч��
	bool				removeFromMap			(U32 itemId);						//��IDɾ�����е�ָ��
	void				removeFromScene			();									//�����Чϵͳ
	bool				setEffectPacketStatus	(U32 itemId,U8 status);
	bool				setEffectPacketsStatus	(EP_ITEM PacketId,U8 status);
	void				prepRenderImage			(const U32 stateKey);
	void				advanceTime				(S32 dt);					//ʱ���ƽ�
	void				refreshDecal			(U32 id, F32* rad, Point3F* pos, ColorI* col);	//���ð뾶
	void				hideEffect				(U32 id, bool hide);	//��������
};

extern EffectPacketContainer g_EffectPacketContainer;
extern EffectDataRepository  g_EffectDataRepository;

#endif


