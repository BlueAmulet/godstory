#pragma once
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/GameEvents/ServerGameNetEvents.h"
#include "T3D/gameConnection.h"
#include "Gameplay/GameObjects/Skill.h"
#include "Gameplay/GameObjects/MPManager.h"
#

// ----------------------------------------------------------------------------
// ��ȡ��ɫID
S32	Player::getPlayerID()
{ 
	return mPlayerId;
}

S32 Player::getAccountID()
{
	return mAccountId;
}

// ----------------------------------------------------------------------------
// ��ȡ��ɫ����
StringTableEntry Player::getPlayerName()
{ 
	return getObjectName();
}

// ----------------------------------------------------------------------------
// ��ȡ����״̬
U32	Player::getInteractionState()
{ 
	return mInteractionState;
}

// ----------------------------------------------------------------------------
// ��ȡ��������
SceneObject* Player::getInteraction()
{ 
	return mInteractionObject;
}

// ----------------------------------------------------------------------------
// ��ȡ�����Ϣ
stTeamInfo& Player::getTeamInfo()
{
	return mTeamInfo;
}

// ----------------------------------------------------------------------------
// ��ȡ��ֵ��ǿ
StatsPlusTable& Player::getPlusTable()
{
	return mPlusTable;
}

// ----------------------------------------------------------------------------
// ��ȡ��ɫ����ֵ
U32 Player::getVigor()
{
	return mVigor;
}
//-----------------------------------------------------------------------------
// ��ȡ��ɫ���ɫ����ֵ
U32 Player::getMaxVigor()
{
	return mMaxVigor;
}

// ----------------------------------------------------------------------------
// ��ȡ���ߵ�ͼ���
S32	Player::getZoneID()
{
	return mZoneID;
}

// ----------------------------------------------------------------------------
// ���ý�ɫ���ߵ�ͼ���
void Player::setZoneID(S32 zoneid)
{
	mZoneID = zoneid;
}

// ----------------------------------------------------------------------------
// �������
U32 Player::getBody()
{
	return mBody;
}

// ----------------------------------------------------------------------------
// �����
U32 Player::getFace()
{
	return mFace;
}

// ----------------------------------------------------------------------------
// ���ͷ��
U32 Player::getHair()
{
	return mHair;
}

//���ͷ����ɫ
U32 Player::getHairCol()
{
	return mHairCol;
}
// ----------------------------------------------------------------------------
// ��ȡPK״̬
bool Player::getPKState()
{ 
	return mSwitchState & SWITCHSTATE_PK;
}

// ----------------------------------------------------------------------------
// ��ȡ����״̬
bool Player::getSwitchState(U32 flag)
{
	return mSwitchState & flag;
}
// ----------------------------------------------------------------------------
// ��ȡ����������ר������ID
U32 Player::getLivingSkillPro()
{
	return mLivingSkillStudyID;
}
// ----------------------------------------------------------------------------
// ��ȡ�����ֵ
U32 Player::getLivingExp()
{
	return mCurrentLivingExp;
}

U32 Player::getLivingCurrentLevelExp()
{
   return  g_LivingLevelExp[mLivingLevel];
}
// ----------------------------------------------------------------------------
// ��ȡ����ȼ�
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
//��ȡʦͽ��Ϣ
MPManager* Player::getMPInfo()
{
	return pMPManagerInfo;
}