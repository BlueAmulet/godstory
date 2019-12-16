//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#include "Gameplay/GameObjects/GameObject.h"
#include "Gameplay/GameObjects/MountObject.h"
#include "Gameplay/GameObjects/NpcObject.h"
#include "Gameplay/GameEvents/ClientGameNetEvents.h"
#include "Gameplay/ClientGameplayState.h"

//-----------------------------------------------------------------------------
// 
// 客户端和服务端共有脚本
// 
//-----------------------------------------------------------------------------
ConsoleMethod( GameObject, getHP, S32, 2, 2, "obj.getHP()")
{
	return object->GetStats().HP;
}

ConsoleMethod( GameObject, getMP, S32, 2, 2, "obj.getMP()")
{
	return object->GetStats().MP;
}

ConsoleMethod( GameObject, getPP, S32, 2, 2, "obj.getPP()")
{
	return object->GetStats().PP;
}

ConsoleMethod( GameObject, getMaxHP, S32, 2, 2, "obj.getMaxHP()")
{
	return object->GetStats().MaxHP;
}

ConsoleMethod( GameObject, getMaxMP, S32, 2, 2, "obj.getMaxMP()")
{
	return object->GetStats().MaxMP;
}

ConsoleMethod( GameObject, getMaxPP, S32, 2, 2, "obj.getMaxPP()")
{
	return object->GetStats().MaxPP;
}

ConsoleMethod( GameObject, getStamina, S32, 2, 2, "obj.getStamina()")
{
	return object->GetStats().Stamina;
}

ConsoleMethod( GameObject, getMana, S32, 2, 2, "obj.getMana()")
{
	return object->GetStats().Mana;
}

ConsoleMethod( GameObject, getStrength, S32, 2, 2, "obj.getStrength()")
{
	return object->GetStats().Strength;
}

ConsoleMethod( GameObject, getIntellect, S32, 2, 2, "obj.getIntellect()")
{
	return object->GetStats().Intellect;
}

ConsoleMethod( GameObject, getAgility, S32, 2, 2, "obj.getAgility()")
{
	return object->GetStats().Agility;
}

ConsoleMethod( GameObject, getPneuma, S32, 2, 2, "obj.getPneuma()")
{
	return object->GetStats().Pneuma;
}

ConsoleMethod( GameObject, getInsight, S32, 2, 2, "obj.getInsight()")
{
	return object->GetStats().Insight;
}

ConsoleMethod( GameObject, getLuck, S32, 2, 2, "obj.getLuck()")
{
	return object->GetStats().Luck;
}

ConsoleMethod( GameObject, getCriticalRating, S32, 2, 2, "obj.getCriticalRating()")
{
	return object->GetStats().CriticalRating;
}

ConsoleMethod( GameObject, getCriticalTimes, S32, 2, 2, "obj.getCriticalTimes()")
{
	return (object->GetStats().CriticalTimes)/100;
}

ConsoleMethod( GameObject, getCritical_gPc, S32, 2, 2, "obj.getCritical_gPc()")
{
	return object->GetStats().Critical_gPc;
}


ConsoleMethod( GameObject, getFatalRating, S32, 2, 2, "obj.getFatalRating()")
{
	return object->GetStats().FatalRating;
}

ConsoleMethod( GameObject, getDodgeRating, S32, 2, 2, "obj.getDodgeRating()")
{
	return object->GetStats().DodgeRating;
}

ConsoleMethod( GameObject, getHitRating, S32, 2, 2, "obj.getHitRating()")
{
	return object->GetStats().HitRating;
}

ConsoleMethod( GameObject, getPhyDamage, S32, 2, 2, "obj.getPhyDamage()")
{
	return object->GetStats().PhyDamage;
}

ConsoleMethod( GameObject, getPhyDefence, S32, 2, 2, "obj.getPhyDefence()")
{
	return object->GetStats().PhyDefence;
}

ConsoleMethod( GameObject, getMuDamage, S32, 2, 2, "obj.getMuDamage()")
{
	return object->GetStats().MuDamage;
}

ConsoleMethod( GameObject, getMuDefence, S32, 2, 2, "obj.getMuDefence()")
{
	return object->GetStats().MuDefence;
}

ConsoleMethod( GameObject, getHuoDamage, S32, 2, 2, "obj.getHuoDamage()")
{
	return object->GetStats().HuoDamage;
}

