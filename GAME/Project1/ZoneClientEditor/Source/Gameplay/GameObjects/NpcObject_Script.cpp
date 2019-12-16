//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#include "Gameplay/GameObjects/NpcObject.h"
#include "Gameplay/GameEvents/ClientGameNetEvents.h"
#include "Gameplay/ClientGameplayState.h"

//-----------------------------------------------------------------------------
// 
// �ͻ��˺ͷ���˹��нű�
// 
//-----------------------------------------------------------------------------

// ��ȡ����ǿ��
ConsoleMethod( NpcObject, getIntensity, S32, 2, 2, "obj.getIntensity()")
{
	return object->getIntensity();
}

// ��ȡ���ﾭ������
ConsoleMethod( NpcObject, getExpScale, F32, 2, 2, "obj.getExpScale()")
{
	return object->getExpScale();
}

// ��ȡ�����ռ���
ConsoleMethod( NpcObject, getExclusivePlayerId, S32, 2, 2, "obj.getExclusivePlayerId()")
{
	return object->getExclusivePlayerId();
}


//-----------------------------------------------------------------------------
// 
// �ͻ��˽ű�
// 
//-----------------------------------------------------------------------------
#ifdef NTJ_CLIENT

#endif

//-----------------------------------------------------------------------------
// 
// ����˽ű�
// 
//-----------------------------------------------------------------------------
#ifdef NTJ_SERVER

// ���ù���ȼ�
ConsoleMethod( NpcObject, setLevel, void, 3, 3, "obj.setLevel(%val)")
{
	object->setLevel(dAtoi(argv[2]));
}

// ���ù����ռ���
ConsoleMethod( NpcObject, setExclusivePlayerId, void, 3, 3, "obj.setExclusivePlayerId(%val)")
{
	object->setExclusivePlayerId(dAtoi(argv[2]));
}

#endif
