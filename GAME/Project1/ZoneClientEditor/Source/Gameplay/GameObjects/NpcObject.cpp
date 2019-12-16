//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#include "console/consoleTypes.h"
#include "console/console.h"
#include "math/mPoint.h"

#include "Gameplay/GameObjects/NpcObject.h"
#include "Gameplay/ai/NpcAI.h"
#include "gfx/gfxFontRenderBatcher.h"
#include "Effects/EffectPacket.h"

#ifdef NTJ_SERVER
#include "Gameplay/ai/NpcHateLink.h"
#include "Gameplay/ai/NpcTeam.h"
#endif

#ifdef NTJ_CLIENT
#include "gui/core/guiTypes.h"
#include "gfx/gfxDrawUtil.h"
#include "UI/dGuiMouseGamePlay.h"
#include "gui/core/guiDefaultControlRender.h"
#endif

// ========================================================================================================================================
//	NpcObject
// ========================================================================================================================================
IMPLEMENT_CO_NETOBJECT_V1(NpcObject);

const SimTime NpcObject::sForceUpdateTicks = 200;
const SimTime NpcObject::sDefaultRiseInterval = 10000;
const SimTime NpcObject::sDefaultCorpseTime = 3000;
const SimTime NpcObject::sDefaultFadeTime = 2000;

NpcObject::NpcObject()
{
	mGameObjectMask |= NpcObjectType;
	m_BelongTo = NULL;
	mExclusivePlayerId = 0;
	mForceUpdate = 0;
	mRiseTime = 0;
	// AI
#ifdef NTJ_SERVER
	m_pAI = NULL;
	mLayerID = 1;
#endif
	m_bAI = true;
	mCanTame = false;
	m_InitialPosition.set(getPosition());
	m_InitialRotation.set(0,0,0);
	m_MoveDestination.set(0,0,0);
	m_WayPointID = 0;		// 路径名,关联gServerPathManager的中的路径 
	m_nSeqIndex  = 0;		// Path中的 Sequence index;
	mHateLinkId  = 0;
	mMissionEffectId = 0;
#ifdef NTJ_CLIENT
	texRect.point.set(0,0);
	texRect.extent.set(256,32);
#endif
}

NpcObject::~NpcObject()
{
	// AI
#ifdef NTJ_SERVER
	if(m_pAI)
		SAFE_DELETE(m_pAI);
#endif
}

bool NpcObject::onAdd()
{
	// 直接从NpcRepository中读取dataBlock
	if(!GameBase::mDataBlock)
		GameBase::mDataBlock = dynamic_cast<GameBaseData*>(g_NpcRepository.GetNpcData(mDataBlockId));

	if(!Parent::onAdd())
		return false;

	// 怪物初始化
	setObjectName(mDataBlock->npcName);
	m_InitialRotation = mRot;
	m_Race = mDataBlock->race;
	m_Family = mDataBlock->family;
	m_Influence = (mDataBlock->influence != Influence_None) ? mDataBlock->influence : m_Influence;
	mCanTame = (mDataBlock->flags & NpcObjectData::Flags_Tame);
	m_MovementAreaRadius_FreeMove = mDataBlock->activeField;
	m_MovementAreaRadius_Trace = mDataBlock->chaseField;
	setCombative(mDataBlock->flags & NpcObjectData::Flags_Combative);

#ifdef NTJ_SERVER
	m_SkillTable.AddDefaultSkill(mDataBlock->defaultSkillId);
	m_BuffTable.AddBuff(Buff::Origin_Base , mDataBlock->isFlags(NpcObjectData::Flags_ConstBuffId) ? mDataBlock->buffId : Macro_GetBuffId(Macro_GetBuffSeriesId(mDataBlock->buffId), m_Level), this);
	onSpawn();

	if(!m_pAI && m_bAI)
	{
		m_pAI = new NpcAI(this);
	}

	// 仇恨链接
	gNpcHateLink.add(this);
#endif

#ifdef NTJ_CLIENT
	if(!isCombative() && mDataBlock->topIcon && mDataBlock->topIcon[0])
	{
		char szIconName[512] = {0,};
		dSprintf(szIconName,sizeof(szIconName),"gameres\\data\\icon\\top\\%s",mDataBlock->topIcon);
		mTextureTopIcon.set(szIconName,&GFXDefaultGUIProfile); 
	}
#endif

	return true;
}

