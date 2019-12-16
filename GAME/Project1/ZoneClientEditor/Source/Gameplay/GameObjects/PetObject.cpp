//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/GameObjects/PetObject.h"
#include "Gameplay/ai/PetAI.h"
#include "Gameplay/GameObjects/PetOperationManager.h"
#include "Effects/EffectPacket.h"

#ifdef NTJ_CLIENT
#include "gui/core/guiTypes.h"
#include "gui/core/guiCanvas.h"
#include "Ui/dGuiMouseGamePlay.h"
#include "gfx/gfxFontRenderBatcher.h"
#include "gui/core/guiDefaultControlRender.h"
#include "Gameplay/ClientPlayerManager.h"
#include "Gameplay/ClientGameplayState.h"
#include "ui/guiPlayerIcon.h"
#endif

// ========================================================================================================================================
//	PetObject
// ========================================================================================================================================
const CalcFactor PetObject::smCalcFactor = {13.0f, 22.8f, 1.50f, 1.26f, 2.83f, 9.0f, 3.0f};
const SimTime PetObject::sDefaultCorpseTime = 3000;
const SimTime PetObject::sDefaultFadeTime = 2000;


IMPLEMENT_CO_NETOBJECT_V1(PetObject);

PetObject::PetObject()
{
	mGameObjectMask |= PetObjectType;
	mMaster = NULL;
	// AI
#ifdef NTJ_CLIENT
	dMemset(mPetName, 0, sizeof(mPetName));
	mMasterId = 0;
	mIsFirstUnpack = true;
#endif
#ifdef NTJ_SERVER
	m_pAI = NULL;
#endif
}

PetObject::~PetObject()
{
	// AI
#ifdef NTJ_SERVER
	if(m_pAI)
		SAFE_DELETE(m_pAI);
#endif
}

bool PetObject::onAdd()
{
	// 直接从PetRepository中读取dataBlock
	if(!GameBase::mDataBlock)
		GameBase::mDataBlock = dynamic_cast<GameBaseData*>(g_PetRepository.GetPetData(mDataBlockId));

	if(!Parent::onAdd())
		return false;

	//setShapeName(mDataBlock->petName);
#ifdef NTJ_SERVER
	if(!m_pAI)
	{
		m_pAI = new PetAI(this);
	}

	// 初始化
	const stPetInfo* info = mMaster->getPetTable().getPetInfo(mPetSlot);
	if (!info)
		return false;

	setObjectName(mDataBlock->petName);
	m_Race = mDataBlock->race;
	m_Family = Family_None;
	m_Level = info->level;
	m_BuffTable.AddBuff(Buff::Origin_Base ,Macro_GetBuffId(Macro_GetBuffSeriesId(mDataBlock->buffId), m_Level), this);
	m_BuffTable.AddBuff(Buff::Origin_Base ,info->randBuffId, this);
	CalcStats();
	setHP(0xFFFFFF);
	setMP(0xFFFFFF);
	setPP(0xFFFFFF);
	m_SkillTable.AddDefaultSkill(mDataBlock->defaultSkillId);

	setCombative(true);
	setShapeName(info->name);

#endif

	return true;
}

bool PetObject::onNewDataBlock(GameBaseData* dptr)
{
	mDataBlock = dynamic_cast<PetObjectData*>(dptr);
	if (!mDataBlock || !Parent::onNewDataBlock(mDataBlock))
		return false;

	return true;
}

void PetObject::processTick(const Move *move)
{
#ifdef NTJ_SERVER
	if(!mMaster)
	{
		deleteObject();
		return;
	}
	if(getDistance(mMaster) > 20.0f)
		setTransform(mMaster->getTransform());

	if(m_pAI)
		m_pAI->Update();

	Move aiMove;
	if (!move && isServerObject() && m_pAI && m_pAI->GetNextMove(aiMove))
		move = &aiMove;
#endif

	Parent::processTick(move);
}

U64 PetObject::packUpdate(NetConnection *conn, U64 mask, BitStream *stream)
{
	U64 retMask = Parent::packUpdate(conn, mask, stream);

#ifdef NTJ_SERVER
	if (stream->writeFlag(mask & MasterMask))
	{
		// 无法取得主人，需要重发
		stream->writeInt(conn->getGhostIndex(mMaster), NetConnection::GhostIdBitSize);
		stream->writeInt(mPetSlot, Base::Bit8);
	}

	if (stream->writeFlag(mask & PetNameMask))
	{
		const stPetInfo *pPetInfo = ((PetTable &)mMaster->getPetTable()).getPetInfo(mPetSlot);
		stream->writeString(pPetInfo->name, 32);
		stream->writeInt(mMaster->getPlayerID(), Base::Bit32);
	}
#endif
	
	return retMask;
}

