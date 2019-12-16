#pragma once
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/GameEvents/ServerGameNetEvents.h"
#include "T3D/gameConnection.h"
#include "Gameplay/GameObjects/Skill.h"
#include "Gameplay/GameObjects/MPManager.h"
#

// ----------------------------------------------------------------------------
// 获取角色ID
S32	Player::getPlayerID()
{ 
	return mPlayerId;
}

S32 Player::getAccountID()
{
	return mAccountId;
}

// ----------------------------------------------------------------------------
// 获取角色名称
StringTableEntry Player::getPlayerName()
{ 
	return getObjectName();
}

// ----------------------------------------------------------------------------
// 获取交互状态
U32	Player::getInteractionState()
{ 
	return mInteractionState;
}

// ----------------------------------------------------------------------------
// 获取交互对象
SceneObject* Player::getInteraction()
{ 
	return mInteractionObject;
}

// ----------------------------------------------------------------------------
// 获取组队信息
stTeamInfo& Player::getTeamInfo()
{
	return mTeamInfo;
}

// ----------------------------------------------------------------------------
// 获取数值增强
StatsPlusTable& Player::getPlusTable()
{
	return mPlusTable;
}

// ----------------------------------------------------------------------------
// 获取角色活力值
U32 Player::getVigor()
{
	return mVigor;
}
//-----------------------------------------------------------------------------
// 获取角色最大色活力值
U32 Player::getMaxVigor()
{
	return mMaxVigor;
}

// ----------------------------------------------------------------------------
// 获取下线地图编号
S32	Player::getZoneID()
{
	return mZoneID;
}

// ----------------------------------------------------------------------------
// 设置角色下线地图编号
void Player::setZoneID(S32 zoneid)
{
	mZoneID = zoneid;
}

// ----------------------------------------------------------------------------
// 获得身体
U32 Player::getBody()
{
	return mBody;
}

// ----------------------------------------------------------------------------
// 获得脸
U32 Player::getFace()
{
	return mFace;
}

// ----------------------------------------------------------------------------
// 获得头发
U32 Player::getHair()
{
	return mHair;
}

//获得头发颜色
U32 Player::getHairCol()
{
	return mHairCol;
}
// ----------------------------------------------------------------------------
// 获取PK状态
bool Player::getPKState()
{ 
	return mSwitchState & SWITCHSTATE_PK;
}

// ----------------------------------------------------------------------------
// 获取开关状态
bool Player::getSwitchState(U32 flag)
{
	return mSwitchState & flag;
}
// ----------------------------------------------------------------------------
// 获取正在修练的专精技能ID
U32 Player::getLivingSkillPro()
{
	return mLivingSkillStudyID;
}
// ----------------------------------------------------------------------------
// 获取生活经验值
U32 Player::getLivingExp()
{
	return mCurrentLivingExp;
}

U32 Player::getLivingCurrentLevelExp()
{
   return  g_LivingLevelExp[mLivingLevel];
}
// ----------------------------------------------------------------------------
// 获取生活等级
U32 Player::getLivingLevel()
{
	return mLivingLevel;
}

void Player::setOtherFlag(U32 flag) 
{
	mOtherFlag |= flag;
#ifdef NTJ_SERVER
	sendPlayerEvent(EventOtherFlagMask);
#endif
}

void Player::clearOtherFlag(U32 flag)
{
	mOtherFlag &= ~flag;
#ifdef NTJ_SERVER
	sendPlayerEvent(EventOtherFlagMask);
#endif
}

bool Player::IsOtherFlag(U32 flag) 
{ 
	return mOtherFlag & flag;
}
// ----------------------------------------------------------------------------
//获取师徒信息
MPManager* Player::getMPInfo()
{
	return pMPManagerInfo;
}