void NpcObject::onRemove()
{
#ifdef NTJ_SERVER
	// 仇恨链接
	gNpcHateLink.remove(this);
	// 队伍
	gNpcTeamManager.leave(this);
#endif

	Parent::onRemove();
}

bool NpcObject::onNewDataBlock(GameBaseData* dptr)
{
	mDataBlock = dynamic_cast<NpcObjectData*>(dptr);
	if (!mDataBlock || !Parent::onNewDataBlock(mDataBlock))
		return false;

	return true;
}

void NpcObject::processTick(const Move *move)
{
	PROFILE_START(NpcObject_processTick);

#ifdef NTJ_SERVER
	if(++mForceUpdate > sForceUpdateTicks)
	{
		mForceUpdate = 0;
		setMaskBits(MoveMask);
	}
	PROFILE_START(NpcObject_AI);
	PROFILE_START(AI_Update);
	if(m_pAI)
		m_pAI->Update();
	PROFILE_END(AI_Update);

	Move aiMove;
	if (!move && isServerObject() && m_pAI && m_pAI->GetNextMove(aiMove))
		move = &aiMove;
	PROFILE_END(NpcObject_AI);
#endif

	Parent::processTick(move);
	PROFILE_END(NpcObject_processTick);
}

U64 NpcObject::packUpdate(NetConnection *conn, U64 mask, BitStream *stream)
{
	U64 retMask = Parent::packUpdate(conn, mask, stream);
	if(stream->writeFlag(mask & GameObject::BaseInfoMask))
	{
		stream->writeInt(mExclusivePlayerId, Base::Bit32);
	}
	return retMask;
}

void NpcObject::unpackUpdate(NetConnection *conn, BitStream *stream)
{
	Parent::unpackUpdate(conn,stream);
	if(stream->readFlag())
	{
		mExclusivePlayerId = stream->readInt(Base::Bit32);
	}
}

void NpcObject::initPersistFields()
{
	Parent::initPersistFields();

	addField("mWayPointID", TypeS32, Offset(m_WayPointID,   NpcObject));
	addField("canTame",		TypeS32, Offset(mCanTame,		NpcObject));
	addField("HateLinkId",	TypeS32, Offset(mHateLinkId,	NpcObject));
}

void NpcObject::onEnabled()
{
	Parent::onEnabled();

	setFade(RenderableSceneObject::Fade_DamageState, false, true, mDataBlock ? mDataBlock->fadeTime : sDefaultFadeTime, 100.0f /*just in case*/);
#ifdef NTJ_SERVER
	m_pAI->SetNextAIState(NpcAI::NPC_AI_IDLE);
#endif
}

void NpcObject::onDisabled()
{
	Parent::onDisabled();

	setFade(RenderableSceneObject::Fade_DamageState, true, true, mDataBlock ? mDataBlock->fadeTime : sDefaultFadeTime, mDataBlock ? mDataBlock->corpseTime : sDefaultCorpseTime);
#ifdef NTJ_SERVER
	m_pAI->SetNextAIState(NpcAI::NPC_AI_DEATH);
	SetTarget(NULL);
	gNpcTeamManager.leave(this);
#endif
}

bool NpcObject::CanAttackTarget(GameObject* obj)
{
	if(!Parent::CanAttackTarget(obj))
		return false;

	// 目标与自身皆为NPC，判断声望
	if(obj->getGameObjectMask() & NpcObjectType)
	{
		if(g_FameRelationship[getInfluence()][obj->getInfluence()] > Fame_Neutrality)
			return false;
	}
	return true;
}

bool NpcObject::isDamageable() const
{
	if(!Parent::isDamageable())
		return false;

#ifndef NTJ_EDITOR
	if(m_pAI && m_pAI->GetCurrentAIState() == NpcAI::NPC_AI_GOHOME)
		return false;
#endif
	return true;
}

bool NpcObject::collideBox(const Point3F &start, const Point3F &end, RayInfo* info)
{
	if (this->isDisabled())
		return false;

	return Parent::collideBox(start, end, info);
}