void PetObject::unpackUpdate(NetConnection *conn, BitStream *stream)
{
#ifdef NTJ_CLIENT
	Parent::unpackUpdate(conn,stream);

	if(stream->readFlag()) // MasterMask
	{
		S32 id = stream->readInt(NetConnection::GhostIdBitSize);
		setMaster(dynamic_cast<Player*>(conn->resolveGhost(id)));
		mPetSlot = stream->readInt(Base::Bit8);
	}

	if (stream->readFlag())
	{
		stream->readString(mPetName,COMMON_STRING_LENGTH);
		mMasterId = stream->readInt(Base::Bit32);
		setDirty(true);
		setShapeName(mPetName);
		setObjectName(mPetName);
		/*GuiObjectIcon *petIconGui = dynamic_cast<GuiObjectIcon*>(Sim::findObject("PetIconGui"));
		if (petIconGui)
		{
			Con::executef(petIconGui, "SetDirty");
		}*/
	}
	if (mIsFirstUnpack)
	{
		mIsFirstUnpack = false;
        Con::executef("PetIconWnd_Refresh");
		Con::executef("PetInfoWnd_RefreshWnd");
	}
#endif	
}

void PetObject::OnStatsChanged()
{
#ifdef NTJ_CLIENT
	// 理论上执行到这里的都是客户端玩家的宠物
	if(getMaster() == g_ClientGameplayState->GetControlObject())
		Con::executef("PetInfoWnd_RefreshWnd");
	else
		Con::errorf("we pack other pet's stats!");
#endif
}

void PetObject::onEnabled()
{
	Parent::onEnabled();
	setFade(RenderableSceneObject::Fade_DamageState, false, true, sDefaultFadeTime);
}

void PetObject::onDisabled()
{
	Parent::onDisabled();
	setFade(RenderableSceneObject::Fade_DamageState, true, true, sDefaultFadeTime, sDefaultCorpseTime);
#ifdef NTJ_SERVER
	m_pAI->SetNextAIState(PetAI::PET_AI_DEATH);
	SetTarget(NULL);
	PetOperationManager::PetDead(mMaster, this);
#endif
}

void PetObject::initPersistFields()
{
	Parent::initPersistFields();
}

void PetObject::setLevel(S32 level)
{
#ifdef NTJ_SERVER
	const stPetInfo* info = mMaster->getPetTable().getPetInfo(mPetSlot);

	m_BuffTable.RemoveBuff(Macro_GetBuffId(Macro_GetBuffSeriesId(mDataBlock->buffId), m_Level));
	m_BuffTable.AddBuff(Buff::Origin_Base ,Macro_GetBuffId(Macro_GetBuffSeriesId(mDataBlock->buffId), level), this);
	m_Level = level;
	CalcStats();

	// 升级特效 [7/23/2009 joy]
	static MatrixF mat;
	mat.identity();
	mat.setPosition(getPosition());
	g_EffectPacketContainer.addEffectPacket(EP_LevelUp, this, mat, NULL, getPosition());

	// 强制计算
	CalcStats();
	setHP(0xFFFFFF);
	setMP(0xFFFFFF);
	setPP(0xFFFFFF);
	setMaskBits(GameObject::BaseInfoMask);
#endif
}

void PetObject::CalcStats()
{
	if(m_BuffTable.GetRefresh())
	{
		PetTable &petTable = (PetTable &)mMaster->getPetTable();
		CalcFactor factor;
		Stats tmpStats = petTable.CalAddedStats(mPetSlot, factor);
		m_BuffTable.CalculateStats_Pet(tmpStats, factor);
		m_SkillTable.Update();
		setMaskBits(StatsMask | SpeedMask | HPMask | MPMask | PPMask);
	}
}

bool PetObject::CanAttackTarget(GameObject* obj)
{
	if(obj == mMaster || !Parent::CanAttackTarget(obj))
		return false;

	return true;
}

void PetObject::addExp(S32 exp)
{
	if (!mMaster)
		return;

	if ( ((PetTable&)mMaster->getPetTable()).addExp(mPetSlot, exp, mMaster) )
	{
#ifdef NTJ_SERVER
		PetOperationManager::ServerSendUpdatePetSlotMsg(mMaster, mPetSlot, stPetInfo::PetOther_Mask);
#endif
	}
}

void PetObject::setMaster(Player* player)
{
	if(!player || player == mMaster)
		return;

	mMaster = player;
	mMaster->setPet(this);
#ifdef NTJ_SERVER
	setMaskBits(MasterMask | PetNameMask);
#endif
}

