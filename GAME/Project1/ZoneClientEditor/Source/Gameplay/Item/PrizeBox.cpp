#include "math/mathIO.h"
#include "math/mBox.h"
#include "T3D/gameConnection.h"

#ifdef NTJ_CLIENT
#include "gfx/gfxDevice.h"
#include "gfx/gfxDrawUtil.h"
#include "gfx/gFont.h"
#include "core/color.h"
#endif

#ifdef NTJ_SERVER
#include "Gameplay/ServerGameplayState.h"
#include "T3D/gameFunctions.h"
#endif

#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/Item/DropItem.h"
#include "Gameplay/Item/ItemShortcut.h"
#include "Gameplay/item/Player_Item.h"

#include "Gameplay/GameObjects/GameObject.h"
#include "Gameplay/GameObjects/NpcObject.h"
#include "Gameplay/item/ItemDisplayRule.h"
#include "Gameplay/Item/Res.h"

#include "Gameplay/Item/PrizeBox.h"

PrizeBoxManager g_PrizeBoxManager;

IMPLEMENT_CO_NETOBJECT_V1(PrizeBox);

PrizeBox::PrizeBox()
{
	mNetFlags.set(Ghostable);

	//mTypeMask |= ItemObjectType;
	mItemMask |= PrizeBoxType;

	mGroundZDiff = 0.0f;
#ifdef NTJ_SERVER
	m_DropItemList = NULL;
	m_currentTime = 0;
	m_dropTime = 0;
#endif

#ifdef NTJ_CLIENT
	mSpeed = 0.0f;
	mAddSpeed = 0.001f;
	mCanPickup = true;    //false;
	m_pItemShortcut = NULL;
	m_IsBox = false;
	mZDiff = 0.0f;
#endif
}

PrizeBox::~PrizeBox()
{
	g_PrizeBoxManager.DeletePrizeBox(this);
	clear();
}

void PrizeBox::clear()
{
#ifdef NTJ_SERVER
	SAFE_DELETE(m_DropItemList);
#endif

#ifdef NTJ_CLIENT
	SAFE_DELETE(m_pItemShortcut);
#endif
}

bool PrizeBox::onAdd()
{
	if(!GameBase::mDataBlock)
		GameBase::mDataBlock = dynamic_cast<GameBaseData*>(g_NpcRepository.GetNpcData(mDataBlockId));
	if (!GameBase::mDataBlock)
		return false;

	if(!Parent::onAdd())
		return false;

	g_PrizeBoxManager.PushBack(this);

#ifdef NTJ_CLIENT
	if (!m_IsBox)
	{
		//根据物品ID获得mDataBlock		m_pItemShortcut->getRes()->getItemID();
		mDataBlockId = getItemDispID(m_pItemShortcut->getRes()->getItemID());
		//GameBaseData *dptr = dynamic_cast<GameBaseData*>(g_NpcRepository.GetNpcData(mDataBlockId));
		GameBaseData *dptr = dynamic_cast<GameBaseData*>(g_NpcRepository.GetNpcData(420000021));
		if (!setDataBlock(dptr))
			return false;

		setScale(m_scale);
		setTransform(m_mat);

		Point3F pos = getPosition();
		pos.z += mGroundZDiff;
		setPosition(pos);
	}
	
#endif
	addToScene();
	return true;
}

void PrizeBox::onRemove()
{
	removeFromScene();

	Parent::onRemove();
}

bool PrizeBox::onNewDataBlock(GameBaseData* dptr)
{
	if (!Parent::onNewDataBlock(dptr))
		return false;

	return true;
}

void PrizeBox::onEnabled()
{
	Parent::onEnabled();
}

void PrizeBox::processTick(const Move * move)
{
	Parent::processTick(move);
	setMaskBits(NameMask);

#ifdef NTJ_SERVER
	if (m_DropItemList->IsHavingNothing())
	{
		deleteObject();
		return;
	}

	m_currentTime = Platform::getRealMilliseconds();
	if (m_currentTime >= m_dropTime)
	{
		this->deleteObject();
	}
#endif

//#ifdef NTJ_CLIENT
//	if (mCanPickup)
//	{
//		return;
//	}
//
//	mSpeed += mAddSpeed;
//
//	Point3F pos = getPosition();
//	if (mZDiff + mSpeed <= mGroundZDiff)
//	{
//		mZDiff += mSpeed;
//	}
//	else
//	{
//		mZDiff = mGroundZDiff;		
//		mCanPickup = true;
//	}
//	pos.z -= mSpeed;
//	setPosition(pos);
//#endif
}