void NpcObject::onCombatBegin(GameObject* pObj)
{
#ifdef NTJ_SERVER
	// 仇恨链接
	gNpcHateLink.onHateLink(this, pObj);
#endif
}

void NpcObject::onCombatEnd(GameObject* pObj)
{
#ifdef NTJ_SERVER
	// 退出队伍
	gNpcTeamManager.leave(this);
#endif
}

void NpcObject::setMissionEffect(U32 effectId)
{
	clearMissionEffect();
	mMissionEffectId = g_EffectPacketContainer.addEffectPacket(effectId,this,getRenderTransform(), this, getRenderPosition());
}

void NpcObject::clearMissionEffect()
{
	if(mMissionEffectId)
		g_EffectPacketContainer.removeEffectPacket(mMissionEffectId);
}

bool NpcObject::checkExclusive(GameObject* pObj)
{
	if(!pObj)
		return false;
	if(!mExclusivePlayerId)
		return true;
	if(pObj->getGameObjectMask() & PlayerObjectType)
	{
		if(((Player*)pObj)->getPlayerID() == mExclusivePlayerId)
			return true;
		return false;
	}
	else if(pObj->getGameObjectMask() & PetObjectType)
	{
		if(((PetObject*)pObj)->getMaster() && ((PetObject*)pObj)->getMaster()->getPlayerID() == mExclusivePlayerId)
			return true;
		return false;
	}
	return false;
}


///////////////////////////////服务端代码////////////////////////////////////////////
///////////////////////////////服务端代码////////////////////////////////////////////
///////////////////////////////服务端代码////////////////////////////////////////////
#ifdef NTJ_SERVER

void NpcObject::SetRiseTime()
{
	if(!mDataBlock || 0 == mDataBlock->riseInterval)
	{
		mRiseTime = (mDataBlock ? mDataBlock->corpseTime + mDataBlock->fadeTime + sDefaultRiseInterval : sDefaultRiseInterval) + Platform::getVirtualMilliseconds();
		return;
	}
	mRiseTime = Platform::getVirtualMilliseconds() + getMax(mDataBlock->riseInterval, mDataBlock->corpseTime + mDataBlock->fadeTime + sDefaultRiseInterval);
}

bool NpcObject::CheckRiseTime()
{
	 if(mRiseTime <= Platform::getVirtualMilliseconds())
	 {
		 if(!mDataBlock || 0 == mDataBlock->riseInterval)
		 {
			 safeDeleteObject();
			 return false;
		 }
		 return true;
	 }
	 return false;
}

void NpcObject::onSpawn()
{
	// 动态缩放
	if(!mDataBlock->isFlags(NpcObjectData::Flags_ConstScale))
	{
		F32 fScale = gRandGen.randF(mDataBlock->minScale, mDataBlock->maxScale);
		Point3F scale(fScale,fScale,fScale);
		setScale(scale);
	}

	// 动态等级
	setLevel(gRandGen.randI(mDataBlock->minLevel, mDataBlock->maxLevel));

	// 附加状态
	AddBuff(Buff::Origin_Buff, mDataBlock->exBuffId_a, this);
	AddBuff(Buff::Origin_Buff, mDataBlock->exBuffId_b, this);
	CalcStats();
	setHP(0xFFFFFF);
	setMP(0xFFFFFF);
	setPP(0xFFFFFF);
	setMaskBits(GameObject::BaseInfoMask);
}

void NpcObject::onReset()
{
	mHateList.reset();
	m_BuffTable.ClearDebuff();
	CalcStats();
	setHP(0xFFFFFF);
	setMP(0xFFFFFF);
	setPP(0xFFFFFF);
	setMaskBits(GameObject::BaseInfoMask);
}

