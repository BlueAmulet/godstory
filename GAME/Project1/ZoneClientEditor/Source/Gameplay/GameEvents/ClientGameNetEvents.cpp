//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#include "common/PacketType.h"
#include "platform/platform.h"
#include "core/dnet.h"
#include "console/simBase.h"
#include "sim/netConnection.h"
#include "ClientGameNetEvents.h"
#include "T3D/gameConnection.h"

#ifdef NTJ_SERVER
#include "Gameplay/Social/ZoneSocial.h"
#include "mail/ZoneMail.h"
#endif

#include "windows.h"
#include "core/bitStream.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/Item/Res.h"
#include "Gameplay/GameEvents/ServerGameNetEvents.h"
#include "Gameplay/Item/Player_Item.h"
#include "Gameplay/Item/Player_EquipIdentify.h"
#include "Gameplay/Item/Player_GemMount.h"

#include "Gameplay/GameObjects/NpcObject.h"
#include "Gameplay/Mission/PlayerMission.h"
#include "Gameplay/ServerGameplayState.h"
#include "Gameplay/GameObjects/LivingSkill.h"
#include "Gameplay/Item/Player_Bank.h"
#include "Gameplay/GameObjects/CollectionObject.h"
#include "Gameplay/Item/Prescription.h"
#include "Gameplay/Item/ProduceEquip.h"
#include "Gameplay/GameObjects/ScheduleManager.h"
#include "Gameplay/GameEvents/SimCallBackEvents.h"
#include "Gameplay/GameObjects/TimerTrigger.h"
#include "Gameplay/Item/IndividualStall.h"
#include "Gameplay/Item/Player_ItemSplit.h"
#include "Gameplay/Item/Player_ItemCompose.h"
#include "Gameplay/Item/Player_SuperMarket.h"
#include "Gameplay/Data/SuperMarketData.h"
#include "T3D/trigger.h"

#ifdef NTJ_SERVER
#include "Common/LogHelper.h"
#include "Common/Log/LogTypes.h"
#include "Gameplay/GameObjects/PetOperationManager.h"
#include "Gameplay/GameObjects/PetMountOperation.h"
#include "Gameplay/GameObjects/SpiritOperationManager.h"
#endif

IMPLEMENT_CO_SERVEREVENT_V1(ClientGameNetEvent);

ClientGameNetEvent::ClientGameNetEvent(U32 info_type)
{
	Parent::m_InfoType = info_type;
}

