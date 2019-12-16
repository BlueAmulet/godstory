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

// 固定的特效包ID
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

//支持的特效类型
enum EPO_Type
{
	EPO_Particle,		//粒子
	EPO_Shape,			//模型
	EPO_Sound,			//声音
	EPO_Light,			//光源
	EPO_Decal,			//贴花
	EPO_Screen,			//屏幕特效
	EPO_Camera,			//镜头特效
	EPO_Program,		//程序特效
	EPO_Spline,			//曲线特效
	EPO_Electricity,	//闪电特效
	EPO_FluidLight,		//流光特效
	EPO_Unknow,
};

struct _objectInfo
{
	S8					parentId;				//容器的ID,容器必须为Shape类型,必须有link点用来给其他特效对象链接
	S8					mountSlotType;			//shapeBase槽类型	//-1为人物身体 其他为装配槽物体
	StringTableEntry	parentPoint;			//父节点
	StringTableEntry	linkPoint;				//自身链接点
};

//特效对象存储数据
struct EffectObjectData
{
	enum StateMask
	{
		StateMask_EnableA	= BIT(0),			//状态1启用
		StateMask_EnableB	= BIT(1),			//状态2启用
		StateMask_EnableC	= BIT(2),			//状态3启用
		StateMask_EnableD	= BIT(3),			//状态4启用
		StateMask_VisibleA	= BIT(4),			//状态1渲染
		StateMask_VisibleB	= BIT(5),			//状态2渲染
		StateMask_VisibleC	= BIT(6),			//状态3渲染
		StateMask_VisibleD	= BIT(7),			//状态4渲染
		StateMask_TraceA	= BIT(8),			//状态1追踪
		StateMask_TraceB	= BIT(9),			//状态2追踪
		StateMask_TraceC	= BIT(10),			//状态3追踪
		StateMask_TraceD	= BIT(11),			//状态4追踪
		StateMask_FadeInA	= BIT(12),			//状态1淡入
		StateMask_FadeInB	= BIT(13),			//状态2淡入
		StateMask_FadeInC	= BIT(14),			//状态3淡入
		StateMask_FadeInD	= BIT(15),			//状态4淡入
		StateMask_FadeOutA	= BIT(16),			//状态1淡出
		StateMask_FadeOutB	= BIT(17),			//状态2淡出
		StateMask_FadeOutC	= BIT(18),			//状态3淡出
		StateMask_FadeOutD	= BIT(19),			//状态4淡出
	};

	enum Flags
	{
		Flags_Bind			= BIT(0),			//是否绑定在对象上
		Flags_RandomOffset	= BIT(1),			//给予坐标偏移一定随机性
		Flags_RandomDelay	= BIT(2),			//随机产生延时
		Flags_Floor			= BIT(3),			//地表碰撞
		Flags_FollowDataFade= BIT(4),			//强制遵循data的淡入淡出规则
		Flags_InvisibleCycle= BIT(5),			//单循环后隐藏
	};

	EP_ITEM				PacketId;				//包ID
	EPO_Type			type;					//特效具体类型

	S8					objectId;				//对象Id
	U32					stateMask;				//状态标志
	U32					flags;					//标志
	Point3F				scale;					//缩放值
	Point3F				offset;					//坐标偏移量
	F32					velocity;				//追踪移动的速度
	S32					traceTime;				//最大追踪时间，超过时间就消失
	_objectInfo			link;					//链接对象信息
	StringTableEntry	DataBlockName;			//datablock名称
	U32					DataBlockId;			//datablockId
	S32					duration;				//特效持续时间, 0为对象自行控制
	S32					delay;					//特效延时时间, 0不延时

	inline bool			isFlags					(U32 _flag) const { return (bool)(flags & _flag);}
	inline bool			isStateMask				(U32 _mask, U32 _status) const { return (bool)(stateMask & (_mask << _status));}

	EffectObjectData() { dMemset(this, 0, sizeof(EffectObjectData));}
};

//特效包存储数据
struct EffectPacketItemData
{
	EP_ITEM				PacketId;				//包ID
	S32					duration;				//包持续时间, 0为对象自行控制
	Vector<EffectObjectData *> objects;			//按照开始时间排序了的特效对象列表

	EffectPacketItemData();
	~EffectPacketItemData();
};

//特效仓库
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

	U32					itemId;					//特效包对象管理Id
	EP_ITEM				PacketId;				//特效包编号
	U32					sourceObjectId;			//原对象getId()，用于绑定特效
	U32					targetObjectId;			//目标对象getId()，用于特效追踪
	U8					curStatus;				//整个包的当前状态阶段

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
//具体的特效对象，有自己各个管理部分的ID，类型，开始结束时间等具体控制信息
class EffectObject
{
	friend class EffectPacketItem;
	friend class EffectPacketContainer;
	U32					Id;						//各个特效对象自己管理的ID
	U8					curStatus;				//对象当前状态阶段
	F32					lastTime;				//特效进行时间
	F32					statusStartTime;		//本状态开始时间
	F32					delayTime;				//延时开始时间
	SimTime				lastRenderTime;			//上次渲染时间