bool PrizeBox::canAddToScope(NetConnection *con)
{
	GameConnection *gameCon = dynamic_cast<GameConnection *>(con);
	if (!gameCon)
		return false;

	Player *player = dynamic_cast<Player *>(gameCon->getControlObject());
	if (!player)
		return false;

#ifdef NTJ_SERVER
	if ( m_DropItemList->IsEmpty(player->getPlayerID()) )
	{
		m_DropItemList->DeletePlayer(player->getPlayerID());
		return false;
	}
#endif

	return true;
}

#ifdef NTJ_SERVER
U64  PrizeBox::packUpdate(NetConnection *con, U64 mask, BitStream *stream)
{
	U64 retMask = Parent::packUpdate(con, mask, stream);

	mathWrite(*stream, getTransform());
	mathWrite(*stream, getScale());
	
	GameConnection *gameCon = dynamic_cast<GameConnection *>(con);
	if (!gameCon)
	{
		stream->writeFlag(false);
		return 0;
	}
	Player *player = dynamic_cast<Player *>(gameCon->getControlObject());
	if (!player)
	{
		stream->writeFlag(false);
		return 0;
	}

	U32 count = m_DropItemList->GetItemCount(player);
	if (count == 0)
	{
		stream->writeFlag(false);
		return 0;
	}

	stream->writeFlag(true);			

	//开始发送数据到客户端
	//stream->writeRangedF32(mGroundZDiff, -512, 512, 16);
	if (count == 1)	//发送单个物品
	{
		stream->writeFlag(false);
		ItemShortcut *pShortcut = dynamic_cast<ItemShortcut *>(m_DropItemList->m_ItemsMap[player->getPlayerID()]->GetSlot(0));
		if (stream->writeFlag(pShortcut != NULL))
		{
			pShortcut->packUpdate(stream);
		}		
	}
	else
	{
		stream->writeFlag(true);
	}

	return retMask;
}
#endif

#ifdef NTJ_CLIENT
void PrizeBox::unpackUpdate(NetConnection *con, BitStream *stream)
{
	Parent::unpackUpdate(con, stream);
	mathRead(*stream, &m_mat);
	mathRead(*stream, &m_scale);
	setScale(m_scale);
	setTransform(m_mat);

	if (!stream->readFlag())
	{
		//无内容可读取
		return;
	}

	//开始接收服务端数据
	//mGroundZDiff = stream->readRangedF32(-512, 512, 16);
	m_IsBox = stream->readFlag();
	if (!m_IsBox)
	{
		if (stream->readFlag())
		{
			if (!m_pItemShortcut)
				m_pItemShortcut = ItemShortcut::CreateEmptyItem();
			m_pItemShortcut->unpackUpdate(stream);
		}
	}
}
#endif

#ifdef NTJ_SERVER
bool PrizeBox::CreatePrizeBox(NpcObject *obj, DropItemList *pList)
{
	if (!pList || pList->IsHavingNothing())
		return false;

	PrizeBox *pBox = new PrizeBox;
	if (!pBox)
		return false;

	NpcObjectData* npcdata = g_NpcRepository.GetNpcData(obj->getDataID());
	pBox->m_DropItemList = pList;
	if (npcdata->packageShapeId == 0)
	{
		delete pBox;
		return false;
	}
	pBox->setDataID(npcdata->packageShapeId);		//测试暂时用怪物模型代替宝箱模型
	pBox->setEnabled(true);
	pBox->m_currentTime = Platform::getRealMilliseconds();
	pBox->m_dropTime = npcdata->dropTime + pBox->m_currentTime;

	Point3F position;
	position = obj->getPosition();
	//position.x += 3;
	//position.y += 3;
	pBox->setPosition(position);
	//pBox->mGroundZDiff = obj->getWorldBox().len_z() / 2;
	pBox->setScale(VectorF(1, 1, 1));
	pBox->setLayerID(obj->getLayerID());
	pBox->registerObject();

	addToMissionGroup(pBox);
	return true;
}
#endif



