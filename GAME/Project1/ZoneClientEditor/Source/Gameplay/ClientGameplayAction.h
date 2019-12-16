//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#pragma once
#include "Gameplay/ClientGameplayParam.h"
#include "platform/types.h"

// ----------------------------------------------------------------------------
// 游戏逻辑操作行为类
// 说明:
//      处理需要向服务端发送网络消息的某个操作行为。这个操作行为不同于GameNetEvent，在
//一个时间点上无法收集所需的数据，也就是说在发送数据前，需要通过多次交互操作后逐个获得
//部分数据，一旦完成所有setParam过程，数据全部收集完成时，才通过sendGameplayEvent给服务端.
class ClientGameplayAction
{
public:
	ClientGameplayAction							(U32 paramCount, U32 type);
	virtual ~ClientGameplayAction					();

	void setCursorName								(StringTableEntry cursorName) { mCursorName = cursorName; }
	inline U32 getActionType						() { return mActionType; }
	inline U32 getParamCount						() { return mParamCount; }
	inline U32 getCurrentParamCount					() { return mCurrentParamCount; }
	bool isParamValid								(ClientGameplayParam* param);
	bool setParam									(ClientGameplayParam* param);
	ClientGameplayParam* getParam					(U32 index);
	void activeCursor								(StringTableEntry cursorName);

	virtual void sendGameplayEvent					();
	virtual void onActivated						();
	virtual void onDone								();
	virtual	void onCancle							();
	virtual void onSetParam							();
protected:
	U32	mActionType;						// 操作行为类型
	U32	mParamCount;						// 数据参数总数
	U32	mCurrentParamCount;					// 当前数据参数计数
	ClientGameplayParam** mParam;			// 数据参数列表
	StringTableEntry mCursorName;			// 设置鼠标指针图标名
};