ConsoleMethod( GameObject, getHuoDefence, S32, 2, 2, "obj.getHuoDefence()")
{
	return object->GetStats().HuoDefence;
}

ConsoleMethod( GameObject, getTuDamage, S32, 2, 2, "obj.getTuDamage()")
{
	return object->GetStats().TuDamage;
}

ConsoleMethod( GameObject, getTuDefence, S32, 2, 2, "obj.getTuDefence()")
{
	return object->GetStats().TuDefence;
}

ConsoleMethod( GameObject, getJinDamage, S32, 2, 2, "obj.getJinDamage()")
{
	return object->GetStats().JinDamage;
}

ConsoleMethod( GameObject, getJinDefence, S32, 2, 2, "obj.getJinDefence")
{
	return object->GetStats().JinDefence;
}

ConsoleMethod( GameObject, getShuiDamage, S32, 2, 2, "obj.getShuiDamage()")
{
	return object->GetStats().ShuiDamage;
}

ConsoleMethod( GameObject, getShuiDefence, S32, 2, 2, "obj.getShuiDefence()")
{
	return object->GetStats().ShuiDefence;
}

ConsoleMethod( GameObject, getPneumaDamage, S32, 2, 2, "obj.getPneumaDamage()")
{
	return object->GetStats().PneumaDamage;
}

ConsoleMethod( GameObject, getPneumaDefence, S32, 2, 2, "obj.getPneumaDefence()")
{
	return object->GetStats().PneumaDefence;
}

ConsoleMethod( GameObject, getHeal, S32, 2, 2, "obj.getHeal()")
{
   return object->GetStats().Heal;
}

ConsoleMethod( GameObject, getAntiCriticalRating, S32, 2, 2, "obj.getAntiCriticalRating()")
{
  return object->GetStats().AntiCriticalRating;
}

// ----------------------------------------------------------------------------
// 获取游戏对象性别
ConsoleMethod( GameObject, getSex, S32, 2, 2, "obj.getSex()")
{
	return object->getSex();
}

// ----------------------------------------------------------------------------
// 获取游戏对象种族
ConsoleMethod( GameObject, getRace, S32, 2, 2, "obj.getRace()")
{
	return object->getRace();
}

// ----------------------------------------------------------------------------
// 获取游戏对象门宗
ConsoleMethod( GameObject, getFamily, S32, 2, 2, "obj.getFamily()")
{
	return object->getFamily();
}

// ----------------------------------------------------------------------------
// 获取游戏对象等级
ConsoleMethod( GameObject, getLevel, S32, 2, 2, "obj.getLevel()")
{
	return object->getLevel();
}

// ----------------------------------------------------------------------------
// 获取游戏对象经验
ConsoleMethod( GameObject, getExp, S32, 2, 2, "obj.getExp()")
{
	return object->getExp();
}

// 获得游戏对象当前等级最高经验
ConsoleMethod( GameObject, getLevelExp, S32, 2, 2, "obj.getLevelExp()")
{
	return g_LevelExp[object->getLevel()];
}

// ----------------------------------------------------------------------------
// 获取游戏对象职业
ConsoleMethod( GameObject, getClasses, S32, 3, 3, "obj.getClasses(%No)")
{
	return object->getClasses((enClassesMaster)dAtoi(argv[2]));
}

// ----------------------------------------------------------------------------
// 获取游戏对象职业
ConsoleMethod( GameObject, getTarget, S32, 2, 2, "obj.getTarget()")
{
	return object->GetTarget() ? object->GetTarget()->getId() : 0;
}

// ----------------------------------------------------------------------------
// 获取对象BUFF个数
ConsoleMethod( GameObject, getBuffCount, S32, 4, 4, "obj.getBuffCount(S32 seriesId, S32 level)")
{
	return object->GetBuffTable().getBuffCount(dAtoi(argv[2]), dAtoi(argv[3]));
}

// ----------------------------------------------------------------------------
// 设置对象势力
ConsoleMethod( GameObject, setInfluence, void, 3, 3, "obj.setInfluence(%Influence)")
{
	object->setInfluence((enInfluence)dAtoi(argv[2]));
}

// ----------------------------------------------------------------------------
// 获取对象势力
ConsoleMethod( GameObject, getInfluence, S32, 2, 2, "obj.getInfluence()")
{
	return object->getInfluence();
}