void NpcObject::addLevel(S32 level)
{
#ifdef NTJ_SERVER
	U32 nOld = m_Level;
	m_Level = mClamp(m_Level + level, 1, EnablePlayerLevel); 

	if (m_Level != nOld)
	{
		if(!mDataBlock->isFlags(NpcObjectData::Flags_ConstBuffId))
		{
			m_BuffTable.RemoveBuff(Macro_GetBuffId(Macro_GetBuffSeriesId(mDataBlock->buffId), nOld));
			m_BuffTable.AddBuff(Buff::Origin_Base, Macro_GetBuffId(Macro_GetBuffSeriesId(mDataBlock->buffId), m_Level), this);
			CalcStats();
		}
		setHP(0xFFFFFF);
		setMP(0xFFFFFF);
		setPP(0xFFFFFF);
		setMaskBits(GameObject::BaseInfoMask);
		setDirty(true);
	}
#endif
}

void NpcObject::setLevel(S32 level)
{
	addLevel(level - (S32)m_Level);
}

#endif

///////////////////////////////客户端代码////////////////////////////////////////////
///////////////////////////////客户端代码////////////////////////////////////////////
///////////////////////////////客户端代码////////////////////////////////////////////
#ifdef NTJ_CLIENT
bool NpcObject::updateData(GuiControlProfile** pStyleList,RectF *rect)
{
	bool bUpdate = false;

	if (m_IsDirty || GuiCanvas::mGlobalDirty || m_BuffTexture.isNull())
	{
		GuiControlProfile* pStyle = pStyleList[dGuiMouseGamePlay::StyleOne];
		GuiControlProfile* pStyle1 = pStyleList[dGuiMouseGamePlay::StyleTwo];
		if(NULL == pStyle || NULL == pStyle1)
			return bUpdate;

		char strBuff[64] = {0,};
		dSprintf(strBuff,sizeof(strBuff),"%s-%d级",getObjectName(),getLevel());
		rect->extent.x = pStyle->mFont->getStrWidthOutline(strBuff);
		rect->extent.y = pStyle->mFont->getHeightOutline();

		if(dStrcmp(mDataBlock->npcTitle,""))
		{
			dSprintf(strBuff,sizeof(strBuff),"%s",mDataBlock->npcTitle);
			rect->extent.x = getMax((U32)rect->extent.x,pStyle1->mFont->getStrWidthOutline(strBuff));
			rect->extent.y += pStyle1->mFont->getHeightOutline();
		}

		if((GFXTextureObject*)mTextureTopIcon)
		{
			rect->extent.x = getMax((U32)rect->extent.x,mTextureTopIcon.getWidth());
			rect->extent.y += mTextureTopIcon.getHeight();
		}

		if( !((GFXTextureObject*)m_BuffTexture && m_BuffTexture.getWidth() == rect->extent.x 
			&& m_BuffTexture.getHeight() == rect->extent.y ) )
		{
			m_BuffTexture.set(rect->extent.x,rect->extent.y,GFXFormat_32BIT,&GFXDefaultRenderTargetProfile);
		}
		bUpdate = true;
	}
	else if(m_BuffTexture.getPointer()->setDirty())
	{
		bUpdate = true;
	}

	Point3F screenPos;
	Point3F objectPos;
	objectPos = getRenderPosition(); 
	objectPos.z += getWorldBox().len_z();
	if (g_UIMouseGamePlay->project(objectPos,&screenPos))
	{
		rect->point.x = screenPos.x - rect->extent.x * 0.5f;
		rect->point.y = screenPos.y - (rect->extent.y + dGuiMouseGamePlay::HpOffsetY + 2);
	}

	return bUpdate;
}

