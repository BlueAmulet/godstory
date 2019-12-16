//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#include "Gameplay/GameObjects/NpcObject.h"
#include "Gameplay/GameEvents/ClientGameNetEvents.h"
#include "Gameplay/ClientGameplayState.h"

//-----------------------------------------------------------------------------
// 
// 客户端和服务端共有脚本
// 
//-----------------------------------------------------------------------------

// 获取怪物强度
ConsoleMethod( NpcObject, getIntensity, S32, 2, 2, "obj.getIntensity()")
{
	return object->getIntensity();
}

// 获取怪物经验缩放
ConsoleMethod( NpcObject, getExpScale, F32, 2, 2, "obj.getExpScale()")
{
	return object->getExpScale();
}

// 获取怪物独占玩家
ConsoleMethod( NpcObject, getExclusivePlayerId, S32, 2, 2, "obj.getExclusivePlayerId()")
{
	return object->getExclusivePlayerId();
}


//-----------------------------------------------------------------------------
// 
// 客户端脚本
// 
//-----------------------------------------------------------------------------
#ifdef NTJ_CLIENT

#endif

//-----------------------------------------------------------------------------
// 
// 服务端脚本
// 
//-----------------------------------------------------------------------------
#ifdef NTJ_SERVER

// 设置怪物等级
ConsoleMethod( NpcObject, setLevel, void, 3, 3, "obj.setLevel(%val)")
{
	object->setLevel(dAtoi(argv[2]));
}

// 设置怪物独占玩家
ConsoleMethod( NpcObject, setExclusivePlayerId, void, 3, 3, "obj.setExclusivePlayerId(%val)")
{
	object->setExclusivePlayerId(dAtoi(argv[2]));
}

#endif