#ifdef NTJ_CLIENT
bool PetObject::updateData(GuiControlProfile** pStyleList,RectF *rect)
{
	bool bUpdate = false;
	if (m_IsDirty || GuiCanvas::mGlobalDirty || m_BuffTexture.isNull())
	{
		GuiControlProfile* pStyle = pStyleList[dGuiMouseGamePlay::StyleOne];
		GuiControlProfile* pStyle1 = pStyleList[dGuiMouseGamePlay::StyleTwo];
		if(NULL == pStyle || NULL == pStyle1)
			return bUpdate;

		char strBuff[64] = {0,};
		Player* pPlayer = PLAYER_MGR->GetLocalPlayer( mMasterId );
		if(NULL == pPlayer)
			return bUpdate;

		dSprintf(strBuff,sizeof(strBuff),"<%s的宠物>",pPlayer->getPlayerName());
		rect->extent.x = pStyle1->mFont->getStrWidthOutline(strBuff);
		rect->extent.y = pStyle1->mFont->getHeightOutline();

		dSprintf(strBuff,sizeof(strBuff),"%s",mPetName);
		rect->extent.x = getMax((U32)rect->extent.x,pStyle->mFont->getStrWidthOutline(strBuff));
		rect->extent.y += pStyle->mFont->getHeightOutline();

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

void PetObject::drawObjectInfo(GameObject *mainPlayer,void *pParam1,void *pParam2)
{
	GuiControlProfile** pStyleList = (GuiControlProfile **)pParam2;
	Player* control_player = (Player*)mainPlayer;

	if(NULL == pStyleList || NULL == control_player)
		return;

	char objectName[64]  = {0,};
	char titleName[COMMON_STRING_LENGTH]   = {0,};

	Point2I objectNameOffset;
	Point2I titleNameOffset;

	ColorI objectNameColor(COLORI_PET_ME);
	ColorI titleNameColor(COLORI_PET_TITLE);

	Player* pPlayer = PLAYER_MGR->GetLocalPlayer( mMasterId );
	if(NULL == pPlayer)
		return;

	if (updateData(pStyleList,&texRect))
	{
		GuiControlProfile* pStyle = pStyleList[dGuiMouseGamePlay::StyleOne];
		GuiControlProfile* pStyle1 = pStyleList[dGuiMouseGamePlay::StyleTwo];
		if(NULL == pStyle || NULL == pStyle1)
			return;

		if(control_player != pPlayer)
		{
			if(control_player->CanAttackTarget(pPlayer))
				objectNameColor.set(COLORI_PLAYER_ENEMY);
			else
				objectNameColor.set(COLORI_PET_OTHERS);
		}
		else
		{
			objectNameColor.set(COLORI_PET_ME);
		}

		GFX->getDrawUtil()->setBlendTyps(GFXDrawUtil::SeparateAlphaBlend);
		GFX->pushActiveRenderTarget();
		GFXTextureTargetRef myTarg = GFX->allocRenderToTextureTarget();
		myTarg->attachTexture(GFXTextureTarget::Color0, m_BuffTexture );
		GFX->setActiveRenderTarget(myTarg);
		GFX->clear( GFXClearTarget, ColorI( 0, 0, 0, 0 ), 1.0f, 0 );
		RectI clipRect = GFX->getClipRect();	
		GFX->setClipRect(RectI(0,0,texRect.extent.x,texRect.extent.y));

		if(dStrcmp(pPlayer->getPlayerName(),""))
		{
			dSprintf(objectName,sizeof(objectName),"<%s的宠物>",pPlayer->getPlayerName());
			objectNameOffset.x = (texRect.extent.x - pStyle1->mFont->getStrWidthOutline(objectName))*0.5f;
			objectNameOffset.y = texRect.extent.y -  pStyle1->mFont->getHeightOutline();	
			GFX->getDrawUtil()->drawTextOutline(pStyle1->mFont,objectNameOffset,objectName,&objectNameColor,true,pStyle1->mFontOutline); 
		}
		//宠物信息
		//const stPetInfo* pPetInfo =  pPlayer->getPetTable().getPetInfo(mPetSlot);
		if (dStrcmp(mPetName,""))
		{
			dSprintf(objectName,sizeof(objectName),"%s",mPetName);
			objectNameOffset.x = (texRect.extent.x - pStyle->mFont->getStrWidthOutline(objectName))*0.5f;
			objectNameOffset.y -= pStyle->mFont->getHeightOutline();
			GFX->getDrawUtil()->drawTextOutline(pStyle->mFont,objectNameOffset,objectName,&objectNameColor,true,pStyle->mFontOutline); 
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

void PetObject::drawObjectWord(void *pParam)
{
	GuiControlProfile** pStyle = (GuiControlProfile **)pParam;

	if(NULL == getChatInfo() || NULL == pStyle)
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