//绘制头顶文字
void NpcObject::drawObjectInfo(GameObject *mainPlayer,void *pParam1,void *pParam2)
{
	GuiControlProfile** pStyleList = (GuiControlProfile **)pParam2;

	if(isDisabled() || NULL == pStyleList || mFadeVal < 0.01f)
		return;
	// 隐身状态 && 不被玩家选中 不显示头顶信息
	if((m_BuffTable.GetEffect() & BuffData::Effect_Invisibility) && mainPlayer->GetTarget() != this)
		return;

	char objectName[64]  = {0,};
	char titleName[COMMON_STRING_LENGTH]   = {0,};

	Point2I objectNameOffset;
	Point2I titleNameOffset;
	Point2I NpcIconOffset;

	ColorI objectNameColor(COLORI_NPC_FRIEND);
	ColorI titleNameColor(COLORI_NPC_TITLE);

	if (updateData(pStyleList,&texRect))
	{
		GuiControlProfile* pStyle = pStyleList[dGuiMouseGamePlay::StyleOne];
		GuiControlProfile* pStyle1 = pStyleList[dGuiMouseGamePlay::StyleTwo];
		if(NULL == pStyle || NULL == pStyle1)
			return;

		GFX->getDrawUtil()->setBlendTyps(GFXDrawUtil::SeparateAlphaBlend);
		GFX->pushActiveRenderTarget();
		GFXTextureTargetRef myTarg = GFX->allocRenderToTextureTarget();
		myTarg->attachTexture(GFXTextureTarget::Color0, m_BuffTexture );
		GFX->setActiveRenderTarget(myTarg);
		GFX->clear( GFXClearTarget, ColorI( 0, 0, 0, 0 ), 1.0f, 0 );
		RectI clipRect = GFX->getClipRect();	
		GFX->setClipRect(RectI(0,0,texRect.extent.x,texRect.extent.y));
		
		if (!isCombative())
		{			
			if (dStrcmp(getObjectName(),""))
			{
				dSprintf(objectName,sizeof(objectName),"%s",getObjectName());
				objectNameOffset.x = (texRect.extent.x - pStyle->mFont->getStrWidthOutline(objectName))*0.5f;
				objectNameOffset.y = texRect.extent.y -  pStyle->mFont->getHeightOutline();

				objectNameColor.set(COLORI_NPC_FRIEND);			
				GFX->getDrawUtil()->drawTextOutline(pStyle->mFont,objectNameOffset,objectName,&objectNameColor,true,pStyle->mFontOutline); 
			}

			if (dStrcmp(mDataBlock->npcTitle,""))
			{
				dSprintf(titleName,sizeof(titleName),"%s",mDataBlock->npcTitle);
				titleNameOffset.x = (texRect.extent.x - pStyle1->mFont->getStrWidthOutline(titleName))*0.5f;
				titleNameOffset.y = objectNameOffset.y - pStyle1->mFont->getHeightOutline();
				GFX->getDrawUtil()->drawTextOutline(pStyle1->mFont,titleNameOffset,titleName,&titleNameColor,true,pStyle1->mFontOutline);
			}

			if((GFXTextureObject*)mTextureTopIcon)
			{
				Point2I ext(mTextureTopIcon.getWidth(),mTextureTopIcon.getHeight());
				NpcIconOffset.x = (texRect.extent.x - ext.x)*0.5f;
				NpcIconOffset.y = titleNameOffset.y - ext.y;
				RectI rect(NpcIconOffset.x,NpcIconOffset.y,ext.x,ext.y);
				GFX->getDrawUtil()->drawBitmapStretch(mTextureTopIcon,rect);
			}
		}
		else
		{
			if (dStrcmp(getObjectName(),""))
			{
				dSprintf(objectName,sizeof(objectName),"%s-%d级",getObjectName(),getLevel());

				objectNameOffset.x = (texRect.extent.x - pStyle->mFont->getStrWidthOutline(objectName))*0.5f;
				objectNameOffset.y = texRect.extent.y -  pStyle->mFont->getHeightOutline();

				if(getDataBlock() && ((NpcObjectData*)getDataBlock())->isFlags(NpcObjectData::Flags_Active))
					objectNameColor.set(COLORI_MONSTER_AGGRESSIVE);
				else
					objectNameColor.set(COLORI_MONSTER_PASSIVE);	

				GFX->getDrawUtil()->drawTextOutline(pStyle->mFont,objectNameOffset,objectName,&objectNameColor,true,pStyle->mFontOutline);
			}

			if (dStrcmp(mDataBlock->npcTitle,""))
			{
				dSprintf(titleName,sizeof(titleName),"%s",mDataBlock->npcTitle);
				titleNameOffset.x = (texRect.extent.x - pStyle1->mFont->getStrWidthOutline(titleName))*0.5f;
				titleNameOffset.y = objectNameOffset.y - pStyle1->mFont->getHeightOutline();
				GFX->getDrawUtil()->drawTextOutline(pStyle1->mFont,titleNameOffset,titleName,&titleNameColor,true,pStyle1->mFontOutline);
			}
		}

		GFX->popActiveRenderTarget();
		GFX->setClipRect( clipRect );
		setDirty(false);
	}

	GFX->getDrawUtil()->setBlendTyps(GFXDrawUtil::LerpAlphaBlend);
	GFX->getDrawUtil()->clearBitmapModulation();
	GFX->getDrawUtil()->setZ(mLastZ);
	RectI rect(mCeil(texRect.point.x),mCeil(texRect.point.y),texRect.extent.x,texRect.extent.y);
	GFX->getDrawUtil()->drawBitmapStretch(m_BuffTexture,rect);
	GFX->getDrawUtil()->setZ(0.0f);
}