PrizeBoxManager::PrizeBoxManager()
{
#ifdef NTJ_SERVER
	m_nMaxCount = 20;
#endif
}

PrizeBoxManager::~PrizeBoxManager()
{
	Clear();
}

void PrizeBoxManager::Clear()
{
	PrizeBoxList::iterator it = m_list.begin();

	while(it != m_list.end())
	{
		if (*it)
		{
#ifdef NTJ_SERVER
			(*it)->deleteObject();
#endif
#ifdef NTJ_CLIENT
			PrizeBoxPickup *pObj = (*it);
			pObj->m_PrizeBox->deleteObject();
			delete pObj;
#endif
			m_list.erase(it++);
		}
		else
		{
			++it;
		}
	}

	m_list.clear();
#ifdef NTJ_SERVER
	m_nMaxCount = 0;
#endif
}

S32 PrizeBoxManager::GetCount()
{
	return m_list.size();
}

void PrizeBoxManager::DeletePrizeBox(PrizeBox *pPrizeBox)
{
	PrizeBoxList::iterator it = m_list.begin();

	while(it != m_list.end())
	{
#ifdef NTJ_SERVER
		if (*it && (*it) == pPrizeBox)
#endif
#ifdef NTJ_CLIENT
		PrizeBoxPickup *pPrizeBoxPickup = (*it);
		if (pPrizeBoxPickup && pPrizeBoxPickup->m_PrizeBox == pPrizeBox)
#endif
		{
#ifdef NTJ_CLIENT
			delete pPrizeBoxPickup;
#endif
			m_list.erase(it);		
			return;
		}
		else
		{
			++it;
		}
	}
}

void PrizeBoxManager::PushBack(PrizeBox *pPrizeBox)
{
#ifdef NTJ_SERVER
	if (pPrizeBox)
	{
		if (GetCount() + 1 > m_nMaxCount)
		{
			PopFront();
		}
		m_list.push_back(pPrizeBox);
	}
#endif

#ifdef NTJ_CLIENT
	PrizeBoxPickup *pPrizeBoxPickup = new PrizeBoxPickup(pPrizeBox);
	m_list.push_back(pPrizeBoxPickup);
#endif
}

void PrizeBoxManager::PopFront()
{
	PrizeBoxList::iterator it = m_list.begin();

	if (it != m_list.end() && *it)
	{
#ifdef NTJ_SERVER
		(*it)->deleteObject();
#endif
#ifdef NTJ_CLIENT
		(*it)->m_PrizeBox->deleteObject();
#endif
	}
}

#ifdef NTJ_CLIENT
PrizeBox* PrizeBoxManager::GetNearest(GameObject *pObj, F32 radius)
{
	PrizeBox *pPrizeBox = NULL;
	F32 fMinDistance = 10000.0f;
	F32 fTemp = 0.0f;

	PrizeBoxList::iterator it = m_list.begin();
	while(it != m_list.end())
	{
		if ((*it)->m_hadPicked)
		{
			it++;
			continue;
		}

		fTemp = pObj->getDistance((*it)->m_PrizeBox);
		if (fMinDistance > fTemp)
		{
			fMinDistance = fTemp;
			pPrizeBox = (*it)->m_PrizeBox;
		}
		++it;
	}

	if (fMinDistance > radius)
	{
		pPrizeBox = NULL;
	}

	return pPrizeBox;
}

void PrizeBoxManager::SetHadPicked(PrizeBox *pPrizeBox)
{
	if (!pPrizeBox)
		return;

	PrizeBoxList::iterator it = m_list.begin();
	while(it != m_list.end())
	{
		if ((*it) && (*it)->m_PrizeBox == pPrizeBox)
		{
			(*it)->m_hadPicked = true;
		}

		++it;
	}
}

void PrizeBoxManager::ClearHadPicked()
{
	PrizeBoxList::iterator it = m_list.begin();
	while(it != m_list.end())
	{
		if ((*it) && (*it)->m_PrizeBox)
		{
			(*it)->m_hadPicked = false;
		}

		++it;
	}
}
#endif