	Point3F				currentPos;				//当前坐标
	Point3F				targetPos;				//追踪坐标(可变化)
	F32					currentVelocity;		//当前速度
	F32					currentTraceTime;		//当前追踪时间

	bool				inited;					//是否初始化了
	SceneObject *		m_pMainObj;				//主对象(场景对象)

public:
	const EffectObjectData *m_pData;			//特效data
	const EffectPacketItem *m_pItem;			//Item对象

public:
	Vector<EffectObject *> childs;				//子链接对象
	EffectObject *		parent;					//父链接对象
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
	bool				Init					(const EffectPacketItem *,const EffectObjectData *);	//从data初始化对象
	void				InitTransform			(const MatrixF &SourceMat, const MatrixF &Objmat);
	void				prepRenderImage			(const U32 stateKey);
	bool				advanceTime				(S32 dt);		//时间推进
	bool				updatePos				(S32 dt);		//更新坐标
	void				setStatus				(U8 status) ;
	const SceneObject*	getObj					() { return m_pMainObj;}

	MatrixF				getLinkMaterix			();				//获得自身链接点矩阵
	const _objectInfo*	getLinkInfo				(){return &m_pData->link;}		//获得链接信息
	Point3F&			getObjPosition			(GameBase *pObj);

	F32					setFade					(bool val, bool fading = false, F32 fadeTime = 500.0f, F32 fadeDelay = 0.0f);
	void				setMatrix				(MatrixF &);	//设置对象矩阵
	void				setAnimationPos			(F32 pos);		//设置动画帧
	void				updateVisibility		();				//更新透明度
};

//特效包对象，包含了自己的ID，开始时间等包整体的信息
//并且他是一个场景对象，用来依据玩家的位置确定pack,unpack
class EffectPacketItem : public RefBase
{
	friend class EffectPacketContainer;
	friend class EffectObject;
	friend class GameBase;

public:
	// 特效包的状态
	enum EffectPacketItemStatus
	{
		Status_A,
		Status_B,
		Status_C,
		Status_D,
		MaxStatus,
	};

protected:
	U32					itemId;					//特效包对象管理Id
	EP_ITEM				PacketId;				//特效包编号
	bool				notify;					//是否删除gameObject的管理
	bool				deleteMask;				//删除标志
	bool				isBind;					//绑定源
	S32					startTime;				//包开始时间
	S32					duration;				//包持续时间, 0为对象自行控制
	S32					lastTime;				//包推进时间
	MatrixF				sourceMat;				//世界坐标系
	U8					curStatus;				//整个包的当前状态阶段
	SimObjectPtr<GameBase> source;				//原对象
	SimObjectPtr<GameBase> target;				//目标对象
	SimObjectPtr<EPItem>   netEP;				//网络对象
	F32					visibility;				//半透明
	Point3F				targetPos;				//目标坐标
	Vector<EffectObject *> objects;				//特效对象
	bool				addToContainer;			//已经加入到特效管理中

public:
	EffectPacketItem();
	~EffectPacketItem();

	inline U32			getId					() { return itemId;}
	inline U32			getPacketId				() { return PacketId;}
protected:
	bool				addEffectObject			(EffectObject *);
	void				prepRenderImage			(const U32 stateKey);
	bool				advanceTime				(S32 dt);		//时间推进
	void				setNotify				(bool flag) {notify = flag;}
	void				setStatus				(U8 status) ;
	void				setDelete				(bool val = true) {deleteMask = val;}
	void				resetTime				(S32 start, S32 dur) { startTime = lastTime = start; duration = dur;}
	F32					setFade					(bool val, bool fading = false, F32 fadeTime = 500.0f, F32 fadeDelay = 0.0f);
};


//特效管理类，负责创建各个具体的特效包对象
class EffectPacketContainer
{
	static U32			iDseed;

	typedef stdext::hash_map<U32,EffectPacketItem *> EffectPacketItem_Hash;
	EffectPacketItem_Hash items;			//特效包列表

public:

	static U32			getId					();
	U32					addEffectPacket			(EP_ITEM PacketId,GameBase *,const MatrixF &,GameBase *,Point3F &,F32 duration = TM_UNKNOW);
	U32					addEffectPacket			(EffectPacketItem *,EP_ITEM PacketId,GameBase *,const MatrixF &,GameBase *,Point3F &,F32 duration = TM_UNKNOW);
	bool				removeEffectPackets		(EP_ITEM PacketId);					//删除所有同类特效包
	bool				removeEffectPacket		(U32 itemId,bool notify=true, U32 time = 0);		//用ID删除特效包
	bool				removeFromMap			(U32 itemId);						//用ID删除表中的指针
	void				removeFromScene			();									//清空特效系统
	bool				setEffectPacketStatus	(U32 itemId,U8 status);
	bool				setEffectPacketsStatus	(EP_ITEM PacketId,U8 status);
	void				prepRenderImage			(const U32 stateKey);
	void				advanceTime				(S32 dt);					//时间推进
	void				refreshDecal			(U32 id, F32* rad, Point3F* pos, ColorI* col);	//设置半径
	void				hideEffect				(U32 id, bool hide);	//设置隐藏
};

extern EffectPacketContainer g_EffectPacketContainer;
extern EffectDataRepository  g_EffectDataRepository;

#endif