void NpcObject::drawObjectWord(void *pParam)
{
	GuiControlProfile** pStyle = (GuiControlProfile **)pParam;

	if(isDisabled() || NULL == getChatInfo() || NULL == pStyle)
		return;

	GuiControlProfile* pProfile = pStyle[dGuiMouseGamePlay::StyleThree];
	if(NULL == pProfile)
		return;

	Point2I iExtent(200,75);
	//生成固定大小纹理(200X75)
	if(mBuffChatTexture.isNull())
		mBuffChatTexture.set(iExtent.x,iExtent.y,GFXFormat_32BIT,&GFXDefaultRenderTargetProfile);

	if(getChatDirty() || GuiCanvas::mGlobalDirty || mBuffChatTexture.getPointer()->setDirty())
	{
		CRichTextDrawer  textDrawer;
		//文本样式
		CRichTextDrawer::Style style;
		dStrcpy( style.fontName, sizeof(style.fontName), pProfile->mFontType);
		style.fontSize = pProfile->mFontSize;
		style.fontColor = pProfile->mFontColor;
		style.fontIsBitmap = pProfile->mBitmapFont;
		style.outline = pProfile->mFontOutline;
		textDrawer.setDefaultStyle( style );
		//设文本默认行高
		textDrawer.setWidth(iExtent.x - 30);
		textDrawer.setDefaultLineHeight(16);

		GFX->getDrawUtil()->setBlendTyps(GFXDrawUtil::SeparateAlphaBlend);
		GFX->pushActiveRenderTarget();
		GFXTextureTargetRef myTarg = GFX->allocRenderToTextureTarget();
		myTarg->attachTexture(GFXTextureTarget::Color0, mBuffChatTexture );
		GFX->setActiveRenderTarget(myTarg);
		GFX->clear( GFXClearTarget, ColorI( 0, 0, 0, 0 ), 1.0f, 0 );
		RectI clipRect = GFX->getClipRect();
		GFX->setClipRect(RectI(0,0,iExtent.x,iExtent.y));

		textDrawer.setContent(getChatInfo());
		U16 sizeHeight = textDrawer.getHeight();
		U16 sizeWidth  = textDrawer.getRealLineWidth();
		//边框 上、下、左、右 各留给文本15个像素显示空位
		RectI borderRect;
		borderRect.point.x = 0;
		borderRect.point.y = mClamp(iExtent.y - sizeHeight -30,0,iExtent.y);
		borderRect.extent.x = mClamp(sizeWidth+30,0,iExtent.x);
		borderRect.extent.y =  mClamp(sizeHeight+30,0,iExtent.y);
		renderBorder(borderRect,pProfile);
		//文本从point(15,10)开始绘
		GFX->setClipRect(RectI(0,0,iExtent.x,iExtent.y - 15));
		textDrawer.render(Point2I(15,borderRect.point.y + 10),borderRect);

		GFX->popActiveRenderTarget();
		GFX->setClipRect( clipRect );
		setChatDirty(false);
	}

	GFX->getDrawUtil()->setBlendTyps(GFXDrawUtil::LerpAlphaBlend);
	GFX->getDrawUtil()->clearBitmapModulation();
	GFX->getDrawUtil()->setZ(mLastZ);
	RectI rect(mCeil(texRect.point.x + texRect.extent.x/2),mCeil(texRect.point.y - iExtent.y),iExtent.x,iExtent.y);
	GFX->getDrawUtil()->drawBitmapStretch(mBuffChatTexture,rect);
	GFX->getDrawUtil()->setZ(0.0f);
}
#endif