void ClientGameNetEvent::process(NetConnection *pCon)
{
#ifdef NTJ_SERVER
	GameConnection* con = (GameConnection*)pCon;
	if(!con)
		return;

	Player* pPlayer = dynamic_cast<Player*>(con->getControlObject());
	if(!pPlayer)
		return;

	switch (m_InfoType)
	{
	case INFO_QUEST_TRIGGER_NPC:
		{
			CHECK_MSGCOUNT(INFO_QUEST_TRIGGER_NPC, 0,3,0)			// 检查消息参数是否有效
			//		0 - NPC的网络索引ID
			//		1 - 对话状态值state
			//		2 - 对话状态返回附加参数param
			NpcObject* npc = dynamic_cast<NpcObject*>(Sim::findObject(GetInt32Arg(0)));
			if(!npc)
				return;

			if(!pPlayer->canInteraction(npc))
				return;

			S32 state = GetInt32Arg(1);
			S32 param = GetInt32Arg(2);
			if(state == -1)
				pPlayer->setInteraction(NULL, Player::INTERACTION_NONE);
			else
			{
				pPlayer->setInteraction(npc, Player::INTERACTION_NPCTALK);
				Con::executef(npc, "OnTrigger", Con::getIntArg(pPlayer->getId()), Con::getIntArg(state), Con::getIntArg(param));
			}			
		}
		break;
	case INFO_QUEST_TRIGGER_OTHER:	// 处理其它方式触发任务的事件
		{
			CHECK_MSGCOUNT(INFO_QUEST_TRIGGER_OTHER, 0,5,0)
				Con::executef("OnTriggerMission", Con::getIntArg(pPlayer->getId()), 
				Con::getIntArg(GetInt32Arg(0)),
				Con::getIntArg(GetInt32Arg(1)),
				Con::getIntArg(GetInt32Arg(2)),
				Con::getIntArg(GetInt32Arg(3)),
				Con::getIntArg(GetInt32Arg(4)));
		}
		break;
	case INFO_SHORTCUT_EXCHANGE:
		{
			CHECK_MSGCOUNT(INFO_SHORTCUT_EXCHANGE, 4, 1, 0)
			ShortcutObject* pShortcutObj = g_ItemManager->getShortcutSlot(pPlayer,GetIntArg(0), GetIntArg(1));
			// 没有shortcut对象且技能ID为0 [6/29/2009 joy]
			if(!pShortcutObj && GetInt32Arg(0) == 0)
				return;
			
			if(pShortcutObj && pShortcutObj->isItemObject())
			{
				ItemShortcut* pItem = dynamic_cast<ItemShortcut*>(pShortcutObj);
				if(!pItem)
					return;

				stExChangeParam* stParam = new stExChangeParam;
				stParam->SrcType	= (enShortcutType)GetIntArg(0);
				stParam->SrcIndex	= GetIntArg(1);
				stParam->DestType	= (enShortcutType)GetIntArg(2);
				stParam->DestIndex	= GetIntArg(3);
				stParam->player		= pPlayer;
				stParam->SkillSeriesId = GetInt32Arg(0);

				if(GetIntArg(2) == SHORTCUTTYPE_STALL || GetIntArg(2) == SHORTCUTTYPE_STALL_BUYLIST)			// 摆摊栏
				{
					stParam->Price = GetInt32Arg(1);
				}
				else if(GetIntArg(2) == SHORTCUTTYPE_EQUIP && GetIntArg(3) == -1)		// 装备栏
				{
					// 智能获取物品能装备到的位置
					stParam->DestIndex = g_ItemManager->getItemEquipPos(pPlayer, pItem);
				}

				enWarnMessage msgCode = g_ItemManager->ExchangeShortcut(stParam);
				delete stParam;
				if(msgCode != MSG_NONE)
					MessageEvent::send(con, SHOWTYPE_ERROR, msgCode, SHOWPOS_SCREEN);

			}
			else if (pShortcutObj && pShortcutObj->isPetObject())
			{
				if (GetIntArg(2) == SHORTCUTTYPE_PET || GetIntArg(2) == SHORTCUTTYPE_PANEL || 
					GetIntArg(2) == SHORTCUTTYPE_STALL_PET || GetIntArg(0) == SHORTCUTTYPE_STALL_PET ||
					GetIntArg(2) == SHORTCUTTYPE_TEMP )
				{
					stExChangeParam* stParam = new stExChangeParam;
					stParam->SrcType = (enShortcutType)GetIntArg(0);
					stParam->SrcIndex = GetIntArg(1);

					stParam->DestType = (enShortcutType)GetIntArg(2);
					stParam->DestIndex = GetIntArg(3);
					stParam->player = pPlayer;
					stParam->SkillSeriesId = GetInt32Arg(0);
					if((enShortcutType)GetIntArg(2) == SHORTCUTTYPE_STALL_PET)
					{
						stParam->Price = GetInt32Arg(1);
					}
					else 
						stParam->Price = 0;
					enWarnMessage msgCode = g_ItemManager->ExchangeShortcut(stParam);
					delete stParam;
					if(msgCode != MSG_NONE)
						MessageEvent::send(con, SHOWTYPE_ERROR, msgCode, SHOWPOS_SCREEN);
				}
			}
			else if (pShortcutObj && pShortcutObj->isMountObject())
			{
				if (GetIntArg(2) == SHORTCUTTYPE_MOUNT_PET_LIST || GetIntArg(2) == SHORTCUTTYPE_PANEL)
				{
					stExChangeParam* stParam = new stExChangeParam;
					stParam->SrcType = (enShortcutType)GetIntArg(0);
					stParam->SrcIndex = GetIntArg(1);

					stParam->DestType = (enShortcutType)GetIntArg(2);
					stParam->DestIndex = GetIntArg(3);
					stParam->player = pPlayer;
					stParam->SkillSeriesId = GetInt32Arg(0);
					enWarnMessage msgCode = g_ItemManager->ExchangeShortcut(stParam);
					delete stParam;
					if(msgCode != MSG_NONE)
						MessageEvent::send(con, SHOWTYPE_ERROR, msgCode, SHOWPOS_SCREEN);
				}
			}
			else if((enShortcutType)GetIntArg(2) == SHORTCUTTYPE_PANEL || (enShortcutType)GetIntArg(2) == SHORTCUTTYPE_NONE)
			{
				if((enShortcutType)GetIntArg(0) == SHORTCUTTYPE_SKILL_COMMON || (enShortcutType)GetIntArg(0) == SHORTCUTTYPE_PANEL
					||(enShortcutType)GetIntArg(0) == SHORTCUTTYPE_LIVINGCATEGORY || (enShortcutType)GetIntArg(0) == SHORTCUTTYPE_SKILL_HINT)
				{
					stExChangeParam* stParam = new stExChangeParam;
					stParam->SrcType = (enShortcutType)GetIntArg(0);
					stParam->SrcIndex = GetIntArg(1);
					stParam->DestType = (enShortcutType)GetIntArg(2);
					stParam->DestIndex=GetIntArg(3);
					stParam->player = pPlayer;
					stParam->Price = 0;
					stParam->SkillSeriesId = GetInt32Arg(0);
					enWarnMessage msgCode = g_ItemManager->ExchangeShortcut(stParam);
					delete stParam;
					if(msgCode != MSG_NONE)
						MessageEvent::send(con, SHOWTYPE_ERROR, msgCode, SHOWPOS_SCREEN);
				}
			}			
		}
		break;
	case INFO_SET_TARGET:
		{
			CHECK_MSGCOUNT(INFO_SET_TARGET, 0, 2, 0)
			GameObject* obj = dynamic_cast<GameObject*>(Sim::findObject(GetInt32Arg(0)));
			bool atk = GetInt32Arg(1);
			if(obj)
				pPlayer->SetTarget(obj,atk);
		}
		break;
	case INFO_SET_AUTOCAST:
		{
			CHECK_MSGCOUNT(INFO_SET_AUTOCAST, 1, 0, 0)
			pPlayer->SetAutoCastEnable((bool)GetIntArg(0) && pPlayer->GetTarget());
		}
		break;
	case INFO_BREAKCAST:
		{
			CHECK_MSGCOUNT(INFO_BREAKCAST, 0, 0, 0)
			pPlayer->BreakCast();
		}
		break;
	case INFO_NPCSHOP:		// NPC系统商店交易
		{
			CHECK_MSGCOUNT(INFO_NPCSHOP, 1, 0, 0);
			U32 mType = GetIntArg(0);
			switch(mType)
			{
			case NPCSHOP_OPEN:		// 打开NPC系统商店
				{
					// 不处理，因为这项操作由服务端NPC对话脚本做了
				}
				break;
			case NPCSHOP_CLOSE:		// 关闭NPC系统商店
				{
					pPlayer->setInteraction(NULL, Player::INTERACTION_NONE);
				}
				break;
			case NPCSHOP_BUY:		// 从NPC商店购买物品
				{
					CHECK_MSGCOUNT(INFO_NPCSHOP, 4, 0, 0);
					enWarnMessage msgCode = g_ItemManager->buyItem(pPlayer, GetIntArg(1), GetIntArg(2), GetIntArg(3));
					if(msgCode != MSG_NONE)
						MessageEvent::send(con, SHOWTYPE_ERROR, msgCode, SHOWPOS_SCREEN);
				}
				break;
			case NPCSHOP_SELL:		// 卖物品给NPC(放入玩家回购栏)
				{
					CHECK_MSGCOUNT(INFO_NPCSHOP, 2, 0, 0);
					g_ItemManager->sellItem(pPlayer, GetIntArg(1));
				}
				break;
			case NPCSHOP_RECYCLE:	// 回购物品
				{
					CHECK_MSGCOUNT(INFO_NPCSHOP, 2, 0, 0);
					g_ItemManager->reBuyItem(pPlayer, GetIntArg(1));
				}
				break;
			case NPCSHOP_PAGE:		// 商品浏览翻页
				{
					CHECK_MSGCOUNT(INFO_NPCSHOP, 2, 0, 0);
					pPlayer->npcShopList.SetQueryTime(0);
					g_ItemManager->setNpcShopPage(pPlayer, GetIntArg(1));
				}
				break;
			case NPCSHOP_FILTER:
				{
					pPlayer->npcShopList.setFilterCase(GetIntArg(2));
					pPlayer->npcShopList.SetQueryTime(0);
					g_ItemManager->setNpcShopPage(pPlayer, GetIntArg(1));
				}
				break;
			}
			break;
		}
	case INFO_RISE_REQUEST:
		{
			CHECK_MSGCOUNT(INFO_RISE_REQUEST, 1, 0, 0);
			switch(GetIntArg(0))
			{
			case Player::RISE_ONTHESPOT:
				{
					// 条件检查
					pPlayer->setDamageState(ShapeBase::Enabled);
					pPlayer->addHP(S32_MAX);
				}
				break;
			case Player::RISE_SPELL:
				{
					// 条件检查
				}
				break;
			case Player::RISE_LOWERWORLD:
				{
					// 传送至复活点					
					TriggerData*  pData = g_TriggerDataManager->getTriggerData(pPlayer->getTriggerId());
					if (pData)
					{
						if(pData->mReluvePos && pData->mReluvePos[0])
						{
							Point3F pos;
							dSscanf(pData->mReluvePos, "%f %f %f", &pos.x, &pos.y, &pos.z);

							pPlayer->setDamageState(ShapeBase::Enabled);
							pPlayer->addHP(S32(pPlayer->GetStats().MaxHP*0.15f));
							pPlayer->addMP(S32(pPlayer->GetStats().MaxMP*0.1f));
							pPlayer->transportObject(g_ServerGameplayState->getCurrentLineId(), pPlayer->getTriggerId(), pData->mZoneID, pos.x, pos.y, pos.z);
							
						}
					}
				}
				break;
			}
		}
		break;
	case INFO_ITEM_DROP: // 扔物品
		{
			CHECK_MSGCOUNT(INFO_ITEM_DROP, 2, 0, 0);
			S32 index = GetIntArg(1);
			ShortcutObject* pShortcut_slot = g_ItemManager->getShortcutSlot(pPlayer, GetIntArg(0), index);
			if(!pShortcut_slot)
				return;
			GameConnection* conn = pPlayer->getControllingClient();
			if(!conn)
				return;
			if((enShortcutType)GetIntArg(0) == SHORTCUTTYPE_PANEL)
			{
				pPlayer->panelList.SetSlot(index, NULL, true);
				pPlayer->panelList.UpdateToClient(conn, index, ITEM_NOSHOW);
			}
			else if((enShortcutType)GetIntArg(0) == SHORTCUTTYPE_INVENTORY)
			{
				ItemShortcut* pItem = dynamic_cast<ItemShortcut*>(pShortcut_slot);
				if(!pItem)
					return;
				if(!pItem->getRes()->canBaseLimit(Res::ITEM_DROP))
					return;
				U32 itemID = pItem->getRes()->getItemID();
				pPlayer->inventoryList.SetSlot(index, NULL, true);
				pPlayer->inventoryList.UpdateToClient(conn, index, ITEM_DROP);
				g_ItemManager->syncPanel(pPlayer, itemID);
				g_MissionManager->UpdateTaskItem(pPlayer, itemID);
			}
			else if((enShortcutType)GetIntArg(0) == SHORTCUTTYPE_IDENTIFY)
			{
				U32 iSrcIndex = pShortcut_slot->getLockedItemIndex();
				pPlayer->identifylist.SetSlot(index, NULL, true);
				pPlayer->identifylist.UpdateToClient(conn, index, ITEM_NOSHOW);
				pPlayer->inventoryList.GetSlot(iSrcIndex)->setSlotState(ShortcutObject::SLOT_COMMON);
				pPlayer->inventoryList.UpdateToClient(conn, iSrcIndex, ITEM_NOSHOW);
			}
			else if((enShortcutType)GetIntArg(0) == SHORTCUTTYPE_STRENGTHEN)
			{				
				U32 iSrcIndex = pShortcut_slot->getLockedItemIndex();
				pPlayer->strengthenlist.SetSlot(index, NULL, true);
				pPlayer->strengthenlist.UpdateToClient(conn, index, ITEM_NOSHOW);
				pPlayer->inventoryList.GetSlot(iSrcIndex)->setSlotState(ShortcutObject::SLOT_COMMON);
				pPlayer->inventoryList.UpdateToClient(conn, iSrcIndex, ITEM_NOSHOW);
			}
			else if((enShortcutType)GetIntArg(0) == SHORTCUTTYPE_EQUIP_PUNCHHOLE)
			{				
				U32 iSrcIndex = pShortcut_slot->getLockedItemIndex();
				pPlayer->punchHoleList.SetSlot(index, NULL, true);
				pPlayer->punchHoleList.UpdateToClient(conn, index, ITEM_NOSHOW);
				pPlayer->inventoryList.GetSlot(iSrcIndex)->setSlotState(ShortcutObject::SLOT_COMMON);
				pPlayer->inventoryList.UpdateToClient(conn, iSrcIndex, ITEM_NOSHOW);
			}
		}
		break;
	case INFO_ITEM_COMPACT:
		{
			CHECK_MSGCOUNT(INFO_ITEM_COMPACT, 0, 0, 0);
			pPlayer->inventoryList.CompactSlots();
			for(int i=0; i<pPlayer->inventoryList.GetMaxSlots(); ++i)
			{
				pPlayer->inventoryList.UpdateToClient(con, i, ITEM_NOSHOW);
			}
		}
		break;
	case INFO_ITEM_SPLIT:
		{
			CHECK_MSGCOUNT(INFO_ITEM_SPLIT, 5, 0, 0);
			S32 src_index = GetIntArg(1);
			S32 des_index = GetIntArg(4);
			S32 iNum = GetIntArg(2);
			if(src_index < 0 || (src_index >= pPlayer->inventoryList.GetMaxSlots()) || des_index < 0 || (des_index >= pPlayer->inventoryList.GetMaxSlots()))
				return;
			if(GetIntArg(0) != GetIntArg(3))
				return;
			ShortcutObject* pSrc = g_ItemManager->getShortcutSlot(pPlayer, GetIntArg(0), src_index);
			ShortcutObject* pDest = g_ItemManager->getShortcutSlot(pPlayer, GetIntArg(3), des_index);

			if(!pSrc)
				return;
			ItemShortcut* pSrcItem = dynamic_cast<ItemShortcut*>(pSrc);
			S32 iQuantity = pSrcItem->getRes()->getQuantity();
			if(!pSrcItem || iQuantity <= iNum)
				return;
			//目标有物品
			if(pDest)
			{
				if(g_ItemManager->isSameItem(pSrc, pDest, false))
				{
					ItemShortcut* pDestItem = dynamic_cast<ItemShortcut*>(pDest);
					Res* pRes = NULL;
					if(pDestItem && (pRes = pDestItem->getRes()) && (pRes->getMaxOverNum() >= pRes->getQuantity() + iNum))
					{
						pSrcItem->getRes()->setQuantity(iQuantity - iNum);
						pDestItem->getRes()->setQuantity(pRes->getQuantity() + iNum);
						pPlayer->inventoryList.UpdateToClient(pPlayer->getControllingClient(), src_index, ITEM_NOSHOW);
						pPlayer->inventoryList.UpdateToClient(pPlayer->getControllingClient(), des_index, ITEM_NOSHOW);
					}
				}
				else
					return;
			}
			//目标位置没有物品
			else
			{
				Res* pSrcRes = pSrcItem->getRes();
				if(!pSrcRes)
					return;
				pSrcRes->setQuantity(iQuantity - iNum);
				ItemShortcut* pTarget = ItemShortcut::CreateItem(pSrcRes->getItemID(), iNum);
				//g_ItemManager->syncPanel(pPlayer, pSrcRes->getItemID());
				if(pTarget)
				{
					pPlayer->inventoryList.SetSlot(des_index, pTarget, false);
				}
				pPlayer->inventoryList.UpdateToClient(pPlayer->getControllingClient(), src_index, ITEM_NOSHOW);
				pPlayer->inventoryList.UpdateToClient(pPlayer->getControllingClient(), des_index, ITEM_LOAD);
			}
		}
		break;
	case INFO_MISSION:		//任务请求处理
		{
			CHECK_MSGCOUNT(INFO_MISSION, 2, 0, 0);
			U32 type = (U32)GetIntArg(0);
			S32 MissionID = (S32)GetIntArg(1);
			if(type == MissionManager::MISSIONREQUEST_ACCEPT)
			{
				//服务端脚本检查玩家是否能接此任务
				const char* ret = Con::executef("CanDoThisMission",
									Con::getIntArg(pPlayer->getId()),
									Con::getIntArg(MissionID),
									Con::getIntArg(1),
									Con::getIntArg(1),
									Con::getIntArg(1));
				if(ret && ret[0] == 0)
					Con::executef("AddMissionAccepted", Con::getIntArg(pPlayer->getId()), Con::getIntArg(MissionID));
			}
			else if(type == MissionManager::MISSIONREQUEST_DELETE)
			{
				Con::executef("OnTriggerMission",
							Con::getIntArg(pPlayer->getId()),
							Con::getIntArg(MissionID),
							Con::getIntArg(-2),
							Con::getIntArg(0),
							Con::getIntArg(0),
							Con::getIntArg(0));
			}
			else if(type == MissionManager::MISSIONREQUEST_SHARE)
			{
				S32 idpos = -1;
				g_MissionManager->ShareMission(pPlayer, MissionID);
			}
			else if(type == MissionManager::MISSIONREQUEST_SHAREGET)
			{
				g_MissionManager->ShareMissionResponse(pPlayer);
			}
		}
		break;
	case INFO_ITEM_REPAIR:
		{
			CHECK_MSGCOUNT(INFO_ITEM_REPAIR, 0, 2, 0);
			switch(GetInt32Arg(0))
			{
			case NPCSHOP_REPAIRSIGLE:		//装备单修
				{
					S32 iSlotType = GetInt32Arg(1);
					S32 iSlotCol  = GetInt32Arg(2);
					U32 iMoney = 0;
					enWarnMessage msgCode = MSG_UNKOWNERROR;
					if(iSlotType == SHORTCUTTYPE_EQUIP)
					{
						ItemShortcut* pItem = (ItemShortcut*)pPlayer->equipList.GetSlot(iSlotCol);
						if(pItem)
						{
							iMoney = pItem->getRes()->getRepairNeedMoney();
							if(!pPlayer->canReduceMoney(iMoney, 1))
							{
								MessageEvent::send(con, SHOWTYPE_ERROR, MSG_PLAYER_MONEYNOTENOUGH);
								return;
							}	
							// 装备栏物品单修
							msgCode = g_ItemManager->repairOneEquip(pPlayer, Res::REPAIR_NORMAL, iSlotType, iSlotCol);
						}						
					}
					else if(iSlotType == SHORTCUTTYPE_INVENTORY)
					{
						ItemShortcut* pItem = (ItemShortcut*)pPlayer->inventoryList.GetSlot(iSlotCol);
						if(pItem)
						{
							iMoney = pItem->getRes()->getRepairNeedMoney();
							if(!pPlayer->canReduceMoney(iMoney, 1))
							{
								MessageEvent::send(con, SHOWTYPE_ERROR, MSG_PLAYER_MONEYNOTENOUGH);
								return;
							}
							// 物品栏物品单修
							msgCode = g_ItemManager->repairOneEquip(pPlayer, Res::REPAIR_NORMAL, iSlotType, iSlotCol);
						}
					}

					if(msgCode != MSG_NONE)
					{
						MessageEvent::send(con, SHOWTYPE_ERROR, msgCode);
						return;
					}
					else
					{
						pPlayer->reduceMoney(iMoney, 1);
					}
				}
				break;
			case NPCSHOP_REPAIRALL:			//装备全修
				{
					U32 iCost = pPlayer->equipList.GetRepairCost();
					if(!pPlayer->canReduceMoney(iCost, 1))
					{
						MessageEvent::send(con, SHOWTYPE_ERROR, MSG_PLAYER_MONEYNOTENOUGH);
						return;
					}
					enWarnMessage msgCode = g_ItemManager->repairAllEquip(pPlayer);
					if(msgCode != MSG_NONE)
					{
						pPlayer->reduceMoney(iCost, 1);
					}					
				}
				break;
			case NPCSHOP_REPAIRFUCTION:
				{

				}
				break;
			case DURABLE_REPAIR:		//经久修理
				{
					ShortcutObject* pShortcut = pPlayer->mRepairList.GetSlot(0);
					ItemShortcut* pItem = dynamic_cast<ItemShortcut*>(pShortcut);
					if(!pItem)
					{
						MessageEvent::send(con, SHOWTYPE_ERROR, MSG_ITEM_ITEMDATAERROR);
						return;
					}
					U32 iMoney = pItem->getRes()->getRepairNeedMoney();
					if(!pPlayer->canReduceMoney(iMoney, 1))
					{
						MessageEvent::send(con, SHOWTYPE_ERROR, MSG_PLAYER_MONEYNOTENOUGH);
						return;
					}
					//查找经久强化装备所需特殊修理道具
					S32 index = pPlayer->inventoryList.FindSameObjectByID(ItemManager::REPAIRCOST_ITEM, true);
					if(index < 0)
					{
						MessageEvent::send(con, SHOWTYPE_ERROR, MSG_ITEM_ITEMDATAERROR);
						return;
					}
					enWarnMessage msgCode = g_ItemManager->repairOneEquip(pPlayer, Res::REPAIR_DURABLE, SHORTCUTTYPE_REPAIR);
					if(msgCode == MSG_NONE)
					{
						pPlayer->reduceMoney(iMoney, 1);
						g_ItemManager->delItemFromInventory(pPlayer, ItemManager::REPAIRCOST_ITEM, 1);
					}
					else
					{
						MessageEvent::send(con, SHOWTYPE_ERROR, msgCode);
						return;
					}
				}
				break;
			case RECOVER_REPAIR:		//恢复修理
				{
					ShortcutObject* pShortcut = pPlayer->mRepairList.GetSlot(0);
					ItemShortcut* pItem = dynamic_cast<ItemShortcut*>(pShortcut);
					if(!pItem)
					{
						MessageEvent::send(con, SHOWTYPE_ERROR, MSG_ITEM_ITEMDATAERROR);
						return;
					}
					U32 iMoney = pItem->getRes()->getRepairNeedMoney() + pItem->getRes()->getRepairNeedMoney1();
					if(!pPlayer->canReduceMoney(iMoney, 1))
					{
						MessageEvent::send(con, SHOWTYPE_ERROR, MSG_PLAYER_MONEYNOTENOUGH);
						return;
					}
					enWarnMessage msgCode = g_ItemManager->repairOneEquip(pPlayer, Res::REPAIR_RECOVER, SHORTCUTTYPE_REPAIR);
					if(msgCode == MSG_NONE)
					{
						pPlayer->reduceMoney(iMoney, 1);
					}
					else
					{
						MessageEvent::send(con, SHOWTYPE_ERROR, msgCode);
						return;
					}
				}
				break;
			case REPAIR_CLOSE:
				{
					pPlayer->setInteraction(NULL, Player::INTERACTION_NONE);
				}
				break;
			default:
				break;
			}
		}
		break;
	case INFO_LEVEL_UP:
		{
			CHECK_MSGCOUNT(INFO_LEVEL_UP, 0, 0, 0);
			if(pPlayer->getLevel() < EnablePlayerLevel && pPlayer->getExp() >= g_LevelExp[pPlayer->getLevel()])
			{
				pPlayer->addExp(-g_LevelExp[pPlayer->getLevel()]);
				pPlayer->addLevel(1);
			}
		}
		break;
	case INFO_TRADE:		//玩家间交易
		{
			CHECK_MSGCOUNT(INFO_TRADE, 0, 1, 0);
			U32 TradeState = GetInt32Arg(0);	// 交易进行中状态
			switch(TradeState)
			{
			case TRADE_ASKFOR:		// 发起交易请求
				{
					CHECK_MSGCOUNT(INFO_TRADE, 0, 2, 0);
					Player* targetPlayer = g_ServerGameplayState->GetPlayer(GetInt32Arg(1));
					if(!targetPlayer)
					{
						MessageEvent::send(pPlayer->getControllingClient(), SHOWTYPE_ERROR, MSG_TRADE_LOSTTARGET);
						return;
					}

					enWarnMessage msgCode = g_ItemManager->canTrade(pPlayer, targetPlayer);
					if(msgCode != MSG_NONE)
					{
						MessageEvent::send(pPlayer->getControllingClient(),SHOWTYPE_ERROR, msgCode);
						return;
					}

					// 清除自己的交易栏数据
					pPlayer->tradeList.Reset();

					// 添加一个交易邀请逻辑延迟事件
					Player::stLogicEvent* logicEvent = new Player::stLogicEvent;
					logicEvent->type		= Player::LOGICEVENT_TRADEINVITE;
					logicEvent->sender		= pPlayer->getPlayerID();
					logicEvent->senderName	= pPlayer->getPlayerName();
					logicEvent->data		= NULL;
					logicEvent->datalen		= 0;
					logicEvent->relationid	= 0;
					U32 eid = targetPlayer->insertLogicEvent(logicEvent);
					//向目标玩家发送交易请求
					if(targetPlayer->getControllingClient())
					{
						ServerGameNetEvent* e = new ServerGameNetEvent(INFO_TRADE);
						e->SetInt32ArgValues(3,	TRADE_ASKFOR, pPlayer->getPlayerID(), eid);
						targetPlayer->getControllingClient()->postNetEvent(e);
					}			
				}
				break;
			case TRADE_ANSWER:		//回复邀请
				{
					CHECK_MSGCOUNT(INFO_TRADE, 0, 3, 0);
					Player* targetPlayer = g_ServerGameplayState->GetPlayer(GetInt32Arg(1));
					Player::stLogicEvent* ev = pPlayer->findLogicEvent(GetInt32Arg(2));
					if(ev == NULL || Platform::getVirtualMilliseconds() - ev->start > Player::MAX_LOGIC_REPLYTIME)
					{
						MessageEvent::send(pPlayer->getControllingClient(),SHOWTYPE_ERROR, MSG_TRADE_INVITE_TIMEOUT);
						return;
					}

					pPlayer->removeLogicEvent(GetInt32Arg(2));

					if(!targetPlayer)
					{
						MessageEvent::send(pPlayer->getControllingClient(),SHOWTYPE_ERROR, MSG_TRADE_LOSTTARGET);
						return;
					}

					enWarnMessage msgCode = g_ItemManager->canTrade(pPlayer, targetPlayer);
					if(msgCode != MSG_NONE)
					{
						MessageEvent::send(pPlayer->getControllingClient(),SHOWTYPE_ERROR, msgCode);
						return;
					}

					pPlayer->setInteraction(targetPlayer, Player::INTERACTION_PLAYERTRADE);
					targetPlayer->setInteraction(pPlayer, Player::INTERACTION_PLAYERTRADE);

					pPlayer->tradeList.SetTradeState(TRADE_BEGIN);
					targetPlayer->tradeList.SetTradeState(TRADE_BEGIN);
					pPlayer->tradeList.Reset();
					targetPlayer->tradeList.Reset();

					if(targetPlayer->getControllingClient())
					{
						ServerGameNetEvent* e = new ServerGameNetEvent(INFO_TRADE);
						e->SetInt32ArgValues(2,	TRADE_ANSWER, pPlayer->getPlayerID());
						targetPlayer->getControllingClient()->postNetEvent(e);
					}

					if(pPlayer->getControllingClient())
					{
						ServerGameNetEvent* e = new ServerGameNetEvent(INFO_TRADE);
						e->SetInt32ArgValues(2,	TRADE_ANSWER, targetPlayer->getPlayerID());
						pPlayer->getControllingClient()->postNetEvent(e);
					}					
				}
				break;
			case TRADE_REJECT:		//拒绝邀请
				{
					CHECK_MSGCOUNT(INFO_TRADE, 0, 3, 0);
					Player* targetPlayer = g_ServerGameplayState->GetPlayer(GetInt32Arg(1));
					Player::stLogicEvent* ev = pPlayer->findLogicEvent(GetInt32Arg(2));
					if(ev == NULL || Platform::getVirtualMilliseconds() - ev->start > Player::MAX_LOGIC_REPLYTIME)
					{
						MessageEvent::send(pPlayer->getControllingClient(),SHOWTYPE_ERROR, MSG_TRADE_INVITE_TIMEOUT);
						return;
					}

					pPlayer->removeLogicEvent(GetInt32Arg(2));

					if(!targetPlayer)
					{
						MessageEvent::send(pPlayer->getControllingClient(),SHOWTYPE_ERROR, MSG_PLAYER_TARGET_LOST);
						return;
					}

					enWarnMessage msgCode = g_ItemManager->canTrade(pPlayer, targetPlayer);
					if(msgCode != MSG_NONE)
					{
						MessageEvent::send(pPlayer->getControllingClient(),SHOWTYPE_ERROR, msgCode);
						return;
					}

					pPlayer->tradeList.SetTradeState(TRADE_NONE);
					targetPlayer->tradeList.SetTradeState(TRADE_NONE);
					pPlayer->tradeList.Reset();
					targetPlayer->tradeList.Reset();
					
					if(targetPlayer->getControllingClient())
					{
						ServerGameNetEvent* evn = new ServerGameNetEvent(INFO_TRADE);
						evn->SetInt32ArgValues(2, TRADE_REJECT, pPlayer->getPlayerID());
						targetPlayer->getControllingClient()->postNetEvent(evn);
					}
				}
				break;
			case TRADE_UPITEM:		// 交易物品上架
				{
					CHECK_MSGCOUNT(INFO_TRADE, 0, 2, 0);
					U32 SlotIndex = GetInt32Arg(1);
					SceneObject* pTargetObject = pPlayer->getInteraction();
					Player* pTarget = NULL;
					if(!pTargetObject || !(pTarget = dynamic_cast<Player*>(pTargetObject)))
					{
						pPlayer->setInteraction(NULL, Player::INTERACTION_NONE);
						MessageEvent::send(pPlayer->getControllingClient(),SHOWTYPE_ERROR, MSG_TRADE_LOSTTARGET);
						return;
					}

					// 注：在任何一方有“确定”交易后，加个保护措施，不能操作物品
					if(pPlayer->tradeList.GetTradeState() == TRADE_END || pTarget->tradeList.GetTradeState() == TRADE_END)
						return;

					// 判断是否已锁定交易，是则先解除锁定
					enWarnMessage msg = g_ItemManager->unLockPlayerTrade(pPlayer);
					if(msg != MSG_NONE)
					{
						pPlayer->setInteraction(NULL, Player::INTERACTION_NONE);
						MessageEvent::send(pPlayer->getControllingClient(),SHOWTYPE_ERROR, msg);
						return;
					}
					msg = g_ItemManager->addItemToTrade(pPlayer,SlotIndex);
					if (msg != MSG_NONE)
					{
						pPlayer->setInteraction(NULL, Player::INTERACTION_NONE);
						MessageEvent::send(pPlayer->getControllingClient(),SHOWTYPE_ERROR, msg);
						return;
					}
				}
				break;
			case TRADE_DOWNITEM:	// 交易物品下架
				{
					CHECK_MSGCOUNT(INFO_TRADE, 0, 2, 0);
					U32 SlotIndex = GetInt32Arg(1);

					SceneObject* pTargetObject = pPlayer->getInteraction();
					Player* pTarget = NULL;
					if(!pTargetObject || !(pTarget = dynamic_cast<Player*>(pTargetObject)))
					{
						pPlayer->setInteraction(NULL, Player::INTERACTION_NONE);
						MessageEvent::send(pPlayer->getControllingClient(),SHOWTYPE_ERROR, MSG_TRADE_LOSTTARGET);
						return;
					}

					// 注：在任何一方有“确定”交易后，加个保护措施，不能操作物品
					if(pPlayer->tradeList.GetTradeState() == TRADE_END || pTarget->tradeList.GetTradeState() == TRADE_END)
						return;
					
					// 判断是否已锁定交易，是则先解除锁定
					enWarnMessage msg = g_ItemManager->unLockPlayerTrade(pPlayer);
					if(msg != MSG_NONE)
					{
						pPlayer->setInteraction(NULL, Player::INTERACTION_NONE);
						MessageEvent::send(pPlayer->getControllingClient(),SHOWTYPE_ERROR, msg);
						return;
					}

					msg = g_ItemManager->delItemFromTrade(pPlayer,SlotIndex);
					if (msg != MSG_NONE)
					{
						pPlayer->setInteraction(NULL, Player::INTERACTION_NONE);
						MessageEvent::send(pPlayer->getControllingClient(),SHOWTYPE_ERROR, msg);
						return;
					}
				}
				break;
			case TRADE_LOCK:		// 交易物品锁定
				{
					CHECK_MSGCOUNT(INFO_TRADE, 0, 2, 0);
					U32 TradeMoney = GetInt32Arg(1);
					if(pPlayer->tradeList.GetTradeState() == TRADE_BEGIN)
					{
						if(pPlayer->getMoney() < TradeMoney)
						{
							 MessageEvent::send(pPlayer->getControllingClient(),SHOWTYPE_ERROR, MSG_PLAYER_MONEYNOTENOUGH);
							 return;
						}
						pPlayer->tradeList.SetTradeState(TRADE_LOCK);
						pPlayer->tradeList.SetTradeMoney(TradeMoney);
						SceneObject* pTargetObject = pPlayer->getInteraction();
						Player* pTarget = NULL;
						if(!pTargetObject || !(pTarget = dynamic_cast<Player*>(pTargetObject)))
						{
							pPlayer->setInteraction(NULL, Player::INTERACTION_NONE);
							MessageEvent::send(pPlayer->getControllingClient(),SHOWTYPE_ERROR, MSG_TRADE_LOSTTARGET);
							return;
						}

						// 判定对方是否也锁定
						S32 iAlsoLock = (pTarget->tradeList.GetTradeState() == TRADE_LOCK)? 1 : 0;
						if(pTarget->getControllingClient())
						{
							ServerGameNetEvent* e = new ServerGameNetEvent(INFO_TRADE);
							e->SetInt32ArgValues(2,	TRADE_LOCKNOTIFY, TradeMoney);
							pTarget->getControllingClient()->postNetEvent(e);
						}

						if(pPlayer->getControllingClient())
						{
							ServerGameNetEvent* e = new ServerGameNetEvent(INFO_TRADE);
							e->SetInt32ArgValues(2,	TRADE_LOCK, iAlsoLock);
							pPlayer->getControllingClient()->postNetEvent(e);
						}
					}
				}
				break;
			case TRADE_END:		// 确定交易
				{
					CHECK_MSGCOUNT(INFO_TRADE, 0, 1, 0);
					SceneObject* pTargetObject = pPlayer->getInteraction();
					Player* pTarget = NULL;
					if(!pTargetObject || !(pTarget = dynamic_cast<Player*>(pTargetObject)))
					{
						pPlayer->setInteraction(NULL, Player::INTERACTION_NONE);
						MessageEvent::send(pPlayer->getControllingClient(),SHOWTYPE_ERROR, MSG_TRADE_LOSTTARGET);
						return;
					}
					if(pPlayer->tradeList.GetTradeState() == TRADE_LOCK &&
						(pTarget->tradeList.GetTradeState() == TRADE_LOCK ||
						pTarget->tradeList.GetTradeState() == TRADE_END))
					{
						pPlayer->tradeList.SetTradeState(TRADE_END);
						// 判定对方是否已经点击了确定交易
						if(pTarget->tradeList.GetTradeState() == TRADE_LOCK)
						{							
							if(pPlayer->getControllingClient())
							{
								ServerGameNetEvent* e = new ServerGameNetEvent(INFO_TRADE);
								e->SetInt32ArgValues(1,	TRADE_END);
								pPlayer->getControllingClient()->postNetEvent(e);
							}
							if(pTarget->getControllingClient())
							{
								ServerGameNetEvent* e = new ServerGameNetEvent(INFO_TRADE);
								e->SetInt32ArgValues(1,	TRADE_ENDNOTIFY);
								pTarget->getControllingClient()->postNetEvent(e);
							}
						}
						else
						{
							enWarnMessage msg = g_ItemManager->exchangeItemForTrade(pPlayer, pTarget);
							if(msg != MSG_NONE)
							{
								MessageEvent::send(pPlayer->getControllingClient(),SHOWTYPE_ERROR, msg);
								return;
							}
						}
					}
				}
				break;
			case TRADE_UNLOCK:		// 解除自己锁定交易
				{
					if(pPlayer->getInteractionState() == Player::INTERACTION_PLAYERTRADE)
					{
						enWarnMessage msg = g_ItemManager->unLockPlayerTrade(pPlayer);
						if(msg != MSG_NONE)
							MessageEvent::send(pPlayer->getControllingClient(),SHOWTYPE_ERROR, msg);
					}
				}
				break;
			case TRADE_CANCEL:		// 取消交易
				{
					pPlayer->setInteraction(NULL, Player::INTERACTION_NONE);
					MessageEvent::send(pPlayer->getControllingClient(),SHOWTYPE_ERROR, MSG_TRADE_CANCEL);
				}
				break;
			default:
				break;
			}
		}
		break;
	case INFO_SWITCH:		// 设置开关状态
		{
			CHECK_MSGCOUNT(INFO_SWITCH, 2, 0, 0);
			pPlayer->setSwitchState(GetIntArg(0), GetIntArg(1));
		}
		break;
	case INFO_UPDATEOTHERFLAG:
		{
			//收到此消息，证明新手标志需要清除
			Con::executef("NewbieInit", Con::getIntArg(pPlayer->getId()));
			pPlayer->clearOtherFlag(Player::OTHERFLAG_NEWBIE);
		}
		break;
	case INFO_ITEM_USE:
		{
			CHECK_MSGCOUNT(INFO_ITEM_USE, 2, 0, 0);
			S32	iType = GetIntArg(0);
			S32	iIndex = GetIntArg(1);
			ShortcutObject* pShort = g_ItemManager->getShortcutSlot(pPlayer, iType, iIndex);
			if(!pShort)
				return;
			GameObject* pGameObject = pPlayer->GetTarget();
			if(!pGameObject)
				g_ItemManager->useItem(pPlayer, iType, iIndex, pPlayer);
			else
				g_ItemManager->useItem(pPlayer, iType, iIndex, pGameObject);

		}
		break;
	case INFO_TRANSPORT:
		{
			CHECK_MSGCOUNT(INFO_TRANSPORT,5,0,0);
			pPlayer->transportObject(g_ServerGameplayState->getCurrentLineId(),GetIntArg(0),GetIntArg(1),GetIntArg(2),GetIntArg(3),GetIntArg(4));
		}
		break;
	case INFO_ITEM_IDENTIFY:
		{
			CHECK_MSGCOUNT(INFO_ITEM_IDENTIFY, 5, 0, 0);
			U32 iType = GetIntArg(0);
			U32 iType1= GetIntArg(1);
			U32 iIndex1 = GetIntArg(2);
			U32 iType2 = GetIntArg(3);
			U32 iIndex2 = GetIntArg(4);
			switch(iType)
			{
			case 0:
				{
					if((iType1 != SHORTCUTTYPE_INVENTORY) && (iType1 != SHORTCUTTYPE_EQUIP))
						return;
					
					ShortcutObject* pObj1 = g_ItemManager->getShortcutSlot(pPlayer, iType1, iIndex1);
					ShortcutObject* pObj2 = g_ItemManager->getShortcutSlot(pPlayer, iType2, iIndex2);
					ItemShortcut* pItem = (ItemShortcut*)(pObj2);
					//如果用坚定符鉴定，pItem1是鉴定符，没有就是要鉴定的物品
					ItemShortcut* pItem1 = (ItemShortcut*)(pObj1);
					if(!pItem1)
						return;
					if(!pObj1)
						return;
					if(!pItem)
					{
						if(gIdentifyManager->canIdentify(pPlayer, pObj1, NULL) == MSG_NONE)
							gIdentifyManager->setIdentifyResult(pPlayer, pObj1, NULL);
					}
					else
					{
						if(pItem1->getRes()->getSubCategory() == Res::CATEGORY_IDENTIFY)
						{
							U32 playerID = pPlayer->getId();
							if(!pObj2)
								return;
							U32 itemID = pItem1->getRes()->getItemID();
							Con::executef("manualIdentify", Con::getIntArg(playerID), Con::getIntArg(iIndex2), Con::getIntArg(itemID), Con::getIntArg(iIndex1), Con::getIntArg(iType2));
						}
						else
						{
							if(gIdentifyManager->canIdentify(pPlayer, pObj1, pObj2) == MSG_NONE)
								gIdentifyManager->setIdentifyResult(pPlayer, pObj1, pObj2);
						}
					}
					
				}
				break;
			case 1:
				{
					//gIdentifyManager->closeIdentify(pPlayer);
					pPlayer->setInteraction(NULL, Player::INTERACTION_NONE);
				}
				break;
			}
		}
		break;
	case INFO_ITEM_STRENGTHEN:
		{
			CHECK_MSGCOUNT(INFO_ITEM_STRENGTHEN, 0, 1, 0);
			U32 iOptType = GetInt32Arg(0);
			switch(iOptType)
			{
			case 0:		//服务端开始强化(服务端脚本执行，不需要客户端发送此类操作)
				{
					EquipStrengthen::openStrengthen(pPlayer, NULL);		//这里不应该被执行
				}
				break;
			case 1:		//服务端结束强化
				{
					pPlayer->setInteraction(NULL, Player::INTERACTION_NONE);
				}
				break;
			case 2:		//服务端执行强化
				{
					enWarnMessage result = EquipStrengthen::setStrengthenResult(pPlayer);
					if (result != MSG_NONE)
					{/*
						MessageEvent *ev = new MessageEvent;
						ev->setMessage(result);
						pCon->postNetEvent(ev);*/
						MessageEvent::send(con, SHOWTYPE_ERROR, result, SHOWPOS_SCREEN);
					}
				}
				break;
			}
		}
		break;
	case INFO_PET_OPERATE:
		{
			CHECK_MSGCOUNT(INFO_PET_OPERATE, 1, 0, 0);

			switch(GetIntArg(0))
			{
			case Player::PetOp_Spawn:
				{
					U32 nSlot = GetIntArg(1);
					bool bSuccess = PetOperationManager::SpawnPet(pPlayer, nSlot);
				}
				break;
			case Player::PetOp_Combo:
				{
				}
				break;
			case Player::PetOp_Dispatch:
				{
				}
				break;
			case Player::PetOp_Disband:
				{
					CHECK_MSGCOUNT(INFO_PET_OPERATE, 2, 0, 0);
					U32 nSlot = GetIntArg(1);
					bool bSuccess = PetOperationManager::DisbandPet(pPlayer, nSlot);
					PetOperationManager::ServerSendPetOptMsg(pPlayer, Player::PetOp_Disband, nSlot, bSuccess);
				}
				break;
			case Player::PetOp_Release:
				{
					CHECK_MSGCOUNT(INFO_PET_OPERATE, 2, 0, 0);
					U32 nSlot = GetIntArg(1);
					bool bSuccess = PetOperationManager::ReleasePet(pPlayer, nSlot);
					PetOperationManager::ServerSendPetOptMsg(pPlayer, Player::PetOp_Release, nSlot, bSuccess);
				}
				break;
			case Player::PetOp_Identify:
				{
					CHECK_MSGCOUNT(INFO_PET_OPERATE, 2, 0, 0);
					switch(GetIntArg(1))
					{
					case 1:		//执行鉴定
						{
							CHECK_MSGCOUNT(INFO_PET_OPERATE, 3, 0, 0);
							U32 nSlot = GetIntArg(2);
							bool bSuccess = PetOperationManager::PetIdentify(pPlayer, nSlot);
							PetOperationManager::ServerSendPetOptMsg(pPlayer, Player::PetOp_Identify, nSlot, bSuccess);
						}
						break;
					case 2:		//关闭对话框
						{							
							pPlayer->setInteraction(NULL, Player::INTERACTION_NONE);
						}
						break;
					}					
				}
				break;
			case Player::PetOp_Insight:
				{
					CHECK_MSGCOUNT(INFO_PET_OPERATE, 2, 0, 0);
					switch(GetIntArg(1))
					{
					case 1:		//执行提高慧根
						{
							CHECK_MSGCOUNT(INFO_PET_OPERATE, 3, 0, 0);
							U32 nSlot = GetIntArg(2);
							bool bSuccess = PetOperationManager::IncreasePetInsight(pPlayer, nSlot);
							PetOperationManager::ServerSendPetOptMsg(pPlayer, Player::PetOp_Insight, nSlot, bSuccess);
						}
						break;
					case 2:		//关闭对话框
						{
							pPlayer->setInteraction(NULL, Player::INTERACTION_NONE);
						}
						break;
					}			
				}
				break;
			case Player::PetOp_LianHua:
				{
					CHECK_MSGCOUNT(INFO_PET_OPERATE, 2, 0, 0);
					switch(GetIntArg(1))
					{
					case 1:		//执行炼化
						{
							CHECK_MSGCOUNT(INFO_PET_OPERATE, 3, 0, 0);
							U32 nSlot = GetIntArg(2);
							bool bSuccess = PetOperationManager::LianHuaPet(pPlayer, nSlot);
							PetOperationManager::ServerSendPetOptMsg(pPlayer, Player::PetOp_LianHua, nSlot, bSuccess);
						}
						break;
					case 2:		//关闭对话框
						{
							pPlayer->setInteraction(NULL, Player::INTERACTION_NONE);
						}
						break;
					}		
				}
				break;
			case Player::PetOp_UseEgg:
				{
					CHECK_MSGCOUNT(INFO_PET_OPERATE, 3, 0, 0);
					S32	iType = GetIntArg(1);
					S32	iIndex = GetIntArg(2);
					bool bSuccess = PetOperationManager::UsePetEgg(pPlayer, iType, iIndex);
				}
				break;
			case Player::PetOp_BecomeBaby:
				{
					CHECK_MSGCOUNT(INFO_PET_OPERATE, 2, 0, 0);
					switch(GetIntArg(1))
					{
					case 1:		//执行还童
						{
							CHECK_MSGCOUNT(INFO_PET_OPERATE, 3, 0, 0);
							PetOperationManager::PetHuanTong(pPlayer, GetIntArg(2));
						}
						break;
					case 2:		//关闭对话框
						{
							pPlayer->setInteraction(NULL, Player::INTERACTION_NONE);
						}
						break;
					}		
				}
				break;
			case Player::PetOp_Learn:
				{
					CHECK_MSGCOUNT(INFO_PET_OPERATE, 2, 1, 0);
					U32 nSlot = GetIntArg(1);
					U32 nPetStudyDataId = GetInt32Arg(0);
					PetOperationManager::PetXiuXing(pPlayer, nSlot, nPetStudyDataId);
				}
				break;
			case Player::PetOp_Feed:
				{
					CHECK_MSGCOUNT(INFO_PET_OPERATE, 4, 0, 0);
					U32 nSlot	= GetIntArg(1);
					S32 iType	= GetIntArg(2);
					S32 iIndex	= GetIntArg(3);
					if (iType == SHORTCUTTYPE_INVENTORY)
					{
						PetOperationManager::PetWeiShi_2(pPlayer, nSlot, iIndex);
					}				
				}
				break;
			case Player::PetOp_ChangeName:
				{
					CHECK_MSGCOUNT(INFO_PET_OPERATE, 2, 0, 1);
					U32 nSlot	= GetIntArg(1);
					StringTableEntry sNewName = GetStringArg(0);
					bool bSuccess = PetOperationManager::PetChangeName(pPlayer, nSlot, sNewName);
					PetOperationManager::ServerSendPetOptMsg(pPlayer, Player::PetOp_ChangeName, nSlot, bSuccess);
				}
				break;
			case Player::PetOp_ChangeProperties:
				{
					CHECK_MSGCOUNT(INFO_PET_OPERATE, 2, 6, 0);
					U32 nSlot = GetIntArg(1);
					bool bSuccess = PetOperationManager::PetChangeProperties(pPlayer, nSlot, GetInt32Arg(0), GetInt32Arg(1),GetInt32Arg(2),
						GetInt32Arg(3), GetInt32Arg(4), GetInt32Arg(5));
					PetOperationManager::ServerSendPetOptMsg(pPlayer, Player::PetOp_ChangeName, nSlot, bSuccess);
				}
				break;
			}
		}
		break;
	case INFO_MOUNT_PET_OPERATE:
		{
			switch(GetIntArg(0))
			{
			case Player::PetOp_UseMountPetEgg:
				{
					CHECK_MSGCOUNT(INFO_PET_OPERATE, 3, 0, 0);
					S32	iType = GetIntArg(1);
					S32	iIndex = GetIntArg(2);
					PetMountOperationManager::UseMountPetEgg(pPlayer, iType, iIndex);
				}
				break;
			case Player::PetOp_SpawnMountPet:
				{
					CHECK_MSGCOUNT(INFO_PET_OPERATE, 2, 0, 0);
					U32 nSlot = GetIntArg(1);
					bool bSuccess = PetMountOperationManager::SpawnMountPet(pPlayer, nSlot);
				}
				break;
			case Player::PetOp_ReleaseMountPet:
				{
					CHECK_MSGCOUNT(INFO_PET_OPERATE, 2, 0, 0);
					U32 nSlot = GetIntArg(1);
					bool bSuccess = PetMountOperationManager::ReleaseMountPet(pPlayer, nSlot);
					PetMountOperationManager::ServerSendMountPetOptMsg(pPlayer, Player::PetOp_ReleaseMountPet, nSlot, bSuccess);
				}
				break;
			case Player::PetOp_DisbandMountPet:
				{
					CHECK_MSGCOUNT(INFO_PET_OPERATE, 2, 0, 0);
					U32 nSlot = GetIntArg(1);
					bool bSuccess = PetMountOperationManager::DisbandMountPet(pPlayer, nSlot);
					PetMountOperationManager::ServerSendMountPetOptMsg(pPlayer, Player::PetOp_DisbandMountPet, nSlot, bSuccess);
				}
				break;			
			default:
				break;
			}
		}
		break;
	case INFO_SPIRIT_OPERATE:
		{
			CHECK_MSGCOUNT(INFO_SPIRIT_OPERATE, 1, 0, 0);
			switch(GetIntArg(0))
			{
			case Player::SpiritOp_OpenSpiritSlot:
				{
					CHECK_MSGCOUNT(INFO_SPIRIT_OPERATE, 2, 0, 0);
					S32 nSlot = GetIntArg(1);
					bool bSuccess = SpiritOperationManager::OpenSpiritSlot(pPlayer, nSlot);
					SpiritOperationManager::ServerSendSpiritOptMsg(pPlayer, Player::SpiritOp_OpenSpiritSlot, nSlot, bSuccess);
				}
				break;
			case Player::SpiritOp_AddSpirit:
				{
					CHECK_MSGCOUNT(INFO_SPIRIT_OPERATE, 2, 1, 0);
					U32 nSpiritId = GetInt32Arg(0);
					S32 nSlot = GetIntArg(1);					
					bool bSuccess = SpiritOperationManager::AddNewSpirit(pPlayer, nSpiritId);
					SpiritOperationManager::ServerSendSpiritOptMsg(pPlayer, Player::SpiritOp_AddSpirit, nSlot, bSuccess);
				}
				break;
			case Player::SpiritOp_RemoveSpirit:
				{
					CHECK_MSGCOUNT(INFO_SPIRIT_OPERATE, 2, 0, 0);
					S32 nSlot = GetIntArg(1);
					bool bSuccess = SpiritOperationManager::DeleteSpirit(pPlayer, nSlot);
					SpiritOperationManager::ServerSendSpiritOptMsg(pPlayer, Player::SpiritOp_RemoveSpirit, nSlot, bSuccess);
				}
				break;
			case Player::SpiritOp_HuanHuaSpirit:
				{
					CHECK_MSGCOUNT(INFO_SPIRIT_OPERATE, 2, 0, 0);
					S32 nSlot = GetIntArg(1);
					bool bSuccess = SpiritOperationManager::HuanHuaSpirit(pPlayer, nSlot);
					SpiritOperationManager::ServerSendSpiritOptMsg(pPlayer, Player::SpiritOp_HuanHuaSpirit, nSlot, bSuccess);
				}
				break;
			case Player::SpiritOp_CancelHuanHuaSpirit:
				{
					CHECK_MSGCOUNT(INFO_SPIRIT_OPERATE, 2, 0, 0);
					S32 nSlot = GetIntArg(1);
					bool bSuccess = SpiritOperationManager::CancelHuanHuaSpirit(pPlayer, nSlot);
					SpiritOperationManager::ServerSendSpiritOptMsg(pPlayer, Player::SpiritOp_CancelHuanHuaSpirit, nSlot, bSuccess);
				}
				break;
			case Player::SpiritOp_ActivateSpirit:
				{
					CHECK_MSGCOUNT(INFO_SPIRIT_OPERATE, 2, 0, 0);
					S32 nSlot = GetIntArg(1);
					bool bSuccess = SpiritOperationManager::ActiviateSpirit(pPlayer, nSlot);
					SpiritOperationManager::ServerSendSpiritOptMsg(pPlayer, Player::SpiritOp_ActivateSpirit, nSlot, bSuccess);
				}
				break;
			case Player::SpiritOp_OpenJueZhaoSlot:
				{
					CHECK_MSGCOUNT(INFO_SPIRIT_OPERATE, 3, 0, 0);
					S32 nSlot = GetIntArg(1);
					S32 nIndex = GetIntArg(2);
					bool bSuccess = SpiritOperationManager::OpenSpiritJueZhao(pPlayer, nSlot, nIndex);
					SpiritOperationManager::ServerSendSpiritOptMsg(pPlayer, Player::SpiritOp_OpenJueZhaoSlot, nSlot, bSuccess);
				}
				break;
			case Player::SpiritOp_AddJueZhao:
				{
					CHECK_MSGCOUNT(INFO_SPIRIT_OPERATE, 3, 0, 0);
					S32 nSlot = GetIntArg(1);
					S32 nIndex = GetIntArg(2);
					bool bSuccess = SpiritOperationManager::AddNewSpiritJueZhao(pPlayer, nSlot, nIndex);
					SpiritOperationManager::ServerSendSpiritOptMsg(pPlayer, Player::SpiritOp_AddJueZhao, nSlot, bSuccess);
				}
				break;
			case Player::SpiritOp_RemoveJueZhao:
				{
					CHECK_MSGCOUNT(INFO_SPIRIT_OPERATE, 3, 0, 0);
					S32 nSlot = GetIntArg(1);
					S32 nIndex = GetIntArg(2);
					bool bSuccess = SpiritOperationManager::RemoveSpiritJueZhao(pPlayer, nSlot, nIndex);
					SpiritOperationManager::ServerSendSpiritOptMsg(pPlayer, Player::SpiritOp_RemoveJueZhao, nSlot, bSuccess);
				}
				break;
			case Player::SpiritOp_AddTalentLevel:
				{
					CHECK_MSGCOUNT(INFO_SPIRIT_OPERATE, 4, 0, 0);
					S32 nSpiritSlot		= GetIntArg(1);
					S32 nTalentIndex	= GetIntArg(2);
					S32 nAddLevel		= GetIntArg(3);
					bool bSuccess = SpiritOperationManager::AddTalentLevel(pPlayer, nSpiritSlot, nTalentIndex, nAddLevel);
					SpiritOperationManager::ServerSendAddTalentLevelMsg(pPlayer, nSpiritSlot, nTalentIndex, bSuccess);
				}
				break;
			case Player::SpiritOp_CloseJueZhaoWnd:
				{
					pPlayer->setInteraction(NULL, Player::INTERACTION_NONE);
				}
				break;
			default:
				break;
			}
		}
		break;
	case INFO_ITEM_IMPRESS:
		{
			CHECK_MSGCOUNT(INFO_ITEM_IMPRESS, 4, 0, 0);
			U32 iType1 = GetIntArg(2);
			U32 iIndex1 = GetIntArg(3);
			U32 iType2 = GetIntArg(0);
			U32 iIndex2 = GetIntArg(1);
			ShortcutObject* pObj1 = g_ItemManager->getShortcutSlot(pPlayer, iType1, iIndex1);
			ShortcutObject* pObj2 = g_ItemManager->getShortcutSlot(pPlayer, iType2, iIndex2);
			if(!pObj1)
				return;
			if(EquipImpress::canImpress(pPlayer, pObj1, pObj2) == MSG_NONE)
				EquipImpress::setImpress(pPlayer, pObj1, pObj2);
		}
		break;
	case INFO_ITEM_SOULLINK:
		{

		}
		break;
	case INFO_ITEM_MOUNTGEM:			//镶嵌宝石
		{
			CHECK_MSGCOUNT(INFO_ITEM_MOUNTGEM, 0, 1, 0);
			S32 nIndex = GetInt32Arg(0);

			ItemShortcut *pEquipItem = dynamic_cast<ItemShortcut *>(pPlayer->mountGemList.GetSlot(0));
			ItemShortcut *pGemItem = dynamic_cast<ItemShortcut *>(pPlayer->mountGemList.GetSlot(nIndex));

			enWarnMessage msg = EquipMountGem::setEquipMountGem(pPlayer, pEquipItem, pGemItem, nIndex);
			if (msg != MSG_NONE)		//镶嵌宝石成功
			{
           
			}
		}
		break;
	case INFO_ITEM_UNMOUNTGEM:		//摘取宝石
		{
			CHECK_MSGCOUNT(INFO_ITEM_MOUNTGEM, 0, 1, 0);
			S32 nIndex = GetInt32Arg(0);

			ItemShortcut *pEquipItem = dynamic_cast<ItemShortcut *>(pPlayer->mountGemList.GetSlot(0));
			ItemShortcut *pGemItem = dynamic_cast<ItemShortcut *>(pPlayer->mountGemList.GetSlot(nIndex));

			enWarnMessage msg = EquipMountGem::unsetEquipMountGem(pPlayer, pEquipItem, pGemItem, nIndex);
			if (msg != MSG_NONE)		//摘取宝石成功
			{
			}
		}
		break;
	case INFO_ITEM_CLOSE_MOUNTGEM:	//关闭镶嵌界面
		{
			enWarnMessage msg = EquipMountGem::clearEquipGemMount(pPlayer);
			if (msg != MSG_NONE)		//摘取宝石成功
			{
			}
		}
		break;
	case INFO_ITEM_PUNCHHOLE:			//装备打孔
		{
			CHECK_MSGCOUNT(INFO_ITEM_MOUNTGEM, 0, 1, 0);
			S32 nOptType = GetInt32Arg(0);	//操作类型

			ItemShortcut *pEquipItem = dynamic_cast<ItemShortcut *>(pPlayer->punchHoleList.GetSlot(0));
			enWarnMessage msg = MSG_NONE;
			
			switch(nOptType)
			{
				case 0:		//打开装备打孔对话框
				{
					msg = EquipPunchHole::openPunchHole(pPlayer, NULL);		//这里不应该被执行
				}
				break;
				case 1:		//关闭装备打孔对话框
				{
					//msg = EquipPunchHole::closePunchHole(pPlayer, pEquipItem);
					pPlayer->setInteraction(NULL, Player::INTERACTION_NONE);
				}
					break;
				case 2:		//执行装备打孔命令
				{
					msg = EquipPunchHole::setEquipPunchHoleResult(pPlayer, pEquipItem);
					if (msg != MSG_NONE)		//打孔成功
					{

					}
				}
				break;
			}			
		}
		break;
	case INFO_SKILL_OPERATE:
		{
			CHECK_MSGCOUNT(INFO_SKILL_OPERATE,0,3,0);
			switch(GetInt32Arg(0))
			{
			case 0: //技能系选择
				{
					pPlayer->setClasses(GetInt32Arg(1),GetInt32Arg(2));

					// 选择完技能系自动打开技能学习
					Con::executef("OpenSkillStudy",Con::getIntArg(pPlayer->getId()),Con::getIntArg(pPlayer->getClasses(0)),
						Con::getIntArg(pPlayer->getClasses(1)),Con::getIntArg(pPlayer->getClasses(2)),
						Con::getIntArg(pPlayer->getClasses(3)),Con::getIntArg(pPlayer->getClasses(4)));
				}
				break;
			case 1://技能学习
                {
                    pPlayer->GetSkillTable().LearnSkill(pPlayer,GetInt32Arg(1));
                }
				break;
			}
		}
		break;
	case INFO_LIVINGSKILL_LEARN:
		{
			CHECK_MSGCOUNT(INFO_LIVINGSKILL_LEARN,0,1,0);
			U32 skillID = GetInt32Arg(0);
			if(pPlayer->pLivingSkill)
				pPlayer->pLivingSkill->learnLivingSkill(pPlayer,skillID);
		}
		break;
	case INFO_BANK:
		{
			switch(GetInt32Arg(0))
			{
			case PlayerBank::Bank_Open:
				{
					g_PlayerBank->openBank(pPlayer);
				}
				break;
			case PlayerBank::Bank_Close:
				{
					//g_PlayerBank->closeBank(pPlayer);
					pPlayer->setInteraction(NULL, Player::INTERACTION_NONE);
				}
				break;
			case PlayerBank::Bank_CheckPassWord:
				{
					g_PlayerBank->checkPassWord(pPlayer, GetStringArg(0));
				}
				break;
			case PlayerBank::Bank_SetPassWord:
				{
					g_PlayerBank->setPassWord(pPlayer, GetStringArg(0));
				}
				break;
			case PlayerBank::Bank_Lock:
				{
					g_PlayerBank->setLocked(pPlayer, true);
				}
				break;
			case PlayerBank::Bank_SaveMoney:
				{
					g_PlayerBank->savingMoney(pPlayer, GetInt32Arg(1));
				}
				break;
			case PlayerBank::Bank_GetMoney:
				{
					g_PlayerBank->takeMoney(pPlayer, GetInt32Arg(1));
				}
				break;
			case PlayerBank::Bank_CleanUp:
				{
					pPlayer->bankList.CompactSlots();
					for(int i=0; i<pPlayer->bankList.GetMaxSlots(); ++i)
					{
						pPlayer->bankList.UpdateToClient(con, i, ITEM_NOSHOW);
					}
				}
				break;
			case PlayerBank::Bank_BuySpace:
				{
					g_PlayerBank->buySpace(pPlayer);
				}
				break;
			case PlayerBank::Bank_ItemMoveIn:
				{
					U32 iIndex = GetInt32Arg(1);
					ShortcutObject* pObj = g_ItemManager->getShortcutSlot(pPlayer, SHORTCUTTYPE_INVENTORY, iIndex);
					ItemShortcut* pItem = NULL;
					if(pObj && (pItem = dynamic_cast<ItemShortcut*>(pObj)))
					{
						enWarnMessage msg = g_PlayerBank->addItemToBank(pPlayer, pItem, pItem->getRes()->getQuantity());
						if(msg != MSG_NONE)
						{
							MessageEvent::send(con, SHOWTYPE_NOTIFY, msg);
							return;
						}
						pPlayer->inventoryList.SetSlot(iIndex, NULL, false);
						pPlayer->inventoryList.UpdateToClient(con, iIndex, ITEM_SAVEBANK);
						//g_PlayerBank->addItemToBank(pPlayer, pItem, pItem->getRes()->getQuantity());
					}
				}
				break;
			case PlayerBank::Bank_ItemMoveOut:
				{
					U32 iIndex = GetInt32Arg(1);
					ShortcutObject* pObj = g_ItemManager->getShortcutSlot(pPlayer, SHORTCUTTYPE_BANK, iIndex);
					ItemShortcut* pItem = NULL;
					if(pObj && (pItem = dynamic_cast<ItemShortcut*>(pObj)))
					{
						S32 index;
						if(g_ItemManager->addItemToInventory(pPlayer, pItem, index, pItem->getRes()->getQuantity()) == MSG_NONE)
						{
							pPlayer->bankList.SetSlot(iIndex, NULL, false);
							pPlayer->bankList.UpdateToClient(con, iIndex, ITEM_NOSHOW);
						}
						
					}
				}
				break;
			}
		}
		break;
	case INFO_COLLECTION_ITEM:
		{
			CHECK_MSGCOUNT(INFO_COLLECTION_ITEM, 0, 2, 0);
			
			CollectionObject* pCollectionObj = dynamic_cast<CollectionObject*>(Sim::findObject(GetInt32Arg(0)));
			if (!pCollectionObj)
				return;
			S32 nOpType = GetInt32Arg(1);			
			switch(nOpType)
			{
			case 1:	//服务端触发采集开始
				{
					if ( !pPlayer->setCollectionTarget(pCollectionObj) )
					{
						//pCollectionObj->serverSendEndMsg(pPlayer);
					}
				}
				break;
			case 2:	//服务端触发采集过程终止（包括正常终止和异常终止）
				{
					//pPlayer->unsetCollectionTarget(pCollectionObj);
					pPlayer->setInteraction(NULL, Player::INTERACTION_NONE);
				}
				break;
			}
		}
		break;
	case INFO_PRESCRIPTION_PRODUCE:
		{
			CHECK_MSGCOUNT(INFO_PRESCRIPTION_PRODUCE,2,1,0);
			U16 itype     = GetIntArg(0);
			U16 iCount    = GetIntArg(1);
			U32 iSerialId = GetInt32Arg(0);
			if(!pPlayer->pScheduleEvent)
				return;

			switch(itype)
			{
			case ProduceEquip::PRODUCE_START:
				{
					if (pPlayer->pScheduleEvent->getEventID())
						return;

					PrescriptionData* pData = g_PrescriptionRepository.getPrescriptionData(iSerialId);
					if(!pData)
						return;

					LivingSkillData* pSkillData = g_LivingSkillRespository.getLivingSkillData(pData->getLivingSkillID());
					if(!pSkillData)
						return;

					LivingSkillData::Cast& pCast =  pSkillData->getCast(); 
					SimTime  vocalTime = pCast.readyTime;
					if(vocalTime <= 0)
						return;

					simCallBackEvents* Event = new simCallBackEvents(INFO_PRESCRIPTION_PRODUCE);
					Event->SetInt32ArgValues(1,iSerialId);
					Event->SetIntArgValues(1,iCount);
					U32 ret = Sim::postEvent(pPlayer,Event,Sim::getCurrentTime()+vocalTime);
					pPlayer->pScheduleEvent->setEvent(ret,vocalTime);
					
				}
				break;
			case  ProduceEquip::PRODUCE_CANCEL:
				{
					ServerGameNetEvent* Event = new ServerGameNetEvent(INFO_PRESCRIPTION_PRODUCE);
					Event->SetInt32ArgValues(1,iSerialId);
					Event->SetIntArgValues(2,ProduceEquip::PRODUCE_CANCEL,iCount);
					if(con->postNetEvent(Event))
						pPlayer->pScheduleEvent->clear();
				}
				break;
			}
		}
		break;
	case INFO_CANCELVOCAL:
		{
			if (pPlayer->pScheduleEvent && pPlayer->pScheduleEvent->getEventID())
			{
				Sim::cancelEvent(pPlayer->pScheduleEvent->getEventID());
				pPlayer->pScheduleEvent->clear();
			}
		}
		break;
	case INFO_TIMER_TRIGGER:
		{
			U32 nOptType = GetInt32Arg(0);
			U32 nId		 = GetInt32Arg(1);

			if (nOptType == 1) //客户端取消计时器的请求
			{
				if (pPlayer->mTimerTriggerMgr)
				{
					pPlayer->mTimerTriggerMgr->Cancel(nId);
				}
			}
		}
		break;
	case INFO_STALL:
		{
			U32 iType = GetInt32Arg(0);
			switch (iType)
			{
			//察看别人摆摊栏
			case TRADE_ASKFOR:
				{
					g_Stall->openStallerStall(pPlayer, con, GetInt32Arg(1));
				}
				break;
			// 打开自己的摆摊
			case TRADE_BEGIN:
				{
					g_Stall->openStall(pPlayer, GetInt32Arg(1), GetInt32Arg(2));
				}
				break;
			//添加到摆摊物品
			case TRADE_UPITEM:
				{
					if(pPlayer->individualStallList.getStallState() == TRADE_LOCK)
					{
						MessageEvent::send(con, SHOWTYPE_ERROR, MSG_TRADE_STALL_NORENAME, SHOWPOS_SCREEN);
						return;
					}
					S32 index = GetInt32Arg(1);
					U32 iType = GetInt32Arg(2);
					g_Stall->upItemToStall(pPlayer, con, index, iType);
				}
				break;
			//下架
			case TRADE_DOWNITEM:
				{
					if(pPlayer->individualStallList.getStallState() == TRADE_LOCK)
					{
						MessageEvent::send(con, SHOWTYPE_ERROR, MSG_TRADE_STALL_UNABLEDOWN, SHOWPOS_SCREEN);
						return;
					}
					S32 index = GetInt32Arg(1);
					U32 iType = GetInt32Arg(2);
					g_Stall->downItemFromStall(pPlayer, con, index, iType);
				}
				break;
			//改价
			case TRADE_SETMONEY:
				{
					U32 index = GetInt32Arg(1);
					U32 money = GetInt32Arg(2);
					U32 iType = GetInt32Arg(3);
					g_Stall->setStallItemPrice(pPlayer, con, index, money, iType);

				}
				break;
			//取消
			case TRADE_CANCEL:
				{
					U32 iType = GetInt32Arg(1);
					g_Stall->closeStall(pPlayer, iType);
				}
				break;
			//重命名
			case STALL_RENAME:
				{
					if(pPlayer->individualStallList.getStallState() != TRADE_LOCK)
					{
						pPlayer->individualStallList.setStallName(GetStringArg(0));
						ServerGameNetEvent* evt = new ServerGameNetEvent(INFO_STALL);
						evt->SetInt32ArgValues(1, STALL_RENAME);
						evt->SetStringArgValues(1, GetStringArg(0));
						con->postNetEvent(evt);
					}
					else
					{
						MessageEvent::send(con, SHOWTYPE_ERROR, MSG_TRADE_STALL_NORENAME, SHOWPOS_SCREEN);
						return;
					}
				}
				break;
			//改摆摊说明
			case STALL_REINFO:
				{
					if(pPlayer->individualStallList.getStallState() == TRADE_LOCK)
					{
						MessageEvent::send(con, SHOWTYPE_ERROR, MSG_TRADE_STALL_NOREINFO, SHOWPOS_SCREEN);
						return;
					}
					else
					{
						pPlayer->individualStallList.setStallInfo(GetStringArg(0));
						ServerGameNetEvent* evt = new ServerGameNetEvent(INFO_STALL);
						evt->SetInt32ArgValues(1, STALL_REINFO);
						evt->SetStringArgValues(1, GetStringArg(0));
						con->postNetEvent(evt);
					}
					
				}
				break;
			//全部下架
			case STALL_REMOVE_ALL:
				{
					g_Stall->downAllStallItem(pPlayer, con);
				}
				break;
			//出摊
			case TRADE_LOCK:
				{
					pPlayer->individualStallList.setStallState(TRADE_LOCK);
					ServerGameNetEvent* evt = new ServerGameNetEvent(INFO_STALL);
					evt->SetInt32ArgValues(1, TRADE_LOCK);
					con->postNetEvent(evt);
				}
				break;
			//收摊
			case TRADE_UNLOCK:
				{
					pPlayer->individualStallList.setStallState(TRADE_UNLOCK);
					for(int i=0; i<pPlayer->individualStallList.getPlayerList().size(); ++i)
					{
						Player* pTargetObj = g_ServerGameplayState->GetPlayer(pPlayer->individualStallList.getPlayerListID(i));
						if(pTargetObj)
						{
							ServerGameNetEvent* evt = new ServerGameNetEvent(INFO_STALL);
							evt->SetInt32ArgValues(3, TRADE_CANCEL, pTargetObj->getPlayerID(), pPlayer->getPlayerID());
							pTargetObj->getControllingClient()->postNetEvent(evt);
						}

					}
					pPlayer->individualStallList.getPlayerList().clear();
					ServerGameNetEvent* evt = new ServerGameNetEvent(INFO_STALL);
					evt->SetInt32ArgValues(1, TRADE_UNLOCK);
					con->postNetEvent(evt);
				}
				break;
			//关闭看别人的摆摊
			case STALL_SEE_CLOSE:
				{
					U32 playerID = pPlayer->getPlayerID();
					SceneObject* pTargetObject = pPlayer->getInteraction();

					Player* pTarget = NULL;
					if(!pTargetObject || !(pTarget = (Player*)pTargetObject))
						return;
					else
						pTarget->individualStallList.delPlayerList(playerID);

					ServerGameNetEvent* evt = new ServerGameNetEvent(INFO_STALL);
					evt->SetInt32ArgValues(2, STALL_SEE_CLOSE, pTarget->getPlayerID());
					con->postNetEvent(evt);
					pPlayer->setInteraction(NULL, Player::INTERACTION_NONE);
				}
				break;
			//摆摊交易
			case TRADE_ENDNOTIFY:
				{
					S32 index = GetInt32Arg(1);
					U32 num = GetInt32Arg(2);
					U32 flag = GetInt32Arg(4);
					g_Stall->stallTrade(pPlayer, con, index, num, GetInt32Arg(3), flag);
				}
				break;
			//浏览信息
			case STALL_SCANSTALLNOTE:
				{
					U32 playerID = GetInt32Arg(1);
					Player* StallPlayer = g_ServerGameplayState->GetPlayer(playerID);
					if(!StallPlayer)
					{
						//MessageEvent::Send
						return;
					}

					for(S32 i = 0; i < StallPlayer->individualStallList.mTradeInfoList.size(); i++)
					{
						StallRecordEvent* ev = new StallRecordEvent(playerID, false, i);
						con->postNetEvent(ev);
					}

					ServerGameNetEvent* ev1 = new ServerGameNetEvent(INFO_STALL);
					ev1->SetInt32ArgValues(2, STALL_INFOMSG, playerID);
					ev1->SetStringArgValues(1,StallPlayer->individualStallList.getStallInfo());
					con->postNetEvent(ev1);									
				}
				break;
			case STALL_ADDTOLOOKUP:
				{
					U32 itemId = GetInt32Arg(1);
					U32 money = GetInt32Arg(2);
					U32 num = GetInt32Arg(3);
					S32 iIndex = GetInt32Arg(4);
					ItemShortcut* pItem = ItemShortcut::CreateItem(itemId, num);
					if(!pItem)
					{
						MessageEvent::send(con, SHOWTYPE_ERROR, MSG_ITEM_ITEMDATAERROR, SHOWPOS_SCREEN);
						return ;
					}
					stStallBuyItem* stallBuyItem = new stStallBuyItem;
					stallBuyItem->buyItem = pItem;
					stallBuyItem->money = money;
					stallBuyItem->num = num;
					enWarnMessage msg = MSG_UNKOWNERROR;
					if(iIndex >= 0)
					{
						pPlayer->stallBuyItemList.mSlots[iIndex] = stallBuyItem;
						pPlayer->stallBuyItemList.UpdateToClient(con, iIndex);
						ServerGameNetEvent* event = new ServerGameNetEvent(INFO_STALL);
						event->SetInt32ArgValues(4, TRADE_SETMONEY, iIndex,money, 3);
						con->postNetEvent(event);
					}
					else
					{
						msg = g_ItemManager->addItemToLookUp(pPlayer, stallBuyItem);
						if(msg != MSG_NONE)
						{
							MessageEvent::send(con, SHOWTYPE_ERROR, msg, SHOWPOS_SCREEN);
							return;
						}
					}
					
				}
				break;
			case STALL_CHATMSG:
				{
					U32 sendId = GetInt32Arg(1);
					U32 recieverID = GetInt32Arg(2);
					Player* pTarget = g_ServerGameplayState->GetPlayer(GetInt32Arg(2));
					if(!pTarget)
						return;

					stChatRecord* pRecord = new stChatRecord;
					pRecord->time = Platform::getTime();
					pRecord->senderID = sendId;
					pRecord->senderName = pPlayer->getPlayerName();
					pRecord->receiverID = recieverID;
					pRecord->receiverName = pTarget->getPlayerName();
					pRecord->contentLen = strlen(GetStringArg(0)) + 1;
					pRecord->chatContent = new char[pRecord->contentLen];
					dSprintf(pRecord->chatContent, pRecord->contentLen , "%s", GetStringArg(0));


					if(pTarget->individualStallList.mChatInfoList.size() > 50)
					{
						SAFE_DELETE_ARRAY(pTarget->individualStallList.mChatInfoList[0]->chatContent)
						delete pTarget->individualStallList.mChatInfoList[0];
						pTarget->individualStallList.mChatInfoList.erase((U32)0);
					}
					pTarget->individualStallList.mChatInfoList.push_back(pRecord);

					U32 index = pTarget->individualStallList.mChatInfoList.size() -1;
					StallRecordEvent* ev = new StallRecordEvent(recieverID, true, index);
					pTarget->getControllingClient()->postNetEvent(ev);

					ServerGameNetEvent* evt = new ServerGameNetEvent(INFO_STALL);
					evt->SetInt32ArgValues(3, STALL_CHATMSG,  recieverID, 1);
					pTarget->getControllingClient()->postNetEvent(evt);
				}
				break;
			case STALL_CLEAR_MSG:
				{
					for(int i=0; i<pPlayer->individualStallList.mChatInfoList.size(); ++i)
					{
						stChatRecord* pRecord = pPlayer->individualStallList.mChatInfoList[i];
						if(pRecord)
							delete pRecord;
						pPlayer->individualStallList.mChatInfoList.erase_fast(i--);
					}
					for(int i=0; i<pPlayer->individualStallList.mTradeInfoList.size(); ++i)
					{
						stTradeRecord* pRecord = pPlayer->individualStallList.mTradeInfoList[i];
						if(pRecord)
							delete pRecord;
						pPlayer->individualStallList.mTradeInfoList.erase_fast(i--);
					}					
					//给所有察看的玩家发送
					for(int i=0; i<pPlayer->individualStallList.getPlayerList().size(); ++i)
					{
						Player* pTargetObj = g_ServerGameplayState->GetPlayer(pPlayer->individualStallList.getPlayerListID(i));
						if(pTargetObj)
						{
							ServerGameNetEvent* evt = new ServerGameNetEvent(INFO_STALL);
							evt->SetInt32ArgValues(1, STALL_CLEAR_MSG);
							pTargetObj->getControllingClient()->postNetEvent(evt);
						}
					}
					//给自己发送
					ServerGameNetEvent* evt = new ServerGameNetEvent(INFO_STALL);
					evt->SetInt32ArgValues(1, STALL_CLEAR_MSG);
					pPlayer->getControllingClient()->postNetEvent(evt);
					
				}
				break;
			}
		}
		break;
	case INFO_QUICKCHANGE:
		{
			U32 stateFlag = GetInt32Arg(0);
			if(stateFlag == 0)//普通状态
			{
				pPlayer->clearOtherFlag(Player::OTHERFLAG_USEFASHION);
			}
			else if(stateFlag == 1)//门宗时装状态
			{
				pPlayer->setOtherFlag(Player::OTHERFLAG_USEFASHION);
				pPlayer->setOtherFlag(Player::OTHERFLAG_FAMILYFASHION);
			}
			else if(stateFlag == 2)
			{
				pPlayer->setOtherFlag(Player::OTHERFLAG_USEFASHION);
				pPlayer->clearOtherFlag(Player::OTHERFLAG_FAMILYFASHION);
			}
			pPlayer->updateFashionFlags();
		}
		break;
	case INFO_GETENCOURAGE:
		Con::evaluatef( "SptSocial_Encourage(%d);", pPlayer->getPlayerID() );
		break;
	case INFO_ENTER_TRANSPORT:
		{
			pPlayer->transportObject( pPlayer->mTransLineId, pPlayer->mTransTriggerId, pPlayer->mTransMapId, pPlayer->mTransX, pPlayer->mTransY, pPlayer->mTransZ );
		}
		break;
	case INFO_CANCELBUFF:
		{
			// 玩家主动取消增益状态
			pPlayer->HandleRemoveRequest(GetInt32Arg( 0 ), GetIntArg( 0 ));
		}
		break;
	case INFO_MPMANAGER:
		{
			CHECK_MSGCOUNT(INFO_MPMANAGER,1,1,0);
			S32 srcPlayerId = GetInt32Arg(0);
			SocialType::Type type = (SocialType::Type) GetIntArg(0);

			Player* srcPlayer = g_ServerGameplayState->GetPlayer(srcPlayerId);
			if(!srcPlayer || !pPlayer->getMPInfo())
				return;

			GameConnection* conn1 = srcPlayer->getControllingClient();
			if(!conn1)
				return;

			if(srcPlayer->getDistance(pPlayer)>3)
			{
				MessageEvent::send(con,SHOWTYPE_NOTIFY,MSG_PLAYER_TOOFAR,SHOWPOS_SCREEN);
				MessageEvent::send(conn1,SHOWTYPE_NOTIFY,MSG_PLAYER_TOOFAR,SHOWPOS_SCREEN);
				return;
			}

			enWarnMessage msg = MSG_NONE;
			switch(type)
			{
			case SocialType::Prentice:
				{
					msg = pPlayer->getMPInfo()->followMaster(pPlayer,srcPlayer);
					if(msg == MSG_NONE)
					{
						g_zoneSocial.chanageLink(pPlayer->getPlayerID(),srcPlayerId,SocialType::Friend,SocialType::Master);
						g_zoneSocial.chanageLink(srcPlayerId,pPlayer->getPlayerID(),SocialType::Friend,SocialType::Prentice);
						pPlayer->getMPInfo()->setStudyLevel(pPlayer,pPlayer->getLevel());
					}
					else
					{
						MessageEvent::send(con,SHOWTYPE_NOTIFY,msg,SHOWPOS_SCREEN);
					}
				}
				break;
			case SocialType::Master:
				{
					msg = pPlayer->getMPInfo()->recruitPrentice(pPlayer,srcPlayer);
					if(msg == MSG_NONE)
					{
						g_zoneSocial.chanageLink(pPlayer->getPlayerID(),srcPlayerId,SocialType::Friend,SocialType::Prentice);
						g_zoneSocial.chanageLink(srcPlayerId,pPlayer->getPlayerID(),SocialType::Friend,SocialType::Master);
						pPlayer->getMPInfo()->setStudyLevel(srcPlayer,srcPlayer->getLevel());
					}
					else
					{
						MessageEvent::send(con,SHOWTYPE_NOTIFY,msg,SHOWPOS_SCREEN);
					}
				}
				break;
			default:
				{
					MessageEvent::send(conn1,SHOWTYPE_NOTIFY,avar("%s拒绝了你的请求",pPlayer->getPlayerName()),SHOWPOS_SCREEN);
					return;
				} 
			}
		}
		break;
	case INFO_MPMCHANGETEXT:
		{
			CHECK_MSGCOUNT(INFO_MPMCHANGETEXT,0,0,1);
			if(NULL == pPlayer->getMPInfo())
				return;

			pPlayer->getMPInfo()->setCallboardString( GetStringArg(0) );
		}
	case INFO_ITEMSPLIT:
		{
			U8 type = GetIntArg(0);

			if(type != ((U8)ItemSplit::ITEMSPLIT_PICKUPITEM))
			{
				CHECK_MSGCOUNT(INFO_ITEMSPLIT,1,0,0);
			}
			else
			{
				CHECK_MSGCOUNT(INFO_ITEMSPLIT,2,0,0);
			}

			switch(type)
			{
			case ItemSplit::ITEMSPLIT_START:
				{
					if(pPlayer->pScheduleEvent->getEventID())
					{
						MessageEvent::send(con,SHOWTYPE_ERROR,MSG_PLAYER_BUSY);
						return;
					}

					pPlayer->mItemSplitList.setLock(true);
					S32 time = pPlayer->mItemSplitList.getReadyTime();
					simCallBackEvents* pEvent = new simCallBackEvents(INFO_ITEMSPLIT);
					U32 ret = Sim::postEvent(pPlayer,pEvent,Sim::getCurrentTime() + time);
					pPlayer->pScheduleEvent->setEvent(ret,time);
				}
				break;
			case ItemSplit::ITEMSPLIT_PICKUPAllITEM:
				{
					enWarnMessage msg = MSG_NONE;
					msg = pPlayer->mItemSplitList.pickupAllItem(pPlayer);
					if(msg != MSG_NONE)
						MessageEvent::send(con,SHOWTYPE_ERROR,msg);
				}
				break;
			case ItemSplit::ITEMSPLIT_CANCEL:
				{
					if(pPlayer->pScheduleEvent->getEventID())
						pPlayer->pScheduleEvent->cancelEvent(pPlayer);

					pPlayer->mItemSplitList.cancelItemSplit(pPlayer);
				}
				break;
			case ItemSplit::ITEMSPLIT_PICKUPITEM:
				{
					enWarnMessage msg = MSG_NONE;
					U8 iIndex = GetIntArg(1);
					msg = pPlayer->mItemSplitList.pickupItemByIndex(pPlayer,iIndex);
					if(msg != MSG_NONE)
						MessageEvent::send(con,SHOWTYPE_ERROR,msg);
				}
				break;
			}
		}
		break;
	case INFO_ITEMCOMPOSE:
		{
			U8 type = GetIntArg(0);
			if(type != ((U8)ItemCompose::ITEMCOMPOSE_PICKUP))
			{
				CHECK_MSGCOUNT(INFO_ITEMCOMPOSE,1,0,0);
			}
			else
			{
				CHECK_MSGCOUNT(INFO_ITEMCOMPOSE,2,0,0);
			}

			switch(type)
			{
			case ItemCompose::ITEMCOMPOSE_START:
				{
					if(pPlayer->pScheduleEvent->getEventID())
					{
						MessageEvent::send(con,SHOWTYPE_ERROR,MSG_PLAYER_BUSY);
						return;
					}

					pPlayer->mItemComposeList.setLock(true);
					S32 time = pPlayer->mItemComposeList.getReadyTime();
					simCallBackEvents* pEvent = new simCallBackEvents(INFO_ITEMCOMPOSE);
					U32 ret = Sim::postEvent(pPlayer,pEvent,Sim::getCurrentTime() + time);
					pPlayer->pScheduleEvent->setEvent(ret,time);

				}
				break;
			case ItemCompose::ITEMCOMPOSE_CANCEL:
				{
					if(pPlayer->pScheduleEvent->getEventID())
						pPlayer->pScheduleEvent->cancelEvent(pPlayer);

					pPlayer->mItemComposeList.cancelItemCompose(pPlayer);
				}
				break;
			case ItemCompose::ITEMCOMPOSE_PICKUP:
				{
					enWarnMessage msg = MSG_NONE;
					U8 iIndex = GetIntArg(1);
					msg = pPlayer->mItemComposeList.pickupItemByIndex(pPlayer,iIndex);
					if(msg != MSG_NONE)
						MessageEvent::send(con,SHOWTYPE_ERROR,msg);
				}
				break;
			}
		}
		break;
	case MAIL_DELETE:
		{
			int id = GetInt32Arg( 0 );
			CZoneMail::DeleteMail( id );
		}
		break;
	case MAIL_READ:
		{
			int id = GetInt32Arg( 0 );
			CZoneMail::ReadMail( id );
		}
		break;
	case MAIL_REQUEST:
		{
			int pageIndex = GetIntArg( 0 );
			int pageCount = GetIntArg( 1 );
			CZoneMail::RecvMail( pPlayer->getPlayerID(), pageIndex, pageCount );
		}
		break;
	case MAIL_SEND:
		{
			const char* title = GetStringArg( 0 );
			const char* content = GetStringArg( 1 );

			int itemId = GetInt32Arg( 0 );
			int itemCount = GetInt32Arg( 1 );
			int money = GetInt32Arg( 2 );

			int recverId = GetInt32Arg( 3 );

			// 这里需要扣除玩家身上的物品和金钱
			int error = 0;
			error = pPlayer->reduceMoney( money );
			if( itemId != 0 && itemCount > 0 )
			{
				error += g_ItemManager->delItemFromInventory( pPlayer, itemId, itemCount );
			}
			if( !error )
			{
				CZoneMail::SendMail( pPlayer->getPlayerID(),recverId, itemId, itemCount,money,  title, content );

				// 通知客户端邮件发送成功
				ServerGameNetEvent* pEvent = new ServerGameNetEvent( MAIL_SEND_OK );
				GameConnection* pConn = pPlayer->getControllingClient();
				if( pConn )
					pConn->postNetEvent( pEvent );
				
			}
			else
			{
				MessageEvent::send(con,SHOWTYPE_ERROR,avar("发送邮件错误！"),SHOWPOS_SCREEN);
			}
		}
		break;
	case MAIL_GETITEMS:
		{
			int nMailId = GetInt32Arg( 0 );
			// 这里给玩家加道具
			CZoneMail::TryGetItems( nMailId, pPlayer->getPlayerID() );
		}
		break;
	case MAIL_GETMONEY:
		{
			int nMailId = GetInt32Arg( 0 );
			// 这里给玩家加金钱
			CZoneMail::TryGetItems( nMailId, pPlayer->getPlayerID() );
		}
		break;
	case MAIL_DELETEALL:
		{
			// 删除玩家已接收的邮件
			CZoneMail::DeleteAllMail( pPlayer->getPlayerID() );
		}
		break;
	case INFO_SUPERMARKET:
		{
			U32 iType = GetInt32Arg(0);
			switch(iType)
			{
			case MARKET_BUY:
				{
					U32 iMarketItemID = GetInt32Arg(1);
					U32 iNum = GetInt32Arg(2);

					SuperMarketRes* pMarketItemRes = g_MarketItemReposity->getSuperMarketRes(iMarketItemID);
					if(!pMarketItemRes)
						return;
					ItemShortcut* pItem = ItemShortcut::CreateItem(pMarketItemRes->mMarketItemID, 1);
					if(!pItem)
						return;
					U32 iMoney = pMarketItemRes->mMarketItemPrice * iNum;
					if(pPlayer->getMoney() < iMoney)
					{
						MessageEvent::send(con, SHOWTYPE_ERROR, MSG_PLAYER_MONEYNOTENOUGH, SHOWPOS_SCREEN);
						return;
					}
					enWarnMessage msg = MSG_NONE;
					S32 index = 0;
					msg = g_ItemManager->addItemToInventory(pPlayer, pItem, index, iNum);
					if(msg != MSG_NONE)
					{
						MessageEvent::send(con, SHOWTYPE_ERROR, msg, SHOWPOS_SCREEN);
						return;
					}
					pPlayer->reduceMoney(iMoney);
				}
				break;
			}
		}
		break;
	case ZONE_CHAT:  // 附近聊天
		dStrcpy( pPlayer->mChatInfo, sizeof( pPlayer->mChatInfo ), GetStringArg( 0 ) );
		pPlayer->setMaskBits( GameObject::ChatInfoMask );
		break;
	case FRIEND_REQUEST:
		{
			ServerGameNetEvent* pEvent = new ServerGameNetEvent( FRIEND_REQUEST );
			pEvent->SetInt32ArgValues( 1, pPlayer->getPlayerID() );
			int destPlayerId = GetInt32Arg( 0 );
			Player* pDestPlayer = g_ServerGameplayState->GetPlayer( destPlayerId );
			if( pDestPlayer )
			{
				GameConnection* pDestCon = pDestPlayer->getControllingClient();
				if( pDestCon )
				{
					pDestCon->postNetEvent( pEvent );
				}
			}
		}
		break;
	case FRIEND_REQUEST+100:
		{
			U32 nPlayerId = GetInt32Arg(0);
			S32 nAddMoney = GetInt32Arg(1);
			Player *pPlayer = g_ServerGameplayState->GetPlayer(nPlayerId);
			if (pPlayer)
			{
				pPlayer->addMoney(nAddMoney);
			}
		}
		break;
	default:
		Con::errorf("未知的游戏事件%d",m_InfoType);
		break;
	}
#endif	
}