// ----------------------------------------------------------------------------
// 获取对象头像
ConsoleMethod( GameObject, getObjectIcon, const char *, 2, 2, "obj.getObjectIcon()")
{
	return object->getObjectIcon();
}

// ----------------------------------------------------------------------------
// Esc取消目标选中

ConsoleMethod( GameObject, EscClearTarget, void, 2, 2, "obj.EscClearTarget()")
{
	object->SetTarget(NULL);
}
// ----------------------------------------------------------------------------
// 任务特效 未使用
ConsoleMethod( NpcObject, setMissionEffect, void, 3, 3, "obj.setMissionEffect()")
{
	object->setMissionEffect(atoi(argv[2]));
}
ConsoleMethod( NpcObject, clearMissionEffect, void, 2, 2, "obj.clearMissionEffect()")
{
	object->clearMissionEffect();
}

// ----------------------------------------------------------------------------
// 获取角色的基础技能等级
ConsoleMethod( GameObject, getSkillLevel, S32, 3, 3, "obj.getSkillLevel(%skillId / %skillSeriesId)")
{
	U32 id = atoi(argv[2]);
	Skill* pSkill = object->GetSkillTable().GetSkill((SkillLevelDecimalBits*10 < id) ? Macro_GetSkillSeriesId(id) : id);
	return pSkill ? pSkill->GetBaseLevel() : 0;
}

//-----------------------------------------------------------------------------
// 
// 客户端脚本
// 
//-----------------------------------------------------------------------------
#ifdef NTJ_CLIENT

// 游戏对象升级
ConsoleFunction( PlayerLevelUp, void, 1, 1, "PlayerLevelUp()")
{
	GameConnection* conn = GameConnection::getConnectionToServer();
	GameObject* pObject = g_ClientGameplayState->GetControlPlayer();
	if (!pObject)
	{
		return;
	}
	if(conn)
	{
		if (pObject->getExp() >= (g_LevelExp[pObject->getLevel()]))
		{
			ClientGameNetEvent* pEvent = new ClientGameNetEvent(INFO_LEVEL_UP);
			conn->postNetEvent(pEvent);
		}
	}
}
// 游戏对象受创
ConsoleMethod( GameObject, onWound, void, 2, 2, "obj.onWound()")
{
	object->onWound();
}

// 游戏对象受创
ConsoleFunction(onWoundTest, void, 2, 2, "onWoundTest(%id)")
{
	GameObject* pObj = dynamic_cast<GameObject*>(Sim::findObject(dAtoi(argv[1])));
	if(pObj)
		pObj->onWound();
}

// 说话
ConsoleMethod( GameObject, Speak, void, 3, 3, "obj.Speak( %msg );")
{
	dStrcpy( object->mChatInfo, sizeof( object->mChatInfo ), argv[2] );
	object->mChatLastUpdateTime = Platform::getVirtualMilliseconds();
	object->setDirty( GameObject::ChatInfoMask );
}

// 寻径
ConsoleMethod( GameObject, SetPath, void, 3, 5, "obj.setPath(%position)")
{
	static Point3F pos;
	if(argc == 3)
		dSscanf(argv[2], "%g %g %g", &pos.x, &pos.y, &pos.z);
	else if(argc == 5)
		pos.set(dAtof(argv[2]), dAtof(argv[3]), dAtof(argv[4]));
	else
		Con::printf("Point3F must be set as { x, y, z } or \"x y z\"");

	object->SetTarget(pos);
}

//获取技能系
ConsoleFunction(GetClasses, S32, 2, 2, "GetClasses(%SeriesID)")
{
	GameObject* obj= g_ClientGameplayState->GetControlPlayer();
	return obj->getClasses(atoi(argv[1]));
}

//设置技能系
ConsoleFunction(SetClasses,void, 3, 3,"SetClasses(%Val,%class)")
{
	GameConnection *conn = dynamic_cast<GameConnection*>(GameConnection::getConnectionToServer());
	if (conn)
	{
		ClientGameNetEvent* mEvent = new ClientGameNetEvent(INFO_SKILL_OPERATE);
		mEvent->SetInt32ArgValues(3,0,dAtoi(argv[1]),dAtoi(argv[2]));
		conn->postNetEvent(mEvent);
	}
}

#endif

//-----------------------------------------------------------------------------
// 
// 服务端脚本
// 
//-----------------------------------------------------------------------------
#ifdef NTJ_SERVER

// 增加游戏对象经验
ConsoleMethod( GameObject, addExp, void, 3, 3, "obj.addExp(%val)")
{
	object->addExp(dAtoi(argv[2]));
}

// 游戏对象获得技能
ConsoleMethod( GameObject, AddSkill, void, 3, 4, "obj.addSkill(%skillid)")
{
	object->AddSkill(dAtoi(argv[2]), argc == 4 ? dAtob(argv[3]) : true);
}

// 游戏对象删除技能
ConsoleMethod( GameObject, RemoveSkill, void, 3, 3, "obj.addSkill(%skillid)")
{
	object->RemoveSkill(dAtoi(argv[2]));
}

// 以下两个函数修改请保持一致
// 游戏对象获得状态
ConsoleMethod( GameObject, AddBuff, void, 3, 4, "obj.addBuff(%buffid, %simId)")
{
	object->AddBuff(Buff::Origin_Buff, dAtoi(argv[2]), argc == 4 ? dynamic_cast<GameObject*>(Sim::findObject(dAtoi(argv[3]))) : NULL);
}

// 游戏对象删除状态
ConsoleMethod( GameObject, RemoveBuff, void, 3, 3, "obj.RemoveBuff(%buffid)")
{
	object->AddBuff(Buff::Origin_Buff, dAtoi(argv[2]), NULL, -1);
}

// 设置性别
ConsoleMethod( GameObject, setSex, void, 3, 3, "obj.setSex(%sex)")
{
	object->setSex((enSex)dAtoi(argv[2]));
}

// 设置种族
ConsoleMethod( GameObject, setRace, void, 3, 3, "obj.setRace(%val)")
{
	object->setRace((enRace)dAtoi(argv[2]));
}

// 设置门宗
ConsoleMethod( GameObject, setFamily, void, 3, 3, "obj.setFamily(%val)")
{
	object->setFamily((enFamily)dAtoi(argv[2]));
}

// 设置坐标 朝向
ConsoleMethod( GameObject, setPosition, void, 3, 6, "obj.setPosition(%pos, %rot)")
{
	static Point3F pos, rot(0,0,0);
	if(argc == 3)
	{
		dSscanf(argv[2], "%g %g %g", &pos.x, &pos.y, &pos.z);
		rot.z = 0;
	}
	else if(argc == 4)
	{
		dSscanf(argv[2], "%g %g %g", &pos.x, &pos.y, &pos.z);
		rot.z = dAtof(argv[3]);
	}
	else if(argc == 5)
	{
		pos.set(dAtof(argv[2]), dAtof(argv[3]), dAtof(argv[4]));
		rot.z = 0;
	}
	else if(argc == 6)
	{
		pos.set(dAtof(argv[2]), dAtof(argv[3]), dAtof(argv[4]));
		rot.z = dAtof(argv[5]);
	}
	else
		Con::printf("Point3F must be set as { x, y, z } or \"x y z\", %rot is a float");

	object->setPosition(pos, rot);
}

// 设置mount
ConsoleMethod( GameObject, mount, S32, 2, 2, "")
{
	MountObject* pMount = new MountObject;
	if(!pMount)
		return 0;

	//pMount->setDataID(dAtoi(argv[1]));
	pMount->setPosition(object->getPosition(), object->getRotation());
	if(!pMount->registerObject())
	{
		// 注册失败，删除返回
		delete pMount;
		return 0;
	}

	pMount->mountObject(object, "link14Point", "link08point");
	object->setControlObject(pMount);
	return pMount->getId();
}

// 设置mount
ConsoleMethod( GameObject, unmount, void, 2, 2, "")
{
	MountObject* pMount = (MountObject*)object->getObjectMount();
	object->unmount();
	//if(!pMount)
	//	return;

	//pMount->unregisterObject();
}

// 设置mount
ConsoleMethod( GameObject, mountTest, void, 2, 2, "")
{
	if (object->isMounted())
		Con::executef(object, "unmount");
	else
		Con::executef(object, "mount");
}

// 设置飞行路径
ConsoleMethod( GameObject, setFlyPath, void, 4, 4, "")
{
	object->setFlyPath(dAtoi(argv[2]), dAtoi(argv[3]));
}

// 说话
ConsoleMethod( GameObject, say, void, 3, 3, "")
{
	object->say(argv[2]);
}

#endif
